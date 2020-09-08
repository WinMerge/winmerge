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

void nextControl()
{
	keybd_event(VK_TAB, 0, 0, 0);
	keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
}

void prevControl()
{
	keybd_event(VK_SHIFT, 0, 0, 0);
	keybd_event(VK_TAB, 0, 0, 0);
	keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
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

std::filesystem::path getExecutablePath()
{
	int argc;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	for (int arg = 0; arg < argc; ++arg)
	{
		auto argstr = std::wstring(argv[arg]);
		size_t pos = argstr.find(_T("--exepath="), 0);
		if (pos != std::wstring::npos)
			return argstr.substr(pos + std::size("--exepath=") - 1);
	}
	return "C:\\Program Files\\WinMerge\\WinMergeU.exe";
}

std::filesystem::path getInstallerPath()
{
	int argc;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	for (int arg = 0; arg < argc; ++arg)
	{
		auto argstr = std::wstring(argv[arg]);
		size_t pos = argstr.find(_T("--installerpath="), 0);
		if (pos != std::wstring::npos)
			return argstr.substr(pos + std::size("--installerpath=") - 1);
	}
	return "../../../Build/WinMerge-2.16.7.0-x64-PerUser-Setup.exe";
}

HWND execWinMerge(const std::string& args)
{
	HWND hwndWinMerge = nullptr;
	auto command = "start \"\" \"" + getExecutablePath().string() + "\" " + args;
	system(command.c_str());
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

HWND execInstaller(const std::string& args)
{
	HWND hwndInstaller = nullptr;
	auto command = "start \"\" \"" + getInstallerPath().string() + "\" " + args;
	system(command.c_str());
	Sleep(3000);
	for (int i = 0; i < 50 && !hwndInstaller; ++i)
	{
		hwndInstaller = FindWindow(L"TWizardForm", nullptr);
		Sleep(100);
	}
	if (hwndInstaller)
	{
		SwitchToThisWindow(hwndInstaller, TRUE);
		waitForInputIdleByHWND(hwndInstaller);
	}
	return hwndInstaller;
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

const char* languageIdToName(int id)
{
	static const std::map<int, const char*> map {
{MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),"ENGLISH"},
{MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_SAUDI_ARABIA),"ARABIC"},
{MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT),"BULGARIAN"},
{MAKELANGID(LANG_CATALAN, SUBLANG_DEFAULT),"CATALAN"},
{MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),"CHINESE_SIMPLIFIED"},
{MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL),"CHINESE_TRADITIONAL"},
{MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT),"CROATIAN"},
{MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT),"CZECH"},
{MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT),"DANISH"},
{MAKELANGID(LANG_DUTCH, SUBLANG_DUTCH),"DUTCH"},
{MAKELANGID(LANG_FARSI, SUBLANG_DEFAULT),"PERSIAN"},
{MAKELANGID(LANG_FINNISH, SUBLANG_FINNISH_FINLAND),"FINNISH"},
{MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH),"FRENCH"},
{MAKELANGID(LANG_GALICIAN, SUBLANG_DEFAULT),"GALICIAN"},
{MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),"GERMAN"},
{MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT),"GREEK"},
{MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT),"HUNGARIAN"},
{MAKELANGID(LANG_ITALIAN, SUBLANG_ITALIAN),"ITALIAN"},
{MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT),"JAPANESE"},
{MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT),"KOREAN"},
{MAKELANGID(LANG_LITHUANIAN, SUBLANG_DEFAULT),"LITHUANIAN"},
{MAKELANGID(LANG_NORWEGIAN, SUBLANG_DEFAULT),"NORWEGIAN"},
{MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT),"POLISH"},
{MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE),"PORTUGUESE"},
{MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN),"PORTUGUESEBRAZILIAN"},
{MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT),"ROMANIAN"},
{MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT),"RUSSIAN"},
{MAKELANGID(LANG_SERBIAN, SUBLANG_SERBIAN_CYRILLIC),"SERBIAN"},
{MAKELANGID(LANG_SINHALESE, SUBLANG_DEFAULT),"SINHALA"},
{MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT),"SLOVAK"},
{MAKELANGID(LANG_SLOVENIAN, SUBLANG_SLOVENIAN_SLOVENIA),"SLOVENIAN"},
{MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_MODERN),"SPANISH"},
{MAKELANGID(LANG_SWEDISH, SUBLANG_DEFAULT),"SWEDISH"},
{MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT),"TURKISH"},
{MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT),"UKRAINIAN"},
	};

	if (map.find(id) != map.end())
		return map.find(id)->second;
	return "";
}

}

