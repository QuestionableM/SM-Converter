#pragma once

struct TileConverterSettings
{
	inline static bool Export8kGroundTextures = false;
	inline static bool ExportGroundTextures = true;
	inline static bool ExportHarvestables = true;
	inline static bool ExportBlueprints = true;
	inline static bool ExportClutter = true;
	inline static bool ExportPrefabs = true;
	inline static bool ExportAssets = true;
	inline static bool ExportDecals = true;
};

struct BlueprintConverterSettings
{

};

struct SharedConverterSettings
{
	inline static bool ExportMaterials = true;
	inline static bool ExportNormals   = true;
	inline static bool ExportUvs       = true;

private:
	SharedConverterSettings() = default;
	SharedConverterSettings(const SharedConverterSettings&&) = delete;
	SharedConverterSettings(SharedConverterSettings&&) = delete;
	SharedConverterSettings(SharedConverterSettings&) = delete;
	~SharedConverterSettings() = default;
};