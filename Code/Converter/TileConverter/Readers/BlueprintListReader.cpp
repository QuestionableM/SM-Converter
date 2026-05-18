#include "Converter/ConvertSettings.hpp"
#include "BlueprintListReader.hpp"

SM_UNMANAGED_CODE

void BlueprintListReader::Read(TileReaderContext& context)
{
	if (*context.pError) return;

	if (!context.counterMode && !TileConverterSettings::ExportBlueprints)
		return;

	if (context.pCellHeader->blueprintListCount == 0 || context.pCellHeader->blueprintListIndex == 0) return;

	DebugOutL(
		"BlueprintList: ", context.pCellHeader->blueprintListSize,
		" / ", context.pCellHeader->blueprintListCompressedSize);

	if (!context.pReader->hasEnoughSpace(
		context.pCellHeader->blueprintListIndex,
		context.pCellHeader->blueprintListCompressedSize))
	{
		DebugErrorL("Not enough space!");
		return;
	}

	std::vector<Byte> v_bytes(context.pCellHeader->blueprintListSize);
	int debugSize = Lz4::DecompressFast(
		reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->blueprintListIndex)),
		reinterpret_cast<char*>(v_bytes.data()),
		context.pCellHeader->blueprintListSize);

	if (debugSize != context.pCellHeader->blueprintListCompressedSize)
	{
		DebugErrorL("DebugSize: ", debugSize, ", header->blueprintListCompressedSize: ", context.pCellHeader->blueprintListCompressedSize);
		context.pError->setError(1, "BlueprintListReader::Read -> debugSize != header->blueprintListCompressedSize\nTile Version: ", context.tileVersion);
		return;
	}

	debugSize = BlueprintListReader::Read(context, v_bytes, context.pCellHeader->blueprintListCount);
	if (debugSize != context.pCellHeader->blueprintListSize)
	{
		DebugErrorL("DebugSize: ", debugSize, ", header->blueprintListSize: ", context.pCellHeader->blueprintListSize);
		context.pError->setError(1, "BlueprintListReader::Read -> debugSize != header->blueprintListSize\nTile Version: ", context.tileVersion);
		return;
	}
}

int BlueprintListReader::Read(
	TileReaderContext& context,
	const std::vector<Byte>& bytes,
	int count)
{
	MemoryWrapper memory(bytes);

	int index = 0;
	for (int a = 0; a < count; a++)
	{
		const glm::vec3 f_pos = memory.Object<glm::vec3>(index);
		const glm::quat f_quat = memory.GetQuat(index + 0xc);
		index += 0x1c;

		std::string value;
		if (context.tileVersion < 7)
		{
			const int val_len = (int)memory.Object<Byte>(index) & 0xff;
			index++;

			value = memory.String(index, val_len);
			index += val_len;
		}
		else
		{
			const int val_len = memory.Object<int>(index);
			index += 4;

			value = memory.String(index, val_len);
			index += val_len;
		}

		if (context.tileVersion >= 13)
		{
			//Skip a random null byte that was added in the newest versions of tiles
			index++;
		}

		if (context.counterMode)
		{
			SMBlueprint::LoadAndCountAutomatic(value);
		}
		else
		{
			SMBlueprint* pNewBlueprint = SMBlueprint::LoadAutomatic(value, f_pos, f_quat);
			if (!pNewBlueprint) continue;

			context.pTilePart->AddObject(pNewBlueprint);
		}
	}

	return index;
}