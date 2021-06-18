#include "pch.h"
#include <gtest/gtest.h>
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
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Empty command line containing space
	TEST_F(MergeCmdLineInfoTest, EmptySpace)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe "));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Empty command line containing tab
	TEST_F(MergeCmdLineInfoTest, EmptyTab)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe\t"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Empty command line containing tab
	TEST_F(MergeCmdLineInfoTest, EmptyEOL)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe\n"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath3)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe  C:\\Temp\\"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath4)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe\tC:\\Temp\\"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath5)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ "));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path
	TEST_F(MergeCmdLineInfoTest, LeftPath6)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\\t"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths
	TEST_F(MergeCmdLineInfoTest, LeftRightPath1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ C:\\Temp2\\"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths
	TEST_F(MergeCmdLineInfoTest, LeftRightPath2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp C:\\Temp2"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with three paths
	TEST_F(MergeCmdLineInfoTest, ThreePaths1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp C:\\Temp2 C:\\Temp3"));
		EXPECT_EQ(3, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(_T("C:\\Temp3"), cmdInfo.m_Files[2]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with three paths
	TEST_F(MergeCmdLineInfoTest, ThreePaths2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ C:\\Temp2\\ C:\\Temp3\\"));
		EXPECT_EQ(3, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(_T("C:\\Temp3"), cmdInfo.m_Files[2]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths, spaces between
	TEST_F(MergeCmdLineInfoTest, PathMid1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\  C:\\Temp2\\"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths, spaces between
	TEST_F(MergeCmdLineInfoTest, PathMid2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\        C:\\Temp2\\"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths, tab between
	TEST_F(MergeCmdLineInfoTest, PathMid3)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\\tC:\\Temp2\\"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths, EOL between
	TEST_F(MergeCmdLineInfoTest, PathMid4)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\\nC:\\Temp2\\"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftLinux1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:/Temp/"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left path, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftLinux2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:/Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftRightLinux1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2/"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathLeftRightLinux2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with three paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathThreeLinux1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2/ C:/Temp3/"));
		EXPECT_EQ(3, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(_T("C:\\Temp3"), cmdInfo.m_Files[2]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with three paths, Linux separators
	TEST_F(MergeCmdLineInfoTest, PathThreeLinux2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:/Temp/ C:/Temp2 C:/Temp3"));
		EXPECT_EQ(3, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(_T("C:\\Temp3"), cmdInfo.m_Files[2]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe \"C:\\Temp\\\""));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\""));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Program Files"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left quoted and right non-quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote3)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\" C:\\Temp\\"));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Program Files"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left non-quoted and right quoted path
	TEST_F(MergeCmdLineInfoTest, PathQuote4)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ \"C:\\Program Files\\\""));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Program Files"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with left and right quoted paths
	TEST_F(MergeCmdLineInfoTest, PathQuote5)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\" \"C:\\Program Files2\\\""));
		EXPECT_EQ(2, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Program Files"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Program Files2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with third path quoted
	TEST_F(MergeCmdLineInfoTest, PathQuote6)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe C:\\Temp\\ C:\\Temp2\\ \"C:\\Program Files\\\" "));
		EXPECT_EQ(3, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(_T("C:\\Program Files"), cmdInfo.m_Files[2]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with three quoted paths
	TEST_F(MergeCmdLineInfoTest, PathQuote7)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe \"C:\\Program Files\\\" \"C:\\Program Files2\\\" \"C:\\Program Files3\\\""));
		EXPECT_EQ(3, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Program Files"), cmdInfo.m_Files[0]);
		EXPECT_EQ(_T("C:\\Program Files2"), cmdInfo.m_Files[1]);
		EXPECT_EQ(_T("C:\\Program Files3"), cmdInfo.m_Files[2]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with a correct codepage 
	TEST_F(MergeCmdLineInfoTest, CorrectCodepage)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -cp 1251"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(1251,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with a correct codepage and with '/'
	TEST_F(MergeCmdLineInfoTest, CorrectCodepage2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /cp 1251"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(1251,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with a wrong specified codepage
	TEST_F(MergeCmdLineInfoTest, IncorrectCodepage)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -cp windows1251"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with a missed codepage
	TEST_F(MergeCmdLineInfoTest, MissedCodepage)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -cp "));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Command line with a missed codepage and with left path
	TEST_F(MergeCmdLineInfoTest, MissedCodepageWithLeftPath)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -cp C:\\Temp "));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}
	
	// Missed codepage with both paths
	TEST_F(MergeCmdLineInfoTest, MissedCodepageWithBothPaths)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -cp C:\\Temp C:\\Temp2"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp2"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Left description
	TEST_F(MergeCmdLineInfoTest, LeftDesc1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl First C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Left description
	TEST_F(MergeCmdLineInfoTest, LeftDesc2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl \"First desc\" C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Left description
	TEST_F(MergeCmdLineInfoTest, LeftDesc3)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl \"First desc\""));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Right description
	TEST_F(MergeCmdLineInfoTest, RightDesc1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dr First C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("First"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Right description
	TEST_F(MergeCmdLineInfoTest, RightDesc2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dr \"First desc\" C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Right description
	TEST_F(MergeCmdLineInfoTest, RightDesc3)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dr \"First desc\""));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Middle description
	TEST_F(MergeCmdLineInfoTest, MiddleDesc1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dm First C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T("First"), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Middle description
	TEST_F(MergeCmdLineInfoTest, MiddleDesc2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dm \"First desc\" C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Middle description
	TEST_F(MergeCmdLineInfoTest, MiddleDesc3)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dm \"First desc\""));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Both descriptions
	TEST_F(MergeCmdLineInfoTest, BothDesc1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl First -dr Second C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Second"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Both descriptions
	TEST_F(MergeCmdLineInfoTest, BothDesc2)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl \"First desc\" -dr \"Second text\" C:\\Temp"));
		EXPECT_EQ(1, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(_T("C:\\Temp"), cmdInfo.m_Files[0]);
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_CMDLINE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Second text"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Both descriptions
	TEST_F(MergeCmdLineInfoTest, BothDesc3)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl \"First desc\" -dr \"Second text\""));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Second text"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Both descriptions
	TEST_F(MergeCmdLineInfoTest, BothDesc4)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl First -dr \"Second text\""));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Second text"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Both descriptions
	TEST_F(MergeCmdLineInfoTest, BothDesc5)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl \"First desc\" -dr Second"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First desc"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Second"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Both descriptions
	TEST_F(MergeCmdLineInfoTest, BothDesc6)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl First -dr Second"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Second"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Three descriptions
	TEST_F(MergeCmdLineInfoTest, ThreeDesc1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl First -dm Second -dr Third"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T("First"), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T("Second"), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Third"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Unpacker
	TEST_F(MergeCmdLineInfoTest, Unpacker1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /unpacker CompareMSExcelFiles.sct"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T("CompareMSExcelFiles.sct"), cmdInfo.m_sUnpacker);
	}

	// Prediffer
	TEST_F(MergeCmdLineInfoTest, Prediffer1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /prediffer PrediffLineFilter.sct"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T("PrediffLineFilter.sct"), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}

	// Compare method
	TEST_F(MergeCmdLineInfoTest, CompareMethod)
	{
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m full"));
			EXPECT_EQ(MergeCmdLineInfo::CompareMethodType::CONTENT, *cmdInfo.m_nCompMethod);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m quick"));
			EXPECT_EQ(MergeCmdLineInfo::CompareMethodType::QUICK_CONTENT, *cmdInfo.m_nCompMethod);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m binary"));
			EXPECT_EQ(MergeCmdLineInfo::CompareMethodType::BINARY_CONTENT, *cmdInfo.m_nCompMethod);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m date"));
			EXPECT_EQ(MergeCmdLineInfo::CompareMethodType::DATE, *cmdInfo.m_nCompMethod);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m datesize"));
			EXPECT_EQ(MergeCmdLineInfo::CompareMethodType::DATE_SIZE, *cmdInfo.m_nCompMethod);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m sizedate"));
			EXPECT_EQ(MergeCmdLineInfo::CompareMethodType::DATE_SIZE, *cmdInfo.m_nCompMethod);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m size"));
			EXPECT_EQ(MergeCmdLineInfo::CompareMethodType::SIZE, *cmdInfo.m_nCompMethod);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /m unknown"));
			EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
			EXPECT_EQ(1, cmdInfo.m_sErrorMessages.size());
		}
	}

	// Single instance mode
	TEST_F(MergeCmdLineInfoTest, SingleInstanceMode)
	{
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe"));
			EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /s-"));
			EXPECT_EQ(0, *cmdInfo.m_nSingleInstance);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /s"));
			EXPECT_EQ(1, *cmdInfo.m_nSingleInstance);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /s:0"));
			EXPECT_EQ(0, *cmdInfo.m_nSingleInstance);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /s:1"));
			EXPECT_EQ(1, *cmdInfo.m_nSingleInstance);
		}
		{
			MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe /s:2"));
			EXPECT_EQ(2, *cmdInfo.m_nSingleInstance);
		}
	}

	// Config
	TEST_F(MergeCmdLineInfoTest, SetConfig1)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -cfg Settings/TreeMode=1 -cfg Settings/ToolbarSize=0"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_EQ(_T("1"), cmdInfo.m_Options[_T("Settings/TreeMode")]);
		EXPECT_EQ(_T("0"), cmdInfo.m_Options[_T("Settings/ToolbarSize")]);
	}

#if 0 // Disabled for now - should we handle this case?
	// Missing description
	TEST_F(MergeCmdLineInfoTest, DescMissing)
	{
		MergeCmdLineInfo cmdInfo(_T("C:\\WinMerge\\WinMerge.exe -dl -dr Second"));
		EXPECT_EQ(0, cmdInfo.m_Files.GetSize());
		EXPECT_EQ(MergeCmdLineInfo::SHOWNORMAL, cmdInfo.m_nCmdShow);
		EXPECT_FALSE(cmdInfo.m_bEscShutdown);
		EXPECT_FALSE(cmdInfo.m_bExitIfNoDiff);
		EXPECT_FALSE(cmdInfo.m_bRecurse);
		EXPECT_FALSE(cmdInfo.m_bNonInteractive);
		EXPECT_FALSE(cmdInfo.m_nSingleInstance.has_value());
		EXPECT_FALSE(cmdInfo.m_bShowUsage);
		EXPECT_FALSE(cmdInfo.m_nCompMethod.has_value());
		EXPECT_EQ(0,cmdInfo.m_nCodepage);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwLeftFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwMiddleFlags);
		EXPECT_EQ(FFILEOPEN_NONE, cmdInfo.m_dwRightFlags);
		EXPECT_EQ(_T(""), cmdInfo.m_sLeftDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sMiddleDesc);
		EXPECT_EQ(_T("Second"), cmdInfo.m_sRightDesc);
		EXPECT_EQ(_T(""), cmdInfo.m_sFileFilter);
		EXPECT_EQ(_T(""), cmdInfo.m_sPreDiffer);
		EXPECT_EQ(_T(""), cmdInfo.m_sUnpacker);
	}
#endif

}  // namespace
