#pragma once

#include "Converter/WriterOffset.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class SMPrefab final : public SMEntity
{
public:
	SMPrefab(const SMPrefab&) = delete;
	SMPrefab(SMPrefab&) = delete;

	SMPrefab(const std::wstring_view& path, const std::wstring_view& flag, const SMEntityTransform& transform);
	~SMPrefab();

	const std::wstring& getPath() const;
	const std::wstring& getFlag() const;
	const std::vector<SMEntity*>& getObjects() const;

	void addObject(SMEntity* object);

	inline EntityType Type() const noexcept override { return EntityType::Prefab; }
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;
	std::size_t GetAmountOfObjects() const override;

private:
	std::wstring m_path;
	std::wstring m_flag;

	// This vector contains blueprints, prefabs and assets
	std::vector<SMEntity*> m_objects;
};

SM_MANAGED_CODE