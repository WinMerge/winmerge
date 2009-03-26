#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include "Constants.h"  // FFILEOPEN_* flags
#include "UnicodeString.h"
#include "MergeCmdLineInfo.h"

using std::vector;

namespace
{
	// The fixture for testing command line parsing.
	class MergeCmdLineInfoTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		MergeCmdLineInfoTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~MergeCmdLineInfoTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
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

	// Empty command line
	// BUG! Empty command line returns FFILEOPEN_CMDLINE as flags!
	TEST_F(MergeCmdLineInfoTest, Empty)
	{
		MergeCmdLineInfo cmdInfo(NULL);
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Empty command line containing space
	// BUG! Empty command line returns FFILEOPEN_CMDLINE as flags!
	TEST_F(MergeCmdLineInfoTest, EmptySpace)
	{
		MergeCmdLineInfo cmdInfo(" ");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Empty command line containing tab
	// BUG! Empty command line returns FFILEOPEN_CMDLINE as flags!
	TEST_F(MergeCmdLineInfoTest, EmptyTab)
	{
		MergeCmdLineInfo cmdInfo("\t");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

}  // namespace

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
