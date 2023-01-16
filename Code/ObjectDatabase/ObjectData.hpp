#pragma once

#include "SubMeshData.hpp"

#include "Utils\GlmUnmanaged.hpp"
#include "Utils\Color.hpp"
#include "Utils\Uuid.hpp"

#include <unordered_map>
#include <string>

#pragma unmanaged

class Mod;

class AssetData
{
public:
	SMUuid m_uuid;
	std::unordered_map<std::string, SMColor> m_defaultColors;
	SMSubMeshBase* m_textures;
	Mod* m_mod;
	std::wstring m_mesh;

	AssetData() = default;
	AssetData(const AssetData&) = delete;
	AssetData(AssetData&&) = delete;
	
	inline ~AssetData()
	{
		delete m_textures;
	}
};

class HarvestableData
{
public:
	SMUuid m_uuid;
	SMSubMeshBase* m_textures;
	Mod* m_mod;
	std::wstring m_mesh;

	HarvestableData() = default;
	HarvestableData(const HarvestableData&) = delete;
	HarvestableData(HarvestableData&) = delete;

	inline ~HarvestableData()
	{
		delete m_textures;
	}
};

class BlockData
{
public:
	SMUuid m_uuid;
	SMTextureList m_textures;
	Mod* m_mod;
	SMColor m_defaultColor;
	int m_tiling;

	BlockData() = default;
	BlockData(const BlockData&) = delete;
	BlockData(BlockData&) = delete;
	~BlockData() = default;
};

class PartData
{
public:
	SMUuid m_uuid;
	SMSubMeshBase* m_textures;
	SMColor m_defaultColor;
	glm::vec3 m_bounds;
	std::wstring m_mesh;
	Mod* m_mod;

	PartData() = default;
	PartData(const PartData&) = delete;
	PartData(PartData&) = delete;

	inline ~PartData()
	{
		delete m_textures;
	}
};

class ClutterData
{
public:
	SMUuid m_uuid;
	SMTextureList m_textures;
	std::wstring m_mesh;
	float m_height;
	float m_scaleVariance;
	bool m_groundNormal;
	Mod* m_mod;

	ClutterData() = default;
	ClutterData(const ClutterData&) = delete;
	ClutterData(ClutterData&) = delete;
	~ClutterData() = default;
};

class DecalData
{
public:
	std::string m_name;
	SMUuid m_uuid;
	SMTextureList m_textures;
	int m_ranges[4];
	Mod* m_mod;

	DecalData() = default;
	DecalData(const DecalData&) = delete;
	DecalData(DecalData&) = delete;
	~DecalData() = default;
};

#pragma managed