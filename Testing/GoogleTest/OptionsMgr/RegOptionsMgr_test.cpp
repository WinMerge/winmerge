#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include "UnicodeString.h"
#include "RegOptionsMgr.h"

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
		mgr.SetRegRootKey("Thingamahoochie\\WinMerge\\UnitTesting");
		EXPECT_EQ(true, mgr.GetBool("BoolOpt2"));
	}
#endif

	// Add new bool option with false default value
	TEST_F(RegOptionsMgrTest, AddBoolOption1)
	{
		CRegOptionsMgr mgr;
		mgr.SetRegRootKey("Thingamahoochie\\WinMerge\\UnitTesting");
		EXPECT_EQ(OPT_OK, mgr.InitOption("BoolOpt1", false));
		EXPECT_EQ(false, mgr.GetBool("BoolOpt1"));
	}

	// Add new bool option with true default value
	TEST_F(RegOptionsMgrTest, AddBoolOption2)
	{
		CRegOptionsMgr mgr;
		mgr.SetRegRootKey("Thingamahoochie\\WinMerge\\UnitTesting");
		EXPECT_EQ(OPT_OK, mgr.InitOption("BoolOpt2", true));
		EXPECT_EQ(true, mgr.GetBool("BoolOpt2"));
	}
}