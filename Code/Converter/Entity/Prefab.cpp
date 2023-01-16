#include "Prefab.hpp"

#include "ObjectDatabase\ModelStorage.hpp"

#pragma unmanaged

std::string SMPrefab::GetMtlName(const std::string& mat_name, const std::size_t& mIdx) const
{
	return "PREFAB_NOT_IMPLEMENTED";
}

void SMPrefab::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const glm::mat4 prefab_matrix = transform_matrix * this->GetTransformMatrix();

	for (const SMEntity* pEntity : this->Objects)
		pEntity->WriteObjectToFile(file, mOffset, prefab_matrix);
}

void SMPrefab::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	for (const SMEntity* pEntity : this->Objects)
		pEntity->FillTextureMap(tex_map);
}

std::size_t SMPrefab::GetAmountOfObjects() const
{
	std::size_t output = 0;

	for (const SMEntity* tEntity : this->Objects)
		output += tEntity->GetAmountOfObjects();

	return output;
}