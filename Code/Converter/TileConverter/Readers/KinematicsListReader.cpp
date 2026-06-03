#include "KinematicsListReader.hpp"

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/Tile.hpp"
#include "Converter/Entity/Kinematic.hpp"
#include "Converter/ConvertSettings.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/BitStream.hpp"
#include "Utils/String.hpp"

SM_UNMANAGED_CODE

#include <lz4/lz4.h>

void KinematicsListReader::Read(TileReaderContext& context)
{
	if (*context.pError) return;

	if (!context.counterMode
		&& !TileConverterSettings::ExportKinematics)
	{
		return;
	}

	for (int a = 0; a < 4; a++)
	{
		const int kinematicsListCompressedSize = context.pCellHeader->kinematicsListCompressedSize[a];
		const int kinematicsListSize           = context.pCellHeader->kinematicsListSize[a];
		const int kinematicsListCount          = context.pCellHeader->kinematicsListCount[a];

		if (kinematicsListCount == 0)
			continue;

		DebugOutL("Kinematics[", a, "]: ", kinematicsListSize, ", ", kinematicsListCompressedSize);

		if (!context.pReader->hasEnoughSpace(
			context.pCellHeader->kinematicsListIndex[a],
			kinematicsListCompressedSize))
		{
			DebugErrorL("Not enough space!");
			continue;
		}

		std::vector<Byte> v_bytes(kinematicsListSize);
		int debugSize = LZ4_decompress_safe(
			reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->kinematicsListIndex[a])),
			reinterpret_cast<char*>(v_bytes.data()),
			kinematicsListCompressedSize,
			kinematicsListSize);
		if (debugSize != kinematicsListSize)
		{
			DebugErrorL("Debug Size: ", debugSize, ", kinematicsListSize: ", kinematicsListSize);
			context.pError->setError(1, "KinematicsListReader::Read -> debugSize != kinematicsListSize\nTile Version: ", context.tileVersion);
			return;
		}

		debugSize = KinematicsListReader::Read(context, v_bytes, a, kinematicsListCount);
		if (debugSize != kinematicsListSize)
		{
			DebugErrorL("Debug Size: ", debugSize, ", kinematicsListSize: ", kinematicsListSize);
			context.pError->setError(1, "KinematicsListReader::Read -> debugSize != kinematicsListSize\nTile Version: ", context.tileVersion);
			return;
		}
	}
}

int KinematicsListReader::Read(
	TileReaderContext& context,
	const std::vector<Byte>& bytes,
	int kinematic_idx,
	int len)
{
	SMEntityTransform v_transform;
	MemoryWrapper v_memory(bytes);
	MEMORY_WRAPPER_DUMP_BYTES(v_memory, "./kinematics_dump", 0);

	int index = 0;
	for (int a = 0; a < len; a++)
	{
		v_transform.position = v_memory.Object<glm::vec3>(index);
		v_transform.rotation = v_memory.GetQuat(index + 0xc);
		v_transform.scale = v_memory.Object<glm::vec3>(index + 0x1c);

		const SMUuid v_uuid = v_memory.Object<SMUuid>(index + 0x28);
		const SMColor v_color = v_memory.Object<unsigned int>(index + 0x38);

		index += 0x3c;

		//It looks like lua strings always end with 0x9, so iterate until 0x9 is found
		while (1)
		{
			if (v_memory.Object<Byte>(index++) == 0x9)
				break;
		}

		const int v_json_str_sz = v_memory.Object<int, true>(index);
		index += 0x4;
		//const std::string v_json_str = v_memory.String(index, v_json_str_sz);
		index += v_json_str_sz;

		//DebugOutL("Json String: ", v_json_str);
		index += 0x1; //NULL BYTE

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

			SMKinematic* v_new_kinematic = new SMKinematic(v_pKmData, v_transform, v_km_model, v_color);
			context.pTilePart->AddObject(v_new_kinematic, kinematic_idx);
		}
	}

	return index;
}