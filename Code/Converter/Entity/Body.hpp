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
	EntityType Type() const noexcept override;

	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;

	void CalculateCenterPoint(glm::vec3& v_input) const override;
	std::size_t GetAmountOfObjects() const override;

private:
	std::vector<SMEntity*> m_objects = {};
	std::string m_bodyName;
};

SM_MANAGED_CODE