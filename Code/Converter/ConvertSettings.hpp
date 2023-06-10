#pragma once

#pragma unmanaged

struct TileConverterSettings
{
	inline static bool Export8kGroundTextures = false;
	inline static bool ExportGroundTextures = true;
	inline static bool ExportHarvestables = true;
	inline static bool ExportBlueprints = true;
	inline static bool ExportKinematics = true;
	inline static bool ExportClutter = true;
	inline static bool ExportPrefabs = true;
	inline static bool ExportAssets = true;
	inline static bool ExportDecals = false;

private:
	TileConverterSettings() = default;
	TileConverterSettings(const TileConverterSettings&) = delete;
	TileConverterSettings(TileConverterSettings&&) = delete;
	~TileConverterSettings() = default;
};

struct BlueprintConverterSettings
{
	inline static int SeparationType = 0;

private:
	BlueprintConverterSettings() = default;
	BlueprintConverterSettings(const BlueprintConverterSettings&) = delete;
	BlueprintConverterSettings(BlueprintConverterSettings&&) = delete;
	~BlueprintConverterSettings() = default;
};

struct SharedConverterSettings
{
	inline static bool ExportMaterials = true;
	inline static bool ExportNormals   = true;
	inline static bool ExportUvs       = true;

private:
	SharedConverterSettings() = default;
	SharedConverterSettings(const SharedConverterSettings&) = delete;
	SharedConverterSettings(SharedConverterSettings&&) = delete;
	~SharedConverterSettings() = default;
};

#pragma managed