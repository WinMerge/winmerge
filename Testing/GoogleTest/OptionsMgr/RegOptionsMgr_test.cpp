#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "UnicodeString.h"
#include "RegOptionsMgr.h"
#include "Environment.h"
#include "paths.h"
#include "TFile.h"

using std::vector;

// TODO:
// - add helper functions for checking the changes in registry

namespace
{
	// The fixture for testing command line parsing.
	class RegOptionsMgrTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		RegOptionsMgrTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~RegOptionsMgrTest()
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

	// Read option that does not exist
	// This causes exception in debug builds
#if 0
	TEST_F(RegOptionsMgrTest, ReadNonexistingBool1)
	{
		CRegOptionsMgr mgr;
		mgr.SetRegRootKey(_T("Thingamahoochie\\WinMerge\\UnitTesting"));
		EXPECT_EQ(true, mgr.GetBool(_T("BoolOpt2"));
	}
#endif

	// Add new bool option with false default value
	TEST_F(RegOptionsMgrTest, AddBoolOption1)
	{
		CRegOptionsMgr mgr(_T("Thingamahoochie\\WinMerge\\UnitTesting"));
		EXPECT_EQ(COption::OPT_OK, mgr.InitOption(_T("BoolOpt1"), false));
		EXPECT_EQ(false, mgr.GetBool(_T("BoolOpt1")));
	}

	// Add new bool option with true default value
	TEST_F(RegOptionsMgrTest, AddBoolOption2)
	{
		CRegOptionsMgr mgr(_T("Thingamahoochie\\WinMerge\\UnitTesting"));
		EXPECT_EQ(COption::OPT_OK, mgr.InitOption(_T("BoolOpt2"), true));
		EXPECT_EQ(true, mgr.GetBool(_T("BoolOpt2")));
	}

	TEST_F(RegOptionsMgrTest, ExportAndImport)
	{
		CRegOptionsMgr mgr(_T("Thingamahoochie\\WinMerge\\UnitTesting"));
		EXPECT_EQ(COption::OPT_OK, mgr.InitOption(_T("StringOpt1"), _T("")));
		EXPECT_EQ(COption::OPT_OK, mgr.SaveOption(_T("StringOpt1"), _T("  abc\r\ndef\tghi  ")));
		EXPECT_EQ(_T("  abc\r\ndef\tghi  "), mgr.GetString(_T("StringOpt1")));
		String inifile = paths::ConcatPath(env::GetTemporaryPath(), _T("wm_tmp.ini"));
		mgr.ExportOptions(inifile);
		EXPECT_EQ(COption::OPT_OK, mgr.SaveOption(_T("StringOpt1"), _T("")));
		EXPECT_EQ(_T(""), mgr.GetString(_T("StringOpt1")));
		mgr.ImportOptions(inifile);
		EXPECT_EQ(_T("  abc\r\ndef\tghi  "), mgr.GetString(_T("StringOpt1")));
		TFile(inifile).remove();
	}
}