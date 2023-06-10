#pragma once

#include "UStd\UnmanagedUnorderedMap.hpp"

#include "Utils\Console.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

template<typename T>
class SMModObjectStorage
{
	static_assert(!std::is_pointer_v<T>, "Template argument should not be a pointer!");
	static_assert(std::is_same_v<T, class BlockData>
		|| std::is_same_v<T, class PartData>
		|| std::is_same_v<T, class AssetData>
		|| std::is_same_v<T, class HarvestableData>
		|| std::is_same_v<T, class DecalData>
		|| std::is_same_v<T, class KinematicData>
		|| std::is_same_v<T, class ClutterData>, "Invalid template type");

public:
	SMModObjectStorage() = default;

	inline ~SMModObjectStorage()
	{
		for (const auto& v_item : DynamicStorage)
			delete v_item.second;
	}

	inline static std::unordered_map<SMUuid, T*> StaticStorage = {};
	std::unordered_map<SMUuid, T*> DynamicStorage = {};

	inline std::unordered_map<SMUuid, T*>& GetStorage(bool add_to_global_db)
	{
		return add_to_global_db ? StaticStorage : this->DynamicStorage;
	}

	inline bool ObjectExists(const std::unordered_map<SMUuid, T*>& v_storage, const SMUuid& v_uuid)
	{
		if (v_storage.find(v_uuid) == v_storage.end())
			return false;

		DebugWarningL("Object with the same uuid already exists! (", v_uuid.ToString(), ")");
		return true;
	}

	using t_adder_function = void (SMModObjectStorage::*)(T*);
	inline t_adder_function GetAdderFunction(bool add_to_global_db)
	{
		return add_to_global_db ? &SMModObjectStorage::AddObjectGlobal : &SMModObjectStorage::AddObjectDynamic;
	}

	inline void AddObjectDynamic(T* v_object)
	{
		DynamicStorage.emplace(v_object->m_uuid, v_object);
	}

	inline void AddObjectGlobal(T* v_object)
	{
		DynamicStorage.emplace(v_object->m_uuid, v_object);
		StaticStorage.emplace(v_object->m_uuid, v_object);
	}

	inline bool ObjectExists(const SMUuid& v_uuid, bool add_to_global_db)
	{
		const std::unordered_map<SMUuid, T*>& v_cur_map = add_to_global_db ? StaticStorage : this->DynamicStorage;
		if (v_cur_map.find(v_uuid) == v_cur_map.end())
			return false;

		DebugWarningL("Object with the same uuid already exists! (", v_uuid.ToString(), ")");
		return true;
	}

	inline void AddObject(T* v_object, bool add_to_global_db)
	{
		const auto v_new_pair = std::make_pair(v_object->m_uuid, v_object);

		DynamicStorage.insert(v_new_pair);

		if (add_to_global_db)
			StaticStorage.insert(v_new_pair);
	}

	inline static void Clear()
	{
		StaticStorage.clear();
	}
};

#pragma managed