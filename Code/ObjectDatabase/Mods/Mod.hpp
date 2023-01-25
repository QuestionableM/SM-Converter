#pragma once

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
	friend class DatabaseLoader;
	friend class AssetListLoader;
	friend class HarvestableListLoader;
	friend class PartListLoader;
	friend class BlockListLoader;
	friend class ClutterListLoader;
	friend class DecalsetListReader;
	friend class DecalsetReader;
	friend class TileConv;
	friend CustomGame;

	template<class T>
	using UuidObjectMap = std::unordered_map<SMUuid, T>;

	template<class T>
	using UuidObjMapIterator = typename UuidObjectMap<T>::const_iterator;

	inline static UuidObjectMap<SMMod*> ModStorage = {};
	inline static std::vector<SMMod*> ModVector = {};

	inline static std::vector<CustomGame*> CustomGameVector = {};

	inline static UuidObjectMap<BlockData*> BlockStorage             = {};
	inline static UuidObjectMap<PartData*> PartStorage               = {};
	inline static UuidObjectMap<AssetData*> AssetStorage             = {};
	inline static UuidObjectMap<HarvestableData*> HarvestableStorage = {};
	inline static UuidObjectMap<DecalData*> DecalStorage             = {};

	inline static UuidObjectMap<ClutterData*> ClutterStorage = {};
	inline static std::vector<ClutterData*> ClutterVector                 = {};

	UuidObjectMap<BlockData*> m_Blocks = {};
	UuidObjectMap<PartData*> m_Parts   = {};
	UuidObjectMap<AssetData*> m_Assets = {};
	UuidObjectMap<HarvestableData*> m_Harvestables = {};
	UuidObjectMap<DecalData*> m_Decals = {};
	UuidObjectMap<ClutterData*> m_Clutter = {};

protected:
	SMUuid m_Uuid;
	std::wstring m_Name;
	std::wstring m_Directory;
	unsigned long long m_WorkshopId;
	bool m_isLocal;

	SMMod() = default;

public:
	SMMod(const SMMod&) = delete;
	SMMod(SMMod&&)      = delete;
	SMMod(SMMod&)       = delete;
	virtual ~SMMod();

	static void ClearModStorage();
	static SMMod* LoadFromDescription(const std::wstring& mod_folder, const bool& is_local);

	static BlockData* GetGlobalBlock(const SMUuid& uuid);
	static PartData* GetGlobalPart(const SMUuid& uuid);
	static AssetData* GetGlobalAsset(const SMUuid& uuid);
	static HarvestableData* GetGlobalHarvestbale(const SMUuid& uuid);
	static DecalData* GetGlobalDecal(const SMUuid& uuid);
	static ClutterData* GetGlobalClutter(const SMUuid& uuid);
	static ClutterData* GetGlobalClutterById(const std::size_t& idx);

	template<bool t_check_blocks = true>
	inline static SMMod* GetModFromBlocksAndParts(const SMUuid& uuid)
	{
		const UuidObjMapIterator<PartData*> v_part_iter = SMMod::PartStorage.find(uuid);
		if (v_part_iter != SMMod::PartStorage.end())
			return v_part_iter->second->m_mod;

		if constexpr (t_check_blocks)
		{
			const UuidObjMapIterator<BlockData*> v_block_iter = SMMod::BlockStorage.find(uuid);
			if (v_block_iter != SMMod::BlockStorage.end())
				return v_block_iter->second->m_mod;
		}

		return nullptr;
	}

	inline static std::vector<CustomGame*>& GetCustomGames()
	{
		return SMMod::CustomGameVector;
	}

	inline static std::size_t GetAmountOfObjects()
	{
		return (SMMod::BlockStorage.size() + SMMod::PartStorage.size() + SMMod::AssetStorage.size()
			+ SMMod::HarvestableStorage.size() + SMMod::ClutterStorage.size() + SMMod::DecalStorage.size());
	}

	inline static std::size_t GetAmountOfMods() { return SMMod::ModVector.size(); }

	void LoadFile(const std::wstring& path, const bool& add_to_global_db);
	static void LoadShapeSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db);
	static void LoadAssetSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db);
	static void LoadHarvestableSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db);

	void ScanDatabaseFolderRecursive(const std::wstring& folder, const bool& add_to_global_db);
	void ScanDatabaseFolder(const std::wstring& folder, const bool& add_to_global_db);

	void SetContentKey() const;

	inline const SMUuid& GetUuid() const { return m_Uuid; }
	inline const std::wstring& GetName() const { return m_Name; }
	inline const unsigned long long& GetWorkshopId() const { return m_WorkshopId; }
	inline const std::wstring& GetDirectory() const { return m_Directory; }

	virtual ModType Type() const = 0;
	virtual void LoadObjectDatabase() = 0;
};

#pragma managed