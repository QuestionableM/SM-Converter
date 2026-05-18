#include "Converter/ConvertSettings.hpp"
#include "PrefabReader.hpp"

SM_UNMANAGED_CODE

void PrefabReader::Read(TileReaderContext& context)
{
	if (*context.pError) return;

	if (!context.counterMode && !TileConverterSettings::ExportPrefabs)
		return;

	if (context.pCellHeader->prefabCount == 0 || context.pCellHeader->prefabIndex == 0) return;

	DebugOutL("Prefab: ", context.pCellHeader->prefabSize, " / ", context.pCellHeader->prefabCompressedSize);

	if (!context.pReader->hasEnoughSpace(
		context.pCellHeader->prefabIndex,
		context.pCellHeader->prefabCompressedSize))
	{
		DebugErrorL("Not enough space!");
		return;
	}

	std::vector<Byte> v_bytes(context.pCellHeader->prefabSize);
	int debugSize = Lz4::DecompressFast(
		reinterpret_cast<const char*>(context.pReader->getPointer(context.pCellHeader->prefabIndex)),
		reinterpret_cast<char*>(v_bytes.data()),
		context.pCellHeader->prefabSize);

	if (debugSize != context.pCellHeader->prefabCompressedSize)
	{
		DebugErrorL("DebugSize: ", debugSize, ", header->prefabCompressedSize: ", context.pCellHeader->prefabCompressedSize);
		context.pError->setError(1, "PrefabReader::Read -> debugSize != header->prefabCompressedSize\nTile Version: ", context.tileVersion);
		return;
	}

	debugSize = PrefabReader::Read(context, v_bytes, context.pCellHeader->prefabCount);
	if (debugSize != context.pCellHeader->prefabSize)
	{
		if (*context.pError) return;

		DebugErrorL("DebugSize: ", debugSize, ", header->prefabSize: ", context.pCellHeader->prefabSize);
		context.pError->setError(1, "PrefabReader::Read -> debugSize != header->prefabSize\nTile Version: ", context.tileVersion);
		return;
	}
}

int PrefabReader::Read(
	TileReaderContext& context,
	const std::vector<Byte>& bytes,
	int prefabCount)
{
	SMEntityTransform v_transform;
	MemoryWrapper memory(bytes);

	int index = 0;
	for (int a = 0; a < prefabCount; a++)
	{
		v_transform.position = memory.Object<glm::vec3>(index);
		v_transform.rotation = memory.GetQuat(index + 0xc);

		if (context.tileVersion < 9)
		{
			v_transform.scale = glm::vec3(1.0f);
			index += 0x1c;
		}
		else
		{
			v_transform.scale = memory.Object<glm::vec3>(index + 0x1c);
			index += 0x28;
		}

		const int string_length = memory.Object<int>(index);
		index += 4;
		const std::string v_path = memory.String(index, string_length);
		index += string_length;

		const int bVar2 = (int)memory.Object<Byte>(index) & 0xff;
		index += 1;
		const std::string v_flag = memory.String(index, bVar2);
		index += bVar2;
		index += 4;

		std::wstring v_pref_path = String::ToWide(v_path);
		KeywordReplacer::ReplaceKeyR(v_pref_path);

		const std::wstring v_pref_flag = String::ToWide(v_flag);
		DebugOutL("Prefab Path: ", v_pref_path);

		SMPrefab* v_new_prefab = PrefabFileReader::Read(v_pref_path, v_pref_flag, v_transform, *context.pError, context.counterMode);
		if (*context.pError) return 0;
		if (context.counterMode || !v_new_prefab) continue;

		context.pTilePart->AddObject(v_new_prefab);
	}

	return index;
}