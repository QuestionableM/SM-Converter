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

class PrefabFileReader
{
	PrefabFileReader() = default;

public:
	template<bool t_mod_counter>
	static SMPrefab* Read(const std::wstring& path, const std::wstring& flag)
	{
		std::vector<Byte> v_bytes;
		if (!File::ReadFileBytes(path, v_bytes))
		{
			DebugErrorL("Couldn't read the specified prefab file: ", path);
			return nullptr;
		}

		return PrefabFileReader::Read<t_mod_counter>(v_bytes, path, flag);
	}

	template<bool t_mod_counter>
	static SMPrefab* Read(const std::vector<Byte>& bytes, const std::wstring& ppath, const std::wstring& pflag)
	{
		MemoryWrapper reader(bytes);

		const std::string v_pref_magic = reader.NextString<true>(4);
		if (v_pref_magic != "PREF")
		{
			DebugErrorL("The specified prefab contains wrong magic value");
			return nullptr;
		}

		SMPrefab* prefab = nullptr;
		if constexpr (!t_mod_counter) {
			prefab = new SMPrefab(ppath, pflag);
		}
		
		const int version = reader.NextObject<int, true>();
		DebugOutL("Prefab Version: ", version);

		BitStream stream(reader);

		PrefabHeader pHeader(version);
		pHeader.Read(stream);

		if (pHeader.hasBlueprints != 0)
		{
			PrefabFileReader::ReadBlueprints<t_mod_counter>(stream, prefab, pHeader.blueprintCount, version);
		}

		if (pHeader.hasPrefabs != 0)
		{
			PrefabFileReader::ReadPrefabs<t_mod_counter>(stream, prefab, pHeader.prefabCount, version);
		}

		if (pHeader.hasNodes != 0)
		{
			PrefabFileReader::ReadNodes(stream, prefab, pHeader.nodeCount);
		}

		if (pHeader.hasAssets != 0)
		{
			PrefabFileReader::ReadAssets<t_mod_counter>(stream, prefab, pHeader.assetCount, version);
		}

		if (pHeader.hasDecals != 0)
		{
			PrefabFileReader::ReadDecals<t_mod_counter>(stream, prefab, pHeader.decalsCount, version);
		}

		if (pHeader.has_0x5c != 0)
		{
			PrefabFileReader::Read_248(stream, prefab, pHeader.count_0x54);
		}

		if (pHeader.has_0x6c != 0)
		{
			PrefabFileReader::Read_1(stream, prefab, pHeader.count_0x64);
		}

		if (pHeader.has_0x7c != 0)
		{
			PrefabFileReader::Read_2(stream, prefab, pHeader.count_0x74);
		}

		return prefab;
	}

