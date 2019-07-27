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

class MainFrameTest : public CommonTest
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
		waitUntilProcessExit(m_hwndWinMerge);
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
		SendMessage(m_hwndWinMerge, WM_COMMAND, ID_WINDOW_CLOSEALL, 0);
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(MainFrameTest, FileNew)
{
	selectMenuAndSaveWindowImage(ID_FILE_NEW);
}

TEST_F(MainFrameTest, FileNew3)
{
	selectMenuAndSaveWindowImage(ID_FILE_NEW3);
}

TEST_F(MainFrameTest, FileOpen)
{
	selectMenuAndSaveWindowImage(ID_FILE_OPEN);
}

TEST_F(MainFrameTest, FileOpenConflict)
{
	HWND hwndOpen = selectOpenDialogMenuAndSaveDialogImage(ID_FILE_OPENCONFLICT);
	if (hwndOpen)
	{
		typeText((getModuleFolder() / "..\\..\\Data\\big_file.conflict\n").c_str());
		waitUntilClose(hwndOpen);
	}
	HWND hwndSave = selectOpenDialogMenuAndSaveDialogImage(ID_WINDOW_CLOSEALL);
	if (hwndSave)
	{
		typeAltPlusKey('R');
		waitUntilClose(hwndSave);
	}
}

TEST_F(MainFrameTest, FileOpenProject)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_FILE_OPENPROJECT);
	if (hwndDlg)
	{
		typeText((getModuleFolder() / "..\\..\\Data\\Dir2.WinMerge\n").c_str());
		waitUntilClose(hwndDlg);
		Sleep(1000);
	}
}

TEST_F(MainFrameTest, FileSaveProject)
{
	selectMenuAndSaveWindowImage(ID_FILE_SAVEPROJECT);
}

TEST_F(MainFrameTest, EditOptions)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_OPTIONS);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, ViewToolbar)
{
	selectMenuAndSaveWindowImage(ID_TOOLBAR_NONE);
	selectMenuAndSaveWindowImage(ID_TOOLBAR_SMALL);
	selectMenuAndSaveWindowImage(ID_TOOLBAR_NONE);
	selectMenuAndSaveWindowImage(ID_TOOLBAR_BIG);
	selectMenuAndSaveWindowImage(ID_TOOLBAR_NONE);
	selectMenuAndSaveWindowImage(ID_TOOLBAR_HUGE);
	selectMenuAndSaveWindowImage(ID_TOOLBAR_NONE);
	selectMenuAndSaveWindowImage(ID_TOOLBAR_SMALL);
}

TEST_F(MainFrameTest, ViewStatusBar)
{
	selectMenuAndSaveWindowImage(ID_VIEW_STATUS_BAR);
	selectMenuAndSaveWindowImage(ID_VIEW_STATUS_BAR);
}

TEST_F(MainFrameTest, ViewTabBar)
{
	selectMenuAndSaveWindowImage(ID_VIEW_TAB_BAR);
	selectMenuAndSaveWindowImage(ID_VIEW_TAB_BAR);
}

TEST_F(MainFrameTest, ToolsFilters)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_TOOLS_FILTERS);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, ToolsGeneratePatch)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_TOOLS_GENERATEPATCH);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, PluginsSettings)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_PLUGINS_LIST);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(MainFrameTest, PluginsPrediffer)
{
	selectMenuAndSaveWindowImage(ID_PREDIFFER_AUTO);
	selectMenuAndSaveWindowImage(ID_PREDIFFER_MANUAL);
}

TEST_F(MainFrameTest, PluginsUnpacker)
{
	selectMenuAndSaveWindowImage(ID_UNPACK_AUTO);
	selectMenuAndSaveWindowImage(ID_UNPACK_MANUAL);
}

TEST_F(MainFrameTest, PluginsReload)
{
	selectMenuAndSaveWindowImage(ID_RELOAD_PLUGINS);
}

TEST_F(MainFrameTest, WindowClose)
{
	selectMenu(ID_FILE_NEW);
	selectMenuAndSaveWindowImage(ID_FILE_CLOSE);
}

TEST_F(MainFrameTest, WindowCloseAll)
{
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_FILE_NEW);
	selectMenuAndSaveWindowImage(ID_WINDOW_CLOSEALL);
}

TEST_F(MainFrameTest, WindowChangePane)
{
	selectMenu(ID_FILE_NEW);
	selectMenuAndSaveWindowImage(ID_WINDOW_CHANGE_PANE);
}

TEST_F(MainFrameTest, WindowArrangement)
{
	selectMenu(ID_FILE_NEW);
	selectMenu(ID_FILE_NEW);
	selectMenuAndSaveWindowImage(ID_WINDOW_TILE_HORZ);
	selectMenuAndSaveWindowImage(ID_WINDOW_TILE_VERT);
	selectMenuAndSaveWindowImage(ID_WINDOW_CASCADE);
}

TEST_F(MainFrameTest, HelpWinMergeHelp)
{
	selectMenuAsync(ID_HELP_CONTENTS);
	Sleep(1000);
	saveForegroundWindowImage();
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpReleaseNotes)
{
	selectMenuAsync(ID_HELP_RELEASENOTES);
	Sleep(1000);
	saveForegroundWindowImage();
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpTranslations)
{
	selectMenuAsync(ID_HELP_TRANSLATIONS);
	Sleep(1000);
	saveForegroundWindowImage();
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpConfiguraiton)
{
	selectMenuAsync(ID_HELP_GETCONFIG);
	Sleep(1000);
	saveForegroundWindowImage();
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpGNULicense)
{
	selectMenuAsync(ID_HELP_GNULICENSE);
	Sleep(1000);
	saveForegroundWindowImage();
	SwitchToThisWindow(m_hwndWinMerge, TRUE);
}

TEST_F(MainFrameTest, HelpAbout)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_APP_ABOUT);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

}
