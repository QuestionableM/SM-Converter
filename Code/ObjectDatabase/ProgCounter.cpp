#include "ProgCounter.hpp"

#pragma unmanaged

std::atomic<ProgState> ProgCounter::State = ProgState::None;
std::atomic_size_t ProgCounter::ProgressMax   = 0;
std::atomic_size_t ProgCounter::ProgressValue = 0;

void ProgCounter::SetState(ProgState nState)
{
	ProgCounter::State = nState;
}

void ProgCounter::SetState(ProgState nState, std::size_t new_max)
{
	ProgCounter::State = nState;

	ProgCounter::ProgressValue = 0;
	ProgCounter::ProgressMax = new_max;
}

struct StateData
{
	std::wstring Description;
	bool DisplayNumbers;
};

static const StateData StateDataArray[] =
{
	{ L"Loading Vanilla Objects... ", true },
	{ L"Loading Modded Objects... ",  true },

	{ L"Reading Tile...",          false },
	{ L"Writing Ground Mesh...",   false },
	{ L"Writing Clutter...  ",     true  },
	{ L"Writing Objects... ",      true  },
	{ L"Writing Material Maps...", false },
	{ L"Writing Color Map...",     false },
	{ L"Writing Mtl File...",      true  },

	{ L"Success!",       false },
	{ L"Convert Failed", false },

	{ L"Filling Ground Dif Texture...",   false },
	{ L"Sampling Ground Dif Texture... ", true  },
	{ L"Writing Ground Dif Texture...",   false },

	{ L"Filling Ground Asg Texture...",   false },
	{ L"Sampling Ground Asg Texture... ", true  },
	{ L"Writing Ground Asg Texture...",   false },

	{ L"Filling Ground Nor Texture...",   false },
	{ L"Sampling Ground Nor Texture... ", true  },
	{ L"Writing Ground Nor Texture...",   false },

	{ L"Parsing Blueprint...", false },
	{ L"Reading Parts...",     true  },
	{ L"Reading Joints...",    true  },

	{ L"Reading Cells...", true },
	{ L"Memory cleanup...", false }
};

const static std::wstring g_no_state_string = L"NO_STATE";
const std::wstring& ProgCounter::GetStateString()
{
	const std::size_t state_idx = static_cast<std::size_t>(ProgCounter::State.load());
	if (state_idx > 0)
	{
		return StateDataArray[state_idx - 1].Description;
	}

	return g_no_state_string;
}

bool ProgCounter::StateHasNumbers()
{
	const std::size_t state_idx = static_cast<std::size_t>(ProgCounter::State.load());
	if (state_idx > 0)
	{
		return StateDataArray[state_idx - 1].DisplayNumbers;
	}

	return false;
}