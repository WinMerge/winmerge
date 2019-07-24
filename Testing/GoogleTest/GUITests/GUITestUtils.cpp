#include "pch.h"

namespace GUITestUtils
{

DWORD waitForInputIdleByHWND(HWND hwnd, DWORD dwMilliseconds)
{
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
	if (!hProcess)
		return WAIT_FAILED;

	DWORD dwResult = WaitForInputIdle(hProcess, dwMilliseconds);
	CloseHandle(hProcess);
	return dwResult;
}

HWND findForegroundDialog()
{
	HWND hwnd = nullptr;
	for (int i = 0; i < 500; ++i)
	{
		hwnd = FindWindow(L"#32770", nullptr);
		if (hwnd && GetForegroundWindow() == hwnd)
			break;
		Sleep(100);
	}
	return hwnd;
}

void typeText(const wchar_t *text)
{
	for (size_t i = 0; i < wcslen(text); ++i)
	{
		SHORT r = VkKeyScan(text[i]);
		if (HIBYTE(r) == 1)
			keybd_event(VK_SHIFT, 0, 0, 0);
		keybd_event(LOBYTE(r), 0, 0, 0);
		keybd_event(LOBYTE(r), 0, KEYEVENTF_KEYUP, 0);
		if (HIBYTE(r) == 1)
			keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	}
}

void typeAltPlusKey(char key)
{
	keybd_event(VK_MENU, 0, 0, 0);
	keybd_event(key, 0, 0, 0);
	keybd_event(key, KEYEVENTF_KEYUP, 0, 0);
	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
}

std::filesystem::path getModuleFileName()
{
	wchar_t szPath[256];
	GetModuleFileName(nullptr, szPath, static_cast<DWORD>(std::size(szPath)));
	return szPath;
}

void waitUntilClose(HWND hwnd)
{
	while (IsWindow(hwnd)) Sleep(100);
}

void waitUntilFocus(HWND hwnd)
{
	while (GetForegroundWindow() != hwnd) Sleep(100);
}

void selectMenu(HWND hwnd, unsigned id)
{
	PostMessage(hwnd, WM_COMMAND, id, 0);
	waitForInputIdleByHWND(hwnd);
}

HWND execWinMerge(const std::string& args)
{
	HWND hwndWinMerge = nullptr;
	system(("start \"\" \"C:\\Program Files\\WinMerge\\WinMergeU.exe\" " + args).c_str());
	for (int i = 0; i < 50 && !hwndWinMerge; ++i)
	{
		hwndWinMerge = FindWindow(L"WinMergeWindowClassW", nullptr);
		Sleep(100);
	}
	if (hwndWinMerge)
	{
		SwitchToThisWindow(hwndWinMerge, TRUE);
		waitForInputIdleByHWND(hwndWinMerge);
	}
	return hwndWinMerge;
}

}
