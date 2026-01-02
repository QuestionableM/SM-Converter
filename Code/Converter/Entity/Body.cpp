#include "Body.hpp"

#include "Converter/BlueprintConverter/BlueprintConverter.hpp"

SM_UNMANAGED_CODE

SMBody::SMBody(const std::string_view& name) :
	SMEntity(),
	m_objects(),
	m_bodyName(name)
{}

SMBody::~SMBody()
{
	for (SMEntity* v_pEntity : m_objects)
		delete v_pEntity;
}

void SMBody::Add(SMEntity* pEntity)
{
	m_objects.push_back(pEntity);
}

SMEntity** SMBody::ResizeAdd(const std::size_t objCount)
{
	const std::size_t v_prevSize = m_objects.size();
	m_objects.resize(v_prevSize + objCount);

	return m_objects.data() + v_prevSize;
}

EntityType SMBody::Type() const
{
	return EntityType::Body;
}

void SMBody::FillTextureMap(EntityTextureMap& textureMap) const
{
	for (const SMEntity* v_entity : m_objects)
		v_entity->FillTextureMap(textureMap);
}

void SMBody::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& offset,
	const glm::mat4& transform) const
{
	std::string v_groupName = "o ";
	v_groupName.append(m_bodyName);

	if (BlueprintConverterSettings::SeparationType == BPObjectSep_All)
	{
		std::string v_finalName;

		const std::size_t v_objectCount = m_objects.size();
		for (std::size_t a = 0; a < v_objectCount; a++)
		{
			v_finalName.assign(v_groupName);
			v_finalName.append(1, '_');
			String::AppendIntegerToString(v_finalName, a + 1);
			v_finalName.append(1, '\n');
			file.write(v_finalName.c_str(), v_finalName.size());

			m_objects[a]->WriteObjectToFile(file, offset, transform);
		}
	}
	else
	{
		v_groupName.append(1, '\n');
		file.write(v_groupName.c_str(), v_groupName.size());

		for (const SMEntity* v_entity : m_objects)
			v_entity->WriteObjectToFile(file, offset, transform);
	}
}

void SMBody::CalculateCenterPoint(glm::vec3& outInput) const
{
	for (const SMEntity* v_entity : m_objects)
		v_entity->CalculateCenterPoint(outInput);
}

std::size_t SMBody::GetAmountOfObjects() const
{
	return m_objects.size();
}