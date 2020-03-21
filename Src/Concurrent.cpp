/* Later delete this file and use Parallel Patterns Library instead */
#include "pch.h"
#include <Windows.h>
#include "Concurrent.h"

namespace Concurrent
{
	CRITICAL_SECTION g_cs;
	bool g_initialized = ([]() { InitializeCriticalSection(&g_cs); return true; })();
}
