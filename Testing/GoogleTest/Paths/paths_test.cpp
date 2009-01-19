#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include "paths.h"

namespace
{
	// The fixture for testing paths functions.
	class PathTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		PathTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~PathTest()
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
	TEST_F(PathTest, EndsWithSlash_rootfolders)
	{
		const TCHAR path[] = "c:";
		const TCHAR path2[] = "c:\\";
		EXPECT_FALSE(paths_EndsWithSlash(path));
		EXPECT_TRUE(paths_EndsWithSlash(path2));
	}

	TEST_F(PathTest, EndsWithSlash_shortfolders)
	{
		const TCHAR path[] = "c:\\path";
		const TCHAR path2[] = "c:\\path\\";
		EXPECT_FALSE(paths_EndsWithSlash(path));
		EXPECT_TRUE(paths_EndsWithSlash(path2));
	}

	TEST_F(PathTest, EndsWithSlash_relfolders)
	{
		const TCHAR path[] = "\\path";
		const TCHAR path2[] = "\\path\\";
		EXPECT_FALSE(paths_EndsWithSlash(path));
		EXPECT_TRUE(paths_EndsWithSlash(path2));
	}

	// Test the paths_DoesPathExist() function
	TEST_F(PathTest, Exists_specialfolders)
	{
		const TCHAR path[] = ".";
		const TCHAR path2[] = "..";
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_files)
	{
		const TCHAR path[] = "paths_test.exe";
		const TCHAR path2[] = "notfound.txt";
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_relfolder)
	{
		const TCHAR path[] = "..\\";
		const TCHAR path2[] = "..\\..\\Paths";
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_relfiles)
	{
		const TCHAR path[] = "..\\paths_test.cpp";
		const TCHAR path2[] = "..\\..\\Paths\\paths_test.cpp";
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_absfolders)
	{
		const TCHAR path[] = "C:";
		const TCHAR path2[] = "C:\\";
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_absfolders2)
	{
		const TCHAR path[] = "C:\\windows";
		const TCHAR path2[] = "C:\\windows\\";
		const TCHAR path3[] = "C:\\gack";
		const TCHAR path4[] = "C:\\gack\\";
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path3));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path4));
	}
	TEST_F(PathTest, Exists_absfiles)
	{
		const TCHAR path1[] = "C:\\windows\\notepad.exe";
		const TCHAR path2[] = "C:\\windows\\gack.txt";
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path1));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path2));
	}
}  // namespace

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
