#pragma once

#include "Utils\Json.hpp"

class Mod;
class ClutterData;
class TextureList;

class ClutterListLoader
{
	ClutterListLoader() = default;

	static bool LoadTextureData(const simdjson::dom::element& fClutter, TextureList& tList, std::wstring& mesh);
	static void LoadClutterData(const simdjson::dom::element& fClutter, ClutterData* pClutter);
public:
	static void Load(const simdjson::dom::element& fClutter, Mod* mod);
};