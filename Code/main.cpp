#include "Utils\WinInclude.hpp"
#include "Utils\Console.hpp"
#include "Gui\MainGui.h"

#pragma unmanaged
#include <locale>
#pragma managed

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    std::setlocale(LC_CTYPE, "en_US.UTF-8");
    CreateDebugConsole(L"World Converter Debug Console");

    SetProcessDPIAware();

    SMConverter::Application::EnableVisualStyles();
    SMConverter::Application::SetCompatibleTextRenderingDefault(true);
    SMConverter::Application::Run(gcnew SMConverter::MainGui());

	return 0;
}