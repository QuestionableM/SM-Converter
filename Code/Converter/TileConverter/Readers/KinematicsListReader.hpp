#pragma once

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\Tile.hpp"
#include "Converter\Entity\Kinematic.hpp"

#include "ObjectDatabase\UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase\ObjectDatabase.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\BitStream.hpp"
#include "Utils\String.hpp"
#include "Utils\Lz4Lib.hpp"

#pragma unmanaged

class KinematicsListReader
{
	KinematicsListReader() = default;

public:
	template<bool t_mod_counter>
	static void Read(CellHeader* header, MemoryWrapper& reader, TilePart* part, int version, ConvertError& v_error)
	{
		if (v_error) return;

		if constexpr (!t_mod_counter) {
			if (!TileConverterSettings::ExportKinematics) return;
		}

		for (int a = 0; a < 4; a++)
		{
			const int kinematicsListCompressedSize = header->kinematicsListCompressedSize[a];
			const int kinematicsListSize = header->kinematicsListSize[a];
			const int kinematicsListCount = header->kinematicsListCount[a];

			if (kinematicsListCount == 0)
				continue;

			DebugOutL("Kinematics[", a, "]: ", kinematicsListSize, ", ", kinematicsListCompressedSize);

			const std::vector<Byte> compressed = reader.Objects<Byte>(header->kinematicsListIndex[a], kinematicsListCompressedSize);
			std::vector<Byte> v_bytes(kinematicsListSize);

			int debugSize = Lz4::DecompressFast(reinterpret_cast<const char*>(compressed.data()),
				reinterpret_cast<char*>(v_bytes.data()), kinematicsListSize);
			if (debugSize != kinematicsListCompressedSize)
			{
				DebugErrorL("Debug Size: ", debugSize, ", kinematicsListCompressedSize: ", kinematicsListCompressedSize);
				v_error = ConvertError(1, L"KinematicsListReader::Read -> debugSize != kinematicsListCompressedSize\nTile Version: " + std::to_wstring(version));
				return;
			}

			debugSize = KinematicsListReader::Read<t_mod_counter>(v_bytes, a, kinematicsListCount, version, part);
			if (debugSize != kinematicsListSize)
			{
				DebugErrorL("Debug Size: ", debugSize, ", kinematicsListSize: ", kinematicsListSize);
				v_error = ConvertError(1, L"KinematicsListReader::Read -> debugSize != kinematicsListSize\nTile Version: " + std::to_wstring(version));
				return;
			}
		}
	}

	template<bool t_mod_counter>
	static int Read(const std::vector<Byte>& bytes, int kinematic_idx, int len, int version, TilePart* part)
	{
		MemoryWrapper v_memory(bytes);
		MEMORY_WRAPPER_DUMP_BYTES(v_memory, "./kinematics_dump", 0);

		int index = 0;
		for (int a = 0; a < len; a++)
		{
			const glm::vec3 v_pos = v_memory.Object<glm::vec3>(index);
			const glm::quat v_quat = v_memory.GetQuat(index + 0xc);
			const glm::vec3 v_size = v_memory.Object<glm::vec3>(index + 0x1c);

			const SMUuid v_uuid = v_memory.Object<SMUuid>(index + 0x28);
			const SMColor v_color = v_memory.Object<unsigned int>(index + 0x38);

			index += 0x3c;

			//It looks like lua strings always end with 0x9, so iterate until 0x9 is found
			while (1)
			{
				if (v_memory.Object<Byte>(index++) == 0x9)
					break;
			}

			const int v_json_str_sz = v_memory.Object<int, true>(index);
			index += 0x4;
			//const std::string v_json_str = v_memory.String(index, v_json_str_sz);
			index += v_json_str_sz;

			//DebugOutL("Json String: ", v_json_str);
			index += 0x1; //NULL BYTE

			const KinematicData* v_km_data = SMMod::GetGlobalObject<KinematicData>(v_uuid);

			if constexpr (t_mod_counter)
			{
				ItemModStats::IncrementModPart((v_km_data != nullptr) ? v_km_data->m_mod : nullptr);
			}
			else
			{
				if (!v_km_data) continue;

				Model* v_km_model = ModelStorage::LoadModel(v_km_data->m_mesh);
				if (!v_km_model) continue;

				SMKinematic* v_new_kinematic = new SMKinematic(v_km_data, v_km_model, v_color);

				v_new_kinematic->SetPosition(v_pos);
				v_new_kinematic->SetRotation(v_quat);
				v_new_kinematic->SetSize(v_size);

				part->AddObject(v_new_kinematic, kinematic_idx);
			}
		}

		return index;
	}
};

#pragma managed