#pragma once

#include "Converter\Entity\Harvestable.hpp"
#include "Converter\ConvertSettings.hpp"
#include "Converter\ConvertError.hpp"

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TilePart.hpp"
#include "Converter\TileConverter\Tile.hpp"

#include "ObjectDatabase\UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\Memory.hpp"
#include "Utils\Lz4Lib.hpp"

#pragma unmanaged

class HarvestableListReader
{
	HarvestableListReader() = default;

public:
	template<bool t_mod_counter>
	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportHarvestables) return;
		}

		for (int a = 0; a < 4; a++)
		{
			const int harvestableListCompressedSize = header->harvestableListCompressedSize[a];
			const int harvestableListSize = header->harvestableListSize[a];

			if (header->harvestableListCount[a] == 0)
				continue;

			DebugOutL("Harvestable[", a, "]: ", harvestableListSize, ", ", harvestableListCompressedSize);

			const std::vector<Byte> compressed = reader.Objects<Byte>(header->harvestableListIndex[a], harvestableListCompressedSize);

			std::vector<Byte> bytes = {};
			bytes.resize(harvestableListSize);

			const int v_tile_version = part->GetParent()->GetVersion();

			int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
				reinterpret_cast<char*>(bytes.data()), header->harvestableListSize[a]);
			if (debugSize != harvestableListCompressedSize)
			{
				DebugErrorL("debugSize: ", debugSize, ", harvestableListCompressedSize: ", harvestableListCompressedSize);
				cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != harvestableListCompressedSize\nTile Version: " + std::to_wstring(v_tile_version));
				return;
			}

			debugSize = HarvestableListReader::Read<t_mod_counter>(bytes, a, header->harvestableListCount[a], v_tile_version, part);
			if (debugSize != header->harvestableListSize[a])
			{
				DebugErrorL("debugSize: ", debugSize, ", header->harvestableListSize[", a, "]: ", header->harvestableListSize[a]);
				cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != header->harvestableListSize[a]\nTile Version: " + std::to_wstring(v_tile_version));
				return;
			}
		}
	}

	template<bool t_mod_counter>
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

			//VERSION 9 no null bytes
			//VERSION 11 has 4 null bytes
			//VERSION 12 seems to have 5 null bytes
			//VERSION 13 also has 5 null bytes
			if (version >= 11) index += 0x4;
			if (version >= 12) index += 0x1;

			HarvestableData* hvs_data = SMMod::GetGlobalHarvestbale(f_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((hvs_data != nullptr) ? hvs_data->m_mod : nullptr);
			}
			else
			{
				if (!hvs_data) continue;

				Model* hvs_model = ModelStorage::LoadModel(hvs_data->m_mesh);
				if (!hvs_model) continue;

				SMHarvestable* pNewHvs = new SMHarvestable(hvs_data, hvs_model, f_color);
				pNewHvs->SetPosition(f_pos);
				pNewHvs->SetRotation(f_quat);
				pNewHvs->SetSize(f_size);

				part->AddObject(pNewHvs, hvs_index);
			}
		}

		return index;
	}
};

#pragma managed