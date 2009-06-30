#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include "FileVersion.h"

namespace
{
	// The fixture for testing paths functions.
	class FileVersionTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		FileVersionTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~FileVersionTest()
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

	// Test the paths_EndsWithSlash() function
	TEST_F(FileVersionTest, construct)
	{
		FileVersion version;
	}

	TEST_F(FileVersionTest, getfilever_notset)
	{
		FileVersion version;
		EXPECT_EQ("", version.GetFileVersionString());
	}
	TEST_F(FileVersionTest, getprodver_notset)
	{
		FileVersion version;
		EXPECT_EQ("0.0.0.0", version.GetProductVersionString());
	}

	TEST_F(FileVersionTest, getfilever_zeros)
	{
		FileVersion version;
		version.SetFileVersion(0, 0);
		EXPECT_EQ("0.0.0.0", version.GetFileVersionString());
	}

	TEST_F(FileVersionTest, getfilever_simple1)
	{
		FileVersion version;
		version.SetFileVersion(1, 2);
		EXPECT_EQ("0.1.0.2", version.GetFileVersionString());
	}
	TEST_F(FileVersionTest, getfilever_real)
	{
		FileVersion version;
		DWORD hi = (1 << 16) | 2;
		DWORD lo = (3 << 16) | 4;
		version.SetFileVersion(hi, lo);
		EXPECT_EQ("1.2.3.4", version.GetFileVersionString());
	}
	
	TEST_F(FileVersionTest, getprodver_zeros)
	{
		FileVersion version;
		version.SetProductVersion(0, 0);
		EXPECT_EQ("0.0.0.0", version.GetProductVersionString());
	}

	TEST_F(FileVersionTest, getprodver_simple1)
	{
		FileVersion version;
		version.SetProductVersion(1, 2);
		EXPECT_EQ("0.1.0.2", version.GetProductVersionString());
	}
	TEST_F(FileVersionTest, getprodver_real)
	{
		FileVersion version;
		DWORD hi = (1 << 16) | 2;
		DWORD lo = (3 << 16) | 4;
		version.SetProductVersion(hi, lo);
		EXPECT_EQ("1.2.3.4", version.GetProductVersionString());
	}

}  // namespace
