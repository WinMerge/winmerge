/** 
 * @file  DirFrameTest.cpp
 *
 * @brief Implementation for DirFrame testcase.
 */

#include "pch.h"
#include "Resource.h"

namespace
{

using namespace GUITestUtils;

class DirFrameTest : public CommonTest
{
protected:
	DirFrameTest()
	{
	}

	virtual ~DirFrameTest()
	{
		// You can do clean-up work	that doesn't throw exceptions here.
	}

	static void SetUpTestCase()
	{
		// You can do set-up work for each test	here.
		std::string dir1 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir1\\").u8string();
		std::string dir2 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir2\\").u8string();
		m_hwndWinMerge = execWinMerge(("/noprefs /maxmize /r " + dir1 + " " + dir2).c_str());
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
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(DirFrameTest, Menus)
{
	// System menu
	typeAltPlusKey(' ');
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("MDIChildSystem");
	// File menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("File");
	// File/ReadOnly menu
	for (int i = 0; i < 6; ++i)
		typeKey(VK_DOWN);
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("FileReadOnly");
	// Edit menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("Edit");
	// View menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("View");
	// View/3-way Compare menu
	for (int i = 0; i < 7; ++i)
		typeKey(VK_DOWN);
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("View3WayCompare");
	typeKey(VK_LEFT);
	// View/Toolbar
	for (int i = 0; i < 8; ++i)
		typeKey(VK_DOWN);
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("ViewToolbar");
	// Merge menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("Merge");
	// Tools menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("Tools");
	// Plugins menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("Plugins");
	// Window menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("Window");
	// Help menu
	typeKey(VK_RIGHT);
	Sleep(200);
	saveForegroundWindowImage("Help");
	// Close menu
	typeKey(VK_MENU);
}

TEST_F(DirFrameTest, FileSaveProject)
{
	selectMenuAndSaveWindowImage(ID_FILE_SAVEPROJECT);
	selectMenu(ID_FILE_CLOSE);
}

TEST_F(DirFrameTest, EditSelectAll)
{
	selectMenuAndSaveWindowImage(ID_EDIT_SELECT_ALL);
}

TEST_F(DirFrameTest, ViewFilters)
{
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWIDENTICAL);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWDIFFERENT);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWUNIQUELEFT);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWUNIQUERIGHT);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWSKIPPED);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWBINARIES);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWIDENTICAL);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWDIFFERENT);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWUNIQUELEFT);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWUNIQUERIGHT);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWSKIPPED);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_OPTIONS_SHOWBINARIES);
	Sleep(500);
}

TEST_F(DirFrameTest, ViewTreeMode)
{
	selectMenuAndSaveWindowImage(ID_VIEW_TREEMODE);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_VIEW_TREEMODE);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_VIEW_EXPAND_ALLSUBDIRS);
	Sleep(500);
	selectMenuAndSaveWindowImage(ID_VIEW_COLLAPSE_ALLSUBDIRS);
	Sleep(500);
}

TEST_F(DirFrameTest, ViewFont)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_VIEW_SELECTFONT);
	if (hwndDlg)
	{
		typeText(L"Fixedsys\n");
		waitUntilClose(hwndDlg);
	}
	saveImage();
	selectMenuAndSaveWindowImage(ID_VIEW_USEDEFAULTFONT);
}

TEST_F(DirFrameTest, ViewSwapPanes)
{
	selectMenuAndSaveWindowImage(ID_VIEW_SWAPPANES);
	selectMenuAndSaveWindowImage(ID_VIEW_SWAPPANES);
}

TEST_F(DirFrameTest, ViewCompareStatistics)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_VIEW_DIR_STATISTICS);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(DirFrameTest, ViewRefresh)
{
	selectMenuAndSaveWindowImage(ID_REFRESH);
	selectMenu(ID_EDIT_SELECT_ALL);
	selectMenuAndSaveWindowImage(ID_RESCAN);
	Sleep(2000);
}

TEST_F(DirFrameTest, ToolsCustomizeColumns)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_TOOLS_CUSTOMIZECOLUMNS);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(DirFrameTest, ToolsGeneratePatch)
{
	typeKey(VK_END);
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_TOOLS_GENERATEPATCH);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(DirFrameTest, ToolsGenerateReport)
{
	HWND hwndDlg = selectOpenDialogMenuAndSaveDialogImage(ID_TOOLS_GENERATEREPORT);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

}
