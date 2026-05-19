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

class SMC_NOVTABLE SMMod
{
public:
	SMMod(
		const std::wstring_view& name,
		const std::wstring_view& directory,
		const SMUuid& uuid,
		const std::uint64_t workshop_id,
		const bool isLocal);

	SMMod(const SMMod&) = delete;
	SMMod(SMMod&&) = delete;

	virtual ~SMMod() = default;

	static void ClearModStorage();
	static SMMod* LoadFromDescription(const std::wstring& mod_folder, bool is_local);

	static ClutterData* GetGlobalClutterById(const std::size_t idx);
	static GarmentData* GetGarment(const std::string_view& category, const SMUuid& uuid);

	static CustomGame* GetCustomGameFromPath(const std::wstring& v_path);
	static CustomGame* GetCustomGameFromUuid(const SMUuid& uuid);
	static SMMod* GetModFromBlocksAndParts(const SMUuid& uuid, const bool checkBlocks);
	static SMMod* GetModFromTileParts(const SMUuid& uuid);
	static SMMod* GetModFromUuid(const SMUuid& modUuid);

	static std::vector<CustomGame*>& GetCustomGames() noexcept;
	static const std::vector<SMMod*>& GetAllMods() noexcept;

	static std::size_t GetAmountOfMods() noexcept;
	static std::size_t GetAmountOfObjects() noexcept;

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

	void LoadFile(const std::wstring_view& path, const bool addToGlobalDb);
	void LoadShapeSetList(const std::wstring_view& path, const bool addToGlobalDb);
	void LoadAssetSetList(const std::wstring_view& path, const bool addToGlobalDb);
	void LoadHarvestableSetList(const std::wstring_view& path, const bool addToGlobalDb);
	void LoadKinematicSetList(const std::wstring_view& path, const bool addToGlobalDb);

	void ScanDatabaseFolderRecursive(const std::wstring_view& folder, const bool addToGlobalDb);
	void ScanDatabaseFolder(const std::wstring_view& folder, const bool addToGlobalDb);

	void SetContentKey() const;

	inline const SMUuid& GetUuid() const noexcept { return m_Uuid; }
	inline const std::wstring& GetName() const noexcept { return m_Name; }
	inline std::uint64_t GetWorkshopId() const noexcept { return m_WorkshopId; }
	inline const std::wstring& GetDirectory() const noexcept { return m_Directory; }

	virtual ModType Type() const noexcept = 0;
	virtual void LoadObjectDatabase() = 0;

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
	std::uint64_t m_WorkshopId;
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