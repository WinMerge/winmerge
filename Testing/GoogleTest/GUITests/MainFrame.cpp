/** 
 * @file  MainFrame.cpp
 *
 * @brief Implementation for MainFrame testcase.
 */

#include "pch.h"
#include <gtest/gtest.h>
#include <Windows.h>
#include <afxres.h>
#include <filesystem>
#include "Resource.h"

namespace
{

DWORD waitForInputIdleByHWND(HWND hwnd, DWORD dwMilliseconds = WAIT_TIMEOUT)
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


/*
void typeTextInWindow(HWND hwnd, const wchar_t *text)
{
	DWORD dwProcessId = 0;
	DWORD idAttachTo = GetWindowThreadProcessId(hwnd, &dwProcessId);
	AttachThreadInput(GetCurrentThreadId(), idAttachTo, TRUE);
	typeText(text);
	AttachThreadInput(GetCurrentThreadId(), idAttachTo, FALSE);
}
*/

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

HWND execWinMerge()
{
	HWND hwndWinMerge = nullptr;
	system("start \"\" \"C:\\Program Files\\WinMerge\\WinMergeU.exe\" /noprefs");
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

class MainFrameTest : public testing::Test
{
protected:
	MainFrameTest()
	{
	}

	virtual ~MainFrameTest()
	{
		// You can do clean-up work	that doesn't throw exceptions here.
	}

	static void SetUpTestCase()
	{
		// You can do set-up work for each test	here.
		m_hwndWinMerge = execWinMerge();
	}

	static void TearDownTestCase()
	{
		PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
	}

	// If	the	constructor	and	destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp()
	{
		// Code	here will be called	immediately	after the constructor (right
		// before each test).
	}

	virtual void TearDown()
	{
		// Code	here will be called	immediately	after each test	(right
		// before the destructor).
		ASSERT_TRUE(IsWindow(m_hwndWinMerge));
		waitForInputIdleByHWND(m_hwndWinMerge);
	}

	// Objects declared here can be used by all tests in the test case for Foo.
	static HWND m_hwndWinMerge;
};

HWND MainFrameTest::m_hwndWinMerge = nullptr;

TEST_F(MainFrameTest, FileNew)
{
	selectMenu(m_hwndWinMerge, ID_FILE_NEW);
	selectMenu(m_hwndWinMerge, ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileNew3)
{
	selectMenu(m_hwndWinMerge, ID_FILE_NEW3);
	selectMenu(m_hwndWinMerge, ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileOpen)
{
	selectMenu(m_hwndWinMerge, ID_FILE_OPEN);
	selectMenu(m_hwndWinMerge, ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileOpenConflict)
{
	selectMenu(m_hwndWinMerge, ID_FILE_OPENCONFLICT);
	HWND hwndOpen = findForegroundDialog();
	ASSERT_TRUE(hwndOpen != nullptr);
	if (hwndOpen)
	{
		typeText((getModuleFileName().parent_path() / "..\\..\\Data\\big_file.conflict\n").c_str());
		waitUntilClose(hwndOpen);
	}
	selectMenu(m_hwndWinMerge, ID_WINDOW_CLOSEALL);
	HWND hwndSave = findForegroundDialog();
	ASSERT_TRUE(hwndSave != nullptr);
	if (hwndSave)
	{
		typeAltPlusKey('R');
		waitUntilClose(hwndSave);
	}
}

TEST_F(MainFrameTest, FileOpenProject)
{
	selectMenu(m_hwndWinMerge, ID_FILE_OPENPROJECT);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText((getModuleFileName().parent_path() / "..\\..\\Data\\Dir2.WinMerge\n").c_str());
		waitUntilClose(hwndDlg);
		Sleep(1000);
	}
	selectMenu(m_hwndWinMerge, ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileSaveProject)
{
	selectMenu(m_hwndWinMerge, ID_FILE_SAVEPROJECT);
	selectMenu(m_hwndWinMerge, ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, EditOptions)
{
	selectMenu(m_hwndWinMerge, ID_OPTIONS);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, ViewToolbar)
{
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_NONE);
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_SMALL);
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_NONE);
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_BIG);
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_NONE);
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_HUGE);
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_NONE);
	selectMenu(m_hwndWinMerge, ID_TOOLBAR_SMALL);
}

TEST_F(MainFrameTest, ViewStatusBar)
{
	selectMenu(m_hwndWinMerge, ID_VIEW_STATUS_BAR);
	selectMenu(m_hwndWinMerge, ID_VIEW_STATUS_BAR);
}

TEST_F(MainFrameTest, ViewTabBar)
{
	selectMenu(m_hwndWinMerge, ID_VIEW_TAB_BAR);
	selectMenu(m_hwndWinMerge, ID_VIEW_TAB_BAR);
}

TEST_F(MainFrameTest, PluginsSettings)
{
	selectMenu(m_hwndWinMerge, ID_PLUGINS_LIST);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

}
