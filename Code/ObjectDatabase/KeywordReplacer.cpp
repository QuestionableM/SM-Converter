#include "KeywordReplacer.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

void KeywordReplacer::CreateKey(std::wstring& key, std::wstring& replacement)
{
	String::ToLowerR(key);
	String::ToLowerR(replacement);

	String::ReplaceAllR(key,         L'\\', L'/');
	String::ReplaceAllR(replacement, L'\\', L'/');
}

void KeywordReplacer::SetReplacement(const std::wstring& key, const std::wstring& replacement)
{
	std::wstring v_lowerKey = key;
	std::wstring v_lowerVal = replacement;

	KeywordReplacer::CreateKey(v_lowerKey, v_lowerVal);

	m_KeyReplacements[std::move(v_lowerKey)] = std::move(v_lowerVal);
}

void KeywordReplacer::CreateContentKey(const SMUuid& v_uuid, const std::wstring& v_replacement)
{
	KeywordReplacer::SetReplacement(L"$content_" + v_uuid.ToWstring(), v_replacement);
}

void KeywordReplacer::SetModData(const std::wstring& path, const SMUuid& uuid)
{
	const std::wstring v_contentKey = L"$content_" + uuid.ToWstring();

	KeywordReplacer::SetReplacement(v_contentKey    , path);
	KeywordReplacer::SetReplacement(L"$mod_data"    , path);
	KeywordReplacer::SetReplacement(L"$content_data", path);
}

void KeywordReplacer::ClearModKeys()
{
	m_KeyReplacements.erase(L"$content_data");
	m_KeyReplacements.erase(L"$mod_data");
}

void KeywordReplacer::LoadResourceUpgrades(const std::wstring& path)
{
	simdjson::dom::document v_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_doc, simdjson::dom::element_type::OBJECT))
		return;

	DebugOutL("Loading resource upgrades: ", 0b1101_fg, path);

	const auto v_upgrade_array = v_doc.root()["upgrade"];
	if (!v_upgrade_array.is_array())
		return;

	for (const auto v_upgrade_list : v_upgrade_array.get_array().value_unsafe())
	{
		if (!v_upgrade_list.is_array()) continue;

		for (const auto v_upgrade_obj : v_upgrade_list.get_array().value_unsafe())
		{
			if (!v_upgrade_obj.is_array()) continue;

			const auto v_upgrade_obj_arr = v_upgrade_obj.get_array().value_unsafe();

			const auto v_upgrade_key = v_upgrade_obj_arr.at(0);
			const auto v_upgrade_val = v_upgrade_obj_arr.at(1);

			if (!(v_upgrade_key.is_string() && v_upgrade_val.is_string())) continue;

			std::wstring v_key_wstr = String::ToWide(v_upgrade_key.get_string().value_unsafe());
			std::wstring v_val_wstr = String::ToWide(v_upgrade_val.get_string().value_unsafe());

			KeywordReplacer::CreateKey(v_key_wstr, v_val_wstr);

			if (m_ResourceUpgrades.find(v_key_wstr) == m_ResourceUpgrades.end())
				m_ResourceUpgrades.emplace(std::move(v_key_wstr), std::move(v_val_wstr));
		}
	}
}

void KeywordReplacer::LoadResourceUpgradesFromConfig()
{
	for (const auto& v_upgrade_path : DatabaseConfig::ResourceUpgradeFiles)
		KeywordReplacer::LoadResourceUpgrades(v_upgrade_path);

	//Nonor normal textures look weird in blender, so i'm just gonna skip em by replacing the string with nothing
	m_ResourceUpgrades[L"$game_data/textures/nonor_nor.png"] = L"";
	m_ResourceUpgrades[L"$game_data/textures/nonor_nor.tga"] = L"";
}

void KeywordReplacer::UpgradeResource(const std::wstring& mPath, std::wstring& mOutput)
{
	std::wstring v_lowerPath = String::ToLower(mPath);
	String::ReplaceAllR(v_lowerPath, L'\\', L'/');

	const auto v_iter = m_ResourceUpgrades.find(v_lowerPath);
	if (v_iter != m_ResourceUpgrades.end())
		mOutput = v_iter->second;
	else
		mOutput = std::move(v_lowerPath);
}

