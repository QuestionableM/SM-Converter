#pragma once

#include "Entity.hpp"

#include "Converter/ConvertSettings.hpp"
#include "Utils/clr_include.hpp"

#include "UStd/UnmanagedFstream.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

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

private:
	std::vector<SMEntity*> m_objects;
	std::string m_bodyName;
};

SM_MANAGED_CODE