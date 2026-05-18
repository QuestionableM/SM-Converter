#include "Converter/ConvertSettings.hpp"
#include "DecalListReader.hpp"

SM_UNMANAGED_CODE

void DecalListReader::Read(TileReaderContext& context)
{
	if (*context.pError) return;

	if (!context.counterMode
		&& !TileConverterSettings::ExportDecals)
	{
		return;
	}

	if (context.pCellHeader->decalCount == 0) return;

	DebugOutL("Decal: ", context.pCellHeader->decalSize, " / ", context.pCellHeader->decalCompressedSize);

	if (!context.pReader->hasEnoughSpace(
		context.pCellHeader->decalIndex,
		context.pCellHeader->decalCompressedSize))
	{
		DebugErrorL("Not enough space!");
		return;
	}

	std::vector<Byte> v_bytes(context.pCellHeader->decalSize);
	int debugSize = Lz4::DecompressFast(
		reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->decalIndex)),
		reinterpret_cast<char*>(v_bytes.data()),
		context.pCellHeader->decalSize);

	if (debugSize != context.pCellHeader->decalCompressedSize)
	{
		DebugErrorL("Debug Size: ", debugSize, ", header->decalCompressedSize: ", context.pCellHeader->decalCompressedSize);
		context.pError->setError(1, "DecalListReader::Read -> debugSize != header->decalCompressedSize\nTile Version: ", context.tileVersion);
		return;
	}

	debugSize = DecalListReader::Read(context, v_bytes, context.pCellHeader->decalCount);
	if (debugSize != context.pCellHeader->decalSize)
	{
		DebugErrorL("Debug Size: ", debugSize, ", header->decalSize: ", context.pCellHeader->decalSize);
		context.pError->setError(1, "DecalListReader::Read -> debugSize != header->decalSize\nTile Version: ", context.tileVersion);
		return;
	}
}

int DecalListReader::Read(
	TileReaderContext& context,
	const std::vector<Byte>& bytes,
	int decal_count)
{
	SMEntityTransform v_transform;
	MemoryWrapper memory(bytes);

	int index = 0;
	for (int a = 0; a < decal_count; a++)
	{
		v_transform.position = memory.Object<glm::vec3>(index); //12 bytes
		v_transform.rotation = memory.GetQuat(index + 0xc); //16 bytes
		v_transform.scale = memory.Object<glm::vec3>(index + 0x1c); //12 bytes

		const SMUuid v_uuid = memory.Object<SMUuid>(index + 0x28); //16 bytes
		const SMColor v_color = memory.Object<unsigned int>(index + 0x38); //4 bytes
		//A random value that is 4 bytes (skipped)

		//Final increment is 64
		if (context.tileVersion >= 12) {
			//Skip a random byte that was added in the newest versions of tiles
			index += 0x41;
		}
		else {
			index += 0x40;
		}

		const DecalData* v_pDecalData = SMMod::GetGlobalObject<DecalData>(v_uuid);

		if (context.counterMode)
		{
			ItemModStats::CountPart((v_pDecalData != nullptr) ? v_pDecalData->m_uuid : SMUuid::Null, false);
		}
		else
		{
			if (!v_pDecalData) continue;

			SMDecal* v_newDecal = new SMDecal(v_pDecalData, v_transform, v_color);
			context.pTilePart->AddObject(v_newDecal);
		}
	}

	return index;
}