void KeywordReplacer::UpgradeResourceR(std::wstring& out_path)
{
	String::ToLowerR(out_path);
	String::ReplaceAllR(out_path, L'\\', L'/');

	const auto v_iter = m_ResourceUpgrades.find(out_path);
	if (v_iter != m_ResourceUpgrades.end())
		out_path.assign(v_iter->second);
}

std::wstring KeywordReplacer::ReplaceKey(const std::wstring& path)
{
	if (path.empty() || path[0] != L'$')
		return path;

	std::wstring v_output = path;
	KeywordReplacer::UpgradeResourceR(v_output);

	const wchar_t* v_keyBeg = v_output.data();
	const wchar_t* v_keyPtr = std::wcschr(v_keyBeg, L'/');
	if (v_keyPtr == nullptr) return v_output;

	const std::wstring_view v_keyChunk(v_keyBeg, v_keyPtr);

	const auto v_iter = m_KeyReplacements.find(v_keyChunk);
	if (v_iter == m_KeyReplacements.end()) return v_output;

	const auto v_pathBegin = v_output.begin();
	v_output.replace(
		v_pathBegin,
		v_pathBegin + (v_keyPtr - v_keyBeg),
		std::wstring_view(v_iter->second)
	);

	return v_output;
}

void KeywordReplacer::ReplaceKeyR(std::wstring& path)
{
	if (path.empty() || path[0] != L'$')
		return;

	KeywordReplacer::UpgradeResourceR(path);

	const wchar_t* v_keyBeg = path.data();
	const wchar_t* v_keyPtr = std::wcschr(v_keyBeg, L'/');
	if (v_keyPtr == nullptr) return;

	const std::wstring_view v_keyChunk(v_keyBeg, v_keyPtr);

	const auto v_iter = m_KeyReplacements.find(v_keyChunk);
	if (v_iter == m_KeyReplacements.end()) return;

	const auto v_pathBegin = path.begin();
	path.replace(
		v_pathBegin,
		v_pathBegin + (v_keyPtr - v_keyBeg),
		std::wstring_view(v_iter->second)
	);
}

bool KeywordReplacer::ReplaceKeyRLua(std::wstring& path)
{
	if (path.empty() || path[0] != L'$')
		return true;

	KeywordReplacer::UpgradeResourceR(path);

	const wchar_t* v_keyBeg = path.data();
	const wchar_t* v_keyPtr = std::wcschr(v_keyBeg, L'/');
	if (v_keyPtr == nullptr) return true;

	const std::wstring_view v_keyChunk(v_keyBeg, v_keyPtr);
	if (v_keyChunk == L"$content_data")
	{
		//Throw key error if $CONTENT_DATA doesn't exist
		if (m_KeyReplacements.find(std::wstring_view(L"$content_data")) == m_KeyReplacements.end() &&
			m_KeyReplacements.find(std::wstring_view(L"$mod_data")) == m_KeyReplacements.end())
		{
			return false;
		}
	}

	const auto v_iter = m_KeyReplacements.find(v_keyChunk);
	if (v_iter == m_KeyReplacements.end()) return true;

	const auto v_pathBegin = path.begin();
	path.replace(
		v_pathBegin,
		v_pathBegin + (v_keyPtr - v_keyBeg),
		std::wstring_view(v_iter->second)
	);

	return true;
}

void KeywordReplacer::Initialize()
{
	DebugOutL(__FUNCTION__);

	KeywordReplacer::Clear();
	KeywordReplacer::LoadResourceUpgradesFromConfig();

	for (const auto& v_kv_pair : DatabaseConfig::DefaultKeywords)
		KeywordReplacer::SetReplacement(v_kv_pair.first, v_kv_pair.second);
}

void KeywordReplacer::Clear()
{
	DebugOutL(__FUNCTION__);

	m_KeyReplacements.clear();
	m_ResourceUpgrades.clear();
}