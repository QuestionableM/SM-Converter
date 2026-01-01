#include "SubMeshData.hpp"

SM_UNMANAGED_CODE

SMTextureList::SMTextureList(SMTextureList&& other) noexcept :
	m_dif(std::move(other.m_dif)),
	m_asg(std::move(other.m_asg)),
	m_nor(std::move(other.m_nor)),
	m_material(std::move(other.m_material)),
	m_defColorIdx(std::move(other.m_defColorIdx)),
	m_shadowOnly(other.m_shadowOnly)
{}

void SMTextureList::operator=(const SMTextureList& other)
{
	m_dif = other.m_dif;
	m_asg = other.m_asg;
	m_nor = other.m_nor;
	m_material = other.m_material;
	m_defColorIdx = other.m_defColorIdx;
	m_shadowOnly = other.m_shadowOnly;
}

void SMTextureList::operator=(SMTextureList&& other) noexcept
{
	m_dif = std::move(other.m_dif);
	m_asg = std::move(other.m_asg);
	m_nor = std::move(other.m_nor);
	m_material = std::move(other.m_material);
	m_defColorIdx = std::move(other.m_defColorIdx);
	m_shadowOnly = other.m_shadowOnly;
}

std::wstring& SMTextureList::getStringRef(std::size_t idx) noexcept
{
	return reinterpret_cast<std::wstring*>(&m_dif)[idx];
}

////////////////// SM SUB MESH LIST ///////////////////

SMSubMeshList::~SMSubMeshList()
{
	const std::size_t v_storageSz = m_storage.size();
	for (std::size_t a = 0; a < v_storageSz; a++)
		delete m_storage[a];
}

const SMTextureList* SMSubMeshList::getTexList(
	const std::string_view& key,
	const std::size_t keyIdx) const
{
	if (keyIdx >= m_storage.size())
		return nullptr;

	return m_storage[keyIdx];
}

void SMSubMeshList::addTexList(
	const std::string_view& key,
	const std::size_t keyIdx,
	SMTextureList* pTexList)
{
	if (m_storage.size() <= keyIdx)
		m_storage.resize(keyIdx + 1, nullptr);

	m_storage[keyIdx] = pTexList;
}

SMSubMeshType SMSubMeshList::type() const
{
	return SMSubMeshType::SubMeshList;
}

////////////////// SM SUB MESH MAP /////////////////////

SMSubMeshMap::SMSubMeshMap()
	: m_storage()
{}

SMSubMeshMap::~SMSubMeshMap()
{
	for (const auto& v_obj : m_storage)
		delete v_obj.second;
}

const SMTextureList* SMSubMeshMap::getTexList(
	const std::string_view& key,
	const std::size_t keyIdx) const
{
	const auto v_iter = m_storage.find(key);
	if (v_iter != m_storage.end())
		return v_iter->second;

	return nullptr;
}

void SMSubMeshMap::addTexList(
	const std::string_view& key,
	const std::size_t keyIdx,
	SMTextureList* pTexList)
{
	if (m_storage.find(key) != m_storage.end())
		return;

	m_storage.emplace(key, pTexList);
}

SMSubMeshType SMSubMeshMap::type() const
{
	return SMSubMeshType::SubMeshMap;
}

SM_MANAGED_CODE