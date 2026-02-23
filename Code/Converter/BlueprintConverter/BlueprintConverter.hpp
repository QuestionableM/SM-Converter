#pragma once

#include "ObjectDatabase/Mods/CustomGameMod.hpp"

#include "Converter/Entity/Blueprint.hpp"
#include "Converter/Entity/Body.hpp"
#include "Converter/ConvertError.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"

SM_UNMANAGED_CODE

enum : std::uint8_t
{
	BPObjectSep_None         = 0,
	BPObjectSep_All          = 1,
	BPObjectSep_Shapes       = 2,
	BPObjectSep_Joints       = 3,
	BPObjectSep_Uuid         = 4,
	BPObjectSep_Color        = 5,
	BPObjectSep_UuidAndColor = 6
};


class BlueprintConv
{
	static void WriteToFileInternal(SMBlueprint* pBlueprint, const std::wstring& blueprintName, ConvertError& error);

	static SMBody* CreateCollection(SMBlueprint* self, const std::string_view& name);

	static void CreateAndAddObjToCollection(SMBlueprint* self, const std::string_view& name, SMEntity* pEntity);
	//A version that doesn't fill body index map, as it is only used in joint separation mode
	static void CreateAndAddObjToCollectionNI(SMBlueprint* self, const std::string_view& name, SMEntity* pEntity);

	static void BlueprintAddObject_SeparateAll(SMBlueprint* self, SMEntity* pEntity);
	static void BlueprintAddObject_SeparateShapes(SMBlueprint* self, SMEntity* pEntity);
	static void BlueprintAddObject_SeparateJoints(SMBlueprint* self, SMEntity* pEntity);
	static void BlueprintAddObject_SeparateUuid(SMBlueprint* self, SMEntity* pEntity);
	static void BlueprintAddObject_SeparateColor(SMBlueprint* self, SMEntity* pEntity);
	static void BlueprintAddObject_SeparateUuidAndColor(SMBlueprint* self, SMEntity* pEntity);

	static SMBlueprint::AddObjectFunction GetAddObjectFunction();

public:
	static void ConvertToModel(
		const std::wstring& bp_path,
		const std::wstring& bp_name,
		ConvertError& error,
		CustomGame* pCustomGame);

private:
	//Is used to group all the objects
	inline static std::unordered_map<std::string, SMBody*, Hashing::StringHasher, std::equal_to<>> BodyGroupMap = {};
	inline static std::unordered_map<std::size_t, SMBody*> BodyIndexMap = {};

	BlueprintConv() = default;
	BlueprintConv(const BlueprintConv&) = delete;
	BlueprintConv(BlueprintConv&) = delete;
	~BlueprintConv() = default;
};

SM_MANAGED_CODE