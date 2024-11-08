#pragma once

#include "Converter/TileConverter\CellHeader.hpp"
#include "Converter/TileConverter\Tile.hpp"
#include "Converter/Entity/Asset.hpp"

#include "ObjectDatabase/UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/String.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class AssetListReader
{
	AssetListReader() = default;

public:
	template<bool t_mod_counter>
	static void Read(
		const CellHeader& header,
		MemoryWrapper& reader,
		TilePart* pPart,
		std::uint32_t version,
		ConvertError& error)
	{
		if (error) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportAssets) return;
		}

		for (int a = 0; a < 4; a++)
		{
			const int assetListCompressedSize = header.assetListCompressedSize[a];
			const int assetListSize = header.assetListSize[a];
			const int assetListCount = header.assetListCount[a];
			const int assetListIndex = header.assetListIndex[a];

			if (assetListCount == 0)
				continue;
			
			DebugOutL("Asset[", a, "]: ", header.assetListSize[a], ", ", header.assetListCompressedSize[a]);

			if (!reader.hasEnoughSpace(assetListIndex, assetListCompressedSize))
			{
				DebugErrorL("Not enough space!");
				continue;
			}

			std::vector<Byte> v_bytes(assetListSize);
			int debugSize = Lz4::DecompressFast(
				reinterpret_cast<const char*>(reader.getPointer(header.assetListIndex[a])),
				reinterpret_cast<char*>(v_bytes.data()),
				assetListSize);

			if (debugSize != assetListCompressedSize)
			{
				DebugErrorL("Debug Size:", debugSize, ", assetListCompressedSize: ", assetListCompressedSize);
				error.setError(1, "AssetListReader::Read -> debugSize != assetListCompressedSize\nTile Version: ", version);
				return;
			}

			debugSize = AssetListReader::Read<t_mod_counter>(v_bytes, a, assetListCount, version, pPart);
			if (debugSize != assetListSize)
			{
				DebugErrorL("Debug Size: ", debugSize, ", assetListSize: ", assetListSize);
				error.setError(1, "AssetListReader::Read -> debugSize != assetListSize\nTile Version: ", version);
				return;
			}
		}
	}

	template<bool t_mod_counter>
	static int Read(
		const std::vector<Byte>& bytes,
		int asset_idx,
		int len,
		std::uint32_t version,
		TilePart* pPart)
	{
		SMEntityTransform v_transform;
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < len; a++)
		{
			v_transform.position = memory.Object<glm::vec3>(index);
			v_transform.rotation = memory.GetQuat(index + 0xc);

			if (version < 5)
			{
				const float f_dimension = memory.Object<float>(index + 0x1c);
				v_transform.scale = glm::vec3(f_dimension);

				index += 0x20;
			}
			else
			{
				v_transform.scale = memory.Object<glm::vec3>(index + 0x1c);

				index += 0x28;
			}

			SMUuid f_uuid;

			if (version < 4)
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

			std::unordered_map<std::string, SMColor, Hashing::StringHasher, std::equal_to<>> color_map;

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
						color_map.emplace(v_str_data, color);
				}
			}

			if (version >= 12)
			{
				//Skip one byte that was added in the newest version of .tile files
				index++;
			}

			const AssetData* asset_data = SMMod::GetGlobalObject<AssetData>(f_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((asset_data != nullptr) ? asset_data->m_mod : nullptr);
			}
			else
			{
				if (!asset_data) continue;

				Model* pModel = ModelStorage::LoadModel(asset_data->m_mesh);
				if (!pModel) continue;

				SMAsset* pNewAsset = new SMAsset(asset_data, v_transform, pModel, std::move(color_map));
				pPart->AddObject(pNewAsset, asset_idx);
			}
		}

		return index;
	}
};

SM_MANAGED_CODE