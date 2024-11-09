#pragma once

#include "Utils/clr_include.hpp"

#include "UStd/UnmanagedString.hpp"

SM_UNMANAGED_CODE

#include <atomic>

enum class ProgState : std::size_t
{
	None = 0,
	LoadingVanilla = 1,
	LoadingModded = 2,

	ReadingTile = 3,
	WritingGroundMesh = 4,
	WritingClutter = 5,
	WritingObjects = 6,
	WritingMaterialMaps = 7,
	WritingColorMap = 8,
	WritingMtlFile = 9,

	ConvertSuccess = 10,
	ConvertFailure = 11,

	FillingGndDif = 12,
	SamplingGndDif = 13,
	WritingGndDif = 14,

	FillingGndAsg = 15,
	SamplingGndAsg = 16,
	WritingGndAsg = 17,

	FillingGndNor = 18,
	SamplingGndNor = 19,
	WritingGndNor = 20,

	ParsingBlueprint = 21,
	ReadingBlueprintParts = 22,
	ReadingBlueprintJoints = 23,

	ReadingCells = 24,
	MemoryCleanup = 25
};

class ProgCounter
{
public:
	static std::atomic<ProgState> State; 
	static std::atomic_size_t ProgressMax;
	static std::atomic_size_t ProgressValue;

	static void SetState(ProgState nState);
	static void SetState(ProgState nState, std::size_t new_max);

	static const std::string_view& GetStateString();
	static bool StateHasNumbers();
};

SM_MANAGED_CODE