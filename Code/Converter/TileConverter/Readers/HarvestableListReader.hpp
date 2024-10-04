#pragma once

#include "Converter/Entity/Harvestable.hpp"
#include "Converter/ConvertSettings.hpp"
#include "Converter/ConvertError.hpp"

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"
#include "Converter/TileConverter/Tile.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Console.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class HarvestableListReader
{
	HarvestableListReader() = default;

public:
	template<bool t_mod_counter>
	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, int version, ConvertError& cError)
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

			if (!reader.hasEnoughSpace(header->harvestableListIndex[a], v_hvs_list_compressed_sz))
			{
				DebugErrorL("Not enough space!");
				continue;
			}

			std::vector<Byte> v_bytes(v_hvs_list_sz);
			int debugSize = Lz4::DecompressFast(
				reinterpret_cast<const char*>(reader.getPointer(header->harvestableListIndex[a])),
				reinterpret_cast<char*>(v_bytes.data()),
				v_hvs_list_sz);

			if (debugSize != v_hvs_list_compressed_sz)
			{
				DebugErrorL("debugSize: ", debugSize, ", v_hvs_list_compressed_sz: ", v_hvs_list_compressed_sz);
				cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != v_hvs_list_compressed_sz\nTile Version: " + std::to_wstring(version));
				return;
			}

			debugSize = HarvestableListReader::Read<t_mod_counter>(v_bytes, a, v_hvs_list_count, version, part);
			if (debugSize != v_hvs_list_sz)
			{
				DebugErrorL("debugSize: ", debugSize, ", v_hvs_list_sz: ", v_hvs_list_sz);
				cError = ConvertError(1, L"HarvestableListReader::Read -> debugSize != v_hvs_list_sz\nTile Version: " + std::to_wstring(version));
				return;
			}
		}
	}

	template<bool t_mod_counter>
	static int Read(const std::vector<Byte>& bytes, int hvs_index, int len, int version, TilePart* part)
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
			if (version >= 12) { index += 0x5; }
			else if (version >= 11) { index += 0x4; }

			const HarvestableData* hvs_data = SMMod::GetGlobalObject<HarvestableData>(f_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((hvs_data != nullptr) ? hvs_data->m_mod : nullptr);
			}
			else
			{
				if (!hvs_data) continue;

				Model* hvs_model = ModelStorage::LoadModel(hvs_data->m_mesh);
				if (!hvs_model) continue;

				SMHarvestable* pNewHvs = new SMHarvestable(hvs_data, v_transform, hvs_model, f_color);
				part->AddObject(pNewHvs, hvs_index);
			}
		}

		return index;
	}
};

SM_MANAGED_CODE