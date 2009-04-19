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

	// NOTE: Windows command line ALWAYS has the executable name as first parameter

	// Empty command line
	TEST_F(MergeCmdLineInfoTest, Empty)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Empty command line containing space
	TEST_F(MergeCmdLineInfoTest, EmptySpace)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe ");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Empty command line containing tab
	TEST_F(MergeCmdLineInfoTest, EmptyTab)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe\t");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Empty command line containing tab
	TEST_F(MergeCmdLineInfoTest, EmptyEOL)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe\n");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath3)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe  C:\\Temp\\");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath4)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe\tC:\\Temp\\");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath5)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ ");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath6)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\\t");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths
	TEST_F(MergeCmdLineInfoTest, LeftRightPath1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ C:\\Temp2\\");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths
	TEST_F(MergeCmdLineInfoTest, LeftRightPath2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp C:\\Temp2");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with three paths
	TEST_F(MergeCmdLineInfoTest, ThreePaths1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp C:\\Temp2 C:\\Temp3");
		EXPECT_EQ(3, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2", cmdInfo.m_Files[1]);
		EXPECT_EQ("C:\\Temp3", cmdInfo.m_Files[2]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with three paths
	TEST_F(MergeCmdLineInfoTest, ThreePaths2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ C:\\Temp2\\ C:\\Temp3\\");
		EXPECT_EQ(3, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ("C:\\Temp3\\", cmdInfo.m_Files[2]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths, spaces between
	TEST_F(MergeCmdLineInfoTest, PathMid1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\  C:\\Temp2\\");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths, spaces between
	TEST_F(MergeCmdLineInfoTest, PathMid2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\        C:\\Temp2\\");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths, tab between
	TEST_F(MergeCmdLineInfoTest, PathMid3)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\\tC:\\Temp2\\");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths, EOL between
	TEST_F(MergeCmdLineInfoTest, PathMid4)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\\nC:\\Temp2\\");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftLinux1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:/Temp/");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left path, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftLinux2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:/Temp");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftRightLinux1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2/");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftRightLinux2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with three paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathThreeLinux1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2/ C:/Temp3/");
		EXPECT_EQ(3, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ("C:\\Temp3\\", cmdInfo.m_Files[2]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with three paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathThreeLinux2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2 C:/Temp3");
		EXPECT_EQ(3, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2", cmdInfo.m_Files[1]);
		EXPECT_EQ("C:\\Temp3", cmdInfo.m_Files[2]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote1)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe \"C:\\Temp\\\"");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote2)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\"");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Program Files\\", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left quoted and right non-quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote3)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\" C:\\Temp\\");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Program Files\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left non-quoted and right quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote4)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ \"C:\\Program Files\\\"");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Program Files\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with left and right quoted paths
	TEST_F(MergeCmdLineInfoTest, PathQuote5)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\" \"C:\\Program Files2\\\"");
		EXPECT_EQ(2, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Program Files\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Program Files2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with third path quoted
	TEST_F(MergeCmdLineInfoTest, PathQuote6)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ C:\\Temp2\\ \"C:\\Program Files\\\" ");
		EXPECT_EQ(3, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Temp2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ("C:\\Program Files\\", cmdInfo.m_Files[2]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with three quoted paths
	TEST_F(MergeCmdLineInfoTest, PathQuote7)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\" \"C:\\Program Files2\\\" \"C:\\Program Files3\\\"");
		EXPECT_EQ(3, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Program Files\\", cmdInfo.m_Files[0]);
		EXPECT_EQ("C:\\Program Files2\\", cmdInfo.m_Files[1]);
		EXPECT_EQ("C:\\Program Files3\\", cmdInfo.m_Files[2]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with a correct codepage 
	TEST_F(MergeCmdLineInfoTest, CorrectCodepage)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe -cp 1251");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(1251,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with a wrong specified codepage
	TEST_F(MergeCmdLineInfoTest, IncorrectCodepage)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe -cp windows1251");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with a missed codepage
	TEST_F(MergeCmdLineInfoTest, MissedCodepage)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe -cp ");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

	// Command line with a missed codepage and with left path
	TEST_F(MergeCmdLineInfoTest, MissedCodepageWithLeftPath)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe -cp C:\\Temp ");
		EXPECT_EQ(0, cmdInfo.m_Files.size());
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}
	
	// Missed codepage with both paths
	TEST_F(MergeCmdLineInfoTest, MissedCodepageWithBothPaths)
	{
		MergeCmdLineInfo cmdInfo("C:\\WinMerge\\WinMerge.exe -cp C:\\Temp C:\\Temp2");
		EXPECT_EQ(1, cmdInfo.m_Files.size());
		EXPECT_EQ("C:\\Temp2", cmdInfo.m_Files[0]);
		EXPECT_EQ(SW_SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bClearCaseTool);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_bSingleInstance);
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ("", cmdInfo.m_sLeftDesc);
		EXPECT_EQ("", cmdInfo.m_sRightDesc);
		EXPECT_EQ("", cmdInfo.m_sFileFilter);
		EXPECT_EQ("", cmdInfo.m_sPreDiffer);
	}

}  // namespace
