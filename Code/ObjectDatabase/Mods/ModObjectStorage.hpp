#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Console.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

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
		|| std::is_same_v<T, class ClutterData>
		|| std::is_same_v<T, class WedgeData>, "Invalid template type");

public:
	SMModObjectStorage() = default;

	inline ~SMModObjectStorage()
	{
		for (const auto& v_item : DynamicStorage)
			delete v_item.second;
	}

	using MapType = std::unordered_map<SMUuid, T*, UuidHasher, std::equal_to<>>;

	inline static MapType StaticStorage = {};
	MapType DynamicStorage = {};

	inline MapType& GetStorage(bool add_to_global_db)
	{
		return add_to_global_db ? StaticStorage : this->DynamicStorage;
	}

	inline bool ObjectExists(const MapType& mapStorage, const SMUuid& uuid)
	{
		if (!mapStorage.contains(uuid)) return false;

		DebugWarningL("Object with the same uuid already exists! (", uuid.toString(), ")");
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

	inline bool ObjectExists(const SMUuid& uuid, bool add_to_global_db)
	{
		const auto& v_curMap = add_to_global_db ? StaticStorage : this->DynamicStorage;
		if (!v_curMap.contains(uuid)) return false;

		DebugWarningL("Object with the same uuid already exists! (", uuid.toString(), ")");
		return true;
	}

	inline void AddObject(T* pObject, bool add_to_global_db)
	{
		DynamicStorage.emplace(pObject->m_uuid, pObject);
		if (add_to_global_db)
			StaticStorage.emplace(pObject->m_uuid, pObject);
	}

	inline static void Clear()
	{
		StaticStorage.clear();
	}
};

SM_MANAGED_CODE