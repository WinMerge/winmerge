#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "FileFilterHelper.h"
#include "Environment.h"
#include "paths.h"

namespace
{
	// The fixture for testing string differencing functions.
	class FileFilterHelperTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		FileFilterHelperTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~FileFilterHelperTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
			env::SetProgPath(env::GetProgPath() + _T("/../FileFilter"));
			m_fileFilterHelper.LoadAllFileFilters();
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
		FileFilterHelper m_fileFilterHelper;
	};

	TEST_F(FileFilterHelperTest, GetManager)
	{
		EXPECT_TRUE(m_fileFilterHelper.GetManager() != NULL);
	}

	TEST_F(FileFilterHelperTest, GetFileFilterPath)
	{
		String filterpath, filtername;
		filterpath = m_fileFilterHelper.GetFileFilterPath(_T("simple include file"));
		EXPECT_TRUE(filterpath.find_first_of(_T("Filters\\simple_include_file.flt")) != String::npos);
		filtername = m_fileFilterHelper.GetFileFilterName(filterpath.c_str());
		EXPECT_TRUE(filtername.compare(_T("simple include file")) == 0);

		filterpath = m_fileFilterHelper.GetFileFilterPath(_T("simple include dir"));
		EXPECT_TRUE(filterpath.find_first_of(_T("Filters\\simple_include_dir.flt")) != String::npos);
		filtername = m_fileFilterHelper.GetFileFilterName(filterpath.c_str());
		EXPECT_TRUE(filtername.compare(_T("simple include dir")) == 0);

		filterpath = m_fileFilterHelper.GetFileFilterPath(_T("non-existent file filter name"));
		EXPECT_TRUE(filterpath.empty());

		filtername = m_fileFilterHelper.GetFileFilterName(_T("non-existent file filter path"));
		EXPECT_TRUE(filtername.empty());
	}

	TEST_F(FileFilterHelperTest, SetFileFilterPath)
	{
		String selected;
		m_fileFilterHelper.SetFileFilterPath(_T(""));
		std::vector<FileFilterInfo> filters = m_fileFilterHelper.GetFileFilters(selected);
		EXPECT_TRUE(selected.compare(_T("")) == 0);

		m_fileFilterHelper.SetFileFilterPath(_T("non-existent file filter path"));
		filters = m_fileFilterHelper.GetFileFilters(selected);
		EXPECT_TRUE(selected.compare(_T("")) == 0);

		m_fileFilterHelper.SetFileFilterPath(m_fileFilterHelper.GetFileFilterPath(_T("simple include file")).c_str());
		filters = m_fileFilterHelper.GetFileFilters(selected);
		EXPECT_TRUE(selected.find_first_of(_T("Filters\\simple_include_file.flt")) != String::npos);
	}

	TEST_F(FileFilterHelperTest, GetFileFilters)
	{
		String selected;
		std::vector<FileFilterInfo> filters = m_fileFilterHelper.GetFileFilters(selected);

		for (std::vector<FileFilterInfo>::iterator it = filters.begin(); it != filters.end(); it++)
		{
			if ((*it).name.compare(_T("simple include file")) == 0)
			{
				EXPECT_TRUE((*it).fullpath.find_first_of(_T("Filters\\simple_include_file.flt")) != String::npos);
				EXPECT_TRUE((*it).description.compare(_T("simple file filter long description")) == 0);
			}
			else if ((*it).name.compare(_T("simple include dir")) == 0)
			{
				EXPECT_TRUE((*it).fullpath.find_first_of(_T("Filters\\simple_include_dir.flt"))  != String::npos);
				EXPECT_TRUE((*it).description.compare(_T("simple directory filter long description")) == 0);
			}
			else
			{
				EXPECT_TRUE(false);
			}
		}
	}

	TEST_F(FileFilterHelperTest, SetFilter)
	{
		m_fileFilterHelper.SetFilter(_T("simple include file"));
		EXPECT_EQ(false, m_fileFilterHelper.IsUsingMask());

		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.c")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cpp")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("a.ext")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("ex.ext")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("aex.ext")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("svn")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.ext")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b.c")));

		m_fileFilterHelper.SetFilter(_T("simple include dir"));

		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.c")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cpp")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.ext")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("svn")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("ex\\svn")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.ext")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b.c")));

	}

	TEST_F(FileFilterHelperTest, SetMask)
	{
		m_fileFilterHelper.UseMask(true);
		EXPECT_EQ(true, m_fileFilterHelper.IsUsingMask());

		m_fileFilterHelper.SetMask(_T(""));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.c")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cpp")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("svn")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b.c")));

		m_fileFilterHelper.SetMask(_T("*.c"));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.c")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("a.cpp")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("a.ext")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b.c")));

		m_fileFilterHelper.SetMask(_T("*.c;*.cpp;*.cxx"));
		EXPECT_EQ(true, m_fileFilterHelper.IsUsingMask());
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.c")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cpp")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cxx")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T(".cpp")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("cpp")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("a.cx")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("a.cpp.h")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("svn")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b.c")));

		m_fileFilterHelper.SetMask(_T("!*.h"));
		EXPECT_EQ(true, m_fileFilterHelper.IsUsingMask());
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.c")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cpp")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cxx")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T(".cpp")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("cpp")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cx")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("a.cpp.h")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("svn")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b.c")));

		m_fileFilterHelper.SetMask(_T("*.c*;!*.cxx;!Makefile;!.git\\;!abc\\;!de*hi\\;!Debug\\;!Release\\"));
		EXPECT_EQ(true, m_fileFilterHelper.IsUsingMask());
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.c")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cpp")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("a.cxx")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T(".cpp")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("cpp")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cx")));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("a.cpp.h")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("Makefile")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("svn")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("a.b.c")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T(".git")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("dir1\\.git")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("abc")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("defghi")));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("de\\hi")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("Release")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("dir1\\Release")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("Debug")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("dir1\\Debug")));

		m_fileFilterHelper.SetMask(_T("abc.\\def.\\*.*"));
		EXPECT_EQ(true, m_fileFilterHelper.IsUsingMask());
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("abc\\def\\efg")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("abc.1\\def\\efg")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("abc\\def.1\\efg")));

		m_fileFilterHelper.SetMask(_T("abc.\\def.\\"));
		EXPECT_EQ(true, m_fileFilterHelper.IsUsingMask());
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("abc\\def")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("abc.1\\def")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("abc\\def.1")));

		// Test for bugs introduced in version 2.16.26
		m_fileFilterHelper.SetMask(_T("*.*"));
		EXPECT_EQ(true, m_fileFilterHelper.IsUsingMask());
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T(".git\\config")));

	}



}  // namespace
