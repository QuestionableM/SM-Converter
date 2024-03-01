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

	for (const auto v_upgrade_list : v_upgrade_array.get_array())
	{
		if (!v_upgrade_list.is_array()) continue;

		for (const auto v_upgrade_obj : v_upgrade_list.get_array())
		{
			if (!v_upgrade_obj.is_array()) continue;

			const auto v_upgrade_obj_arr = v_upgrade_obj.get_array();

			const auto v_upgrade_key = v_upgrade_obj_arr.at(0);
			const auto v_upgrade_val = v_upgrade_obj_arr.at(1);

			if (!(v_upgrade_key.is_string() && v_upgrade_val.is_string())) continue;

			std::wstring v_key_wstr = String::ToWide(v_upgrade_key.get_string());
			std::wstring v_val_wstr = String::ToWide(v_upgrade_val.get_string());

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

	const StringMap::const_iterator v_iter = m_ResourceUpgrades.find(v_lowerPath);
	if (v_iter != m_ResourceUpgrades.end())
	{
		mOutput = v_iter->second;
	}
	else
	{
		mOutput = std::move(v_lowerPath);
	}
}

std::wstring KeywordReplacer::ReplaceKey(const std::wstring& path)
{
	if (path.empty() || path[0] != L'$')
		return path;

	std::wstring v_output;
	KeywordReplacer::UpgradeResource(path, v_output);

	const wchar_t* v_key_beg = path.data();
	const wchar_t* v_key_ptr = std::wcschr(v_key_beg, L'/');
	if (v_key_ptr == nullptr)
		return v_output;

	const std::size_t v_key_idx = v_key_ptr - v_key_beg;

	const std::wstring v_key_chunk = v_output.substr(0, v_key_idx);
	const StringMap::const_iterator v_iter = m_KeyReplacements.find(v_key_chunk);
	if (v_iter == m_KeyReplacements.end())
		return v_output;

	return (v_iter->second + v_output.substr(v_key_idx));
}

void KeywordReplacer::ReplaceKeyR(std::wstring& path)
{
	if (path.empty() || path[0] != L'$')
		return;

	KeywordReplacer::UpgradeResource(path, path);

	const wchar_t* v_key_beg = path.data();
	const wchar_t* v_key_ptr = std::wcschr(v_key_beg, L'/');
	if (v_key_ptr == nullptr)
		return;

	const std::size_t v_key_idx = v_key_ptr - v_key_beg;

	const std::wstring v_key_chunk = path.substr(0, v_key_idx);
	const StringMap::const_iterator v_iter = m_KeyReplacements.find(v_key_chunk);
	if (v_iter == m_KeyReplacements.end())
		return;

	path = (v_iter->second + path.substr(v_key_idx));
}

bool KeywordReplacer::ReplaceKeyRLua(std::wstring& path)
{
	if (path.empty() || path[0] != L'$')
		return true;

	KeywordReplacer::UpgradeResource(path, path);

	const std::size_t v_key_idx = path.find(L'/');
	if (v_key_idx == std::wstring::npos)
		return true;

	const std::wstring v_key_chunk = path.substr(0, v_key_idx);
	if (v_key_chunk == L"$content_data")
	{
		//Throw key error if $CONTENT_DATA doesn't exist
		if (m_KeyReplacements.find(L"$content_data") == m_KeyReplacements.end() &&
			m_KeyReplacements.find(L"$mod_data") == m_KeyReplacements.end())
			return false;
	}

	const StringMap::const_iterator v_iter = m_KeyReplacements.find(v_key_chunk);
	if (v_iter == m_KeyReplacements.end())
		return true;

	path = (v_iter->second + path.substr(v_key_idx));
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