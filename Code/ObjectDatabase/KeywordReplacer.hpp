#pragma once

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

class KeywordReplacer
{
	static void CreateKey(std::wstring& key, std::wstring& replacement);

public:
	//Creates $CONTENT_<uuid> key
	static void CreateContentKey(const SMUuid& v_uuid, const std::wstring& v_replacement);
	//Sets any keyword replacement
	static void SetReplacement(const std::wstring& key, const std::wstring& replacement);
	//Sets $CONTENT_DATA, $MOD_DATA and $CONTENT_<mod_uuid> keys
	static void SetModData(const std::wstring& path, const SMUuid& uuid);
	//Clears $CONTENT_DATA and $MOD_DATA keys
	static void ClearModKeys();

	static void LoadResourceUpgrades(const std::wstring& path);
	static void LoadResourceUpgradesFromConfig();
	static void UpgradeResource(const std::wstring& mPath, std::wstring& mOutput);

	static std::wstring ReplaceKey(const std::wstring& path);
	static void ReplaceKeyR(std::wstring& path);
	//Returns false if $CONTENT_DATA or $MOD_DATA key is found but not defined
	static bool ReplaceKeyRLua(std::wstring& path);

	//Initializes the tile keyword replacer
	static void Initialize();
	static void Clear();

private:
	using StringMap = std::unordered_map<std::wstring, std::wstring>;

	inline static StringMap m_KeyReplacements  = {};
	inline static StringMap m_ResourceUpgrades = {};
};

#pragma managed