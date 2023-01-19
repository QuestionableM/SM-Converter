#pragma once

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#pragma unmanaged

enum class SMSubMeshType : unsigned char
{
	SubMeshList = 0,
	SubMeshMap = 1
};

class SMTextureList
{
public:
	std::wstring dif;
	std::wstring asg;
	std::wstring nor;

	std::string material;
	std::string def_color_idx;
	bool is_shadow_only = false;

	std::wstring& GetStringRef(const std::size_t& idx)
	{
		return reinterpret_cast<std::wstring*>(&dif)[idx];
	}

	SMTextureList() = default;
	~SMTextureList() = default;
};

class SMSubMeshBase
{
public:
	inline virtual const SMTextureList* GetTexList(const std::string& key, const std::size_t& key_idx) const = 0;
	inline virtual void AddTexList(const std::string& key, const std::size_t& key_idx, SMTextureList* sub_data) = 0;

	inline virtual SMSubMeshType Type() const = 0;

	SMSubMeshBase() = default;
	SMSubMeshBase(const SMSubMeshBase&&) = delete;
	SMSubMeshBase(const SMSubMeshBase&) = delete;
	SMSubMeshBase(SMSubMeshBase&) = delete;
	virtual ~SMSubMeshBase() = default;
};

class SMSubMeshList : public SMSubMeshBase
{
public:
	SMSubMeshList() = default;

	SMSubMeshList(const SMSubMeshList&&) = delete;
	SMSubMeshList(const SMSubMeshList&) = delete;
	SMSubMeshList(SMSubMeshList&) = delete;

	inline ~SMSubMeshList()
	{
		for (std::size_t a = 0; a < m_Storage.size(); a++)
			delete m_Storage[a];
	}

	inline const SMTextureList* GetTexList(const std::string& key, const std::size_t& key_idx) const override
	{
		if (key_idx >= m_Storage.size())
			return nullptr;

		return m_Storage[key_idx];
	}

	inline void AddTexList(const std::string& key, const std::size_t& key_idx, SMTextureList* sub_data) override
	{
		if (m_Storage.size() <= key_idx)
			m_Storage.resize(key_idx + 1, nullptr);

		m_Storage[key_idx] = sub_data;
	}

	inline SMSubMeshType Type() const override
	{
		return SMSubMeshType::SubMeshList;
	}

private:
	std::vector<SMTextureList*> m_Storage = {};
};

class SMSubMeshMap : public SMSubMeshBase
{
	using StorageMap = std::unordered_map<std::string, SMTextureList*>;

public:
	SMSubMeshMap() = default;

	SMSubMeshMap(const SMSubMeshMap&&) = delete;
	SMSubMeshMap(const SMSubMeshMap&) = delete;
	SMSubMeshMap(SMSubMeshMap&) = delete;

	inline ~SMSubMeshMap()
	{
		for (const auto& v_obj : m_Storage)
			delete v_obj.second;
	}

	inline const SMTextureList* GetTexList(const std::string& key, const std::size_t& key_idx) const override
	{
		const StorageMap::const_iterator v_iter = m_Storage.find(key);
		if (v_iter == m_Storage.end())
			return nullptr;

		return v_iter->second;
	}

	inline void AddTexList(const std::string& key, const std::size_t& key_idx, SMTextureList* sub_data) override
	{
		if (m_Storage.find(key) != m_Storage.end())
			return;

		m_Storage.insert(std::make_pair(key, sub_data));
	}

	inline SMSubMeshType Type() const override
	{
		return SMSubMeshType::SubMeshMap;
	}

private:
	StorageMap m_Storage = {};
};

#pragma managed