#include "Gui\MainGui.h"

#include <Windows.h>

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    SMConverter::Application::EnableVisualStyles();
    SMConverter::Application::SetCompatibleTextRenderingDefault(false);
    SMConverter::Application::Run(gcnew SMConverter::MainGui());

	return 0;
}