#pragma once

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TilePart.hpp"
#include "Converter\TileConverter\Tile.hpp"
#include "Converter\Entity\Decal.hpp"

#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Memory.hpp"
#include "Utils\Lz4Lib.hpp"

#pragma unmanaged

class DecalListReader
{
	DecalListReader() = default;

public:
	inline static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError || !TileConverterSettings::ExportDecals) return;

		if (header->decalCount != 0)
		{
			DebugOutL("Decal: ", header->decalSize, " / ", header->decalCompressedSize);

			const std::vector<Byte> compressed = reader.Objects<Byte>(header->decalIndex, header->decalCompressedSize);

			std::vector<Byte> bytes = {};
			bytes.resize(header->decalSize);

			int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
				reinterpret_cast<char*>(bytes.data()), header->decalSize);
			if (debugSize != header->decalCompressedSize)
			{
				cError = ConvertError(1, L"DecalListReader::Read -> debugSize != header->decalCompressedSize");
				return;
			}

			debugSize = DecalListReader::Read(bytes, header->decalCount, part);
			DebugOutL(0b0111_fg, "Debug Size: ", debugSize, ", DecalSize: ", header->decalSize);
			if (debugSize != header->decalSize)
			{
				cError = ConvertError(1, L"DecalListReader::Read -> debugSize != header->decalSize");
				return;
			}
		}
	}

	inline static int Read(const std::vector<Byte>& bytes, const int& decal_count, TilePart* part)
	{
		const int version = part->GetParent()->GetVersion();

		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < decal_count; a++)
		{
			const glm::vec3 v_pos = memory.Object<glm::vec3>(index);
			const glm::quat v_quat = memory.GetQuat(index + 0xc);
			const glm::vec3 v_size = memory.Object<glm::vec3>(index + 0x1c);
			index += 0x28;

			const SMUuid v_uuid = memory.Object<SMUuid>(index);
			index += 0x10;

			const SMColor v_color = memory.Object<unsigned int>(index);
			index += 0x4;

			//Some value that takes 4 bytes
			index += 0x4;

			//TODO: Verify on version 12 tiles
			if (version >= 12)
			{
				//Skip a random byte that was added in the newest versions of tiles
				index++;
			}

			const DecalData* v_decalData = Mod::GetGlobalDecal(v_uuid);
			if (!v_decalData) continue;

			SMDecal* v_newDecal = new SMDecal(v_decalData, v_color);
			v_newDecal->SetPosition(v_pos);
			v_newDecal->SetRotation(v_quat);
			v_newDecal->SetSize(v_size);

			part->AddObject(v_newDecal);
		}

		return index;
	}
};

#pragma managed