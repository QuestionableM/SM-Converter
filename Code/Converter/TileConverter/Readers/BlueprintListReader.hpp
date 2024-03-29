#pragma once

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"
#include "Converter/TileConverter/Tile.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class BlueprintListReader
{
	BlueprintListReader() = default;

public:
	template<bool t_mod_counter>
	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, int version, ConvertError& cError)
	{
		if (cError) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportBlueprints) return;
		}

		if (header->blueprintListCount == 0 || header->blueprintListIndex == 0) return;

		DebugOutL("BlueprintList: ", header->blueprintListSize, " / ", header->blueprintListCompressedSize);

		std::vector<Byte> compressed = reader.Objects<Byte>(header->blueprintListIndex, header->blueprintListCompressedSize);
		std::vector<Byte> bytes(header->blueprintListSize);

		int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
			reinterpret_cast<char*>(bytes.data()), header->blueprintListSize);
		if (debugSize != header->blueprintListCompressedSize)
		{
			DebugErrorL("DebugSize: ", debugSize, ", header->blueprintListCompressedSize: ", header->blueprintListCompressedSize);
			cError = ConvertError(1, L"BlueprintListReader::Read -> debugSize != header->blueprintListCompressedSize\nTile Version: " + std::to_wstring(version));
			return;
		}

		debugSize = BlueprintListReader::Read<t_mod_counter>(bytes, header->blueprintListCount, version, part);
		if (debugSize != header->blueprintListSize)
		{
			DebugErrorL("DebugSize: ", debugSize, ", header->blueprintListSize: ", header->blueprintListSize);
			cError = ConvertError(1, L"BlueprintListReader::Read -> debugSize != header->blueprintListSize\nTile Version: " + std::to_wstring(version));
			return;
		}
	}

	template<bool t_mod_counter>
	static int Read(const std::vector<Byte>& bytes, int count, int version, TilePart* part)
	{
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

				value = memory.String(index, val_len);
				index += val_len;
			}
			else
			{
				const int val_len = memory.Object<int>(index);
				index += 4;

				value = memory.String(index, val_len);
				index += val_len;
			}

			if (version >= 13)
			{
				//Skip a random null byte that was added in the newest versions of tiles
				index++;
			}

			if constexpr (t_mod_counter)
			{
				SMBlueprint::LoadAndCountAutomatic(value);
			}
			else
			{
				SMBlueprint* pNewBlueprint = SMBlueprint::LoadAutomatic(value, f_pos, f_quat);
				if (!pNewBlueprint) continue;

				part->AddObject(pNewBlueprint);
			}
		}

		return index;
	}
};

SM_MANAGED_CODE