#pragma once

#include "Utils\Color.hpp"
#include "Utils\Uuid.hpp"

#include <unordered_map>
#include <string>

#include <glm.hpp>

#pragma unmanaged

enum class TextureDataType
{
	SubMeshList,
	SubMeshMap,
	None
};

class TextureList
{
public:
	std::wstring dif;
	std::wstring asg;
	std::wstring nor;

	std::wstring material;
	std::wstring def_color_idx;

	std::wstring& GetStringRef(const std::size_t& idx);

	TextureList() = default;
	~TextureList() = default;
};

class TextureData
{
	std::unordered_map<std::wstring, TextureList> MaterialMap = {};
	TextureDataType type = TextureDataType::None;

public:
	void AddEntry(const std::wstring& name, const TextureList& tex_list);
	bool GetEntry(const std::wstring& name, TextureList& list_ref) const;

	TextureDataType Type() const;

	TextureData() = default;
	TextureData(const TextureDataType& type);
	~TextureData() = default;
};

class Mod;

class AssetData
{
public:
	SMUuid Uuid;
	std::unordered_map<std::wstring, SMColor> DefaultColors;
	TextureData Textures;
	Mod* pMod;
	std::wstring Mesh;

	AssetData() = default;
	AssetData(const AssetData&) = delete;
	AssetData(AssetData&&) = delete;
	~AssetData() = default;
};

class HarvestableData
{
public:
	SMUuid Uuid;
	TextureData Textures;
	Mod* pMod;
	std::wstring Mesh;

	HarvestableData() = default;
	HarvestableData(const HarvestableData&) = delete;
	HarvestableData(HarvestableData&) = delete;
	~HarvestableData() = default;
};

class BlockData
{
public:
	SMUuid Uuid;
	TextureList Textures;
	Mod* pMod;
	SMColor DefaultColor;
	int Tiling;

	BlockData() = default;
	BlockData(const BlockData&) = delete;
	BlockData(BlockData&) = delete;
	~BlockData() = default;
};

class PartData
{
public:
	SMUuid Uuid;
	TextureData Textures;
	SMColor DefaultColor;
	glm::vec3 Bounds;
	std::wstring Mesh;
	Mod* pMod;

	PartData() = default;
	PartData(const PartData&) = delete;
	PartData(PartData&) = delete;
	~PartData() = default;
};

class ClutterData
{
public:
	SMUuid Uuid;
	TextureList Textures;
	std::wstring Mesh;
	float Height;
	float ScaleVariance;
	bool GroundNormal;
	Mod* pMod;

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
	TextureList m_textures;
	int m_ranges[4];
	Mod* m_mod;

	DecalData() = default;
	DecalData(const DecalData&) = delete;
	DecalData(DecalData&) = delete;
	~DecalData() = default;
};

#pragma managed