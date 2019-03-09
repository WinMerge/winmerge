#include "pch.h"
#include <gtest/gtest.h>
#include "Environment.h"

namespace
{
	// The fixture for testing paths functions.
	class EnvironmentTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		EnvironmentTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~EnvironmentTest()
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

	TEST_F(EnvironmentTest, LoadRegistryFromFile)
	{
		EXPECT_EQ(false, env::LoadRegistryFromFile(_T("nonexistfile.reg")));

		system(
			"reg add    HKCU\\Software\\Thingamahoochie\\test /v Path /t REG_EXPAND_SZ /d \"abc\" & "
			"reg export HKCU\\Software\\Thingamahoochie\\test test.reg & "
			"reg delete HKCU\\Software\\Thingamahoochie\\test /f");
		EXPECT_EQ(true, env::LoadRegistryFromFile(_T("test.reg")));
		EXPECT_EQ(0, system("reg query HKCU\\Software\\Thingamahoochie\\test /v Path | findstr abc"));
		system("reg delete HKCU\\Software\\Thingamahoochie\\test /f");
		remove("test.reg");
	}

	TEST_F(EnvironmentTest, SaveRegistry)
	{
		EXPECT_EQ(false, env::SaveRegistryToFile(_T("nonexistfile.reg"), _T("Software\\Thingamahoochie\\tes")));

		system("reg add    HKCU\\Software\\Thingamahoochie\\test /v Path /t REG_EXPAND_SZ /d \"abc\"");
		system("echo. > test2.reg");
		EXPECT_EQ(true, env::SaveRegistryToFile(_T("test2.reg"), _T("Software\\Thingamahoochie\\test")));
		EXPECT_EQ(0, system("type test2.reg|findstr Path"));
		system("reg delete HKCU\\Software\\Thingamahoochie\\test /f");
		remove("test2.reg");
	}


}  // namespace
