#pragma once

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/Tile.hpp"
#include "Converter/Entity/Kinematic.hpp"

#include "ObjectDatabase/UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/BitStream.hpp"
#include "Utils/String.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class KinematicsListReader
{
	KinematicsListReader() = default;

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
			if (!TileConverterSettings::ExportKinematics) return;
		}

		for (int a = 0; a < 4; a++)
		{
			const int kinematicsListCompressedSize = header.kinematicsListCompressedSize[a];
			const int kinematicsListSize = header.kinematicsListSize[a];
			const int kinematicsListCount = header.kinematicsListCount[a];

			if (kinematicsListCount == 0)
				continue;

			DebugOutL("Kinematics[", a, "]: ", kinematicsListSize, ", ", kinematicsListCompressedSize);

			if (!reader.hasEnoughSpace(header.kinematicsListIndex[a], kinematicsListCompressedSize))
			{
				DebugErrorL("Not enough space!");
				continue;
			}

			std::vector<Byte> v_bytes(kinematicsListSize);
			int debugSize = Lz4::DecompressFast(
				reinterpret_cast<const char*>(reader.getPointer(header.kinematicsListIndex[a])),
				reinterpret_cast<char*>(v_bytes.data()),
				kinematicsListSize);

			if (debugSize != kinematicsListCompressedSize)
			{
				DebugErrorL("Debug Size: ", debugSize, ", kinematicsListCompressedSize: ", kinematicsListCompressedSize);
				error.setError(1, "KinematicsListReader::Read -> debugSize != kinematicsListCompressedSize\nTile Version: ", version);
				return;
			}

			debugSize = KinematicsListReader::Read<t_mod_counter>(v_bytes, a, kinematicsListCount, version, pPart);
			if (debugSize != kinematicsListSize)
			{
				DebugErrorL("Debug Size: ", debugSize, ", kinematicsListSize: ", kinematicsListSize);
				error.setError(1, "KinematicsListReader::Read -> debugSize != kinematicsListSize\nTile Version: ", version);
				return;
			}
		}
	}

	template<bool t_mod_counter>
	static int Read(
		const std::vector<Byte>& bytes,
		int kinematic_idx,
		int len,
		std::uint32_t version,
		TilePart* pPart)
	{
		SMEntityTransform v_transform;
		MemoryWrapper v_memory(bytes);
		MEMORY_WRAPPER_DUMP_BYTES(v_memory, "./kinematics_dump", 0);

		int index = 0;
		for (int a = 0; a < len; a++)
		{
			v_transform.position = v_memory.Object<glm::vec3>(index);
			v_transform.rotation = v_memory.GetQuat(index + 0xc);
			v_transform.scale = v_memory.Object<glm::vec3>(index + 0x1c);

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

				SMKinematic* v_new_kinematic = new SMKinematic(v_km_data, v_transform, v_km_model, v_color);
				pPart->AddObject(v_new_kinematic, kinematic_idx);
			}
		}

		return index;
	}
};

SM_MANAGED_CODE