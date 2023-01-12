#include "Console.hpp"

#ifdef SMC_ENABLE_DEBUG_CONSOLE

#pragma unmanaged

__ConsoleOutputHandler DebugConsole::Out = __ConsoleOutputHandler();
HANDLE DebugConsole::Handle = NULL;

#endif