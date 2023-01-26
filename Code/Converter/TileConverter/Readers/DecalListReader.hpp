#pragma once

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TilePart.hpp"
#include "Converter\TileConverter\Tile.hpp"
#include "Converter\Entity\Decal.hpp"

#include "ObjectDatabase\UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Memory.hpp"
#include "Utils\Lz4Lib.hpp"

#pragma unmanaged

class DecalListReader
{
	DecalListReader() = default;

public:
	template<bool t_mod_counter, int t_tile_version>
	inline static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& cError)
	{
		if (cError) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportDecals) return;
		}

		if (header->decalCount == 0) return;

		DebugOutL("Decal: ", header->decalSize, " / ", header->decalCompressedSize);

		const std::vector<Byte> compressed = reader.Objects<Byte>(header->decalIndex, header->decalCompressedSize);
		std::vector<Byte> bytes(header->decalSize);

		int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
			reinterpret_cast<char*>(bytes.data()), header->decalSize);
		if (debugSize != header->decalCompressedSize)
		{
			DebugErrorL("Debug Size: ", debugSize, ", header->decalCompressedSize: ", header->decalCompressedSize);
			cError = ConvertError(1, L"DecalListReader::Read -> debugSize != header->decalCompressedSize\nTile Version: " + std::to_wstring(t_tile_version));
			return;
		}

		debugSize = DecalListReader::Read<t_mod_counter, t_tile_version>(bytes, header->decalCount, part);
		if (debugSize != header->decalSize)
		{
			DebugErrorL("Debug Size: ", debugSize, ", header->decalSize: ", header->decalSize);
			cError = ConvertError(1, L"DecalListReader::Read -> debugSize != header->decalSize\nTile Version: " + std::to_wstring(t_tile_version));
			return;
		}
	}

	template<bool t_mod_counter, int t_tile_version>
	inline static int Read(const std::vector<Byte>& bytes, const int& decal_count, TilePart* part)
	{
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < decal_count; a++)
		{
			const glm::vec3 v_pos = memory.Object<glm::vec3>(index); //12 bytes
			const glm::quat v_quat = memory.GetQuat(index + 0xc); //16 bytes
			const glm::vec3 v_size = memory.Object<glm::vec3>(index + 0x1c); //12 bytes

			const SMUuid v_uuid = memory.Object<SMUuid>(index + 0x28); //16 bytes
			const SMColor v_color = memory.Object<unsigned int>(index + 0x38); //4 bytes
			//A random value that is 4 bytes (skipped)

			//Final increment is 64
			if constexpr (t_tile_version >= 12) {
				//Skip a random byte that was added in the newest versions of tiles
				index += 0x41;
			} else {
				index += 0x40;
			}

			const DecalData* v_decalData = SMMod::GetGlobalDecal(v_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((v_decalData != nullptr) ? v_decalData->m_mod : nullptr);
			}
			else
			{
				if (!v_decalData) continue;

				SMDecal* v_newDecal = new SMDecal(v_decalData, v_color);
				v_newDecal->SetPosition(v_pos);
				v_newDecal->SetRotation(v_quat);
				v_newDecal->SetSize(v_size);

				part->AddObject(v_newDecal);
			}
		}

		return index;
	}
};

#pragma managed