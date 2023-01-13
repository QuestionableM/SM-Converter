#pragma once

#include <string>

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
	WritingGndNor = 20
};

class ProgCounter
{
public:
	static ProgState State;
	static std::size_t ProgressMax;
	static std::size_t ProgressValue;

	static void SetState(const ProgState& nState);
	static void SetState(const ProgState& nState, const std::size_t& new_max);

	static std::wstring GetStateString();
	static bool StateHasNumbers();
};