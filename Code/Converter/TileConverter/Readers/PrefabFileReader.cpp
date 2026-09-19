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

	std::ofstream v_outputDump("./dumps/prefab_dump.bin", std::ios::binary);
	if (v_outputDump.is_open())
	{
		v_outputDump.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
		v_outputDump.close();
	}

	if (reader.NextObject<std::uint32_t>() != 0x50524546)
	{
		DebugErrorL("Invalid Prefab Magic");
		return nullptr;
	}

	const std::uint32_t version = reader.NextObject<std::uint32_t, true>();
	if (version <= 0 || version > 13)
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

	if (v_header.blueprintStream.count != 0 && TileConverterSettings::ExportBlueprints)
	{
		const std::uint32_t v_blueprintBytes = PrefabFileReader::ReadBlueprints(context, v_header);
		THROW_PREFAB_ERROR(v_blueprintBytes != v_header.blueprintStream.bitSize, context.pError);
	}

	if (v_header.prefabStream.count != 0 && TileConverterSettings::ExportPrefabs)
	{
		const std::uint32_t v_prefabBytes = PrefabFileReader::ReadPrefabs(context, v_header);
		if (*context.pError) return;

		THROW_PREFAB_ERROR(v_prefabBytes != v_header.prefabStream.bitSize, context.pError);
	}

	if (v_header.assetStream.count != 0 && TileConverterSettings::ExportAssets)
	{
		const std::uint32_t v_assetBytes = PrefabFileReader::ReadAssets(context, v_header);
		THROW_PREFAB_ERROR(v_assetBytes != v_header.assetStream.bitSize, context.pError);
	}

	if (v_header.decalStream.count != 0 && TileConverterSettings::ExportDecals)
	{
		const std::uint32_t v_decalBytes = PrefabFileReader::ReadDecals(context, v_header);
		THROW_PREFAB_ERROR(v_decalBytes != v_header.decalStream.bitSize, context.pError);
	}

	if (v_header.kinematicsStream.count != 0 && TileConverterSettings::ExportKinematics)
	{
		const std::uint32_t v_kinematicBytes = PrefabFileReader::ReadKinematics(context, v_header);
		THROW_PREFAB_ERROR(v_kinematicBytes != v_header.kinematicsStream.bitSize, context.pError);
	}
}

std::uint32_t PrefabFileReader::ReadBlueprints(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;
	stream.SetIndex(header.blueprintStream.bitOffset);

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_blueprints", 100);
	const std::uint32_t v_idxStart = stream.Index();

	for (std::uint32_t a = 0; a < header.blueprintStream.count; a++)
	{
		const std::uint32_t v_bpDataLen = stream.ReadU32();
		const std::string v_bpData = stream.ReadString(v_bpDataLen);

		const glm::vec3 v_bpPosition = stream.ReadVec3();
		const glm::quat v_bpRotation = stream.ReadQuat();

		// Skip some integer
		stream.Move(4 * 8);

		// A random byte was added in the version 10 of prefabs
		if (context.prefabVersion >= 10)
			stream.Move(1 * 8);

		// Skip another 4 null bytes introduced in the newest prefab version
		if (context.prefabVersion >= 13)
			stream.Move(4 * 8);

		if (context.counterMode)
		{
			SMBlueprint::LoadAndCountAutomatic(v_bpData);
		}
		else
		{
			SMBlueprint* blueprint = SMBlueprint::LoadAutomatic(v_bpData, v_bpPosition, v_bpRotation);
			if (!blueprint) continue;

			context.pPrefab->addObject(blueprint);
		}
	}

	return stream.Index() - v_idxStart;
}

