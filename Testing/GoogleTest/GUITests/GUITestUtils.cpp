#include "pch.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

namespace GUITestUtils
{


time_t getStartTime()
{
	static time_t g_startTime = time(nullptr);
	return g_startTime;
}

bool saveWindowImageAsPNG(HWND hwnd, const std::filesystem::path& filename)
{
	RECT rc{};
	DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(RECT));
	CImage img;
	img.Create(rc.right - rc.left, rc.bottom - rc.top, 24);
	HDC hdc = GetDC(nullptr);
	BitBlt(img.GetDC(), 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, rc.left, rc.top, SRCCOPY);
	img.ReleaseDC();
	ReleaseDC(nullptr, hdc);
	return SUCCEEDED(img.Save(filename.c_str(), Gdiplus::ImageFormatPNG));
}

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

void waitUntilProcessExit(HWND hwnd)
{
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, dwProcessId);
	if (!hProcess)
		return;

	WaitForSingleObject(hProcess, INFINITE);
	CloseHandle(hProcess);
}

bool isMenuItemChecked(HWND hwnd, int id)
{
	MENUITEMINFO mii{sizeof(MENUITEMINFO)};
	mii.fMask = MIIM_STATE;
	if (GetMenuItemInfo(GetMenu(hwnd), id, false, &mii))
		return mii.fState == MFS_CHECKED;
	return false;
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
	size_t len = wcslen(text);
	for (size_t i = 0; i < len; ++i)
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

void typeKey(unsigned char vk)
{
	keybd_event(vk, 0, 0, 0);
	keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
}

std::filesystem::path getModuleFileName()
{
	wchar_t szPath[256];
	GetModuleFileName(nullptr, szPath, static_cast<DWORD>(std::size(szPath)));
	return szPath;
}

std::filesystem::path getModuleFolder()
{
	return getModuleFileName().parent_path();
}

void waitUntilClose(HWND hwnd)
{
	while (IsWindow(hwnd)) Sleep(100);
}

void waitUntilFocus(HWND hwnd)
{
	while (GetForegroundWindow() != hwnd) Sleep(100);
}

void selectMenu(HWND hwnd, unsigned id, bool async)
{
	if (async)
		PostMessage(hwnd, WM_COMMAND, id, 0);
	else
	{
		SendMessage(hwnd, WM_COMMAND, id, 0);
	}
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

const std::set<int> languages()
{
	if (wcsstr(GetCommandLineW(), L"--all-languages") == nullptr)
		return { MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) };
	return {
MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA),
MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_CATALAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL),
MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT),
MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT),
MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH),
MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT),
MAKELANGID(LANG_FINNISH, SUBLANG_FINNISH_FINLAND),
MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH),
MAKELANGID(LANG_GALICIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),
MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT),
MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN),
MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT),
MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_LITHUANIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_NORWEGIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT),
MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE),
MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN),
MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT),
MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_CYRILLIC),
MAKELANGID(LANG_SINHALESE, SUBLANG_DEFAULT),
MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT),
MAKELANGID(LANG_SLOVENIAN, SUBLANG_SLOVENIAN_SLOVENIA),
MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN),
MAKELANGID(LANG_SWEDISH, SUBLANG_DEFAULT),
MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT),
MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT),
	};
}

}

