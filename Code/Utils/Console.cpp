#include "Console.hpp"

#ifdef SMC_ENABLE_DEBUG_CONSOLE

SM_UNMANAGED_CODE

__ConsoleOutputHandler DebugConsole::Out = __ConsoleOutputHandler();
HANDLE DebugConsole::Handle = NULL;

#endif