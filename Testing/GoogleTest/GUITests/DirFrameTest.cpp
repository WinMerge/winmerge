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

class DirFrameTest : public testing::Test
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
		std::string dir1 = (getModuleFileName().parent_path() / "..\\..\\Data\\Compare\\Dir1\\").u8string();
		std::string dir2 = (getModuleFileName().parent_path() / "..\\..\\Data\\Compare\\Dir2\\").u8string();
		m_hwndWinMerge = execWinMerge(("/noprefs /maxmize /r " + dir1 + " " + dir2).c_str());
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

HWND DirFrameTest::m_hwndWinMerge = nullptr;

TEST_F(DirFrameTest, FileSaveProject)
{
	selectMenu(ID_FILE_SAVEPROJECT);
	selectMenu(ID_FILE_CLOSE);
}

TEST_F(DirFrameTest, EditSelectAll)
{
	selectMenu(ID_EDIT_SELECT_ALL);
}

TEST_F(DirFrameTest, ViewFilters)
{
	selectMenu(ID_OPTIONS_SHOWIDENTICAL);
	selectMenu(ID_OPTIONS_SHOWDIFFERENT);
	selectMenu(ID_OPTIONS_SHOWUNIQUELEFT);
	selectMenu(ID_OPTIONS_SHOWUNIQUERIGHT);
	selectMenu(ID_OPTIONS_SHOWSKIPPED);
	selectMenu(ID_OPTIONS_SHOWBINARIES);
	Sleep(500);
	selectMenu(ID_OPTIONS_SHOWIDENTICAL);
	Sleep(500);
	selectMenu(ID_OPTIONS_SHOWDIFFERENT);
	Sleep(500);
	selectMenu(ID_OPTIONS_SHOWUNIQUELEFT);
	Sleep(500);
	selectMenu(ID_OPTIONS_SHOWUNIQUERIGHT);
	Sleep(500);
	selectMenu(ID_OPTIONS_SHOWSKIPPED);
	Sleep(500);
	selectMenu(ID_OPTIONS_SHOWBINARIES);
	Sleep(500);
}

TEST_F(DirFrameTest, ViewTreeMode)
{
	selectMenu(ID_VIEW_TREEMODE);
	Sleep(500);
	selectMenu(ID_VIEW_TREEMODE);
	Sleep(500);
	selectMenu(ID_VIEW_EXPAND_ALLSUBDIRS);
	Sleep(500);
	selectMenu(ID_VIEW_COLLAPSE_ALLSUBDIRS);
	Sleep(500);
}

TEST_F(DirFrameTest, ViewFont)
{
	selectMenu(ID_VIEW_SELECTFONT);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"Fixedsys\n");
		waitUntilClose(hwndDlg);
	}
	selectMenu(ID_VIEW_USEDEFAULTFONT);
}

TEST_F(DirFrameTest, ViewSwapPanes)
{
	selectMenu(ID_VIEW_SWAPPANES);
	selectMenu(ID_VIEW_SWAPPANES);
}

TEST_F(DirFrameTest, ViewCompareStatistics)
{
	selectMenu(ID_VIEW_DIR_STATISTICS);
	HWND hwndDlg = findForegroundDialog();
	ASSERT_TRUE(hwndDlg != nullptr);
	if (hwndDlg)
	{
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(DirFrameTest, ViewRefresh)
{
	selectMenu(ID_REFRESH);
	selectMenu(ID_EDIT_SELECT_ALL);
	selectMenu(ID_RESCAN);
	Sleep(2000);
}

}
