#pragma once

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\Tile.hpp"
#include "Converter\Entity\Asset.hpp"

#include "ObjectDatabase\UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase\ObjectDatabase.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\String.hpp"
#include "Utils\Lz4Lib.hpp"

#pragma unmanaged

class AssetListReader
{
	AssetListReader() = default;

public:
	template<bool t_mod_counter, int t_tile_version>
	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportAssets) return;
		}

		for (int a = 0; a < 4; a++)
		{
			const int assetListCompressedSize = header->assetListCompressedSize[a];
			const int assetListSize = header->assetListSize[a];
			const int assetListCount = header->assetListCount[a];

			if (assetListCount == 0)
				continue;
			
			DebugOutL("Asset[", a, "]: ", header->assetListSize[a], ", ", header->assetListCompressedSize[a]);

			const std::vector<Byte> compressed = reader.Objects<Byte>(header->assetListIndex[a], assetListCompressedSize);
			std::vector<Byte> bytes(assetListSize);

			int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
				reinterpret_cast<char*>(bytes.data()), assetListSize);
			if (debugSize != assetListCompressedSize)
			{
				DebugErrorL("Debug Size:", debugSize, ", assetListCompressedSize: ", assetListCompressedSize);
				cError = ConvertError(1, L"AssetListReader::Read -> debugSize != assetListCompressedSize\nTile Version: " + std::to_wstring(t_tile_version));
				return;
			}

			debugSize = AssetListReader::Read<t_mod_counter, t_tile_version>(bytes, a, assetListCount, part);
			if (debugSize != assetListSize)
			{
				DebugErrorL("Debug Size: ", debugSize, ", assetListSize: ", assetListSize);
				cError = ConvertError(1, L"AssetListReader::Read -> debugSize != assetListSize\nTile Version: " + std::to_wstring(t_tile_version));
				return;
			}
		}
	}

	template<bool t_mod_counter, int t_tile_version>
	static int Read(const std::vector<Byte>& bytes, const int& asset_idx, const int& len, TilePart* part)
	{
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < len; a++)
		{
			const glm::vec3 f_pos = memory.Object<glm::vec3>(index);
			const glm::quat f_quat = memory.GetQuat(index + 0xc);
			glm::vec3 f_size;

			if constexpr (t_tile_version < 5)
			{
				const float f_dimension = memory.Object<float>(index + 0x1c);
				f_size = glm::vec3(f_dimension);

				index += 0x20;
			}
			else
			{
				f_size = memory.Object<glm::vec3>(index + 0x1c);

				index += 0x28;
			}

			SMUuid f_uuid;

			if constexpr (t_tile_version < 4)
			{
				// int bVar4 = memory.Byte(index++);
				// String str = memory.String(bVar4, index);
				// System.out.printf("  ReadString: [%s]\n", str);

				// TODO: ????
			}
			else
			{
				f_uuid = memory.Object<SMUuid>(index);
				index += 0x10;
			}

			std::unordered_map<std::string, SMColor> color_map;

			int bVar4 = (int)memory.Object<Byte>(index++) & 0xff;
			if (bVar4 != 0)
			{
				const int length = bVar4;

				for (int j = 0; j < length; j++)
				{
					bVar4 = (int)memory.Object<Byte>(index++) & 0xff;
					const std::string v_str_data = memory.String(index, bVar4);
					index += bVar4;

					const unsigned int color = memory.Object<unsigned int>(index);
					index += 4;

					if (color_map.find(v_str_data) == color_map.end())
						color_map.insert(std::make_pair(v_str_data, color));
				}
			}

			if constexpr (t_tile_version >= 12)
			{
				//Skip one byte that was added in the newest version of .tile files
				index++;
			}

			AssetData* asset_data = SMMod::GetGlobalAsset(f_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((asset_data != nullptr) ? asset_data->m_mod : nullptr);
			}
			else
			{
				if (!asset_data) continue;

				Model* pModel = ModelStorage::LoadModel(asset_data->m_mesh);
				if (!pModel) continue;

				SMAsset* pNewAsset = new SMAsset(asset_data, pModel, color_map);
				pNewAsset->SetPosition(f_pos);
				pNewAsset->SetRotation(f_quat);
				pNewAsset->SetSize(f_size);

				part->AddObject(pNewAsset, asset_idx);
			}
		}

		return index;
	}
};

#pragma managed