#pragma once

#include "Entity.hpp"

#include "Converter/ConvertSettings.hpp"
#include "Utils/clr_include.hpp"

#include "UStd/UnmanagedFstream.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include <unordered_map>

SM_UNMANAGED_CODE

class SMBody : public SMEntity
{
public:
	SMBody(const std::string_view& name);
	~SMBody();

	void Add(SMEntity* pEntity);
	// Returns the pointer to the beginning of added area for fast insertion
	SMEntity** ResizeAdd(const std::size_t objCount);

	EntityType Type() const override;

	void FillTextureMap(EntityTextureMap& textureMap) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transform) const override;

	void CalculateCenterPoint(glm::vec3& outInput) const override;
	std::size_t GetAmountOfObjects() const override;

	void ApplyPreTransformByBodyMap(
		const std::unordered_map<std::size_t, glm::mat4>& bodyTransforms,
		const std::unordered_map<std::size_t, std::size_t>& childToBodyMap);

private:
	std::vector<SMEntity*> m_objects;
	std::string m_bodyName;
};

SM_MANAGED_CODE