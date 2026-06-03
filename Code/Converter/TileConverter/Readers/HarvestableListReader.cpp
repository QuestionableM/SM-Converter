#include "HarvestableListReader.hpp"

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"
#include "Converter/TileConverter/Tile.hpp"
#include "Converter/Entity/Harvestable.hpp"
#include "Converter/ConvertSettings.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/Console.hpp"
#include "Utils/Memory.hpp"

SM_UNMANAGED_CODE

#include <lz4/lz4.h>

void HarvestableListReader::Read(TileReaderContext& context)
{
	if (*context.pError) return;

	if (!context.counterMode
		&& !TileConverterSettings::ExportHarvestables)
	{
		return;
	}

	for (int a = 0; a < 4; a++)
	{
		const int v_hvsListCompressedSz = context.pCellHeader->harvestableListCompressedSize[a];
		const int v_hvsListSz           = context.pCellHeader->harvestableListSize[a];
		const int v_hvsListCount        = context.pCellHeader->harvestableListCount[a];

		if (v_hvsListCount == 0)
			continue;

		DebugOutL("Harvestable[", a, "]: ", v_hvsListSz, ", ", v_hvsListCompressedSz);

		if (!context.pReader->hasEnoughSpace(
			context.pCellHeader->harvestableListIndex[a],
			v_hvsListCompressedSz))
		{
			DebugErrorL("Not enough space!");
			continue;
		}

		std::vector<Byte> v_bytes(v_hvsListSz);
		int debugSize = LZ4_decompress_safe(
			reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->harvestableListIndex[a])),
			reinterpret_cast<char*>(v_bytes.data()),
			v_hvsListCompressedSz,
			v_hvsListSz);
		if (debugSize != v_hvsListSz)
		{
			DebugErrorL("debugSize: ", debugSize, ", v_hvsListSz: ", v_hvsListSz);
			context.pError->setError(1, "HarvestableListReader::Read -> debugSize != v_hvsListSz\nTile Version: ", context.tileVersion);
			return;
		}

		debugSize = HarvestableListReader::Read(context, v_bytes, a, v_hvsListCount);
		if (debugSize != v_hvsListSz)
		{
			DebugErrorL("debugSize: ", debugSize, ", v_hvsListSz: ", v_hvsListSz);
			context.pError->setError(1, "HarvestableListReader::Read -> debugSize != v_hvsListSz\nTile Version: ", context.tileVersion);
			return;
		}
	}
}

int HarvestableListReader::Read(
	TileReaderContext& context,
	const std::vector<Byte>& bytes,
	int hvs_index,
	int len)
{
	SMEntityTransform v_transform;
	MemoryWrapper memory(bytes);

	int index = 0;
	for (int a = 0; a < len; a++)
	{
		v_transform.position = memory.Object<glm::vec3>(index); //12
		v_transform.rotation = memory.GetQuat(index + 0xc); //16
		v_transform.scale = memory.Object<glm::vec3>(index + 0x1c); //12

		const SMUuid f_uuid = memory.Object<SMUuid>(index + 0x28); //16
		const SMColor f_color = memory.Object<unsigned int>(index + 0x38); //4

		index += 0x3c;

		//VERSION 9 no null bytes
		//VERSION 11 has 4 null bytes
		//VERSION 12 seems to have 5 null bytes
		//VERSION 13 also has 5 null bytes
		if (context.tileVersion >= 12) { index += 0x5; }
		else if (context.tileVersion >= 11) { index += 0x4; }

		const HarvestableData* v_pHvsData = SMMod::GetGlobalObject<HarvestableData>(f_uuid);

		if (context.counterMode)
		{
			ItemModStats::CountPart((v_pHvsData != nullptr) ? v_pHvsData->m_uuid : SMUuid::Null, false);
		}
		else
		{
			if (!v_pHvsData) continue;

			Model* hvs_model = ModelStorage::LoadModel(v_pHvsData->m_mesh);
			if (!hvs_model) continue;

			SMHarvestable* pNewHvs = new SMHarvestable(v_pHvsData, v_transform, hvs_model, f_color);
			context.pTilePart->AddObject(pNewHvs, hvs_index);
		}
	}

	return index;
}