#pragma once

#include "SubMeshData.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class SMMod;

class AssetData
{
public:
	AssetData(
		const SMUuid& uuid,
		std::wstring&& mesh,
		SMSubMeshBase* pTextures,
		SMMod* pMod
	) :
		m_uuid(uuid),
		m_defaultColors(),
		m_textures(pTextures),
		m_mod(pMod),
		m_mesh(std::move(mesh))
	{}

	AssetData(const AssetData&) = delete;
	AssetData(AssetData&&) = delete;
	
	inline ~AssetData()
	{
		delete m_textures;
	}

public:
	SMUuid m_uuid;
	std::wstring m_mesh;
	std::unordered_map<std::string, SMColor> m_defaultColors;

	SMSubMeshBase* m_textures;
	SMMod* m_mod;
};

class HarvestableData
{
public:
	HarvestableData(
		const SMUuid& uuid,
		std::wstring&& mesh,
		SMSubMeshBase* pTextures,
		SMMod* pMod
	) :
		m_uuid(uuid),
		m_mesh(std::move(mesh)),
		m_textures(pTextures),
		m_mod(pMod)
	{}

	HarvestableData(const HarvestableData&) = delete;
	HarvestableData(HarvestableData&&) = delete;

	inline ~HarvestableData()
	{
		delete m_textures;
	}

public:
	SMUuid m_uuid;
	std::wstring m_mesh;

	SMSubMeshBase* m_textures;
	SMMod* m_mod;
};

class KinematicData
{
public:
	KinematicData(
		const SMUuid& uuid,
		std::wstring&& mesh,
		SMSubMeshBase* pTextures,
		SMMod* pMod
	) :
		m_uuid(uuid),
		m_mesh(std::move(mesh)),
		m_textures(pTextures),
		m_mod(pMod)
	{}

	KinematicData(const KinematicData&) = delete;
	KinematicData(KinematicData&&) = delete;

	inline ~KinematicData()
	{
		delete m_textures;
	}

public:
	SMUuid m_uuid;
	std::wstring m_mesh;

	SMSubMeshBase* m_textures;
	SMMod* m_mod;
};

class BlockData
{
public:
	BlockData(
		const SMUuid& uuid,
		SMTextureList&& texList,
		const std::string_view& defaultColor,
		int tiling,
		SMMod* pMod
	) :
		m_uuid(uuid),
		m_textures(std::move(texList)),
		m_defaultColor(defaultColor),
		m_tiling(tiling),
		m_mod(pMod)
	{}

	BlockData(const BlockData&) = delete;
	BlockData(BlockData&&) = delete;
	~BlockData() = default;

public:
	SMUuid m_uuid;
	SMTextureList m_textures;

	SMColor m_defaultColor;
	int m_tiling;

	SMMod* m_mod;
};

class PartData
{
public:
	PartData(
		const SMUuid& uuid,
		std::wstring&& mesh,
		SMSubMeshBase* pTextures,
		const std::string_view& defaultColor,
		glm::vec3 bounds,
		SMMod* pMod
	) :
		m_uuid(uuid),
		m_mesh(std::move(mesh)),
		m_textures(pTextures),
		m_defaultColor(defaultColor),
		m_bounds(bounds),
		m_mod(pMod)
	{}

	PartData(const PartData&) = delete;
	PartData(PartData&&) = delete;

	inline ~PartData()
	{
		delete m_textures;
	}

public:
	SMUuid m_uuid;
	std::wstring m_mesh;
	SMSubMeshBase* m_textures;
	SMColor m_defaultColor;
	glm::vec3 m_bounds;
	SMMod* m_mod;
};

struct ClutterDataParams
{
	float m_height;
	float m_scaleVariance;
	bool m_groundNormal;
};

class ClutterData
{
public:
	ClutterData(
		const SMUuid& uuid,
		std::wstring&& path,
		SMTextureList&& texList,
		ClutterDataParams params,
		SMMod* pMod
	) :
		m_uuid(uuid),
		m_mesh(std::move(path)),
		m_textures(std::move(texList)),
		m_height(params.m_height),
		m_scaleVariance(params.m_scaleVariance),
		m_groundNormal(params.m_groundNormal),
		m_mod(pMod)
	{}

	ClutterData(const ClutterData&) = delete;
	ClutterData(ClutterData&&) = delete;
	~ClutterData() = default;

public:
	SMUuid m_uuid;
	std::wstring m_mesh;
	SMTextureList m_textures;
	float m_height;
	float m_scaleVariance;
	bool m_groundNormal;
	SMMod* m_mod;
};

/*
v_new_decal->m_name.assign(v_decal.key);
		v_new_decal->m_uuid = v_decal_uuid;
		v_new_decal->m_textures = v_texList;
		v_new_decal->m_mod = mod;
*/

class DecalData
{
public:
	DecalData(
		const SMUuid& uuid,
		const std::string_view& name,
		const SMTextureList& texList,
		SMMod* pMod
	) :
		m_uuid(uuid),
		m_name(name),
		m_textures(texList),
		m_ranges(),
		m_mod(pMod)
	{}

	DecalData(const DecalData&) = delete;
	DecalData(DecalData&&) = delete;
	~DecalData() = default;

public:
	SMUuid m_uuid;
	std::string m_name;
	SMTextureList m_textures;
	int m_ranges[4];
	SMMod* m_mod;
};

class GarmentData
{
public:
	GarmentData(
		const SMUuid& uuid,
		std::wstring&& maleMesh,
		std::wstring&& femaleMesh,
		SMSubMeshBase* maleData,
		SMSubMeshBase* femaleData
	) :
		m_uuid(uuid),
		m_male_mesh(std::move(maleMesh)),
		m_female_mesh(std::move(femaleMesh)),
		m_male_data(maleData),
		m_female_data(femaleData)
	{}

	GarmentData(const GarmentData&) = delete;
	GarmentData(GarmentData&&) = delete;

	inline ~GarmentData()
	{
		delete m_male_data;
		delete m_female_data;
	}

public:
	SMUuid m_uuid;

	std::wstring m_male_mesh;
	std::wstring m_female_mesh;

	SMSubMeshBase* m_male_data;
	SMSubMeshBase* m_female_data;
};

SM_MANAGED_CODE