#pragma once

#include "Converter\Entity\Harvestable.hpp"
#include "Converter\ConvertSettings.hpp"
#include "Converter\ConvertError.hpp"

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TilePart.hpp"
#include "Converter\TileConverter\Tile.hpp"

#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\Memory.hpp"

#include <lz4\lz4.h>

class HarvestableListReader
{
	HarvestableListReader() = default;

public:

#pragma warning(push)
#pragma warning(disable : 4996)

	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError || !TileConverterSettings::ExportHarvestables) return;

		for (int a = 0; a < 4; a++)
		{
			const int harvestableListCompressedSize = header->harvestableListCompressedSize[a];
			const int harvestableListSize = header->harvestableListSize[a];

			if (header->harvestableListCount[a] != 0)
			{
				DebugOutL("Harvestable[", a, "]: ", harvestableListSize, ", ", harvestableListCompressedSize);

				const std::vector<Byte> compressed = reader.Objects<Byte>(header->harvestableListIndex[a], harvestableListCompressedSize);

				std::vector<Byte> bytes = {};
				bytes.resize(harvestableListSize);

				int debugSize = LZ4_decompress_fast(reinterpret_cast<const char*>(compressed.data()),
					reinterpret_cast<char*>(bytes.data()), header->harvestableListSize[a]);
				if (debugSize != harvestableListCompressedSize)
				{
					cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != harvestableListCompressedSize");
					return;
				}

				debugSize = HarvestableListReader::Read(bytes, a, header->harvestableListCount[a], part->GetParent()->GetVersion(), part);
				if (debugSize != header->harvestableListSize[a])
				{
					cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != header->harvestableListSize[a]");
					return;
				}
			}
		}
	}

#pragma warning(pop)

	static int Read(const std::vector<Byte>& bytes, const int& hvs_index, const int& len, const int& version, TilePart* part)
	{
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < len; a++)
		{
			const glm::vec3 f_pos = memory.Object<glm::vec3>(index); //12
			const glm::quat f_quat = memory.GetQuat(index + 0xc); //16
			const glm::vec3 f_size = memory.Object<glm::vec3>(index + 0x1c); //12

			const SMUuid f_uuid = memory.Object<SMUuid>(index + 0x28); //16
			const SMColor f_color = memory.Object<unsigned int>(index + 0x38); //4

			index += 0x3c;

			if (version >= 13)
			{
				//Skip 5 unknown bytes that were added in the newest version of tiles
				index += 0x5;
			}

			HarvestableData* hvs_data = Mod::GetGlobalHarvestbale(f_uuid);
			if (!hvs_data) continue;

			Model* hvs_model = ModelStorage::LoadModel(hvs_data->m_mesh);
			if (!hvs_model) continue;

			SMHarvestable* pNewHvs = new SMHarvestable(hvs_data, hvs_model, f_color);
			pNewHvs->SetPosition(f_pos);
			pNewHvs->SetRotation(f_quat);
			pNewHvs->SetSize(f_size);

			part->AddObject(pNewHvs, hvs_index);
		}

		return index;
	}
};