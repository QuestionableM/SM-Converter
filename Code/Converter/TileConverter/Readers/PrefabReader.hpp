#pragma once

#include "Converter\TileConverter\Readers\PrefabFileReader.hpp"
#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TilePart.hpp"
#include "Converter\TileConverter\Tile.hpp"
#include "Converter\Entity\Prefab.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Memory.hpp"
#include "Utils\Lz4Lib.hpp"

#pragma unmanaged

class PrefabReader
{
	PrefabReader() = default;

public:
	template<bool t_mod_counter, int t_tile_version>
	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, ConvertError& v_error)
	{
		if (v_error) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportPrefabs) return;
		}

		if (header->prefabCount == 0 || header->prefabIndex == 0) return;

		DebugOutL("Prefab: ", header->prefabSize, " / ", header->prefabCompressedSize);

		std::vector<Byte> compressed = reader.Objects<Byte>(header->prefabIndex, header->prefabCompressedSize);
		std::vector<Byte> bytes(header->prefabSize);

		int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
			reinterpret_cast<char*>(bytes.data()), header->prefabSize);
		if (debugSize != header->prefabCompressedSize)
		{
			DebugErrorL("DebugSize: ", debugSize, ", header->prefabCompressedSize: ", header->prefabCompressedSize);
			v_error = ConvertError(1, L"PrefabReader::Read -> debugSize != header->prefabCompressedSize\nTile Version: " + std::to_wstring(t_tile_version));
			return;
		}

		debugSize = PrefabReader::Read<t_mod_counter, t_tile_version>(bytes, header->prefabCount, part, v_error);
		if (debugSize != header->prefabSize)
		{
			//If the error happened in the PrefabReader::Read, then don't overwrite it with the one below
			if (v_error) return;

			DebugErrorL("DebugSize: ", debugSize, ", header->prefabSize: ", header->prefabSize);
			v_error = ConvertError(1, L"PrefabReader::Read -> debugSize != header->prefabSize\nTile Version: " + std::to_wstring(t_tile_version));
			return;
		}
	}

	template<bool t_mod_counter, int t_tile_version>
	static int Read(const std::vector<Byte>& bytes, const int& prefabCount, TilePart* part, ConvertError& v_error)
	{
		MemoryWrapper memory(bytes);

		int index = 0;
		for (int a = 0; a < prefabCount; a++)
		{
			const glm::vec3 f_pos = memory.Object<glm::vec3>(index);
			const glm::quat f_quat = memory.GetQuat(index + 0xc);
			
			glm::vec3 f_size;

			if constexpr (t_tile_version < 9)
			{
				f_size = glm::vec3(1.0f);
				index += 0x1c;
			}
			else
			{
				f_size = memory.Object<glm::vec3>(index + 0x1c);
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

			SMPrefab* v_new_prefab = PrefabFileReader::Read<t_mod_counter>(v_pref_path, v_pref_flag, v_error);
			if (v_error) return 0;

			if constexpr (!t_mod_counter)
			{
				if (!v_new_prefab) continue;

				v_new_prefab->SetPosition(f_pos);
				v_new_prefab->SetRotation(f_quat);
				v_new_prefab->SetSize(f_size);

				part->AddObject(v_new_prefab);
			}
		}

		return index;
	}
};

#pragma managed