#pragma once

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TilePart.hpp"

#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Uuid.hpp"
#include "Utils\Lz4Lib.hpp"

#pragma unmanaged

class ClutterReader
{
	ClutterReader() = default;

public:
	inline static void Read(CellHeader* header, MemoryWrapper& memory, TilePart* part, ConvertError& cError)
	{
		if (cError || !TileConverterSettings::ExportClutter) return;

		const std::vector<Byte> bytes = ClutterReader::Read(header, memory, cError);
		if (bytes.empty()) return;

		ClutterReader::Read(bytes, part);
	}

	inline static std::vector<Byte> Read(CellHeader* header, MemoryWrapper& memory, ConvertError& cError)
	{
		DebugOutL("Clutter: ", header->clutterCompressedSize, " ", header->clutterSize);

		const std::vector<Byte> compressed = memory.Objects<Byte>(header->clutterIndex, header->clutterCompressedSize);
		if (compressed.empty())
			return {};

		std::vector<Byte> bytes = {};
		bytes.resize(header->clutterSize);

		const int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
			reinterpret_cast<char*>(bytes.data()), header->clutterSize);
		DebugOutL("DebugSize: ", debugSize, ", ClutterCompressedSize: ", header->clutterCompressedSize);
		if (debugSize != header->clutterCompressedSize)
		{
			cError = ConvertError(1, L"ClutterReader::Read -> debugSize != header->clutterCompressedSize");
			return {};
		}

		return bytes;
	}

	inline static void Read(const std::vector<Byte>& bytes, TilePart* part)
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
				const ClutterData* pData = SMMod::GetGlobalClutter(clutter_uuid);
				if (pData)
				{
					DebugOutL(0b1101_fg, "Clutter: ", clutter_uuid.ToString(), " -> ", uVar7);
				}
				else
				{
					DebugErrorL("The specified clutter doesn't exist: ", clutter_uuid.ToString());
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
			const SignedByte cur_byte = memory.Object<SignedByte>(1 + a + offset);
			part->m_Clutter[a] = cur_byte;

			if (cur_byte < 0) continue;

			ClutterData* clData = SMMod::GetGlobalClutterById(cur_byte);
			if (!clData) continue;

			Model* pModel = ModelStorage::LoadModel(clData->m_mesh);
			if (!pModel) continue;

			part->m_ClutterMap[a] = new SMTileClutter(clData, pModel);
		}
	}
};

#pragma managed