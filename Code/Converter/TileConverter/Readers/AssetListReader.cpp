#include "Converter/ConvertSettings.hpp"
#include "AssetListReader.hpp"

SM_UNMANAGED_CODE

void AssetListReader::Read(TileReaderContext& context)
{
	if (*context.pError) return;

	if (!context.counterMode && !TileConverterSettings::ExportAssets)
		return;

	for (int a = 0; a < 4; a++)
	{
		const int assetListCompressedSize = context.pCellHeader->assetListCompressedSize[a];
		const int assetListSize           = context.pCellHeader->assetListSize[a];
		const int assetListCount          = context.pCellHeader->assetListCount[a];
		const int assetListIndex          = context.pCellHeader->assetListIndex[a];

		if (assetListCount == 0)
			continue;

		DebugOutL("Asset[", a, "]: ", context.pCellHeader->assetListSize[a], ", ", context.pCellHeader->assetListCompressedSize[a]);

		if (!context.pReader->hasEnoughSpace(assetListIndex, assetListCompressedSize))
		{
			DebugErrorL("Not enough space!");
			continue;
		}

		std::vector<Byte> v_bytes(assetListSize);
		int debugSize = Lz4::DecompressFast(
			reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->assetListIndex[a])),
			reinterpret_cast<char*>(v_bytes.data()),
			assetListSize);

		if (debugSize != assetListCompressedSize)
		{
			DebugErrorL("Debug Size:", debugSize, ", assetListCompressedSize: ", assetListCompressedSize);
			context.pError->setError(1, "AssetListReader::Read -> debugSize != assetListCompressedSize\nTile Version: ", context.tileVersion);
			return;
		}

		debugSize = AssetListReader::Read(context, v_bytes, a, assetListCount);
		if (debugSize != assetListSize)
		{
			DebugErrorL("Debug Size: ", debugSize, ", assetListSize: ", assetListSize);
			context.pError->setError(1, "AssetListReader::Read -> debugSize != assetListSize\nTile Version: ", context.tileVersion);
			return;
		}
	}
}

int AssetListReader::Read(
	TileReaderContext& context,
	const std::vector<Byte>& bytes,
	int asset_idx,
	int len)
{
	SMEntityTransform v_transform;
	MemoryWrapper memory(bytes);

	int index = 0;
	for (int a = 0; a < len; a++)
	{
		v_transform.position = memory.Object<glm::vec3>(index);
		v_transform.rotation = memory.GetQuat(index + 0xc);

		if (context.tileVersion < 5)
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

		if (context.tileVersion < 4)
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

		if (context.tileVersion >= 12)
		{
			//Skip one byte that was added in the newest version of .tile files
			index++;
		}

		const AssetData* v_pAssetData = SMMod::GetGlobalObject<AssetData>(f_uuid);

		if (context.counterMode)
		{
			ItemModStats::CountPart((v_pAssetData != nullptr) ? v_pAssetData->m_uuid : SMUuid::Null, false);
		}
		else
		{
			if (!v_pAssetData) continue;

			Model* pModel = ModelStorage::LoadModel(v_pAssetData->m_mesh);
			if (!pModel) continue;

			SMAsset* pNewAsset = new SMAsset(v_pAssetData, v_transform, pModel, std::move(color_map));
			context.pTilePart->AddObject(pNewAsset, asset_idx);
		}
	}

	return index;
}