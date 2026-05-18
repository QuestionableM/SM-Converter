#include "Converter/ConvertSettings.hpp"
#include "PrefabFileReader.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/KeywordReplacer.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Converter/Entity/Kinematic.hpp"
#include "Converter/Entity/Blueprint.hpp"
#include "Converter/Entity/Prefab.hpp"
#include "Converter/Entity/Decal.hpp"
#include "Converter/Entity/Asset.hpp"

#include "UStd/UnmanagedAlgorithm.hpp"

#include "Utils/Console.hpp"
#include "Utils/Memory.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

SMPrefab* PrefabFileReader::Read(
	const std::wstring_view& path,
	const std::wstring_view& flag,
	const SMEntityTransform& transform,
	ConvertError& error,
	const bool counterMode)
{
	std::vector<Byte> v_bytes;
	if (!File::ReadFileBytes(path, v_bytes))
	{
		DebugErrorL("Couldn't read the specified prefab file: ", path);
		return nullptr;
	}

	return PrefabFileReader::Read(v_bytes, path, flag, transform, error, counterMode);
}

SMPrefab* PrefabFileReader::Read(
	const std::vector<Byte>& bytes,
	const std::wstring_view& ppath,
	const std::wstring_view& pflag,
	const SMEntityTransform& transform,
	ConvertError& error,
	const bool counterMode)
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

	PrefabReaderContext v_prefabContext;
	v_prefabContext.pReader = &reader;
	v_prefabContext.pBitStream = nullptr;
	v_prefabContext.pError = &error;
	v_prefabContext.pPrefab = nullptr;
	v_prefabContext.prefabVersion = version;
	v_prefabContext.counterMode = counterMode;

	if (!v_prefabContext.counterMode)
		v_prefabContext.pPrefab = new SMPrefab(ppath, pflag, transform);

	PrefabFileReader::ReadPrefabHeader(v_prefabContext);

	if (!v_prefabContext.counterMode && *v_prefabContext.pError)
	{
		delete v_prefabContext.pPrefab;
		return nullptr;
	}

	return v_prefabContext.pPrefab;
}

void PrefabFileReader::ReadPrefabHeader(PrefabReaderContext& context)
{
	BitStream v_stream(*context.pReader);
	context.pBitStream = &v_stream;

	PrefabHeader v_header;
	v_header.Read(v_stream, context.prefabVersion);

	if (v_header.hasBlueprints != 0)
	{
		const int v_blueprint_bytes = PrefabFileReader::ReadBlueprints(context, v_header);
		THROW_PREFAB_ERROR(v_blueprint_bytes != v_header.hasBlueprints, context.pError);
	}

	if (v_header.hasPrefabs != 0)
	{
		const int v_prefab_bytes = PrefabFileReader::ReadPrefabs(context, v_header);
		if (*context.pError) return;

		THROW_PREFAB_ERROR(v_prefab_bytes != v_header.hasPrefabs, context.pError);
	}

	//Skipped the node reader as it's not needed
	if (v_header.hasNodes != 0)
		v_stream.SetIndex(v_stream.Index() + v_header.hasNodes);

	if (v_header.hasAssets != 0)
	{
		const int v_asset_bytes = PrefabFileReader::ReadAssets(context, v_header);
		THROW_PREFAB_ERROR(v_asset_bytes != v_header.hasAssets, context.pError);
	}

	if (v_header.hasDecals != 0)
	{
		const int v_decal_bytes = PrefabFileReader::ReadDecals(context, v_header);
		THROW_PREFAB_ERROR(v_decal_bytes != v_header.hasDecals, context.pError);
	}

	if (v_header.has_0x5c != 0)
		PrefabFileReader::Read_248(context, v_header);

	if (v_header.hasKinematics != 0)
	{
		const int v_kinematic_bytes = PrefabFileReader::ReadKinematics(context, v_header);
		THROW_PREFAB_ERROR(v_kinematic_bytes != v_header.hasKinematics, context.pError);
	}

	if (v_header.has_0x7c != 0)
		PrefabFileReader::Read_2(context, v_header);
}

int PrefabFileReader::ReadBlueprints(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;

	if (!TileConverterSettings::ExportBlueprints)
	{
		stream.Move(header.hasBlueprints);
		return header.hasBlueprints;
	}

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_blueprints", 100);
	const int v_idx_start = stream.Index();

	for (int a = 0; a < header.blueprintCount; a++)
	{
		const int string_length = stream.ReadInt();
		const std::string value = stream.ReadString(string_length);

		const glm::vec3 f_pos = stream.ReadVec3();
		const glm::quat f_quat = stream.ReadQuat();

		stream.ReadInt();

		// A random byte was added in the version 10 of prefabs
		if (context.prefabVersion >= 10)
			stream.ReadByte();

		if (context.counterMode)
		{
			SMBlueprint::LoadAndCountAutomatic(value);
		}
		else
		{
			SMBlueprint* blueprint = SMBlueprint::LoadAutomatic(value, f_pos, f_quat);
			if (!blueprint) continue;

			context.pPrefab->addObject(blueprint);
		}
	}

	return stream.Index() - v_idx_start;
}

