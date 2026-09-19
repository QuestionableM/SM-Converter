#include "Converter/ConvertSettings.hpp"
#include "BlueprintListReader.hpp"
#include "Utils/Memory.hpp"

SM_UNMANAGED_CODE

#include <lz4/lz4.h>

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
	int debugSize = LZ4_decompress_safe(
		reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->blueprintListIndex)),
		reinterpret_cast<char*>(v_bytes.data()),
		context.pCellHeader->blueprintListCompressedSize,
		context.pCellHeader->blueprintListSize);
	if (debugSize != context.pCellHeader->blueprintListSize)
	{
		DebugErrorL("DebugSize: ", debugSize, ", header->blueprintListSize: ", context.pCellHeader->blueprintListSize);
		context.pError->setError(1, "BlueprintListReader::Read -> debugSize != header->blueprintListSize\nTile Version: ", context.tileVersion);
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
	std::ofstream v_output("./dumps/bp_list_data.bin", std::ios::binary);
	if (v_output.is_open())
	{
		v_output.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
		v_output.close();
	}

	MemoryWrapper memory(bytes);

	std::uint32_t v_idx = 0;
	for (int a = 0; a < count; a++)
	{
		const glm::vec3 f_pos = memory.Object<glm::vec3>(v_idx);
		const glm::quat f_quat = memory.GetQuat(v_idx + 0xc);
		v_idx += 0x1c;

		std::string v_bpData;
		if (context.tileVersion < 7)
		{
			const std::uint8_t v_bpDataLen = memory.Object<std::uint8_t>(v_idx);
			v_idx++;

			v_bpData = memory.String(v_idx, v_bpDataLen);
			v_idx += v_bpDataLen;
		}
		else
		{
			const std::uint32_t v_bpDataLen = memory.Object<std::uint32_t>(v_idx);
			v_idx += 4;

			v_bpData = memory.String(v_idx, v_bpDataLen);
			v_idx += v_bpDataLen;
		}

		// Skip a random null byte that was added in the newest versions of tiles
		if (context.tileVersion >= 13)
			v_idx++;

		// Skip 8 more random bytes
		if (context.tileVersion >= 15)
			v_idx += 8;

		if (context.counterMode)
		{
			SMBlueprint::LoadAndCountAutomatic(v_bpData);
		}
		else
		{
			SMBlueprint* pNewBlueprint = SMBlueprint::LoadAutomatic(v_bpData, f_pos, f_quat);
			if (!pNewBlueprint) continue;

			context.pTilePart->AddObject(pNewBlueprint);
		}
	}

	return v_idx;
}