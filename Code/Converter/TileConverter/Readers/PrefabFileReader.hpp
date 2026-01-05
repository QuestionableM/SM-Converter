#pragma once

#include "ObjectDatabase/KeywordReplacer.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Converter/TileConverter/Readers/PrefabHeader.hpp"
#include "Converter/ConvertSettings.hpp"

#include "Converter/Entity/Blueprint.hpp"
#include "Converter/Entity/Prefab.hpp"
#include "Converter/Entity/Decal.hpp"

#include "UStd/UnmanagedAlgorithm.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Console.hpp"
#include "Utils/Memory.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

#define STR_WIDE(n) L ## n
#define STR_WIDE2(n) STR_WIDE(#n)
#define STR_WIDE3(n) STR_WIDE(n)

#define STR_INDIR(A) #A
#define STR(A) STR_INDIR(A)

#define THROW_PREFAB_ERROR(expression, error) \
	if ((expression)) { \
		DebugErrorL("Prefab Error: ", #expression); \
		error.setError(1, __FUNCTION__ "(" STR(__LINE__) ") -> " #expression); \
		return; \
	}

class PrefabFileReader
{
	PrefabFileReader() = default;

public:
	template<bool t_mod_counter>
	inline static SMPrefab* Read(
		const std::wstring& path,
		const std::wstring& flag,
		const SMEntityTransform& transform,
		ConvertError& error)
	{
		std::vector<Byte> v_bytes;
		if (!File::ReadFileBytes(path, v_bytes))
		{
			DebugErrorL("Couldn't read the specified prefab file: ", path);
			return nullptr;
		}

		return PrefabFileReader::Read<t_mod_counter>(v_bytes, path, flag, transform, error);
	}

	template<bool t_mod_counter>
	inline static void ReadPrefabHeader(
		MemoryWrapper& reader,
		SMPrefab* pPrefab,
		std::uint32_t version,
		ConvertError& error)
	{
		BitStream v_stream(reader);

		PrefabHeader v_header;
		v_header.Read(v_stream, version);

		if (v_header.hasBlueprints != 0)
		{
			const int v_blueprint_bytes = PrefabFileReader::ReadBlueprints<t_mod_counter>(v_stream, pPrefab, v_header, version);
			THROW_PREFAB_ERROR(v_blueprint_bytes != v_header.hasBlueprints, error);
		}

		if (v_header.hasPrefabs != 0)
		{
			const int v_prefab_bytes = PrefabFileReader::ReadPrefabs<t_mod_counter>(v_stream, pPrefab, v_header, version, error);
			if (error) return;

			THROW_PREFAB_ERROR(v_prefab_bytes != v_header.hasPrefabs, error);
		}

		//Skipped the node reader as it's not needed
		if (v_header.hasNodes != 0)
			v_stream.SetIndex(v_stream.Index() + v_header.hasNodes);

		if (v_header.hasAssets != 0)
		{
			const int v_asset_bytes = PrefabFileReader::ReadAssets<t_mod_counter>(v_stream, pPrefab, v_header, version);
			THROW_PREFAB_ERROR(v_asset_bytes != v_header.hasAssets, error);
		}

		if (v_header.hasDecals != 0)
		{
			const int v_decal_bytes = PrefabFileReader::ReadDecals<t_mod_counter>(v_stream, pPrefab, v_header, version);
			THROW_PREFAB_ERROR(v_decal_bytes != v_header.hasDecals, error);
		}

		if (v_header.has_0x5c != 0)
			PrefabFileReader::Read_248(v_stream, pPrefab, v_header, version);

		if (v_header.hasKinematics != 0)
		{
			const int v_kinematic_bytes = PrefabFileReader::ReadKinematics<t_mod_counter>(v_stream, pPrefab, v_header, version);
			THROW_PREFAB_ERROR(v_kinematic_bytes != v_header.hasKinematics, error);
		}

		if (v_header.has_0x7c != 0)
			PrefabFileReader::Read_2(v_stream, pPrefab, v_header, version);
	}

	template<bool t_mod_counter>
	inline static SMPrefab* Read(
		const std::vector<Byte>& bytes,
		const std::wstring_view& ppath,
		const std::wstring_view& pflag,
		const SMEntityTransform& transform,
		ConvertError& error)
	{
		MemoryWrapper reader(bytes);

		if (reader.NextObject<std::uint32_t>() != 0x50524546)
		{
			DebugErrorL("Invalid Prefab Magic");
			return nullptr;
		}

		const std::uint32_t version = reader.NextObject<std::uint32_t, true>();
		if (version <= 0 || version > 10)
		{
			error.setError(1, "Unsupported Prefab Version: ", version);
			return nullptr;
		}

		SMPrefab* prefab = nullptr;
		if constexpr (!t_mod_counter) {
			prefab = new SMPrefab(ppath, pflag, transform);
		}

		PrefabFileReader::ReadPrefabHeader<t_mod_counter>(reader, prefab, version, error);

		if constexpr (!t_mod_counter) {
			if (error)
			{
				delete prefab;
				return nullptr;
			}
		}

		return prefab;
	}

	template<bool t_mod_counter>
	static int ReadBlueprints(
		BitStream& stream,
		SMPrefab* prefab,
		PrefabHeader& v_header,
		std::uint32_t version)
	{
		if (!TileConverterSettings::ExportBlueprints)
		{
			stream.Move(v_header.hasBlueprints);
			return v_header.hasBlueprints;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(version) + L"_blueprints", 100);
		const int v_idx_start = stream.Index();

		for (int a = 0; a < v_header.blueprintCount; a++)
		{
			const int string_length = stream.ReadInt();
			const std::string value = stream.ReadString(string_length);

			const glm::vec3 f_pos = stream.ReadVec3();
			const glm::quat f_quat = stream.ReadQuat();

			stream.ReadInt();

			//a random byte was added in the version 10 of prefabs
			if (version >= 10)
				stream.ReadByte();

			if constexpr (t_mod_counter)
			{
				SMBlueprint::LoadAndCountAutomatic(value);
			}
			else
			{
				SMBlueprint* blueprint = SMBlueprint::LoadAutomatic(value, f_pos, f_quat);
				if (!blueprint) continue;

				prefab->addObject(blueprint);
			}
		}

		return stream.Index() - v_idx_start;
	}

	template<bool t_mod_counter>
	static int ReadPrefabs(
		BitStream& stream,
		SMPrefab* pPrefab,
		PrefabHeader& header,
		std::uint32_t version,
		ConvertError& error)
	{
		if (!TileConverterSettings::ExportPrefabs)
		{
			stream.Move(header.hasPrefabs);
			return header.hasPrefabs;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(version) + L"_prefabs", 100);
		const int v_start_idx = stream.Index();

		SMEntityTransform v_transform;
		for (int a = 0; a < header.prefabCount; a++)
		{
			const int v_str_length = stream.ReadInt();
			std::wstring v_pref_path = String::ToWide(stream.ReadString(v_str_length));
			KeywordReplacer::ReplaceKeyR(v_pref_path);

			DebugOutL(0b1011_fg, "Recursive Prefab Path: ", v_pref_path);

			v_transform.position = stream.ReadVec3();
			v_transform.rotation = stream.ReadQuat();
			v_transform.scale = (version < 5) ? glm::vec3(1.0f) : stream.ReadVec3();

			stream.ReadInt();
			stream.ReadInt();

			if constexpr (t_mod_counter)
			{
				PrefabFileReader::Read<t_mod_counter>(v_pref_path, L"", v_transform, error);
				if (error) return 0;
			}
			else
			{
				SMPrefab* rec_prefab = PrefabFileReader::Read<t_mod_counter>(v_pref_path, L"", v_transform, error);
				if (error) return 0;
				if (!rec_prefab) continue;

				pPrefab->addObject(rec_prefab);
			}
		}

		return stream.Index() - v_start_idx;
	}

	static void ReadNodes(BitStream& stream, SMPrefab* prefab, int count)
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

	template<bool t_mod_counter>
	static int ReadAssets(
		BitStream& stream,
		SMPrefab* prefab,
		PrefabHeader& v_header,
		std::uint32_t version)
	{
		if (!TileConverterSettings::ExportAssets)
		{
			stream.Move(v_header.hasAssets);
			return v_header.hasAssets;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(version) + L"_assets", 100);
		const int v_idx_start = stream.Index();

		SMEntityTransform v_transform;
		for (int a = 0; a < v_header.assetCount; a++)
		{
			v_transform.position = stream.ReadVec3();
			v_transform.rotation = stream.ReadQuat();
			v_transform.scale = stream.ReadVec3();

			const SMUuid uuid = stream.ReadUuid();
			const int materialCount = stream.ReadByte();

			std::unordered_map<std::string, SMColor, Hashing::StringHasher, std::equal_to<>> color_map;

			if (materialCount != 0)
			{
				for (int b = 0; b < materialCount; b++)
				{
					const int length = stream.ReadByte();
					std::string str = stream.ReadString(length);

					const unsigned int color = stream.ReadInt();

					if (color_map.find(str) == color_map.end())
						color_map.emplace(std::move(str), color);
				}
			}

			if (version >= 10)
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

				SMAsset* nAsset = new SMAsset(v_asset_data, v_transform, pModel, std::move(color_map));
				prefab->addObject(nAsset);
			}
		}

		return stream.Index() - v_idx_start;
	}

	template<bool t_mod_counter>
	static int ReadDecals(
		BitStream& stream,
		SMPrefab* prefab,
		PrefabHeader& v_header,
		std::uint32_t version)
	{
		if (!TileConverterSettings::ExportDecals)
		{
			stream.Move(v_header.hasDecals);
			return v_header.hasDecals;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(version) + L"_decals", 100);
		const int v_start_idx = stream.Index();

		SMEntityTransform v_transform;
		for (int a = 0; a < v_header.decalsCount; a++)
		{
			v_transform.position = stream.ReadVec3();
			v_transform.rotation = stream.ReadQuat();
			v_transform.scale = stream.ReadVec3();

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

				SMDecal* v_newDecal = new SMDecal(v_decal_data, v_transform, v_color);
				prefab->addObject(v_newDecal);
			}
		}

		return stream.Index() - v_start_idx;
	}

	static int Read_248(
		BitStream& stream,
		SMPrefab* prefab,
		PrefabHeader& v_header,
		std::uint32_t version)
	{
		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(version) + L"_248", 100);
		DebugWarningL("UNIMPLEMENTED -> ", stream.Index());

		stream.Move(v_header.has_0x5c);
		return v_header.has_0x5c;
	}

	template<bool t_mod_counter>
	static int ReadKinematics(
		BitStream& stream,
		SMPrefab* prefab,
		PrefabHeader& v_header,
		std::uint32_t version)
	{
		if (!TileConverterSettings::ExportKinematics)
		{
			stream.Move(v_header.hasBlueprints);
			return v_header.hasBlueprints;
		}

		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(version) + L"_kinematics", 5000);
		const int v_start_idx = stream.Index();

		SMEntityTransform v_transform;
		for (int a = 0; a < v_header.kinematicsCount; a++)
		{
			v_transform.position = stream.ReadVec3();
			v_transform.rotation = stream.ReadQuat();
			v_transform.scale = stream.ReadVec3();

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

				prefab->addObject(
					new SMKinematic(
						v_kinematic_data,
						v_transform,
						v_km_model,
						v_color
					)
				);
			}
		}

		return stream.Index() - v_start_idx;
	}

	static int Read_2(
		BitStream& stream,
		SMPrefab* prefab,
		PrefabHeader& v_header,
		std::uint32_t version)
	{
		BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(version) + L"_2", 100);
		DebugWarningL("UNIMPLEMENTED -> ", stream.Index());

		stream.Move(v_header.has_0x7c);
		return v_header.has_0x7c;
	}
};

SM_MANAGED_CODE