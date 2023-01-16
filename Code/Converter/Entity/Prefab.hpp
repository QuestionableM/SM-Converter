#pragma once

#include "Converter\WriterOffset.hpp"
#include "Entity.hpp"

#pragma unmanaged

class SMPrefab : public SMEntity
{
public:
	SMPrefab(const SMPrefab&) = delete;
	SMPrefab(SMPrefab&) = delete;

	inline SMPrefab(const std::wstring& path, const std::wstring& flag = L"")
	{
		this->path = path;
		this->flag = flag;
	}

	inline ~SMPrefab()
	{
		for (SMEntity*& pObject : this->Objects)
			delete pObject;
	}

	inline std::wstring GetPath() const { return this->path; }
	inline std::wstring GetFlag() const { return this->flag; }
	inline std::vector<SMEntity*> GetObjects() const { return this->Objects; }

	inline void AddObject(SMEntity* object)
	{
		//Check if the object is valid (Valid objects: Harvestable, Blueprint, Prefab, Asset)
		assert((static_cast<unsigned short>(object->Type()) & 0b100011011) != 0);

		this->Objects.push_back(object);
	}

	inline EntityType Type() const override { return EntityType::Prefab; }
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;
	std::size_t GetAmountOfObjects() const override;

private:
	std::wstring path;
	std::wstring flag;

	//This vector contains blueprints, prefabs and assets
	std::vector<SMEntity*> Objects = {};
};

#pragma managed