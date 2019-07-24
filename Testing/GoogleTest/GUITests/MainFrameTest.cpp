/** 
 * @file  MainFrame.cpp
 *
 * @brief Implementation for MainFrame testcase.
 */

#include "pch.h"
#include "Resource.h"

namespace
{

using namespace GUITestUtils;

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

	void selectMenu(UINT id)
	{
		GUITestUtils::selectMenu(m_hwndWinMerge, id);
	}

	// Objects declared here can be used by all tests in the test case for Foo.
	static HWND m_hwndWinMerge;
};

HWND MainFrameTest::m_hwndWinMerge = nullptr;

TEST_F(MainFrameTest, FileNew)
{
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileNew3)
{
	selectMenu(ID_FILE_NEW3);
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileOpen)
{
	selectMenu(ID_FILE_OPEN);
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileOpenConflict)
{
	selectMenu(ID_FILE_OPENCONFLICT);
	HWND hwndOpen = findForegroundDialog();
	ASSERT_TRUE(hwndOpen != nullptr);
	if (hwndOpen)
	{
		typeText((getModuleFileName().parent_path() / "..\\..\\Data\\big_file.conflict\n").c_str());
		waitUntilClose(hwndOpen);
	}
	selectMenu(ID_WINDOW_CLOSEALL);
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
	selectMenu(ID_FILE_OPENPROJECT);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText((getModuleFileName().parent_path() / "..\\..\\Data\\Dir2.WinMerge\n").c_str());
		waitUntilClose(hwndDlg);
		Sleep(1000);
	}
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, FileSaveProject)
{
	selectMenu(ID_FILE_SAVEPROJECT);
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, EditOptions)
{
	selectMenu(ID_OPTIONS);
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
	selectMenu(ID_TOOLBAR_NONE);
	selectMenu(ID_TOOLBAR_SMALL);
	selectMenu(ID_TOOLBAR_NONE);
	selectMenu(ID_TOOLBAR_BIG);
	selectMenu(ID_TOOLBAR_NONE);
	selectMenu(ID_TOOLBAR_HUGE);
	selectMenu(ID_TOOLBAR_NONE);
	selectMenu(ID_TOOLBAR_SMALL);
}

TEST_F(MainFrameTest, ViewStatusBar)
{
	selectMenu(ID_VIEW_STATUS_BAR);
	selectMenu(ID_VIEW_STATUS_BAR);
}

TEST_F(MainFrameTest, ViewTabBar)
{
	selectMenu(ID_VIEW_TAB_BAR);
	selectMenu(ID_VIEW_TAB_BAR);
}

TEST_F(MainFrameTest, ToolsFilters)
{
	selectMenu(ID_TOOLS_FILTERS);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, ToolsGeneratePatch)
{
	selectMenu(ID_TOOLS_GENERATEPATCH);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, PluginsSettings)
{
	selectMenu(ID_PLUGINS_LIST);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, PluginsPrediffer)
{
	selectMenu(ID_PREDIFFER_AUTO);
	selectMenu(ID_PREDIFFER_MANUAL);
}

TEST_F(MainFrameTest, PluginsUnpacker)
{
	selectMenu(ID_UNPACK_AUTO);
	selectMenu(ID_UNPACK_MANUAL);
}

TEST_F(MainFrameTest, PluginsReload)
{
	selectMenu(ID_RELOAD_PLUGINS);
}

TEST_F(MainFrameTest, WindowClose)
{
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_FILE_CLOSE);
}

TEST_F(MainFrameTest, WindowCloseAll)
{
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, WindowChangePane)
{
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_WINDOW_CHANGE_PANE);
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, WindowArrangement)
{
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_WINDOW_TILE_HORZ);
	selectMenu(ID_WINDOW_TILE_VERT);
	selectMenu(ID_WINDOW_CASCADE);
	selectMenu(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, HelpWinMergeHelp)
{
	selectMenu(ID_HELP_CONTENTS);
	Sleep(1000);
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpReleaseNotes)
{
	selectMenu(ID_HELP_RELEASENOTES);
	Sleep(1000);
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpTranslations)
{
	selectMenu(ID_HELP_TRANSLATIONS);
	Sleep(1000);
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpConfiguraiton)
{
	selectMenu(ID_HELP_GETCONFIG);
	Sleep(1000);
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpGNULicense)
{
	selectMenu(ID_HELP_GNULICENSE);
	Sleep(1000);
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpAbout)
{
	selectMenu(ID_APP_ABOUT);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

}
