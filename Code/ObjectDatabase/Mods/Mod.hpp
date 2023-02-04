#pragma once

#include "ObjectDatabase\Mods\ModObjectStorage.hpp"
#include "ObjectDatabase\ObjectData.hpp"

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedString.hpp"

#include "Utils\Uuid.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

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
	static SMMod* LoadFromDescription(const std::wstring& mod_folder, const bool& is_local);

	template<typename T>
	inline static T* GetGlobalObject(const SMUuid& uuid)
	{
		static_assert(!std::is_pointer_v<T>, "GetObject -> Template argument should not be a pointer!");

		const std::unordered_map<SMUuid, T*>::const_iterator v_iter = SMModObjectStorage<T>::StaticStorage.find(uuid);
		if (v_iter != SMModObjectStorage<T>::StaticStorage.end())
			return v_iter->second;

		DebugErrorL("Couldn't find an object with the specified uuid: ", uuid.ToString());
		return nullptr;
	}

	static ClutterData* GetGlobalClutterById(const std::size_t& idx);

	template<bool t_check_blocks = true>
	inline static SMMod* GetModFromBlocksAndParts(const SMUuid& uuid)
	{
		const std::unordered_map<SMUuid, PartData*>::const_iterator v_part_iter = SMModObjectStorage<PartData>::StaticStorage.find(uuid);
		if (v_part_iter != SMModObjectStorage<PartData>::StaticStorage.end())
			return v_part_iter->second->m_mod;

		if constexpr (t_check_blocks) {
			const std::unordered_map<SMUuid, BlockData*>::const_iterator v_block_iter = SMModObjectStorage<BlockData>::StaticStorage.find(uuid);
			if (v_block_iter != SMModObjectStorage<BlockData>::StaticStorage.end())
				return v_block_iter->second->m_mod;
		}

		return nullptr;
	}

	template<typename T, bool t_allow_replacements>
	inline static void MergeMaps(std::unordered_map<SMUuid, T*>& v_output, std::unordered_map<SMUuid, T*>& to_add)
	{
		for (const auto& v_item : to_add)
		{
			const auto v_iter = v_output.find(v_item.first);
			if (v_iter == v_output.end())
			{
				v_output.insert(v_item);
				continue;
			}

			if constexpr (t_allow_replacements) {
				//Override the elements
				v_iter->second = v_item.second;
			}
		}
	}

	inline static std::vector<CustomGame*>& GetCustomGames() { return SMMod::CustomGameVector; }
	inline static const std::vector<SMMod*>& GetAllMods() { return SMMod::ModVector; }

	inline static std::size_t GetAmountOfObjects()
	{
		return SMModObjectStorage<BlockData>::StaticStorage.size()
			+ SMModObjectStorage<PartData>::StaticStorage.size()
			+ SMModObjectStorage<AssetData>::StaticStorage.size()
			+ SMModObjectStorage<HarvestableData>::StaticStorage.size()
			+ SMModObjectStorage<ClutterData>::StaticStorage.size()
			+ SMModObjectStorage<DecalData>::StaticStorage.size()
			+ SMModObjectStorage<KinematicData>::StaticStorage.size();
	}

	inline static std::size_t GetAmountOfMods() { return SMMod::ModVector.size(); }

	void LoadFile(const std::wstring& path, const bool& add_to_global_db);
	static void LoadShapeSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db);
	static void LoadAssetSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db);
	static void LoadHarvestableSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db);
	static void LoadKinematicSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db);

	void ScanDatabaseFolderRecursive(const std::wstring& folder, const bool& add_to_global_db);
	void ScanDatabaseFolder(const std::wstring& folder, const bool& add_to_global_db);

	void SetContentKey() const;

	inline const SMUuid& GetUuid() const { return m_Uuid; }
	inline const std::wstring& GetName() const { return m_Name; }
	inline const unsigned long long& GetWorkshopId() const { return m_WorkshopId; }
	inline const std::wstring& GetDirectory() const { return m_Directory; }

	virtual ModType Type() const = 0;
	virtual void LoadObjectDatabase() = 0;

	SMMod(const SMMod&) = delete;
	SMMod(SMMod&&) = delete;
	SMMod(SMMod&) = delete;
	virtual ~SMMod() = default;

	SMModObjectStorage<BlockData> m_Blocks;
	SMModObjectStorage<PartData> m_Parts;
	SMModObjectStorage<AssetData> m_Assets;
	SMModObjectStorage<HarvestableData> m_Harvestables;
	SMModObjectStorage<DecalData> m_Decals;
	SMModObjectStorage<KinematicData> m_Kinematics;
	SMModObjectStorage<ClutterData> m_Clutter;
	inline static std::vector<ClutterData*> ClutterVector = {};

	inline static SMMod* GameDataMod = nullptr;

protected:
	SMUuid m_Uuid;
	std::wstring m_Name;
	std::wstring m_Directory;
	unsigned long long m_WorkshopId;
	bool m_isLocal;

	SMMod() = default;

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

#pragma managed