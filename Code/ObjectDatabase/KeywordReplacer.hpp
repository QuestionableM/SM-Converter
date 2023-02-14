#pragma once

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

class KeywordReplacer
{
	using StringMap = std::unordered_map<std::wstring, std::wstring>;

	inline static StringMap m_KeyReplacements     = {};
	inline static StringMap m_TileKeyReplacements = {};

	inline static StringMap m_ResourceUpgrades = {};

	static void CreateKey(std::wstring& key, std::wstring& replacement);
public:
	static void SetReplacement(StringMap& v_map, const std::wstring& key, const std::wstring& replacement);
	static void SetModData(const std::wstring& path, const SMUuid& uuid);

	//Clears $CONTENT_DATA key
	static void ClearContentKey();

	static void UpgradeResource(const std::wstring& mPath, std::wstring& mOutput);

	static void LoadResourceUpgradesFromConfig();
	static void LoadResourceUpgrades(const std::wstring& path);

	static std::wstring ReplaceKey(const std::wstring& path);
	static void ReplaceKeyR(std::wstring& path);
	static void ReplaceKeyTileR(std::wstring& path);
	//Returns false if $CONTENT_DATA key is found but not defined
	static bool ReplaceKeyRLua(std::wstring& path);

	static void Initialize();
	static void Clear();
};

#pragma managed