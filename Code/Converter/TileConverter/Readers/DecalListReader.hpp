#pragma once

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"
#include "Converter/TileConverter/Tile.hpp"
#include "Converter/Entity/Decal.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class DecalListReader
{
	DecalListReader() = default;

public:
	template<bool t_mod_counter>
	inline static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, int version, ConvertError& cError)
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
			cError = ConvertError(1, L"DecalListReader::Read -> debugSize != header->decalCompressedSize\nTile Version: " + std::to_wstring(version));
			return;
		}

		debugSize = DecalListReader::Read<t_mod_counter>(bytes, header->decalCount, version, part);
		if (debugSize != header->decalSize)
		{
			DebugErrorL("Debug Size: ", debugSize, ", header->decalSize: ", header->decalSize);
			cError = ConvertError(1, L"DecalListReader::Read -> debugSize != header->decalSize\nTile Version: " + std::to_wstring(version));
			return;
		}
	}

	template<bool t_mod_counter>
	inline static int Read(const std::vector<Byte>& bytes, int decal_count, int version, TilePart* part)
	{
		SMEntityTransform v_transform;
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < decal_count; a++)
		{
			v_transform.position = memory.Object<glm::vec3>(index); //12 bytes
			v_transform.rotation = memory.GetQuat(index + 0xc); //16 bytes
			v_transform.scale = memory.Object<glm::vec3>(index + 0x1c); //12 bytes

			const SMUuid v_uuid = memory.Object<SMUuid>(index + 0x28); //16 bytes
			const SMColor v_color = memory.Object<unsigned int>(index + 0x38); //4 bytes
			//A random value that is 4 bytes (skipped)

			//Final increment is 64
			if (version >= 12) {
				//Skip a random byte that was added in the newest versions of tiles
				index += 0x41;
			} else {
				index += 0x40;
			}

			const DecalData* v_decalData = SMMod::GetGlobalObject<DecalData>(v_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((v_decalData != nullptr) ? v_decalData->m_mod : nullptr);
			}
			else
			{
				if (!v_decalData) continue;

				SMDecal* v_newDecal = new SMDecal(v_decalData, v_transform, v_color);
				part->AddObject(v_newDecal);
			}
		}

		return index;
	}
};

SM_MANAGED_CODE