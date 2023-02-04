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
	template<bool t_mod_counter, int t_tile_version>
	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportHarvestables) return;
		}

		for (int a = 0; a < 4; a++)
		{
			const int v_hvs_list_compressed_sz = header->harvestableListCompressedSize[a];
			const int v_hvs_list_sz = header->harvestableListSize[a];
			const int v_hvs_list_count = header->harvestableListCount[a];

			if (v_hvs_list_count == 0)
				continue;

			DebugOutL("Harvestable[", a, "]: ", v_hvs_list_sz, ", ", v_hvs_list_compressed_sz);

			const std::vector<Byte> compressed = reader.Objects<Byte>(header->harvestableListIndex[a], v_hvs_list_compressed_sz);
			std::vector<Byte> bytes(v_hvs_list_sz);

			int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
				reinterpret_cast<char*>(bytes.data()), v_hvs_list_sz);
			if (debugSize != v_hvs_list_compressed_sz)
			{
				DebugErrorL("debugSize: ", debugSize, ", v_hvs_list_compressed_sz: ", v_hvs_list_compressed_sz);
				cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != v_hvs_list_compressed_sz\nTile Version: " + std::to_wstring(t_tile_version));
				return;
			}

			debugSize = HarvestableListReader::Read<t_mod_counter, t_tile_version>(bytes, a, v_hvs_list_count, part);
			if (debugSize != v_hvs_list_sz)
			{
				DebugErrorL("debugSize: ", debugSize, ", v_hvs_list_sz: ", v_hvs_list_sz);
				cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != v_hvs_list_sz\nTile Version: " + std::to_wstring(t_tile_version));
				return;
			}
		}
	}

	template<bool t_mod_counter, int t_tile_version>
	static int Read(const std::vector<Byte>& bytes, const int& hvs_index, const int& len, TilePart* part)
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
			if constexpr (t_tile_version >= 12) { index += 0x5; }
			else if constexpr (t_tile_version >= 11) { index += 0x4; }

			HarvestableData* hvs_data = SMMod::GetGlobalObject<HarvestableData>(f_uuid);

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