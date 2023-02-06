#pragma once

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ObjectDatabase.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Converter\TileConverter\Readers\PrefabHeader.hpp"
#include "Converter\ConvertSettings.hpp"

#include "Converter\Entity\Blueprint.hpp"
#include "Converter\Entity\Prefab.hpp"
#include "Converter\Entity\Decal.hpp"

#include "UStd\UnmanagedAlgorithm.hpp"

#include "Utils\Console.hpp"
#include "Utils\Memory.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

#define STR_WIDE(n) L ## n
#define STR_WIDE2(n) STR_WIDE(#n)
#define STR_WIDE3(n) STR_WIDE(n)

#define STR_INDIR(A) #A
#define STR(A) STR_INDIR(A)

#define THROW_PREFAB_ERROR(expression, error) \
	if ((expression)) { \
		DebugErrorL("Prefab Error: ", #expression); \
		error = ConvertError(1, STR_WIDE3(__FUNCTION__) L" (" STR_WIDE3(STR(__LINE__)) L") -> " STR_WIDE2(expression)); \
		return; \
	}

class PrefabFileReader
{
	PrefabFileReader() = default;

public:
	template<bool t_mod_counter>
	inline static SMPrefab* Read(const std::wstring& path, const std::wstring& flag, ConvertError& v_error)
	{
		std::vector<Byte> v_bytes;
		if (!File::ReadFileBytes(path, v_bytes))
		{
			DebugErrorL("Couldn't read the specified prefab file: ", path);
			return nullptr;
		}

		return PrefabFileReader::Read<t_mod_counter>(v_bytes, path, flag, v_error);
	}

	template<bool t_mod_counter, int t_pref_version>
	inline static void ReadPrefabHeader(MemoryWrapper& v_reader, SMPrefab* prefab, ConvertError& v_error)
	{
		BitStream v_stream(v_reader);

		PrefabHeader v_header;
		v_header.Read<t_pref_version>(v_stream);

		if (v_header.hasBlueprints != 0)
		{
			const int v_blueprint_bytes = PrefabFileReader::ReadBlueprints<t_mod_counter, t_pref_version>(v_stream, prefab, v_header);
			THROW_PREFAB_ERROR(v_blueprint_bytes != v_header.hasBlueprints, v_error);
		}

		if (v_header.hasPrefabs != 0)
		{
			const int v_prefab_bytes = PrefabFileReader::ReadPrefabs<t_mod_counter, t_pref_version>(v_stream, prefab, v_header, v_error);
			if (v_error) return;

			THROW_PREFAB_ERROR(v_prefab_bytes != v_header.hasPrefabs, v_error);
		}

		//Skipped the node reader as it's not needed
		if (v_header.hasNodes != 0)
			v_stream.SetIndex(v_stream.Index() + v_header.hasNodes);

		if (v_header.hasAssets != 0)
		{
			const int v_asset_bytes = PrefabFileReader::ReadAssets<t_mod_counter, t_pref_version>(v_stream, prefab, v_header);
			THROW_PREFAB_ERROR(v_asset_bytes != v_header.hasAssets, v_error);
		}

		if (v_header.hasDecals != 0)
		{
			const int v_decal_bytes = PrefabFileReader::ReadDecals<t_mod_counter, t_pref_version>(v_stream, prefab, v_header);
			THROW_PREFAB_ERROR(v_decal_bytes != v_header.hasDecals, v_error);
		}

		if (v_header.has_0x5c != 0)
			PrefabFileReader::Read_248<t_pref_version>(v_stream, prefab, v_header);

		if (v_header.hasKinematics != 0)
		{
			const int v_kinematic_bytes = PrefabFileReader::ReadKinematics<t_mod_counter, t_pref_version>(v_stream, prefab, v_header);
			THROW_PREFAB_ERROR(v_kinematic_bytes != v_header.hasKinematics, v_error);
		}

		if (v_header.has_0x7c != 0)
			PrefabFileReader::Read_2<t_pref_version>(v_stream, prefab, v_header);
	}

	template<bool t_mod_counter>
	inline static SMPrefab* Read(const std::vector<Byte>& bytes, const std::wstring& ppath, const std::wstring& pflag, ConvertError& v_error)
	{
		MemoryWrapper reader(bytes);

		const std::string v_pref_magic = reader.NextString<true>(4);
		if (v_pref_magic != "PREF")
		{
			DebugErrorL("The specified prefab contains wrong magic value");
			return nullptr;
		}

		using v_prefab_loader_func = void (*)(MemoryWrapper&, SMPrefab*, ConvertError&);
		const static v_prefab_loader_func v_prefab_loaders[] =
		{
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 1>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 2>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 3>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 4>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 5>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 6>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 7>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 8>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 9>,
			PrefabFileReader::ReadPrefabHeader<t_mod_counter, 10>
		};

		const int version = reader.NextObject<int, true>();
		if (version <= 0 || version > 10)
		{
			v_error = ConvertError(1, L"Unsupported Prefab Version: " + std::to_wstring(version));
			return nullptr;
		}

		SMPrefab* prefab = nullptr;
		if constexpr (!t_mod_counter) {
			prefab = new SMPrefab(ppath, pflag);
		}

		v_prefab_loaders[version - 1](reader, prefab, v_error);

		if constexpr (!t_mod_counter) {
			if (v_error)
			{
				delete prefab;
				return nullptr;
			}
		}

		return prefab;
	}

	template<bool t_mod_counter, int t_pref_version>
	static int ReadBlueprints(BitStream& stream, SMPrefab* prefab, PrefabHeader& v_header)
	{
		if (!TileConverterSettings::ExportBlueprints)
		{
			stream.Move(v_header.hasBlueprints);
			return v_header.hasBlueprints;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(t_pref_version) + L"_blueprints", 100);
		const int v_idx_start = stream.Index();

		for (int a = 0; a < v_header.blueprintCount; a++)
		{
			const int string_length = stream.ReadInt();
			const std::string value = stream.ReadString(string_length);

			const glm::vec3 f_pos = stream.ReadVec3();
			const glm::quat f_quat = stream.ReadQuat();

			stream.ReadInt();

			//a random byte was added in the version 10 of prefabs
			if constexpr (t_pref_version >= 10)
				stream.ReadByte();

			if constexpr (t_mod_counter)
			{
				SMBlueprint::LoadAndCountAutomatic(value);
			}
			else
			{
				SMBlueprint* blueprint = SMBlueprint::LoadAutomatic(value);
				if (!blueprint) continue;

				blueprint->SetPosition(f_pos);
				blueprint->SetRotation(f_quat);

				prefab->AddObject(blueprint);
			}
		}

		return stream.Index() - v_idx_start;
	}

	template<bool t_mod_counter, int t_pref_version>
	static int ReadPrefabs(BitStream& stream, SMPrefab* prefab, PrefabHeader& v_header, ConvertError& v_error)
	{
		if (!TileConverterSettings::ExportPrefabs)
		{
			stream.Move(v_header.hasPrefabs);
			return v_header.hasPrefabs;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(t_pref_version) + L"_prefabs", 100);
		const int v_start_idx = stream.Index();

		for (int a = 0; a < v_header.prefabCount; a++)
		{
			const int v_str_length = stream.ReadInt();
			std::wstring v_pref_path = String::ToWide(stream.ReadString(v_str_length));
			KeywordReplacer::ReplaceKeyR(v_pref_path);

			DebugOutL(0b1011_fg, "Recursive Prefab Path: ", v_pref_path);

			const glm::vec3 f_pos = stream.ReadVec3();
			const glm::quat f_quat = stream.ReadQuat();
			glm::vec3 f_size;

			if constexpr (t_pref_version < 5)
			{
				f_size = glm::vec3(1.0f);
			}
			else
			{
				f_size = stream.ReadVec3();
			}

			stream.ReadInt();
			stream.ReadInt();

			if constexpr (t_mod_counter)
			{
				PrefabFileReader::Read<t_mod_counter>(v_pref_path, L"", v_error);
				if (v_error) return 0;
			}
			else
			{
				SMPrefab* rec_prefab = PrefabFileReader::Read<t_mod_counter>(v_pref_path, L"", v_error);
				if (v_error) return 0;

				if (!rec_prefab) continue;

				rec_prefab->SetPosition(f_pos);
				rec_prefab->SetRotation(f_quat);
				rec_prefab->SetSize(f_size);

				prefab->AddObject(rec_prefab);
			}
		}

		return stream.Index() - v_start_idx;
	}

	static void ReadNodes(BitStream& stream, SMPrefab* prefab, const int& count)
	{
		const int uVar2 = stream.ReadByte();
		std::vector<std::string> tags = {};

		if (uVar2 != 0)
		{
			tags.reserve(uVar2);
			for (int a = 0; a < uVar2; a++)
			{
				const int size = stream.ReadByte();
				tags.push_back(stream.ReadString(size));
			}
		}

		for (int a = 0; a < count; a++)
		{
			const glm::vec3 f_pos = stream.ReadVec3();
			const glm::quat f_quat = stream.ReadQuat();
			const glm::vec3 f_size = stream.ReadVec3();

			/*
			NodeImpl node = new NodeImpl(tags);
			node.setPosition(f_pos); //+ f_position
			node.setRotation(f_quat);
			node.setSize(f_size);
			*/

			const int bVar2 = stream.ReadByte();
			if (bVar2 != 0)
			{
				for (int b = 0; b < bVar2; b++)
				{
					const int idx = stream.ReadByte();
					std::string tag_name = tags[idx];
					
					//node.setTagState(tag_name, true);
				}
			}

			const int uVar3 = stream.ReadInt();
			if (uVar3 != 0)
			{
				stream.ReadBytes(uVar3);
				//Object deserialized = LuaDeserializer.DeserializePure(blob);
			}

			//prefab.addNode(node);
		}
	}

	template<bool t_mod_counter, int t_pref_version>
	static int ReadAssets(BitStream& stream, SMPrefab* prefab, PrefabHeader& v_header)
	{
		if (!TileConverterSettings::ExportAssets)
		{
			stream.Move(v_header.hasAssets);
			return v_header.hasAssets;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(t_pref_version) + L"_assets", 100);
		const int v_idx_start = stream.Index();

		for (int a = 0; a < v_header.assetCount; a++)
		{
			const glm::vec3 f_pos = stream.ReadVec3();
			const glm::quat f_quat = stream.ReadQuat();
			const glm::vec3 f_size = stream.ReadVec3();

			const SMUuid uuid = stream.ReadUuid();
			const int materialCount = stream.ReadByte();

			std::unordered_map<std::string, SMColor> color_map;

			if (materialCount != 0)
			{
				for (int b = 0; b < materialCount; b++)
				{
					const int length = stream.ReadByte();
					const std::string str = stream.ReadString(length);

					const unsigned int color = stream.ReadInt();

					if (color_map.find(str) == color_map.end())
						color_map.insert(std::make_pair(str, color));
				}
			}

			if constexpr (t_pref_version >= 10)
				stream.ReadByte();

			const AssetData* v_asset_data = SMMod::GetGlobalObject<AssetData>(uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((v_asset_data != nullptr) ? v_asset_data->m_mod : nullptr);
			}
			else
			{
				if (!v_asset_data) continue;

				Model* pModel = ModelStorage::LoadModel(v_asset_data->m_mesh);
				if (!pModel) continue;

				SMAsset* nAsset = new SMAsset(v_asset_data, pModel, color_map);
				nAsset->SetPosition(f_pos);
				nAsset->SetRotation(f_quat);
				nAsset->SetSize(f_size);

				prefab->AddObject(nAsset);
			}
		}

		return stream.Index() - v_idx_start;
	}

	template<bool t_mod_counter, int t_pref_version>
	static int ReadDecals(BitStream& stream, SMPrefab* prefab, PrefabHeader& v_header)
	{
		if (!TileConverterSettings::ExportDecals)
		{
			stream.Move(v_header.hasDecals);
			return v_header.hasDecals;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(t_pref_version) + L"_decals", 100);
		const int v_start_idx = stream.Index();

		for (int a = 0; a < v_header.decalsCount; a++)
		{
			const glm::vec3 v_pos = stream.ReadVec3();
			const glm::quat v_quat = stream.ReadQuat();
			const glm::vec3 v_size = stream.ReadVec3();

			const SMUuid v_uuid = stream.ReadUuid();
			const SMColor v_color = stream.ReadInt();

			//Read a random 4 byte value
			stream.ReadInt();

			const DecalData* v_decal_data = SMMod::GetGlobalObject<DecalData>(v_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((v_decal_data != nullptr) ? v_decal_data->m_mod : nullptr);
			}
			else
			{
				if (!v_decal_data) continue;

				SMDecal* v_newDecal = new SMDecal(v_decal_data, v_color);
				v_newDecal->SetPosition(v_pos);
				v_newDecal->SetRotation(v_quat);
				v_newDecal->SetSize(v_size);

				prefab->AddObject(v_newDecal);
			}
		}

		return stream.Index() - v_start_idx;
	}

	template<int t_pref_version>
	static int Read_248(BitStream& stream, SMPrefab* prefab, PrefabHeader& v_header)
	{
		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(t_pref_version) + L"_248", 100);
		DebugWarningL("UNIMPLEMENTED -> ", stream.Index());

		stream.Move(v_header.has_0x5c);
		return v_header.has_0x5c;
	}

	template<bool t_mod_counter, int t_pref_version>
	static int ReadKinematics(BitStream& stream, SMPrefab* prefab, PrefabHeader& v_header)
	{
		if (!TileConverterSettings::ExportKinematics)
		{
			stream.Move(v_header.hasBlueprints);
			return v_header.hasBlueprints;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(t_pref_version) + L"_kinematics", 5000);
		const int v_start_idx = stream.Index();

		for (int a = 0; a < v_header.kinematicsCount; a++)
		{
			const glm::vec3 v_pos = stream.ReadVec3();
			const glm::quat v_quat = stream.ReadQuat();
			const glm::vec3 v_size = stream.ReadVec3();

			const SMUuid v_uuid = stream.ReadUuid();
			const SMColor v_color = stream.ReadInt();

			while (1)
			{
				if (stream.ReadByte() == 0x9)
					break;
			}

			//Blueprint size
			const int v_bp_json_sz = stream.ReadInt();
			//Skip the blueprint string
			stream.Move(v_bp_json_sz * 8);
			//Read the null byte
			const Byte test = stream.ReadByte();

			const KinematicData* v_kinematic_data = SMMod::GetGlobalObject<KinematicData>(v_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((v_kinematic_data != nullptr) ? v_kinematic_data->m_mod : nullptr);
			}
			else
			{
				if (!v_kinematic_data) continue;

				Model* v_km_model = ModelStorage::LoadModel(v_kinematic_data->m_mesh);
				if (!v_km_model) continue;

				SMKinematic* v_new_kinematic = new SMKinematic(v_kinematic_data, v_km_model, v_color);
				v_new_kinematic->SetPosition(v_pos);
				v_new_kinematic->SetRotation(v_quat);
				v_new_kinematic->SetSize(v_size);

				prefab->AddObject(v_new_kinematic);
			}
		}

		return stream.Index() - v_start_idx;
	}

	template<int t_pref_version>
	static int Read_2(BitStream& stream, SMPrefab* prefab, PrefabHeader& v_header)
	{
		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(t_pref_version) + L"_2", 100);
		DebugWarningL("UNIMPLEMENTED -> ", stream.Index());

		stream.Move(v_header.has_0x7c);
		return v_header.has_0x7c;
	}
};

#pragma managed