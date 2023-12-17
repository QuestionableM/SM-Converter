#include "Entity.hpp"

#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#pragma unmanaged

void SMEntityWithModel::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& mOffset,
	const glm::mat4& transform_matrix) const
{
	const glm::mat4 model_matrix = transform_matrix * this->GetTransformMatrix();

	m_model->WriteToFile(model_matrix, mOffset, file, this);

	ProgCounter::ProgressValue++;
}