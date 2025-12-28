#pragma once

#include "ObjectDatabase/Mods/ModObjectStorage.hpp"
#include "ObjectDatabase/ObjectData.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"
#include "Utils/Uuid.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

enum class ModType
{
	BlocksAndParts,
	TerrainAssets,
	GameData,
	CustomGame
};

class CustomGame;

class SMMod
{
public:
	static void ClearModStorage();
	static SMMod* LoadFromDescription(const std::wstring& mod_folder, bool is_local);

	template<typename T>
	inline static const T* GetGlobalObject(const SMUuid& uuid)
	{
		static_assert(!std::is_pointer_v<T>, "GetObject -> Template argument should not be a pointer!");

		const auto v_iter = SMModObjectStorage<T>::StaticStorage.find(uuid);
		if (v_iter != SMModObjectStorage<T>::StaticStorage.end())
			return v_iter->second;
		else
			return nullptr;
	}

	inline static ClutterData* GetGlobalClutterById(std::size_t idx)
	{
		if (SMMod::ClutterVector.size() <= idx)
		{
			DebugErrorL("The clutter index is out of bounds! (Size: ", SMMod::ClutterVector.size(), ", Index: ", idx, ")");
			return nullptr;
		}

		return SMMod::ClutterVector[idx];
	}

	static CustomGame* GetCustomGameFromPath(const std::wstring& v_path);

	template<bool t_check_blocks = true>
	inline static SMMod* GetModFromBlocksAndParts(const SMUuid& uuid)
	{
		const PrecomputedUuidHash v_uuidHash(uuid);

		const auto v_partIter = SMModObjectStorage<PartData>::StaticStorage.find(v_uuidHash);
		if (v_partIter != SMModObjectStorage<PartData>::StaticStorage.end())
			return v_partIter->second->m_mod;

		if constexpr (t_check_blocks) {
			const auto v_blockIter = SMModObjectStorage<BlockData>::StaticStorage.find(v_uuidHash);
			if (v_blockIter != SMModObjectStorage<BlockData>::StaticStorage.end())
				return v_blockIter->second->m_mod;
		}

		return nullptr;
	}

	template<typename T, bool t_allow_replacements>
	inline static void MergeMaps(
		std::unordered_map<SMUuid, T*, UuidHasher, std::equal_to<>>& v_output,
		std::unordered_map<SMUuid, T*, UuidHasher, std::equal_to<>>& to_add)
	{
		for (const auto& v_item : to_add)
		{
			const auto v_iter = v_output.find(v_item.first);
			if (v_iter == v_output.end())
			{
				v_output.emplace(v_item.first, v_item.second);
				continue;
			}

			if constexpr (t_allow_replacements) {
				//Override the elements
				v_iter->second = v_item.second;
			}
		}
	}


	inline static std::vector<CustomGame*>& GetCustomGames() noexcept { return SMMod::CustomGameVector; }
	inline static const std::vector<SMMod*>& GetAllMods() noexcept { return SMMod::ModVector; }

	inline static GarmentData* GetGarment(const std::string& category, const SMUuid& uuid)
	{
		const auto v_categoryIter = SMMod::GarmentStorage.find(category);
		if (v_categoryIter == SMMod::GarmentStorage.end())
			return nullptr;

		const auto v_curItem = v_categoryIter->second.find(uuid);
		if (v_curItem == v_categoryIter->second.end())
			return nullptr;

		return v_curItem->second;
	}

	inline static std::size_t GetAmountOfMods() noexcept { return SMMod::ModVector.size(); }
	inline static std::size_t GetAmountOfObjects() noexcept
	{
		return SMModObjectStorage<BlockData>::StaticStorage.size()
			+ SMModObjectStorage<PartData>::StaticStorage.size()
			+ SMModObjectStorage<AssetData>::StaticStorage.size()
			+ SMModObjectStorage<HarvestableData>::StaticStorage.size()
			+ SMModObjectStorage<ClutterData>::StaticStorage.size()
			+ SMModObjectStorage<DecalData>::StaticStorage.size()
			+ SMModObjectStorage<KinematicData>::StaticStorage.size()
			+ SMModObjectStorage<WedgeData>::StaticStorage.size();
	}

	void LoadFile(const std::wstring& path, bool add_to_global_db);
	static void LoadShapeSetList(const std::wstring& path, SMMod* v_mod, bool add_to_global_db);
	static void LoadAssetSetList(const std::wstring& path, SMMod* v_mod, bool add_to_global_db);
	static void LoadHarvestableSetList(const std::wstring& path, SMMod* v_mod, bool add_to_global_db);
	static void LoadKinematicSetList(const std::wstring& path, SMMod* v_mod, bool add_to_global_db);

	void ScanDatabaseFolderRecursive(const std::wstring& folder, bool add_to_global_db);
	void ScanDatabaseFolder(const std::wstring& folder, bool add_to_global_db);

	void SetContentKey() const;

	inline const SMUuid& GetUuid() const noexcept { return m_Uuid; }
	inline const std::wstring& GetName() const noexcept { return m_Name; }
	inline std::uint64_t GetWorkshopId() const noexcept { return m_WorkshopId; }
	inline const std::wstring& GetDirectory() const noexcept { return m_Directory; }

	virtual ModType Type() const noexcept = 0;
	virtual void LoadObjectDatabase() = 0;

	SMMod(
		const std::wstring& name,
		const std::wstring& directory,
		const SMUuid& uuid,
		std::uint64_t workshop_id,
		bool isLocal);

	SMMod(const SMMod&) = delete;
	SMMod(SMMod&&) = delete;

	virtual ~SMMod() = default;

	SMModObjectStorage<BlockData> m_Blocks;
	SMModObjectStorage<PartData> m_Parts;
	SMModObjectStorage<AssetData> m_Assets;
	SMModObjectStorage<HarvestableData> m_Harvestables;
	SMModObjectStorage<DecalData> m_Decals;
	SMModObjectStorage<KinematicData> m_Kinematics;
	SMModObjectStorage<ClutterData> m_Clutter;
	SMModObjectStorage<WedgeData> m_Wedges;
	inline static std::vector<ClutterData*> ClutterVector = {};
	inline static std::unordered_map<
		std::string,
		std::unordered_map<SMUuid, GarmentData*>,
		Hashing::StringHasher,
		std::equal_to<>
	> GarmentStorage = {};

	inline static SMMod* GameDataMod = nullptr;

protected:
	SMUuid m_Uuid;
	std::wstring m_Name;
	std::wstring m_Directory;
	unsigned long long m_WorkshopId;
	bool m_isLocal;

private:
	//Mod and Custom game storage/vectors are filled inside the constructors of the mods

	inline static std::unordered_map<SMUuid, SMMod*> ModStorage = {};
	inline static std::vector<SMMod*> ModVector = {};

	inline static std::unordered_map<SMUuid, SMMod*> CustomGameStorage = {};
	inline static std::vector<CustomGame*> CustomGameVector = {};

	friend class BlocksAndPartsMod;
	friend class TerrainAssetsMod;
	friend class DatabaseLoader;
	friend CustomGame;
};

SM_MANAGED_CODE