std::uint32_t PrefabFileReader::ReadPrefabs(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;
	stream.SetIndex(header.prefabStream.bitOffset);

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_prefabs", 100);
	const std::uint32_t v_startIdx = stream.Index();

	SMEntityTransform v_transform;
	for (std::uint32_t a = 0; a < header.prefabStream.count; a++)
	{
		const int v_prefPathLen = stream.ReadInt();
		std::wstring v_prefPath = String::ToWide(stream.ReadString(v_prefPathLen));
		KeywordReplacer::ReplaceKeyR(v_prefPath);

		DebugOutL(0b1011_fg, "Recursive Prefab Path: ", v_prefPath);

		v_transform.position = stream.ReadVec3();
		v_transform.rotation = stream.ReadQuat();
		v_transform.scale = (context.prefabVersion < 5) ? glm::vec3(1.0f) : stream.ReadVec3();

		stream.ReadInt();
		stream.ReadInt();

		if (context.counterMode)
		{
			PrefabFileReader::Read(v_prefPath, L"", v_transform, *context.pError, context.counterMode);
			if (*context.pError) return 0;
		}
		else
		{
			SMPrefab* v_pRecPrefab = PrefabFileReader::Read(v_prefPath, L"", v_transform, *context.pError, context.counterMode);
			if (*context.pError) return 0;
			if (!v_pRecPrefab) continue;

			context.pPrefab->addObject(v_pRecPrefab);
		}
	}

	return stream.Index() - v_startIdx;
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

std::uint32_t PrefabFileReader::ReadAssets(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;
	stream.SetIndex(header.assetStream.bitOffset);

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_assets", 100);
	const std::uint32_t v_idxStart = stream.Index();

	SMEntityTransform v_transform;
	for (std::uint32_t a = 0; a < header.assetStream.count; a++)
	{
		v_transform.position = stream.ReadVec3();
		v_transform.rotation = stream.ReadQuat();
		v_transform.scale = stream.ReadVec3();

		const SMUuid v_uuid = stream.ReadUuid();
		const std::uint32_t v_materialCount = stream.ReadU8();

		std::unordered_map<std::string, SMColor, Hashing::StringHasher, std::equal_to<>> v_colorMap;

		if (v_materialCount != 0)
		{
			for (std::uint32_t b = 0; b < v_materialCount; b++)
			{
				const std::uint32_t v_matNameLen = stream.ReadU8();
				const std::string v_matName = stream.ReadString(v_matNameLen);
				const std::uint32_t color = stream.ReadU32();

				if (v_colorMap.find(v_matName) == v_colorMap.end())
					v_colorMap.emplace(std::move(v_matName), color);
			}
		}

		if (context.prefabVersion >= 10)
			stream.ReadByte();

		const AssetData* v_pAssetData = SMMod::GetGlobalObject<AssetData>(v_uuid);

		if (context.counterMode)
		{
			ItemModStats::CountPart((v_pAssetData != nullptr) ? v_pAssetData->m_uuid : SMUuid::Null, false);
		}
		else
		{
			if (!v_pAssetData) continue;

			Model* pModel = ModelStorage::LoadModel(v_pAssetData->m_mesh);
			if (!pModel) continue;

			SMAsset* nAsset = new SMAsset(v_pAssetData, v_transform, pModel, std::move(v_colorMap));
			context.pPrefab->addObject(nAsset);
		}
	}

	return stream.Index() - v_idxStart;
}

std::uint32_t PrefabFileReader::ReadDecals(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;
	stream.SetIndex(header.decalStream.bitOffset);

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_decals", 100);
	const std::uint32_t v_startIdx = stream.Index();

	SMEntityTransform v_transform;
	for (std::uint32_t a = 0; a < header.decalStream.count; a++)
	{
		v_transform.position = stream.ReadVec3();
		v_transform.rotation = stream.ReadQuat();
		v_transform.scale = stream.ReadVec3();

		const SMUuid v_uuid = stream.ReadUuid();
		const SMColor v_color = stream.ReadInt();
		// Skip a random 4 byte value
		stream.Move(4 * 8);

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

	return stream.Index() - v_startIdx;
}

std::uint32_t PrefabFileReader::ReadKinematics(
	PrefabReaderContext& context,
	PrefabHeader& header)
{
	BitStream& stream = *context.pBitStream;
	stream.SetIndex(header.kinematicsStream.bitOffset);

	BIT_STREAM_DUMP_INTO_FILE(stream, L"./prefab_dump/prefab" + std::to_wstring(context.prefabVersion) + L"_kinematics", 5000);
	const std::uint32_t v_startIdx = stream.Index();

	SMEntityTransform v_transform;
	for (std::uint32_t a = 0; a < header.kinematicsStream.count; a++)
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

	return stream.Index() - v_startIdx;
}