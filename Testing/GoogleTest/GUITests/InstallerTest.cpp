/** 
 * @file  InstallerTest.cpp
 *
 * @brief Implementation for Installer testcase.
 */

#include "pch.h"
#include "Resource.h"

namespace
{

using namespace GUITestUtils;

class InstallerTest : public CommonTest
{
public:
	InstallerTest()
	{
		std::string lang = languageIdToName(GetParam());
		m_hwndWinMerge = execInstaller(("/LANG=" + lang).c_str());
	}

	virtual ~InstallerTest()
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
	}

	// Objects declared here can be used by all tests in the test case for Foo.
};

TEST_P(InstallerTest, Pages)
{
	saveImage("LicenseAgreement");
	typeKey(VK_RETURN); Sleep(200);
	saveImage("SelectComponents");
	typeKey(VK_RETURN); Sleep(200);
	saveImage("SelectAdditionalTasks");
	typeKey(VK_RETURN); Sleep(200);
	saveImage("3-WayMerge");
	typeKey(VK_RETURN); Sleep(200);
	saveImage("ReadyToInstall");
	typeKey(VK_RETURN); Sleep(200);
	Sleep(15000);
	saveImage("Information");
	typeKey(VK_RETURN); Sleep(200);
	saveImage("CompletingTheWinMergeSetupWizard");
	typeKey(VK_RETURN); Sleep(1000);
	
	for (;;)
	{
		HWND hwndWinMerge = FindWindow(L"WinMergeWindowClassW", nullptr);
		if (!hwndWinMerge)
			break;
		PostMessage(hwndWinMerge, WM_CLOSE, 0, 0);
		Sleep(100);
	}
}

}

INSTANTIATE_TEST_CASE_P(InstallerTestInstance,
	InstallerTest,
	testing::ValuesIn(GUITestUtils::languages()));

