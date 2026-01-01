#pragma once

#include "Utils/WinInclude.hpp"
#include "Utils/Hashing.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

SM_UNMANAGED_CODE

enum class SMSubMeshType : std::uint8_t
{
	SubMeshList = 0,
	SubMeshMap = 1
};

class SMTextureList
{
public:
	SMTextureList() = default;
	~SMTextureList() = default;

	SMTextureList(const SMTextureList& other) = default;
	SMTextureList(SMTextureList&& other) noexcept;

	void operator=(const SMTextureList& other);
	void operator=(SMTextureList&& other) noexcept;

	// Should only go from 0 to 2
	std::wstring& getStringRef(std::size_t idx) noexcept;

public:
	std::wstring m_dif;
	std::wstring m_asg;
	std::wstring m_nor;

	std::string m_material;
	std::string m_defColorIdx;
	bool m_shadowOnly = false;
};

class SMC_NOVTABLE SMSubMeshBase
{
public:
	SMSubMeshBase() = default;
	SMSubMeshBase(const SMSubMeshBase&) = delete;
	SMSubMeshBase(SMSubMeshBase&&) = delete;
	virtual ~SMSubMeshBase() = default;

	virtual const SMTextureList* getTexList(const std::string_view& key, const std::size_t keyIdx) const = 0;
	virtual void addTexList(const std::string_view& key, const std::size_t keyIdx, SMTextureList* pTexList) = 0;
	virtual SMSubMeshType type() const = 0;
};

class SMSubMeshList final : public SMSubMeshBase
{
public:
	SMSubMeshList() = default;
	SMSubMeshList(const SMSubMeshList&) = delete;
	SMSubMeshList(SMSubMeshList&&) = delete;
	virtual ~SMSubMeshList();

	const SMTextureList* getTexList(const std::string_view& key, const std::size_t keyIdx) const override;
	void addTexList(const std::string_view& key, const std::size_t keyIdx, SMTextureList* pTexList) override;

	SMSubMeshType type() const override;

private:
	std::vector<SMTextureList*> m_storage;
};

class SMSubMeshMap final : public SMSubMeshBase
{
	using StorageMap = std::unordered_map<std::string, SMTextureList*, Hashing::StringHasher, std::equal_to<>>;

public:
	SMSubMeshMap();
	SMSubMeshMap(const SMSubMeshMap&) = delete;
	SMSubMeshMap(SMSubMeshMap&&) = delete;
	virtual ~SMSubMeshMap();

	const SMTextureList* getTexList(const std::string_view& key, const std::size_t keyIdx) const override;
	void addTexList(const std::string_view& key, const std::size_t keyIdx, SMTextureList* pTexList) override;

	SMSubMeshType type() const override;

private:
	StorageMap m_storage;
};

SM_MANAGED_CODE