#include "Converter/ConvertSettings.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"
#include "Utils/Uuid.hpp"

#include "ClutterReader.hpp"

SM_UNMANAGED_CODE

#include <lz4/lz4.h>

void ClutterReader::Read(TileReaderContext& context)
{
	if (*context.pError || !TileConverterSettings::ExportClutter) return;

	std::vector<Byte> v_bytes;
	if (!ClutterReader::Read(context, v_bytes)) return;

	ClutterReader::Read(v_bytes, context.pTilePart);
}

bool ClutterReader::Read(
	TileReaderContext& context,
	std::vector<Byte>& outBytes)
{
	DebugOutL(
		"Clutter: ", context.pCellHeader->clutterCompressedSize,
		" ", context.pCellHeader->clutterSize);

	if (!context.pReader->hasEnoughSpace(
		context.pCellHeader->clutterIndex,
		context.pCellHeader->clutterCompressedSize))
	{
		DebugErrorL("Not enough space!");
		return false;
	}

	std::vector<Byte> v_bytes(context.pCellHeader->clutterSize);
	const int debugSize = LZ4_decompress_safe(
		reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->clutterIndex)),
		reinterpret_cast<char*>(v_bytes.data()),
		context.pCellHeader->clutterCompressedSize,
		context.pCellHeader->clutterSize);
	if (debugSize != context.pCellHeader->clutterSize)
	{
		DebugErrorL("DebugSize: ", debugSize, ", header->clutterSize: ", context.pCellHeader->clutterSize);
		context.pError->setError(1, "ClutterReader::Read -> debugSize != header->clutterSize");
		return false;
	}

	outBytes = std::move(v_bytes);
	return true;
}

void ClutterReader::Read(
	const std::vector<Byte>& bytes,
	TilePart* pPart)
{
	MemoryWrapper memory(bytes);

	const Byte first_byte = memory.Object<Byte>(0);
	if (first_byte != 0)
	{
		const std::size_t length = (std::size_t)((int)first_byte & 0xff);
		std::size_t offset = 2;

		for (std::size_t i = 0; i < length; i++)
		{
			int uVar7 = 0;

			for (std::size_t j = 0; j < 0x10; j++)
			{
				const int read = (int)memory.Object<char>(offset + j);

				const int a = (uVar7 ^ read);
				const int b = 0x9e3779b9;
				const int c = (uVar7 << 6);
				const int d = (uVar7 >> 2);

				uVar7 = a + b + c + d;
			}

			const SMUuid clutter_uuid = memory.Object<SMUuid>(offset);
			const ClutterData* v_clutter_data = SMMod::GetGlobalObject<ClutterData>(clutter_uuid);
			if (v_clutter_data)
			{
				DebugOutL(0b1101_fg, "Clutter: ", clutter_uuid.toString(), " -> ", uVar7);
			}
			else
			{
				DebugErrorL("The specified clutter doesn't exist: ", clutter_uuid.toString());
			}

			/*
			int iVar8 = 0;
			if(iVar8 != 1) {
				//Log("TerrainGrass", "false && \"Uuid dosen't exist in the clutter manager, defaulting to 0.\"", 149);
			} else {
				// FUN_009b8120(&local_64,local_3c,(byte *)((int)&bytes + 2));
			}
			*/

			offset += 0x11;
		}

		memory.Set(offset - 2);
	}


	const std::size_t offset = memory.Index();
	for (std::size_t a = 0; a < 0x4000; a++)
	{
		const SignedByte v_clutter_byte = memory.Object<SignedByte>(1 + a + offset);
		pPart->m_clutter[a] = v_clutter_byte;

		if (v_clutter_byte < 0) continue;

		ClutterData* v_clutter_data = SMMod::GetGlobalClutterById(v_clutter_byte);
		if (!v_clutter_data) continue;

		Model* v_clutter_model = ModelStorage::LoadModel(v_clutter_data->m_mesh);
		if (!v_clutter_model) continue;

		pPart->m_clutterMap[a] = new SMTileClutter(v_clutter_data, v_clutter_model);
	}
}