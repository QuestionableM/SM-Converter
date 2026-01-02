#pragma once

#include "Converter/WriterOffset.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class SMPrefab final : public SMEntity
{
public:
	SMPrefab(const std::wstring_view& path, const std::wstring_view& flag, const SMEntityTransform& transform);
	SMPrefab(const SMPrefab&) = delete;
	SMPrefab(SMPrefab&) = delete;
	~SMPrefab();

	const std::wstring& getPath() const noexcept;
	const std::wstring& getFlag() const noexcept;
	const std::vector<SMEntity*>& getObjects() const noexcept;

	void addObject(SMEntity* object);

	EntityType Type() const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transform) const override;
	std::size_t GetAmountOfObjects() const override;

private:
	std::wstring m_path;
	std::wstring m_flag;

	// This vector contains blueprints, prefabs and assets
	std::vector<SMEntity*> m_objects;
};

SM_MANAGED_CODE