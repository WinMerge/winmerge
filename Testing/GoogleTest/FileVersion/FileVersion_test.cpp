#include "pch.h"
#include <gtest/gtest.h>
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

	// Test the paths::EndsWithSlash() function
	TEST_F(FileVersionTest, construct)
	{
		FileVersion version;
	}

	TEST_F(FileVersionTest, getfilever_notset)
	{
		FileVersion version;
		EXPECT_EQ(_T(""), version.GetFileVersionString());
	}

	TEST_F(FileVersionTest, getfilever_zeros)
	{
		FileVersion version;
		version.SetFileVersion(0, 0);
		EXPECT_EQ(_T("0.0.0.0"), version.GetFileVersionString());
	}

	TEST_F(FileVersionTest, getfilever_simple1)
	{
		FileVersion version;
		version.SetFileVersion(1, 2);
		EXPECT_EQ(_T("0.1.0.2"), version.GetFileVersionString());
	}
	TEST_F(FileVersionTest, getfilever_real)
	{
		FileVersion version;
		unsigned hi = (1 << 16) | 2;
		unsigned lo = (3 << 16) | 4;
		version.SetFileVersion(hi, lo);
		EXPECT_EQ(_T("1.2.3.4"), version.GetFileVersionString());
	}
	
}  // namespace
