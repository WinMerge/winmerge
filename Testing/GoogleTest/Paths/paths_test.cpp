#include "pch.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <tchar.h>
#include "Environment.h"
#include "paths.h"

// Expects that C:\Temp exists for creating temporary testing files.

// TODO:
// paths::normalize() with rel path returns drive letter? how to test?
// paths::GetLongPath() - how to test?
// paths::GetPairComparability() tests
// paths::IsShortcut() how to test so that test data is in SVN?
// ExpandShortcut() how to test so that test data is in SVN?
// paths::EnsurePathExist() tests


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

	//*************************
	// paths::EndsWithSlash()
	//*************************

	TEST_F(PathTest, EndsWithSlash_rootfolders)
	{
		const tchar_t path[] = _T("c:");
		const tchar_t path2[] = _T("c:\\");
		EXPECT_FALSE(paths::EndsWithSlash(path));
		EXPECT_TRUE(paths::EndsWithSlash(path2));
	}

	TEST_F(PathTest, EndsWithSlash_shortfolders)
	{
		const tchar_t path[] = _T("c:\\path");
		const tchar_t path2[] = _T("c:\\path\\");
		EXPECT_FALSE(paths::EndsWithSlash(path));
		EXPECT_TRUE(paths::EndsWithSlash(path2));
	}

	TEST_F(PathTest, EndsWithSlash_relfolders)
	{
		const tchar_t path[] = _T("\\path");
		const tchar_t path2[] = _T("\\path\\");
		EXPECT_FALSE(paths::EndsWithSlash(path));
		EXPECT_TRUE(paths::EndsWithSlash(path2));
	}

	//*************************
	// paths::DoesPathExist()
	//*************************

	TEST_F(PathTest, Exists_specialfolders)
	{
		const tchar_t path[] = _T(".");
		const tchar_t path2[] = _T("..");
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path));
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_files)
	{
		const tchar_t path2[] = _T("notfound.txt");
		String path = env::GetProgPath() + _T("../README.md");
		EXPECT_EQ(paths::IS_EXISTING_FILE, paths::DoesPathExist(path));
		EXPECT_EQ(paths::DOES_NOT_EXIST, paths::DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_relfolder)
	{
		const tchar_t path[] = _T("..\\");
		const tchar_t path2[] = _T("..\\Paths");
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path));
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_relfiles)
	{
		const tchar_t path[] = _T(".\\..\\Paths\\paths_test.cpp");
		const tchar_t path2[] = _T("..\\Paths\\paths_test.cpp");
		EXPECT_EQ(paths::IS_EXISTING_FILE, paths::DoesPathExist(path));
		EXPECT_EQ(paths::IS_EXISTING_FILE, paths::DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_absfolders)
	{
		const tchar_t path[] = _T("C:");
		const tchar_t path2[] = _T("C:\\");
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path));
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path2));
	}
	TEST_F(PathTest, Exists_absfolders2)
	{
		const tchar_t path[] = _T("C:\\windows");
		const tchar_t path2[] = _T("C:\\windows\\");
		const tchar_t path3[] = _T("C:\\gack");
		const tchar_t path4[] = _T("C:\\gack\\");
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path));
		EXPECT_EQ(paths::IS_EXISTING_DIR, paths::DoesPathExist(path2));
		EXPECT_EQ(paths::DOES_NOT_EXIST, paths::DoesPathExist(path3));
		EXPECT_EQ(paths::DOES_NOT_EXIST, paths::DoesPathExist(path4));
	}
	TEST_F(PathTest, Exists_absfiles)
	{
		const tchar_t path1[] = _T("C:\\windows\\notepad.exe");
		const tchar_t path2[] = _T("C:\\windows\\gack.txt");
		EXPECT_EQ(paths::IS_EXISTING_FILE, paths::DoesPathExist(path1));
		EXPECT_EQ(paths::DOES_NOT_EXIST, paths::DoesPathExist(path2));
	}

	//*************************
	// paths::FindFileName()
	//*************************

	TEST_F(PathTest, FindName_absfiles)
	{
		tchar_t path[] = _T("c:\\abc.txt");
		EXPECT_EQ(path + 3, paths::FindFileName(path));
	}

	TEST_F(PathTest, FindName_absfiles2)
	{
		tchar_t path[] = _T("c:\\temp\\abc.txt");
		EXPECT_EQ(path + 8, paths::FindFileName(path));
	}

	//*************************
	// paths::normalize()
	//*************************

	TEST_F(PathTest, Normalize_abspath)
	{
		String path = _T("c:\\abc.txt");
		String path_orig = path;
		paths::normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath2)
	{
		String path = _T("c:\\temp\\abc.txt");
		String path_orig = path;
		paths::normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath3)
	{
		String curdir = std::filesystem::current_path().native();
		String path(curdir, 2);
		String path_orig = curdir;
		paths::normalize(path);
		EXPECT_EQ(path_orig, path);
	}

	TEST_F(PathTest, Normalize_abspath4)
	{
		String path = _T("c:\\");
		String path_orig = path;
		paths::normalize(path);
		EXPECT_EQ(path_orig, path);
	}
	TEST_F(PathTest, Normalize_abspath5)
	{
		String path = _T("c:\\Temp\\");
		String path_orig = _T("c:\\Temp");
		paths::normalize(path);
		EXPECT_EQ(path_orig, path);
	}

	//*************************
	// paths::CreateIfNeeded()
	//*************************

	TEST_F(PathTest, Create_abspath1)
	{
		EXPECT_TRUE(paths::CreateIfNeeded(_T("c:\\Temp")));
	}
	TEST_F(PathTest, Create_abspath2)
	{
		EXPECT_TRUE(paths::CreateIfNeeded(_T("c:\\Temp\\wm_test")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
	}
	TEST_F(PathTest, Create_abspath3)
	{
		EXPECT_TRUE(paths::CreateIfNeeded(_T("c:\\Temp\\wm_test")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
	}
	TEST_F(PathTest, Create_abspath4)
	{
		EXPECT_TRUE(paths::CreateIfNeeded(_T("c:\\Temp\\wm_test\\abc\\dce")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test\\abc\\dce"), 0));
	}
	TEST_F(PathTest, Create_abspath5)
	{
		EXPECT_TRUE(paths::CreateIfNeeded(_T("c:\\Temp\\wm_test\\abc\\dce")));
		EXPECT_EQ(0, _taccess(_T("c:\\Temp\\wm_test\\abc\\dce"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test\\abc\\dce")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test\\abc\\dce"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test\\abc")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test\\abc"), 0));
		EXPECT_EQ(0, _trmdir(_T("c:\\Temp\\wm_test")));
		EXPECT_NE(0, _taccess(_T("c:\\Temp\\wm_test"), 0));
	}

	//*********************
	// paths::ConcatPath()
	//*********************

	TEST_F(PathTest, Concat_abspath1)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths::ConcatPath(_T("c:\\Temp"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_abspath2)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths::ConcatPath(_T("c:\\Temp\\"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_abspath3)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths::ConcatPath(_T("c:\\Temp"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_abspath4)
	{
		EXPECT_EQ(_T("c:\\Temp\\wm_test"), paths::ConcatPath(_T("c:\\Temp\\"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_relpath1)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths::ConcatPath(_T("\\Temp"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_relpath2)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths::ConcatPath(_T("\\Temp\\"), _T("wm_test")));
	}
	TEST_F(PathTest, Concat_relpath3)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths::ConcatPath(_T("\\Temp"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_relpath4)
	{
		EXPECT_EQ(_T("\\Temp\\wm_test"), paths::ConcatPath(_T("\\Temp\\"), _T("\\wm_test")));
	}
	TEST_F(PathTest, Concat_missingFilename1)
	{
		EXPECT_EQ(_T("c:\\Temp"), paths::ConcatPath(_T("c:\\Temp"), _T("")));
	}
	TEST_F(PathTest, Concat_missingFilename2)
	{
		EXPECT_EQ(_T("\\Temp\\"), paths::ConcatPath(_T("\\Temp\\"), _T("")));
	}

	//*************************
	// paths::GetParentPath()
	//*************************

	TEST_F(PathTest, Parent_abspath1)
	{
		EXPECT_EQ(_T("c:\\Temp"), paths::GetParentPath(_T("c:\\Temp\\wm_test")));
	}
	TEST_F(PathTest, Parent_abspath2)
	{
		EXPECT_EQ(_T("c:\\Temp\\abc"), paths::GetParentPath(_T("c:\\Temp\\abc\\wm_test")));
	}
	
	// FAILS!
	// paths::GetParentPath(_T("c:\\Temp")) "returns c:"
	TEST_F(PathTest, Parent_abspath3)
	{
		EXPECT_EQ(_T("c:\\"), paths::GetParentPath(_T("c:\\Temp")));
	}

	// FAILS!
	// paths::GetParentPath(_T("c:\\Temp\\")) "returns c:"
	TEST_F(PathTest, Parent_abspath4)
	{
		EXPECT_EQ(_T("c:\\"), paths::GetParentPath(_T("c:\\Temp\\")));
	}
	TEST_F(PathTest, Parent_relpath1)
	{
		EXPECT_EQ(_T("\\temp"), paths::GetParentPath(_T("\\temp\\abc")));
	}
	TEST_F(PathTest, Parent_relpath2)
	{
		EXPECT_EQ(_T("\\temp\\abc"), paths::GetParentPath(_T("\\temp\\abc\\cde")));
	}

	//*************************
	// paths::GetLastSubdir()
	//*************************

	TEST_F(PathTest, LastSubdir_abspath1)
	{
		EXPECT_EQ(_T("\\abc"), paths::GetLastSubdir(_T("c:\\temp\\abc")));
	}
	
	// FAILS!
	// paths::GetLastSubdir(_T("c:\\temp")) returns "c:\\temp"
	TEST_F(PathTest, LastSubdir_abspath2)
	{
		EXPECT_EQ(_T("\\temp"), paths::GetLastSubdir(_T("c:\\temp")));
	}

	// FAILS!
	// paths::GetLastSubdir(_T("c:\\temp")) returns "c:\\temp"
	TEST_F(PathTest, LastSubdir_abspath3)
	{
		EXPECT_EQ(_T("\\temp"), paths::GetLastSubdir(_T("c:\\temp\\")));
	}
	TEST_F(PathTest, LastSubdir_relpath1)
	{
		EXPECT_EQ(_T("\\temp"), paths::GetLastSubdir(_T("abc\\temp\\")));
	}
	TEST_F(PathTest, LastSubdir_relpath2)
	{
		EXPECT_EQ(_T("\\dce"), paths::GetLastSubdir(_T("abc\\temp\\dce")));
	}

	//*************************
	// paths::IsPathAbsolute()
	//*************************

	TEST_F(PathTest, IsAbsolute_abspath1)
	{
		EXPECT_TRUE(paths::IsPathAbsolute(_T("c:\\abc")));
	}
	TEST_F(PathTest, IsAbsolute_abspath2)
	{
		EXPECT_TRUE(paths::IsPathAbsolute(_T("c:\\abc\\")));
	}
	TEST_F(PathTest, IsAbsolute_abspath3)
	{
		EXPECT_TRUE(paths::IsPathAbsolute(_T("c:\\abc\\cde")));
	}
	TEST_F(PathTest, IsAbsolute_abspath4)
	{
		EXPECT_TRUE(paths::IsPathAbsolute(_T("c:\\abc\\cde\\")));
	}
	TEST_F(PathTest, IsAbsolute_abspath5)
	{
		EXPECT_TRUE(paths::IsPathAbsolute(_T("c:\\")));
	}
	TEST_F(PathTest, IsAbsolute_relpath1)
	{
		EXPECT_FALSE(paths::IsPathAbsolute(_T("\\cde")));
	}
	TEST_F(PathTest, IsAbsolute_relpath2)
	{
		EXPECT_FALSE(paths::IsPathAbsolute(_T("\\cde\\")));
	}
	TEST_F(PathTest, IsAbsolute_relpath3)
	{
		EXPECT_FALSE(paths::IsPathAbsolute(_T("\\cde\\abd")));
	}
	TEST_F(PathTest, IsAbsolute_relpath4)
	{
		EXPECT_FALSE(paths::IsPathAbsolute(_T("\\cde\\abd\\")));
	}
	TEST_F(PathTest, IsAbsolute_relpath5)
	{
		EXPECT_FALSE(paths::IsPathAbsolute(_T("cde\\abd")));
	}

	TEST_F(PathTest, IsAbsolute_relpath6)
	{
		EXPECT_FALSE(paths::IsPathAbsolute(_T("cde\\abd")));
	}

}  // namespace
