#pragma once

#include "ObjectDatabase\ObjectData.hpp"

#include "Utils\Uuid.hpp"
#include "Utils\Json.hpp"

#include <unordered_map>
#include <string>

enum class ModType
{
	BlocksAndParts,
	TerrainAssets,
	GameData
};

class Mod
{
	friend class DatabaseLoader;
	friend class AssetListLoader;
	friend class HarvestableListLoader;
	friend class PartListLoader;
	friend class BlockListLoader;
	friend class ClutterListLoader;
	friend class DecalsetListReader;
	friend class DecalsetReader;

	template<class T>
	using UuidObjectMap = std::unordered_map<SMUuid, T>;

	template<class T>
	using UuidObjMapIterator = typename UuidObjectMap<T>::const_iterator;

	inline static UuidObjectMap<Mod*> ModStorage = {};
	inline static std::vector<Mod*> ModVector = {};

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
	bool m_isLocal;

	Mod() = default;

public:
	Mod(const Mod&) = delete;
	Mod(Mod&&)      = delete;
	Mod(Mod&)       = delete;
	virtual ~Mod();

	static void ClearModStorage();
	static Mod* LoadFromDescription(const std::wstring& mod_folder, const bool& is_local);

	static BlockData* GetGlobalBlock(const SMUuid& uuid);
	static PartData* GetGlobalPart(const SMUuid& uuid);
	static AssetData* GetGlobalAsset(const SMUuid& uuid);
	static HarvestableData* GetGlobalHarvestbale(const SMUuid& uuid);
	static DecalData* GetGlobalDecal(const SMUuid& uuid);
	static ClutterData* GetGlobalClutter(const SMUuid& uuid);
	static ClutterData* GetGlobalClutterById(const std::size_t& idx);

	inline static std::size_t GetAmountOfObjects()
	{
		return (Mod::BlockStorage.size() + Mod::PartStorage.size() + Mod::AssetStorage.size()
			+ Mod::HarvestableStorage.size() + Mod::ClutterStorage.size() + Mod::DecalStorage.size());
	}

	inline static std::size_t GetAmountOfMods() { return Mod::ModVector.size(); }

	void LoadFile(const std::wstring& path);

	void ScanDatabaseFolderRecursive(const std::wstring& folder);
	void ScanDatabaseFolder(const std::wstring& folder);

	virtual ModType Type() const = 0;
	virtual void LoadObjectDatabase() = 0;
};