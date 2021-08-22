/** 
 * @file  FileTest.cpp
 *
 * @brief Implementation for File Compare testcase.
 */

#include "pch.h"
#include "Resource.h"

namespace
{

using namespace GUITestUtils;

class FileTest : public CommonTest
{
public:
	FileTest()
	{

	}

	virtual ~FileTest()
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
		PostMessage(m_hwndWinMerge, WM_CLOSE, 0, 0);
		waitUntilProcessExit(m_hwndWinMerge);
	}

	void SetUp3WayCompare()
	{
		std::string dir1 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir1\\file123_diffsize1.txt").u8string();
		std::string dir2 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir2\\file123_diffsize1.txt").u8string();
		std::string dir3 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir3\\file123_diffsize1.txt").u8string();
		std::string lang = std::to_string(GetParam());
		m_hwndWinMerge = execWinMerge(("/noprefs /maximize /cfg Locale/LanguageId=" + lang + " /r " + dir1 + " " + dir2 + " " + dir3).c_str());
	}

	void SetUp2WayCompare()
	{
		std::string dir1 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir1\\file123_diffsize1.txt").u8string();
		std::string dir2 = (getModuleFolder() / "..\\..\\Data\\Compare\\Dir2\\file123_diffsize1.txt").u8string();
		std::string lang = std::to_string(GetParam());
		m_hwndWinMerge = execWinMerge(("/noprefs /maximize /cfg Locale/LanguageId=" + lang + " /r " + dir1 + " " + dir2).c_str());
	}

	void SetUp3WayCompareFiles()
	{
		std::string dir1 = (getModuleFolder() / "..\\..\\Data\\Compare1\\Dir1\\").u8string();
		std::string dir2 = (getModuleFolder() / "..\\..\\Data\\Compare1\\Dir2\\").u8string();
		std::string dir3 = (getModuleFolder() / "..\\..\\Data\\Compare1\\Dir3\\").u8string();
		std::string lang = std::to_string(GetParam());
		m_hwndWinMerge = execWinMerge(("/noprefs /maximize /cfg Locale/LanguageId=" + lang + " /r " + dir1 + " " + dir2 + " " + dir3).c_str());
	}
	// Objects declared here can be used by all tests in the test case for Foo.
};



TEST_P(FileTest, ViewSwapPanes3Way)
{
	SetUp3WayCompare();
	Sleep(200); 
	selectMenuAndSaveWindowImage(ID_SWAPPANES_SWAP12);
	Sleep(200);
	selectMenuAndSaveWindowImage(ID_SWAPPANES_SWAP23);
	Sleep(200);
	selectMenuAndSaveWindowImage(ID_SWAPPANES_SWAP13);
	Sleep(200);
}

TEST_P(FileTest, ViewSwapPanes2Way)
{	
	SetUp2WayCompare();
	Sleep(200);
	selectMenuAndSaveWindowImage(ID_SWAPPANES_SWAP12);
	Sleep(200);
}

TEST_P(FileTest, ViewSwapPanes2WayInvalid)
{
	SetUp2WayCompare();
	Sleep(200);
	selectMenuAndSaveWindowImage(ID_SWAPPANES_SWAP12);
	Sleep(200);
	// The next 2 options are disabled, nothing should happen, no exceptions etc.
	selectMenuAndSaveWindowImage(ID_SWAPPANES_SWAP23);
	Sleep(200);
	selectMenuAndSaveWindowImage(ID_SWAPPANES_SWAP13);
	Sleep(200);
}

TEST_P(FileTest, ViewFileNavigation3Way)
{
	SetUp3WayCompareFiles();
	Sleep(200);
	selectMenuAndSaveWindowImage(ID_MERGE_COMPARE);
	Sleep(200);
	selectMenuAsync(ID_LASTFILE);
	Sleep(2000);
	//The confirmation message box in test mode seems to come up irrespective of saved options
	selectYesOnFileNavConfirmationMessageBox();
	Sleep(200);

	selectMenuAsync(ID_FIRSTFILE);
	Sleep(2000);
	selectYesOnFileNavConfirmationMessageBox();
	Sleep(200);

	selectMenuAsync(ID_NEXTFILE);
	Sleep(2000);
	selectYesOnFileNavConfirmationMessageBox();
	Sleep(200);

	selectMenuAsync(ID_PREVFILE);
	Sleep(2000);
	selectYesOnFileNavConfirmationMessageBox();
	Sleep(200);

}

}

INSTANTIATE_TEST_SUITE_P(FileTestInstance,
	FileTest,
	testing::ValuesIn(GUITestUtils::languages()));

