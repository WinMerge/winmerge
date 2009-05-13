#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <direct.h>
#include "paths.h"

// Expects that C:\Temp exists for creating temporary testing files.

// TODO:
// paths_normalize() with rel path returns drive letter? how to test?
// paths_GetLongPath() - how to test?
// GetPairComparability() tests
// paths_IsShortcut() how to test so that test data is in SVN?
// ExpandShortcut() how to test so that test data is in SVN?
// paths_EnsurePathExist() tests


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

	TEST_F(PathTest, FindName_absfiles)
	{
		char path[] = "c:\\abc.txt";
		EXPECT_EQ(path + 3, paths_FindFileName(path));
	}

	TEST_F(PathTest, FindName_absfiles2)
	{
		char path[] = "c:\\temp\\abc.txt";
		EXPECT_EQ(path + 8, paths_FindFileName(path));
	}

	TEST_F(PathTest, Normalize_abspath)
	{
		String path = "c:\\abc.txt";
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath2)
	{
		String path = "c:\\temp\\abc.txt";
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath3)
	{
		String path = "C:";
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}

	TEST_F(PathTest, Normalize_abspath4)
	{
		String path = "c:\\";
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath5)
	{
		String path = "c:\\Temp\\";
		String path_orig = "c:\\Temp";
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}

	TEST_F(PathTest, Create_abspath1)
	{
		EXPECT_TRUE(paths_CreateIfNeeded("c:\\Temp"));
	}
	TEST_F(PathTest, Create_abspath2)
	{
		EXPECT_TRUE(paths_CreateIfNeeded("c:\\Temp\\wm_test"));
		EXPECT_EQ(0, _access("c:\\Temp\\wm_test", 0));
	}
	TEST_F(PathTest, Create_abspath3)
	{
		EXPECT_TRUE(paths_CreateIfNeeded("c:\\Temp\\wm_test"));
		EXPECT_EQ(0, _access("c:\\Temp\\wm_test", 0));
		EXPECT_EQ(0, rmdir("c:\\Temp\\wm_test"));
		EXPECT_NE(0, _access("c:\\Temp\\wm_test", 0));
	}
	TEST_F(PathTest, Create_abspath4)
	{
		EXPECT_TRUE(paths_CreateIfNeeded("c:\\Temp\\wm_test\\abc\\dce"));
		EXPECT_EQ(0, _access("c:\\Temp\\wm_test\\abc\\dce", 0));
	}
	TEST_F(PathTest, Create_abspath5)
	{
		EXPECT_TRUE(paths_CreateIfNeeded("c:\\Temp\\wm_test\\abc\\dce"));
		EXPECT_EQ(0, _access("c:\\Temp\\wm_test\\abc\\dce", 0));
		EXPECT_EQ(0, rmdir("c:\\Temp\\wm_test\\abc\\dce"));
		EXPECT_NE(0, _access("c:\\Temp\\wm_test\\abc\\dce", 0));
		EXPECT_EQ(0, rmdir("c:\\Temp\\wm_test\\abc"));
		EXPECT_NE(0, _access("c:\\Temp\\wm_test\\abc", 0));
		EXPECT_EQ(0, rmdir("c:\\Temp\\wm_test"));
		EXPECT_NE(0, _access("c:\\Temp\\wm_test", 0));
	}

	TEST_F(PathTest, Concat_abspath1)
	{
		EXPECT_EQ("c:\\Temp\\wm_test", paths_ConcatPath("c:\\Temp", "wm_test"));
	}
	TEST_F(PathTest, Concat_abspath2)
	{
		EXPECT_EQ("c:\\Temp\\wm_test", paths_ConcatPath("c:\\Temp\\", "wm_test"));
	}
	TEST_F(PathTest, Concat_abspath3)
	{
		EXPECT_EQ("c:\\Temp\\wm_test", paths_ConcatPath("c:\\Temp", "\\wm_test"));
	}
	TEST_F(PathTest, Concat_abspath4)
	{
		EXPECT_EQ("c:\\Temp\\wm_test", paths_ConcatPath("c:\\Temp\\", "\\wm_test"));
	}
	TEST_F(PathTest, Concat_relpath1)
	{
		EXPECT_EQ("\\Temp\\wm_test", paths_ConcatPath("\\Temp", "wm_test"));
	}
	TEST_F(PathTest, Concat_relpath2)
	{
		EXPECT_EQ("\\Temp\\wm_test", paths_ConcatPath("\\Temp\\", "wm_test"));
	}
	TEST_F(PathTest, Concat_relpath3)
	{
		EXPECT_EQ("\\Temp\\wm_test", paths_ConcatPath("\\Temp", "\\wm_test"));
	}
	TEST_F(PathTest, Concat_relpath4)
	{
		EXPECT_EQ("\\Temp\\wm_test", paths_ConcatPath("\\Temp\\", "\\wm_test"));
	}

	TEST_F(PathTest, Parent_abspath1)
	{
		EXPECT_EQ("c:\\Temp", paths_GetParentPath("c:\\Temp\\wm_test"));
	}
	TEST_F(PathTest, Parent_abspath2)
	{
		EXPECT_EQ("c:\\Temp\\abc", paths_GetParentPath("c:\\Temp\\abc\\wm_test"));
	}
	
	// FAILS!
	// paths_GetParentPath("c:\\Temp") "returns c:"
	TEST_F(PathTest, Parent_abspath3)
	{
		EXPECT_EQ("c:\\", paths_GetParentPath("c:\\Temp"));
	}

	// FAILS!
	// paths_GetParentPath("c:\\Temp\\") "returns c:"
	TEST_F(PathTest, Parent_abspath4)
	{
		EXPECT_EQ("c:\\", paths_GetParentPath("c:\\Temp\\"));
	}
	TEST_F(PathTest, Parent_relpath1)
	{
		EXPECT_EQ("\\temp", paths_GetParentPath("\\temp\\abc"));
	}
	TEST_F(PathTest, Parent_relpath2)
	{
		EXPECT_EQ("\\temp\\abc", paths_GetParentPath("\\temp\\abc\\cde"));
	}

	TEST_F(PathTest, LastSubdir_abspath1)
	{
		EXPECT_EQ("\\abc", paths_GetLastSubdir("c:\\temp\\abc"));
	}
	
	// FAILS!
	// paths_GetLastSubdir("c:\\temp") returns "c:\\temp"
	TEST_F(PathTest, LastSubdir_abspath2)
	{
		EXPECT_EQ("\\temp", paths_GetLastSubdir("c:\\temp"));
	}

	// FAILS!
	// paths_GetLastSubdir("c:\\temp") returns "c:\\temp"
	TEST_F(PathTest, LastSubdir_abspath3)
	{
		EXPECT_EQ("\\temp", paths_GetLastSubdir("c:\\temp\\"));
	}
	TEST_F(PathTest, LastSubdir_relpath1)
	{
		EXPECT_EQ("\\temp", paths_GetLastSubdir("abc\\temp\\"));
	}
	TEST_F(PathTest, LastSubdir_relpath2)
	{
		EXPECT_EQ("\\dce", paths_GetLastSubdir("abc\\temp\\dce"));
	}

	TEST_F(PathTest, IsAbsolute_abspath1)
	{
		EXPECT_TRUE(paths_IsPathAbsolute("c:\\abc"));
	}
	TEST_F(PathTest, IsAbsolute_abspath2)
	{
		EXPECT_TRUE(paths_IsPathAbsolute("c:\\abc\\"));
	}
	TEST_F(PathTest, IsAbsolute_abspath3)
	{
		EXPECT_TRUE(paths_IsPathAbsolute("c:\\abc\\cde"));
	}
	TEST_F(PathTest, IsAbsolute_abspath4)
	{
		EXPECT_TRUE(paths_IsPathAbsolute("c:\\abc\\cde\\"));
	}
	TEST_F(PathTest, IsAbsolute_abspath5)
	{
		EXPECT_TRUE(paths_IsPathAbsolute("c:\\"));
	}
	TEST_F(PathTest, IsAbsolute_relpath1)
	{
		EXPECT_FALSE(paths_IsPathAbsolute("\\cde"));
	}
	TEST_F(PathTest, IsAbsolute_relpath2)
	{
		EXPECT_FALSE(paths_IsPathAbsolute("\\cde\\"));
	}
	TEST_F(PathTest, IsAbsolute_relpath3)
	{
		EXPECT_FALSE(paths_IsPathAbsolute("\\cde\\abd"));
	}
	TEST_F(PathTest, IsAbsolute_relpath4)
	{
		EXPECT_FALSE(paths_IsPathAbsolute("\\cde\\abd\\"));
	}
	TEST_F(PathTest, IsAbsolute_relpath5)
	{
		EXPECT_FALSE(paths_IsPathAbsolute("cde\\abd"));
	}

	TEST_F(PathTest, IsAbsolute_relpath6)
	{
		EXPECT_FALSE(paths_IsPathAbsolute("cde\\abd"));
	}

}  // namespace