int PrefabFileReader::ReadPrefabs(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;

	if (!TileConverterSettings::ExportPrefabs)
	{
		stream.Move(header.hasPrefabs);
		return header.hasPrefabs;
	}

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_prefabs", 100);
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
		v_transform.scale = (context.prefabVersion < 5) ? glm::vec3(1.0f) : stream.ReadVec3();

		stream.ReadInt();
		stream.ReadInt();

		if (context.counterMode)
		{
			PrefabFileReader::Read(v_pref_path, L"", v_transform, *context.pError, context.counterMode);
			if (*context.pError) return 0;
		}
		else
		{
			SMPrefab* rec_prefab = PrefabFileReader::Read(v_pref_path, L"", v_transform, *context.pError, context.counterMode);
			if (*context.pError) return 0;
			if (!rec_prefab) continue;

			context.pPrefab->addObject(rec_prefab);
		}
	}

	return stream.Index() - v_start_idx;
}

void PrefabFileReader::ReadNodes(
	PrefabReaderContext& context,
	SMPrefab* prefab,
	int count)
{
	BitStream& stream = *context.pBitStream;

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

int PrefabFileReader::ReadAssets(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;

	if (!TileConverterSettings::ExportAssets)
	{
		stream.Move(header.hasAssets);
		return header.hasAssets;
	}

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_assets", 100);
	const int v_idx_start = stream.Index();

	SMEntityTransform v_transform;
	for (int a = 0; a < header.assetCount; a++)
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

		if (context.prefabVersion >= 10)
			stream.ReadByte();

		const AssetData* v_pAssetData = SMMod::GetGlobalObject<AssetData>(uuid);

		if (context.counterMode)
		{
			ItemModStats::CountPart((v_pAssetData != nullptr) ? v_pAssetData->m_uuid : SMUuid::Null, false);
		}
		else
		{
			if (!v_pAssetData) continue;

			Model* pModel = ModelStorage::LoadModel(v_pAssetData->m_mesh);
			if (!pModel) continue;

			SMAsset* nAsset = new SMAsset(v_pAssetData, v_transform, pModel, std::move(color_map));
			context.pPrefab->addObject(nAsset);
		}
	}

	return stream.Index() - v_idx_start;
}

int PrefabFileReader::ReadDecals(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;

	if (!TileConverterSettings::ExportDecals)
	{
		stream.Move(header.hasDecals);
		return header.hasDecals;
	}

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_decals", 100);
	const int v_start_idx = stream.Index();

	SMEntityTransform v_transform;
	for (int a = 0; a < header.decalsCount; a++)
	{
		v_transform.position = stream.ReadVec3();
		v_transform.rotation = stream.ReadQuat();
		v_transform.scale = stream.ReadVec3();

		const SMUuid v_uuid = stream.ReadUuid();
		const SMColor v_color = stream.ReadInt();

		//Read a random 4 byte value
		stream.ReadInt();

		const DecalData* v_pDecalData = SMMod::GetGlobalObject<DecalData>(v_uuid);

		if (context.counterMode)
		{
			ItemModStats::CountPart((v_pDecalData != nullptr) ? v_pDecalData->m_uuid : SMUuid::Null, false);
		}
		else
		{
			if (!v_pDecalData) continue;

			SMDecal* v_newDecal = new SMDecal(v_pDecalData, v_transform, v_color);
			context.pPrefab->addObject(v_newDecal);
		}
	}

	return stream.Index() - v_start_idx;
}

int PrefabFileReader::Read_248(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_248", 100);
	DebugWarningL("UNIMPLEMENTED -> ", stream.Index());

	stream.Move(header.has_0x5c);
	return header.has_0x5c;
}

int PrefabFileReader::ReadKinematics(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;

	if (!TileConverterSettings::ExportKinematics)
	{
		stream.Move(header.hasBlueprints);
		return header.hasBlueprints;
	}

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_kinematics", 5000);
	const int v_start_idx = stream.Index();

	SMEntityTransform v_transform;
	for (int a = 0; a < header.kinematicsCount; a++)
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

		const KinematicData* v_pKmData = SMMod::GetGlobalObject<KinematicData>(v_uuid);

		if (context.counterMode)
		{
			ItemModStats::CountPart((v_pKmData != nullptr) ? v_pKmData->m_uuid : SMUuid::Null, false);
		}
		else
		{
			if (!v_pKmData) continue;

			Model* v_km_model = ModelStorage::LoadModel(v_pKmData->m_mesh);
			if (!v_km_model) continue;

			context.pPrefab->addObject(
				new SMKinematic(
					v_pKmData,
					v_transform,
					v_km_model,
					v_color
				)
			);
		}
	}

	return stream.Index() - v_start_idx;
}

int PrefabFileReader::Read_2(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_2", 100);
	DebugWarningL("UNIMPLEMENTED -> ", stream.Index());

	stream.Move(header.has_0x7c);
	return header.has_0x7c;
}