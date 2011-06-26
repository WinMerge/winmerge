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
			TCHAR exepath[_MAX_PATH];
			GetModuleFileName(NULL, exepath, sizeof(exepath));
			if (_tcsstr(exepath, _T("Paths_test")) != NULL)
				SetCurrentDirectory(paths_GetParentPath(exepath).c_str());
			else
				SetCurrentDirectory((paths_GetParentPath(exepath) + _T("\\..\\Paths\\Debug")).c_str());
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	//*************************
	// paths_EndsWithSlash()
	//*************************

	TEST_F(PathTest, EndsWithSlash_rootfolders)
	{
		const TCHAR path[] = _T("c:");
		const TCHAR path2[] = _T("c:\\");
		EXPECT_FALSE(paths_EndsWithSlash(path));
		EXPECT_TRUE(paths_EndsWithSlash(path2));
	}

	TEST_F(PathTest, EndsWithSlash_shortfolders)
	{
		const TCHAR path[] = _T("c:\\path");
		const TCHAR path2[] = _T("c:\\path\\");
		EXPECT_FALSE(paths_EndsWithSlash(path));
		EXPECT_TRUE(paths_EndsWithSlash(path2));
	}

	TEST_F(PathTest, EndsWithSlash_relfolders)
	{
		const TCHAR path[] = _T("\\path");
		const TCHAR path2[] = _T("\\path\\");
		EXPECT_FALSE(paths_EndsWithSlash(path));
		EXPECT_TRUE(paths_EndsWithSlash(path2));
	}

	//*************************
	// paths_DoesPathExist()
	//*************************

	TEST_F(PathTest, Exists_specialfolders)
	{
		const TCHAR path[] = _T(".");
		const TCHAR path2[] = _T("..");
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_files)
	{
		const TCHAR path[] = _T("paths_test.exe");
		const TCHAR path2[] = _T("notfound.txt");
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_relfolder)
	{
		const TCHAR path[] = _T("..\\");
		const TCHAR path2[] = _T("..\\..\\Paths");
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_relfiles)
	{
		const TCHAR path[] = _T("..\\paths_test.cpp");
		const TCHAR path2[] = _T("..\\..\\Paths\\paths_test.cpp");
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_absfolders)
	{
		const TCHAR path[] = _T("C:");
		const TCHAR path2[] = _T("C:\\");
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_absfolders2)
	{
		const TCHAR path[] = _T("C:\\windows");
		const TCHAR path2[] = _T("C:\\windows\\");
		const TCHAR path3[] = _T("C:\\gack");
		const TCHAR path4[] = _T("C:\\gack\\");
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path));
		EXPECT_EQ(IS_EXISTING_DIR, paths_DoesPathExist(path2));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path3));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path4));
	}
	TEST_F(PathTest, Exists_absfiles)
	{
		const TCHAR path1[] = _T("C:\\windows\\notepad.exe");
		const TCHAR path2[] = _T("C:\\windows\\gack.txt");
		EXPECT_EQ(IS_EXISTING_FILE, paths_DoesPathExist(path1));
		EXPECT_EQ(DOES_NOT_EXIST, paths_DoesPathExist(path2));
	}

	//*************************
	// paths_FindFileName()
	//*************************

	TEST_F(PathTest, FindName_absfiles)
	{
		TCHAR path[] = _T("c:\\abc.txt");
		EXPECT_EQ(path + 3, paths_FindFileName(path));
	}

	TEST_F(PathTest, FindName_absfiles2)
	{
		TCHAR path[] = _T("c:\\temp\\abc.txt");
		EXPECT_EQ(path + 8, paths_FindFileName(path));
	}

	//*************************
	// paths_normalize()
	//*************************

	TEST_F(PathTest, Normalize_abspath)
	{
		String path = _T("c:\\abc.txt");
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath2)
	{
		String path = _T("c:\\temp\\abc.txt");
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath3)
	{
		String path = _T("C:");
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}

	TEST_F(PathTest, Normalize_abspath4)
	{
		String path = _T("c:\\");
		String path_orig = path;
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath5)
	{
		String path = _T("c:\\Temp\\");
		String path_orig = _T("c:\\Temp");
		paths_normalize(path);
		EXPECT_EQ(path_orig, path);
	}

	//*************************
	// paths_CreateIfNeeded()
	//*************************

	TEST_F(PathTest, Create_abspath1)
	{
		EXPECT_TRUE(paths_CreateIfNeeded(_T("c:\\Temp")));
	}
	TEST_F(PathTest, Create_abspath2)
	{
		EXPECT_TRUE(paths_CreateIfNeeded(_T("c:\\Temp\\wm_test")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
	}
	TEST_F(PathTest, Create_abspath3)
	{
		EXPECT_TRUE(paths_CreateIfNeeded(_T("c:\\Temp\\wm_test")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
	}
	TEST_F(PathTest, Create_abspath4)
	{
		EXPECT_TRUE(paths_CreateIfNeeded(_T("c:\\Temp\\wm_test\\abc\\dce")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test\\abc\\dce"), 0));
	}
	TEST_F(PathTest, Create_abspath5)
	{
		EXPECT_TRUE(paths_CreateIfNeeded(_T("c:\\Temp\\wm_test\\abc\\dce")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test\\abc\\dce"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test\\abc\\dce")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test\\abc\\dce"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test\\abc")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test\\abc"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
	}

	//*********************
	// paths_ConcatPath()
	//*********************

	TEST_F(PathTest, Concat_abspath1)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths_ConcatPath(_T("c:\\Temp"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_abspath2)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths_ConcatPath(_T("c:\\Temp\\"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_abspath3)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths_ConcatPath(_T("c:\\Temp"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_abspath4)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths_ConcatPath(_T("c:\\Temp\\"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_relpath1)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths_ConcatPath(_T("\\Temp"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_relpath2)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths_ConcatPath(_T("\\Temp\\"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_relpath3)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths_ConcatPath(_T("\\Temp"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_relpath4)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths_ConcatPath(_T("\\Temp\\"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_missingFilename1)
	{
		EXPECT_EQ(_T("c:\\Temp"), paths_ConcatPath(_T("c:\\Temp"), _T("")));
	}
	TEST_F(PathTest, Concat_missingFilename2)
	{
		EXPECT_EQ(_T("\\Temp\\"), paths_ConcatPath(_T("\\Temp\\"), _T("")));
	}

	//*************************
	// paths_GetParentPath()
	//*************************

	TEST_F(PathTest, Parent_abspath1)
	{
		EXPECT_EQ(_T("c:\\Temp"), paths_GetParentPath(_T("c:\\Temp\\wm_test")));
	}
	TEST_F(PathTest, Parent_abspath2)
	{
		EXPECT_EQ(_T("c:\\Temp\\abc"), paths_GetParentPath(_T("c:\\Temp\\abc\\wm_test")));
	}
	
	// FAILS!
	// paths_GetParentPath(_T("c:\\Temp")) "returns c:"
	TEST_F(PathTest, Parent_abspath3)
	{
		EXPECT_EQ(_T("c:\\"), paths_GetParentPath(_T("c:\\Temp")));
	}

	// FAILS!
	// paths_GetParentPath(_T("c:\\Temp\\")) "returns c:"
	TEST_F(PathTest, Parent_abspath4)
	{
		EXPECT_EQ(_T("c:\\"), paths_GetParentPath(_T("c:\\Temp\\")));
	}
	TEST_F(PathTest, Parent_relpath1)
	{
		EXPECT_EQ(_T("\\temp"), paths_GetParentPath(_T("\\temp\\abc")));
	}
	TEST_F(PathTest, Parent_relpath2)
	{
		EXPECT_EQ(_T("\\temp\\abc"), paths_GetParentPath(_T("\\temp\\abc\\cde")));
	}

	//*************************
	// paths_GetLastSubdir()
	//*************************

	TEST_F(PathTest, LastSubdir_abspath1)
	{
		EXPECT_EQ(_T("\\abc"), paths_GetLastSubdir(_T("c:\\temp\\abc")));
	}
	
	// FAILS!
	// paths_GetLastSubdir(_T("c:\\temp")) returns "c:\\temp"
	TEST_F(PathTest, LastSubdir_abspath2)
	{
		EXPECT_EQ(_T("\\temp"), paths_GetLastSubdir(_T("c:\\temp")));
	}

	// FAILS!
	// paths_GetLastSubdir(_T("c:\\temp")) returns "c:\\temp"
	TEST_F(PathTest, LastSubdir_abspath3)
	{
		EXPECT_EQ(_T("\\temp"), paths_GetLastSubdir(_T("c:\\temp\\")));
	}
	TEST_F(PathTest, LastSubdir_relpath1)
	{
		EXPECT_EQ(_T("\\temp"), paths_GetLastSubdir(_T("abc\\temp\\")));
	}
	TEST_F(PathTest, LastSubdir_relpath2)
	{
		EXPECT_EQ(_T("\\dce"), paths_GetLastSubdir(_T("abc\\temp\\dce")));
	}

	//*************************
	// paths_IsPathAbsolute()
	//*************************

	TEST_F(PathTest, IsAbsolute_abspath1)
	{
		EXPECT_TRUE(paths_IsPathAbsolute(_T("c:\\abc")));
	}
	TEST_F(PathTest, IsAbsolute_abspath2)
	{
		EXPECT_TRUE(paths_IsPathAbsolute(_T("c:\\abc\\")));
	}
	TEST_F(PathTest, IsAbsolute_abspath3)
	{
		EXPECT_TRUE(paths_IsPathAbsolute(_T("c:\\abc\\cde")));
	}
	TEST_F(PathTest, IsAbsolute_abspath4)
	{
		EXPECT_TRUE(paths_IsPathAbsolute(_T("c:\\abc\\cde\\")));
	}
	TEST_F(PathTest, IsAbsolute_abspath5)
	{
		EXPECT_TRUE(paths_IsPathAbsolute(_T("c:\\")));
	}
	TEST_F(PathTest, IsAbsolute_relpath1)
	{
		EXPECT_FALSE(paths_IsPathAbsolute(_T("\\cde")));
	}
	TEST_F(PathTest, IsAbsolute_relpath2)
	{
		EXPECT_FALSE(paths_IsPathAbsolute(_T("\\cde\\")));
	}
	TEST_F(PathTest, IsAbsolute_relpath3)
	{
		EXPECT_FALSE(paths_IsPathAbsolute(_T("\\cde\\abd")));
	}
	TEST_F(PathTest, IsAbsolute_relpath4)
	{
		EXPECT_FALSE(paths_IsPathAbsolute(_T("\\cde\\abd\\")));
	}
	TEST_F(PathTest, IsAbsolute_relpath5)
	{
		EXPECT_FALSE(paths_IsPathAbsolute(_T("cde\\abd")));
	}

	TEST_F(PathTest, IsAbsolute_relpath6)
	{
		EXPECT_FALSE(paths_IsPathAbsolute(_T("cde\\abd")));
	}

}  // namespace
