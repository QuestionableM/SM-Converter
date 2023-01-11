#pragma once

#include "Utils\Uuid.hpp"

#include <unordered_map>

class KeywordReplacer
{
	using StringMap = std::unordered_map<std::wstring, std::wstring>;

	inline static StringMap m_KeyReplacements  = {};
	inline static StringMap m_ResourceUpgrades = {};

	static void CreateKey(std::wstring& key, std::wstring& replacement);
public:
	static void SetReplacement(const std::wstring& key, const std::wstring& replacement);
	static void SetModData(const std::wstring& path, const SMUuid& uuid);

	//Clears $CONTENT_DATA key
	static void ClearContentKey();
	//Checks if $CONTENT_DATA key is defined
	static bool ContentKeyExists();

	static void UpgradeResource(const std::wstring& mPath, std::wstring& mOutput);

	static void LoadResourceUpgradesFromConfig();
	static void LoadResourceUpgrades(const std::wstring& path);

	static std::wstring ReplaceKey(const std::wstring& path);
	static void ReplaceKeyR(std::wstring& path);
	//Returns false if $CONTENT_DATA key is found but not defined
	static bool ReplaceKeyRLua(std::wstring& path);

	static void Initialize();
	static void Clear();
};