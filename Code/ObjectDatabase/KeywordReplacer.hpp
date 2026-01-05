#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class KeywordReplacer
{
	static void CreateKey(std::wstring& key, std::wstring& replacement);

public:
	//Creates $CONTENT_<uuid> key
	static void CreateContentKey(const SMUuid& uuid, const std::wstring_view& replacement);
	//Sets any keyword replacement
	static void SetReplacement(const std::wstring_view& key, const std::wstring_view& replacement);
	//Sets $CONTENT_DATA, $MOD_DATA and $CONTENT_<mod_uuid> keys
	static void SetModData(const std::wstring& path, const SMUuid& uuid);
	//Clears $CONTENT_DATA and $MOD_DATA keys
	static void ClearModKeys();

	static void LoadResourceUpgrades(const std::wstring& path);
	static void LoadResourceUpgradesFromConfig();
	static void UpgradeResource(const std::wstring& mPath, std::wstring& mOutput);
	static void UpgradeResourceR(std::wstring& out_path);

	static std::wstring ReplaceKey(const std::wstring& path);
	static void ReplaceKeyR(std::wstring& path);
	//Returns false if $CONTENT_DATA or $MOD_DATA key is found but not defined
	static bool ReplaceKeyRLua(std::wstring& path);

	//Initializes the tile keyword replacer
	static void Initialize();
	static void Clear();

private:
	using StringMap = std::unordered_map<std::wstring, std::wstring, Hashing::WstringHasher, std::equal_to<>>;

	inline static StringMap m_KeyReplacements  = {};
	inline static StringMap m_ResourceUpgrades = {};
};

SM_MANAGED_CODE