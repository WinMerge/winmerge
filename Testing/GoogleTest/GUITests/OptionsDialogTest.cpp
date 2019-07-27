/** 
 * @file  OptionsDialog.cpp
 *
 * @brief Implementation for OptionsDialog testcase.
 */

#include "pch.h"
#include "Resource.h"

namespace
{

using namespace GUITestUtils;

class OptionsDialogTest : public CommonTest
{
protected:
	OptionsDialogTest()
	{
	}

	virtual ~OptionsDialogTest()
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
	}

	void selectCategory(HWND hwndDlg, int index)
	{
		HWND hwndCtrl = GetDlgItem(hwndDlg, IDC_TREEOPT_PAGES);
		SetFocus(hwndCtrl);
		HTREEITEM hTreeItem = TreeView_GetFirstVisible(hwndCtrl);
		for (int i = 0; i < index; ++i)
			hTreeItem = TreeView_GetNextVisible(hwndCtrl, hTreeItem);
		TreeView_SelectItem(hwndCtrl, hTreeItem);
		Sleep(500);
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(OptionsDialogTest, GeneralPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 0);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, CompareGeneralPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 2);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, CompareFolderPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 3);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, CompareBinaryPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 4);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, CompareImagePage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 5);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, EditorPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 6);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, ColorsDifferencesPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 8);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, ColorsSyntaxPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 9);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, ColorsTextPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 10);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, ColorsMarkersPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 11);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, ColorsFolderComparePage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 12);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, ArchiveSupportPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 13);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, SystemPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 14);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, BackupFilesPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 15);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, CodepagePage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 16);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

TEST_F(OptionsDialogTest, ShellIntegrationPage)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 17);
		saveForegroundDialogImage();
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

}
