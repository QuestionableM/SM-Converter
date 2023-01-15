#pragma once

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TilePart.hpp"
#include "Converter\TileConverter\Tile.hpp"

#include "Utils\Memory.hpp"

#include <lz4\lz4.h>

class BlueprintListReader
{
	BlueprintListReader() = default;
public:

#pragma warning(push)
#pragma warning(disable : 4996)

	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError || !TileConverterSettings::ExportBlueprints) return;
		if (header->blueprintListCount == 0 || header->blueprintListIndex == 0) return;

		DebugOutL("BlueprintList: ", header->blueprintListSize, " / ", header->blueprintListCompressedSize);

		std::vector<Byte> compressed = reader.Objects<Byte>(header->blueprintListIndex, header->blueprintListCompressedSize);

		std::vector<Byte> bytes;
		bytes.resize(header->blueprintListSize);

		int debugSize = LZ4_decompress_fast((char*)compressed.data(), (char*)bytes.data(), header->blueprintListSize);
		if (debugSize != header->blueprintListCompressedSize)
		{
			cError = ConvertError(1, L"BlueprintListReader::Read -> debugSize != header->blueprintListCompressedSize");
			return;
		}

		debugSize = BlueprintListReader::Read(bytes, header->blueprintListCount, part);
		if (debugSize != header->blueprintListSize)
		{
			cError = ConvertError(1, L"BlueprintListReader::Read -> debugSize != header->blueprintListSize");
			return;
		}
	}

#pragma warning(pop)

	static int Read(const std::vector<Byte>& bytes, const int& count, TilePart* part)
	{
		const int version = part->GetParent()->GetVersion();
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < count; a++)
		{
			const glm::vec3 f_pos = memory.Object<glm::vec3>(index);
			const glm::quat f_quat = memory.GetQuat(index + 0xc);
			index += 0x1c;

			std::string value;
			if (version < 7)
			{
				const int val_len = (int)memory.Object<Byte>(index) & 0xff;
				index++;

				std::vector<char> value_data = memory.Objects<char>(index, val_len);
				index += val_len;

				value = std::string(value_data.begin(), value_data.end());
			}
			else
			{
				const int val_len = memory.Object<int>(index);
				index += 4;

				std::vector<char> value_data = memory.Objects<char>(index, val_len);
				index += val_len;

				value = std::string(value_data.begin(), value_data.end());
			}

			if (version >= 13)
			{
				//Skip a random null byte that was added in the newest versions of tiles
				index++;
			}

			SMBlueprint* pNewBlueprint = SMBlueprint::LoadAutomatic(value);
			if (!pNewBlueprint) continue;

			pNewBlueprint->SetPosition(f_pos);
			pNewBlueprint->SetRotation(f_quat);

			part->AddObject(pNewBlueprint);
		}

		return index;
	}
};