#include "WineDetect.hpp"
#include "WinInclude.hpp"

bool WineDetect::IsRunningUnderWine()
{
	HMODULE v_hmod = GetModuleHandleA("ntdll.dll");
	if (!v_hmod) return false;

	return GetProcAddress(v_hmod, "wine_get_version") != nullptr;
}