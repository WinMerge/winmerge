/** 
 * @file  OptionsDialogTest.cpp
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
public:
	OptionsDialogTest()
	{
		std::string lang = std::to_string(GetParam());
		m_hwndWinMerge = execWinMerge(("/noprefs /cfg Locale/LanguageId=" + lang).c_str());
	}

	virtual ~OptionsDialogTest()
	{
		// You can do clean-up work	that doesn't throw exceptions here.
		PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
		waitUntilProcessExit(m_hwndWinMerge);
	}

	static void SetUpTestCase()
	{
		// You can do set-up work for each test	here.
	}

	static void TearDownTestCase()
	{
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
		HTREEITEM hTreeItem = TreeView_GetRoot(hwndCtrl);
		for (int i = 0; i < index; ++i)
			hTreeItem = TreeView_GetNextVisible(hwndCtrl, hTreeItem);
		TreeView_SelectItem(hwndCtrl, hTreeItem);
		Sleep(100);
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_P(OptionsDialogTest, Pages)
{
	HWND hwndDlg = selectOpenDialogMenu(ID_OPTIONS);
	if (hwndDlg)
	{
		selectCategory(hwndDlg, 0);
		saveForegroundDialogImage("General");
		selectCategory(hwndDlg, 2);
		saveForegroundDialogImage("Compare/General");
		selectCategory(hwndDlg, 3);
		saveForegroundDialogImage("Compare/Folder");
		selectCategory(hwndDlg, 4);
		saveForegroundDialogImage("Compare/Table");
		selectCategory(hwndDlg, 5);
		saveForegroundDialogImage("Compare/Binary");
		selectCategory(hwndDlg, 6);
		saveForegroundDialogImage("Compare/Image");
		selectCategory(hwndDlg, 7);
		saveForegroundDialogImage("Compare/Webpage");
		selectCategory(hwndDlg, 8);
		saveForegroundDialogImage("Message Boxes");
		selectCategory(hwndDlg, 10);
		saveForegroundDialogImage("Editor/General");
		selectCategory(hwndDlg, 11);
		saveForegroundDialogImage("Editor/Compare/Merge");
		selectCategory(hwndDlg, 12);
		saveForegroundDialogImage("Editor/Syntax");
		selectCategory(hwndDlg, 14);
		saveForegroundDialogImage("Colors/Color Schemes");
		selectCategory(hwndDlg, 15);
		saveForegroundDialogImage("Colors/Differences");
		selectCategory(hwndDlg, 16);
		saveForegroundDialogImage("Colors/Syntax");
		selectCategory(hwndDlg, 17);
		saveForegroundDialogImage("Colors/Text");
		selectCategory(hwndDlg, 18);
		saveForegroundDialogImage("Colors/Markers");
		selectCategory(hwndDlg, 19);
		saveForegroundDialogImage("Colors/FolderCompare");
		selectCategory(hwndDlg, 20);
		saveForegroundDialogImage("Colors/System");
		selectCategory(hwndDlg, 21);
		saveForegroundDialogImage("Archive Support");
		selectCategory(hwndDlg, 22);
		saveForegroundDialogImage("Project");
		selectCategory(hwndDlg, 23);
		saveForegroundDialogImage("System");
		selectCategory(hwndDlg, 24);
		saveForegroundDialogImage("Backup Files");
		selectCategory(hwndDlg, 25);
		saveForegroundDialogImage("Codepage");
		selectCategory(hwndDlg, 26);
		saveForegroundDialogImage("Shell Integration");
		typeText(L"\x1b");
		waitUntilClose(hwndDlg);
	}
}

}

INSTANTIATE_TEST_SUITE_P(OptionsDialogTestInstance,
	OptionsDialogTest,
	testing::ValuesIn(GUITestUtils::languages()));

