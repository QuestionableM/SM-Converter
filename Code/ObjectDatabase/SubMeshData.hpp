#pragma once

#include "Utils/clr_include.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

SM_UNMANAGED_CODE

struct StringHetHash
{
	using is_transparent = void;

	inline std::size_t operator()(const std::string& str) const noexcept
	{
		return std::hash<std::string>{}(str);
	}

	inline std::size_t operator()(const std::string_view& str) const noexcept
	{
		return std::hash<std::string_view>{}(str);
	}
};

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
	std::wstring dif;
	std::wstring asg;
	std::wstring nor;

	std::string material;
	std::string def_color_idx;
	bool is_shadow_only = false;
};

class SMSubMeshBase
{
public:
	SMSubMeshBase() = default;
	SMSubMeshBase(const SMSubMeshBase&) = delete;
	SMSubMeshBase(SMSubMeshBase&&) = delete;
	virtual ~SMSubMeshBase() = default;

	virtual const SMTextureList* getTexList(const std::string& key, std::size_t key_idx) const = 0;
	virtual void addTexList(const std::string_view& key, std::size_t key_idx, SMTextureList* sub_data) = 0;
	virtual SMSubMeshType type() const = 0;
};

class SMSubMeshList : public SMSubMeshBase
{
public:
	SMSubMeshList() = default;

	SMSubMeshList(const SMSubMeshList&) = delete;
	SMSubMeshList(SMSubMeshList&&) = delete;

	~SMSubMeshList();

	const SMTextureList* getTexList(const std::string& key, std::size_t key_idx) const override;
	void addTexList(const std::string_view& key, std::size_t key_idx, SMTextureList* sub_data) override;

	SMSubMeshType type() const override;

private:
	std::vector<SMTextureList*> m_storage = {};
};

class SMSubMeshMap : public SMSubMeshBase
{
	using StorageMap = std::unordered_map<std::string, SMTextureList*, StringHetHash, std::equal_to<>>;

public:
	SMSubMeshMap() = default;

	SMSubMeshMap(const SMSubMeshMap&) = delete;
	SMSubMeshMap(SMSubMeshMap&&) = delete;

	~SMSubMeshMap();

	const SMTextureList* getTexList(const std::string& key, std::size_t key_idx) const override;
	void addTexList(const std::string_view& key, std::size_t key_idx, SMTextureList* sub_data) override;

	SMSubMeshType type() const override;

private:
	StorageMap m_storage = {};
};

SM_MANAGED_CODE