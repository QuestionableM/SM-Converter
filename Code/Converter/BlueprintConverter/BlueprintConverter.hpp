#pragma once

#include "Converter\Entity\Blueprint.hpp"
#include "Converter\Entity\Body.hpp"

#include "Converter\ConvertError.hpp"

#include <unordered_map>

#pragma unmanaged

enum : int
{
	BPObjectSep_None         = 0,
	BPObjectSep_All          = 1,
	BPObjectSep_Joints       = 2,
	BPObjectSep_Uuid         = 3,
	BPObjectSep_Color        = 4,
	BPObjectSep_UuidAndColor = 5
};


class BlueprintConv
{
	static void WriteToFileInternal(SMBlueprint* blueprint, const std::wstring& bp_name, ConvertError& v_error);

	static void CreateAndAddObjToCollection(SMBlueprint* self, const std::string& v_name, SMEntity* v_entity);

	static void BlueprintAddObject_SeparateAll(SMBlueprint* self, SMEntity* v_entity);
	static void BlueprintAddObject_SeparateJoints(SMBlueprint* self, SMEntity* v_entity);
	static void BlueprintAddObject_SeparateUuid(SMBlueprint* self, SMEntity* v_entity);
	static void BlueprintAddObject_SeparateColor(SMBlueprint* self, SMEntity* v_entity);
	static void BlueprintAddObject_SeparateUuidAndColor(SMBlueprint* self, SMEntity* v_entity);

	static SMBlueprint::AddObjectFunction GetAddObjectFunction();

public:
	static void ConvertToModel(const std::wstring& bp_path, const std::wstring& bp_name, ConvertError& v_error);

private:
	//Is used to group all the objects
	inline static std::unordered_map<std::string, SMBody*> BodyGroupMap = {};
	inline static std::unordered_map<std::size_t, SMBody*> BodyIndexMap = {};

	BlueprintConv() = default;
	BlueprintConv(const BlueprintConv&&) = delete;
	BlueprintConv(const BlueprintConv&) = delete;
	BlueprintConv(BlueprintConv&) = delete;
	~BlueprintConv() = default;
};

#pragma managed