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
	static void Read(
		const CellHeader& header,
		MemoryWrapper& reader,
		TilePart* pPart,
		std::uint32_t version,
		ConvertError& error)
	{
		if (error) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportBlueprints) return;
		}

		if (header.blueprintListCount == 0 || header.blueprintListIndex == 0) return;

		DebugOutL("BlueprintList: ", header.blueprintListSize, " / ", header.blueprintListCompressedSize);

		if (!reader.hasEnoughSpace(header.blueprintListIndex, header.blueprintListCompressedSize))
		{
			DebugErrorL("Not enough space!");
			return;
		}
		
		std::vector<Byte> v_bytes(header.blueprintListSize);
		int debugSize = Lz4::DecompressFast(
			reinterpret_cast<const char*>(reader.getPointer(header.blueprintListIndex)),
			reinterpret_cast<char*>(v_bytes.data()),
			header.blueprintListSize);

		if (debugSize != header.blueprintListCompressedSize)
		{
			DebugErrorL("DebugSize: ", debugSize, ", header->blueprintListCompressedSize: ", header.blueprintListCompressedSize);
			error.setError(1, "BlueprintListReader::Read -> debugSize != header->blueprintListCompressedSize\nTile Version: ", version);
			return;
		}

		debugSize = BlueprintListReader::Read<t_mod_counter>(v_bytes, header.blueprintListCount, version, pPart);
		if (debugSize != header.blueprintListSize)
		{
			DebugErrorL("DebugSize: ", debugSize, ", header->blueprintListSize: ", header.blueprintListSize);
			error.setError(1, "BlueprintListReader::Read -> debugSize != header->blueprintListSize\nTile Version: ", version);
			return;
		}
	}

	template<bool t_mod_counter>
	static int Read(
		const std::vector<Byte>& bytes,
		int count,
		std::uint32_t version,
		TilePart* pPart)
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

				pPart->AddObject(pNewBlueprint);
			}
		}

		return index;
	}
};

SM_MANAGED_CODE