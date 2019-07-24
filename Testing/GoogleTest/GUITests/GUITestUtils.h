#include <Windows.h>
#include <filesystem>

namespace GUITestUtils
{
	DWORD waitForInputIdleByHWND(HWND hwnd, DWORD dwMilliseconds = WAIT_TIMEOUT);
	HWND findForegroundDialog();
	void typeText(const wchar_t *text);
	void typeAltPlusKey(char key);
	std::filesystem::path getModuleFileName();
	void waitUntilClose(HWND hwnd);
	void waitUntilFocus(HWND hwnd);
	void selectMenu(HWND hwnd, unsigned id);
	HWND execWinMerge(const std::string& args = "/noprefs /maxmize");
}
