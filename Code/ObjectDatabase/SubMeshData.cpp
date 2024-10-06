#include "SubMeshData.hpp"

SM_UNMANAGED_CODE

SMTextureList::SMTextureList(SMTextureList&& other) noexcept :
	dif(std::move(other.dif)),
	asg(std::move(other.asg)),
	nor(std::move(other.nor)),
	material(std::move(other.material)),
	def_color_idx(std::move(other.def_color_idx)),
	is_shadow_only(other.is_shadow_only)
{}

void SMTextureList::operator=(const SMTextureList& other)
{
	this->dif = other.dif;
	this->asg = other.asg;
	this->nor = other.nor;
	this->material = other.material;
	this->def_color_idx = other.def_color_idx;
	this->is_shadow_only = other.is_shadow_only;
}

void SMTextureList::operator=(SMTextureList&& other) noexcept
{
	this->dif = std::move(other.dif);
	this->asg = std::move(other.asg);
	this->nor = std::move(other.nor);
	this->material = std::move(other.material);
	this->def_color_idx = std::move(other.def_color_idx);
	this->is_shadow_only = other.is_shadow_only;
}

std::wstring& SMTextureList::getStringRef(std::size_t idx) noexcept
{
	return reinterpret_cast<std::wstring*>(&dif)[idx];
}

////////////////// SM SUB MESH LIST ///////////////////

SMSubMeshList::~SMSubMeshList()
{
	const std::size_t v_storageSz = m_storage.size();
	for (std::size_t a = 0; a < v_storageSz; a++)
		delete m_storage[a];
}

const SMTextureList* SMSubMeshList::getTexList(const std::string& key, std::size_t key_idx) const
{
	if (key_idx >= m_storage.size())
		return nullptr;

	return m_storage[key_idx];
}

void SMSubMeshList::addTexList(const std::string_view& key, std::size_t key_idx, SMTextureList* sub_data)
{
	if (m_storage.size() <= key_idx)
		m_storage.resize(key_idx + 1, nullptr);

	m_storage[key_idx] = sub_data;
}

SMSubMeshType SMSubMeshList::type() const
{
	return SMSubMeshType::SubMeshList;
}

////////////////// SM SUB MESH MAP /////////////////////

SMSubMeshMap::~SMSubMeshMap()
{
	for (const auto& v_obj : m_storage)
		delete v_obj.second;
}

const SMTextureList* SMSubMeshMap::getTexList(const std::string& key, std::size_t key_idx) const
{
	const auto v_iter = m_storage.find(key);
	if (v_iter != m_storage.end())
		return v_iter->second;

	return nullptr;
}

void SMSubMeshMap::addTexList(const std::string_view& key, std::size_t key_idx, SMTextureList* sub_data)
{
	if (m_storage.find(key) != m_storage.end())
		return;

	m_storage.emplace(key, sub_data);
}

SMSubMeshType SMSubMeshMap::type() const
{
	return SMSubMeshType::SubMeshMap;
}

SM_MANAGED_CODE