	template<bool t_mod_counter>
	static void ReadBlueprints(BitStream& stream, SMPrefab* prefab, const int& count, const int& version)
	{
		for (int a = 0; a < count; a++)
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
				if (TileConverterSettings::ExportBlueprints)
				{
					SMBlueprint* blueprint = SMBlueprint::LoadAutomatic(value);
					if (!blueprint) continue;

					blueprint->SetPosition(f_pos);
					blueprint->SetRotation(f_quat);

					prefab->AddObject(blueprint);
				}
			}
		}
	}

	template<bool t_mod_counter>
	static void ReadPrefabs(BitStream& stream, SMPrefab* prefab, const int& count, const int& version)
	{
		for (int a = 0; a < count; a++)
		{
			const int v_str_length = stream.ReadInt();
			std::wstring v_pref_path = String::ToWide(stream.ReadString(v_str_length));
			KeywordReplacer::ReplaceKeyR(v_pref_path);

			DebugOutL(0b1011_fg, "Recursive Prefab Path: ", v_pref_path);

			const glm::vec3 f_pos = stream.ReadVec3();
			const glm::quat f_quat = stream.ReadQuat();
			glm::vec3 f_size;

			if (version < 5)
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
				PrefabFileReader::Read<t_mod_counter>(v_pref_path, L"");
			}
			else
			{
				if (TileConverterSettings::ExportPrefabs)
				{
					SMPrefab* rec_prefab = PrefabFileReader::Read<t_mod_counter>(v_pref_path, L"");
					if (!rec_prefab) continue;

					rec_prefab->SetPosition(f_pos);
					rec_prefab->SetRotation(f_quat);
					rec_prefab->SetSize(f_size);

					prefab->AddObject(rec_prefab);
				}
			}
		}
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

	template<bool t_mod_counter>
	static void ReadAssets(BitStream& stream, SMPrefab* prefab, const int& count, const int& version)
	{
		const int v_idx = stream.Index();

		std::ofstream v_blueprint_dump("./asset_dump.txt");
		if (v_blueprint_dump.is_open())
		{
			for (std::size_t a = 0; a < 200; a++)
			{
				unsigned char cur_byte = static_cast<unsigned char>(stream.ReadByte());
				v_blueprint_dump.write(reinterpret_cast<const char*>(&cur_byte), 1);
			}

			v_blueprint_dump.close();
		}

		stream.SetIndex(v_idx);

		for (int a = 0; a < count; a++)
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

			if (version >= 10)
				stream.ReadByte();

			if constexpr (t_mod_counter)
			{
				AssetData* v_asset_data = SMMod::GetGlobalAsset(uuid);
				ItemModStats::IncrementModPart((v_asset_data != nullptr) ? v_asset_data->m_mod : nullptr);
			}
			else
			{
				if (TileConverterSettings::ExportAssets)
				{
					AssetData* asset_data = SMMod::GetGlobalAsset(uuid);
					if (!asset_data) continue;

					Model* pModel = ModelStorage::LoadModel(asset_data->m_mesh);
					if (!pModel) continue;

					SMAsset* nAsset = new SMAsset(asset_data, pModel, color_map);
					nAsset->SetPosition(f_pos);
					nAsset->SetRotation(f_quat);
					nAsset->SetSize(f_size);

					prefab->AddObject(nAsset);
				}
			}
		}
	}

	template<bool t_mod_counter>
	static void ReadDecals(BitStream& stream, SMPrefab* prefab, const int& count, const int& version)
	{
		DebugOutL("Reading ", count, " decals...");

		for (int a = 0; a < count; a++)
		{
			const glm::vec3 v_pos = stream.ReadVec3();
			const glm::quat v_quat = stream.ReadQuat();
			const glm::vec3 v_size = stream.ReadVec3();

			const SMUuid v_uuid = stream.ReadUuid();
			const SMColor v_color = stream.ReadInt();

			//Read a random 4 byte value
			stream.ReadInt();

			if constexpr (t_mod_counter)
			{
				const DecalData* v_decal_data = SMMod::GetGlobalDecal(v_uuid);
				ItemModStats::IncrementModPart((v_decal_data != nullptr) ? v_decal_data->m_mod : nullptr);
			}
			else
			{
				if (TileConverterSettings::ExportDecals)
				{
					const DecalData* v_decalData = SMMod::GetGlobalDecal(v_uuid);
					if (!v_decalData) continue;

					SMDecal* v_newDecal = new SMDecal(v_decalData, v_color);
					v_newDecal->SetPosition(v_pos);
					v_newDecal->SetRotation(v_quat);
					v_newDecal->SetSize(v_size);

					prefab->AddObject(v_newDecal);
				}
			}
		}
	}

	static void Read_248(BitStream& stream, SMPrefab* prefab, const int& count)
	{
		DebugWarningL("UNIMPLEMENTED -> ", stream.Index());
	}

	static void Read_1(BitStream& stream, SMPrefab* prefab, const int& count)
	{
		DebugWarningL("UNIMPLEMENTED -> ", stream.Index());
	}

	static void Read_2(BitStream& stream, SMPrefab* prefab, const int& count)
	{
		DebugWarningL("UNIMPLEMENTED -> ", stream.Index());
	}
};

#pragma managed