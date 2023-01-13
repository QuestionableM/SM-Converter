#pragma once

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\Tile.hpp"
#include "Converter\Entity\Asset.hpp"

#include "ObjectDatabase\ObjectDatabase.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"
#include "Utils\String.hpp"

#include <lz4\lz4.h>

class AssetListReader
{
	AssetListReader() = default;

public:

#pragma warning(push)
#pragma warning(disable : 4996)

	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError || !TileConverterSettings::ExportAssets) return;

		for (int a = 0; a < 4; a++)
		{
			const int assetListCompressedSize = header->assetListCompressedSize[a];
			const int assetListSize = header->assetListSize[a];

			if (header->assetListCount[a] != 0)
			{
				DebugOutL("Asset[", a, "]: ", header->assetListSize[a], ", ", header->assetListCompressedSize[a]);
				const int assetListIndex = header->assetListIndex[a];

				const std::vector<Byte> compressed = reader.Objects<Byte>(assetListIndex, assetListCompressedSize);

				std::vector<Byte> bytes = {};
				bytes.resize(assetListSize);

				int debugSize = LZ4_decompress_fast(reinterpret_cast<const char*>(compressed.data()),
					reinterpret_cast<char*>(bytes.data()), assetListSize);
				if (debugSize != assetListCompressedSize)
				{
					cError = ConvertError(1, L"AssetListReader::Read -> debugSize != assetListCompressedSize");
					return;
				}

				debugSize = AssetListReader::Read(bytes, a, header->assetListCount[a], part->GetParent()->GetVersion(), part);
				DebugOutL(0b0111_fg, "Debug Size: ", debugSize, ", AssetListSize: ", assetListSize);
				if (debugSize != assetListSize)
				{
					cError = ConvertError(1, L"AssetListReader::Read -> debugSize != assetListSize");
					return;
				}
			}
		}
	}

#pragma warning(pop)

	static int Read(const std::vector<Byte>& bytes, const int& asset_idx, const int& len, const int& version, TilePart* part)
	{
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < len; a++)
		{
			const glm::vec3 f_pos = memory.Object<glm::vec3>(index);
			const glm::quat f_quat = memory.GetQuat(index + 0xc);
			glm::vec3 f_size;

			if (version < 5)
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

			std::unordered_map<std::wstring, SMColor> color_map;

			int bVar4 = (int)memory.Object<Byte>(index++) & 0xff;
			if (bVar4 != 0)
			{
				const int length = bVar4;

				for (int j = 0; j < length; j++)
				{
					bVar4 = (int)memory.Object<Byte>(index++) & 0xff;
					const std::vector<char> str_vec = memory.Objects<char>(index, bVar4);

					const std::wstring wstr_data = String::ToWide(std::string(str_vec.begin(), str_vec.end()));
					index += bVar4;
					const unsigned int color = memory.Object<unsigned int>(index);
					index += 4;

					if (color_map.find(wstr_data) == color_map.end())
						color_map.insert(std::make_pair(wstr_data, color));
				}
			}

			if (version >= 13)
			{
				//Skip one byte that was added in the newest version of .tile files
				index++;
			}

			AssetData* asset_data = Mod::GetGlobalAsset(f_uuid);
			if (!asset_data) continue;

			Model* pModel = ModelStorage::LoadModel(asset_data->Mesh);
			if (!pModel) continue;

			Asset* pNewAsset = new Asset(asset_data, pModel, color_map);
			pNewAsset->SetPosition(f_pos);
			pNewAsset->SetRotation(f_quat);
			pNewAsset->SetSize(f_size);

			part->AddObject(pNewAsset, asset_idx);
		}

		return index;
	}
};