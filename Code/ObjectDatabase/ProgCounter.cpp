#include "ProgCounter.hpp"

SM_UNMANAGED_CODE

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
	std::string_view description;
	bool displayNumbers;
};

static const StateData g_stateArray[] =
{
	{ "NO_STATE", false },

	{ "Loading Vanilla Objects... ", true },
	{ "Loading Modded Objects... ",  true },

	{ "Reading Tile...",          false },
	{ "Writing Ground Mesh...",   false },
	{ "Writing Clutter...  ",     true  },
	{ "Writing Objects... ",      true  },
	{ "Writing Material Maps...", false },
	{ "Writing Color Map...",     false },
	{ "Writing Mtl File...",      true  },

	{ "Success!",       false },
	{ "Convert Failed", false },

	{ "Filling Ground Dif Texture...",   false },
	{ "Sampling Ground Dif Texture... ", true  },
	{ "Writing Ground Dif Texture...",   false },

	{ "Filling Ground Asg Texture...",   false },
	{ "Sampling Ground Asg Texture... ", true  },
	{ "Writing Ground Asg Texture...",   false },

	{ "Filling Ground Nor Texture...",   false },
	{ "Sampling Ground Nor Texture... ", true  },
	{ "Writing Ground Nor Texture...",   false },

	{ "Parsing Blueprint...", false },
	{ "Reading Parts...",     true  },
	{ "Reading Joints...",    true  },

	{ "Reading Cells..." , true  },
	{ "Memory cleanup...", false }
};

const std::string_view& ProgCounter::GetStateString()
{
	return g_stateArray[static_cast<std::size_t>(ProgCounter::State.load())].description;
}

bool ProgCounter::StateHasNumbers()
{
	return g_stateArray[static_cast<std::size_t>(ProgCounter::State.load())].displayNumbers;
}