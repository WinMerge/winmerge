#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "FileFilterHelper.h"
#include "FileFilter.h"
#include "Environment.h"
#include "paths.h"
#include "DiffItem.h"

namespace
{
	void SetDiffItem(const String& path, const String& left, const String& right, bool isfile, DIFFITEM& di)
	{
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.diffcode |= isfile ? DIFFCODE::FILE : DIFFCODE::DIR;
		di.diffFileInfo[0].path = path;
		di.diffFileInfo[0].filename = left;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[0].flags.attributes = FILE_ATTRIBUTE_DIRECTORY;
		di.diffFileInfo[1].path = path;
		di.diffFileInfo[1].filename = right;
		di.diffFileInfo[1].mtime = Poco::Timestamp();
		di.diffFileInfo[1].flags.attributes = FILE_ATTRIBUTE_DIRECTORY;
	}

	// The fixture for testing string differencing functions.
	class FileFilterHelperTest : public testing::Test
	{
	protected:
		String m_oldProgPath;

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
			m_oldProgPath = env::GetProgPath();
			env::SetProgPath(m_oldProgPath + _T("/../FileFilter"));
			m_fileFilterHelper.LoadAllFileFilters();
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
			env::SetProgPath(m_oldProgPath);
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

		filterpath = m_fileFilterHelper.GetFileFilterPath(_T("error include"));
		EXPECT_TRUE(filterpath.find_first_of(_T("Filters\\error_include.flt")) != String::npos);
		filtername = m_fileFilterHelper.GetFileFilterName(filterpath.c_str());
		EXPECT_TRUE(filtername.compare(_T("error include")) == 0);

		filterpath = m_fileFilterHelper.GetFileFilterPath(_T("non-existent file filter name"));
		EXPECT_TRUE(filterpath.empty());

		filtername = m_fileFilterHelper.GetFileFilterName(_T("non-existent file filter path"));
		EXPECT_TRUE(filtername.empty());
	}

	TEST_F(FileFilterHelperTest, GetFileFilters)
	{
		std::vector<FileFilterInfo> filters = m_fileFilterHelper.GetFileFilters();

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
			else if ((*it).name.compare(_T("error include")) == 0)
			{
				EXPECT_TRUE((*it).fullpath.find_first_of(_T("Filters\\error_include.flt"))  != String::npos);
				EXPECT_TRUE((*it).description.compare(_T("error file filter long description")) == 0);
			}
			else
			{
				EXPECT_TRUE(false);
			}
		}
	}

	TEST_F(FileFilterHelperTest, SetMask1)
	{
		m_fileFilterHelper.SetMask(_T("simple include file"));

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

		m_fileFilterHelper.SetMask(_T("fp:simple include dir"));

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

	TEST_F(FileFilterHelperTest, SetMask2)
	{
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
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T("abc\\def\\efg")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("abc.1\\def\\efg")));
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(_T("abc\\def.1\\efg")));

		m_fileFilterHelper.SetMask(_T("abc.\\def.\\"));
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(_T("abc\\def")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("abc.1\\def")));
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(_T("abc\\def.1")));

		DIFFITEM di{};

		// Test for bugs introduced in version 2.16.26
		m_fileFilterHelper.SetMask(_T("*.*"));
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(_T(".git\\config")));
		SetDiffItem(_T(".git"), _T("config"), _T("config"), true, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(di));

		m_fileFilterHelper.SetMask(_T("f: \\.o$ ; f: \\.lib$ ; f: \\.bak$; d: \\\\\\.svn$; d: \\\\_svn$;d:\\\\cvs$;d:\\\\\\.git$;d:\\\\\\.bzr$;d:\\\\\\.hg$;"));
		SetDiffItem(_T("abc"), _T("a.o"), _T("A.o"), true, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(di));
		SetDiffItem(_T(""), _T("abc.lib"), _T("abc.lib"), true, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(di));
		SetDiffItem(_T(""), _T("a d.bak"), _T("a d.bak"), true, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeFile(di));
		SetDiffItem(_T(""), _T(".svn"), _T(".svn"), false, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(di));
		SetDiffItem(_T("abc"), _T("_svn"), _T("_svn"), false, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(di));
		SetDiffItem(_T("abc"), _T("cvs"), _T("cvs"), false, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(di));
		SetDiffItem(_T("abc\\def"), _T(".git"), _T(".git"), false, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(di));
		SetDiffItem(_T("abc\\def"), _T(".bzr"), _T(".bzr"), false, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(di));
		SetDiffItem(_T("abc\\def"), _T(".hg"), _T(".hg"), false, di);
		EXPECT_EQ(true, m_fileFilterHelper.includeDir(di));
		SetDiffItem(_T("abc\\def"), _T("a.obj"), _T("a.obj"), true, di);
		EXPECT_EQ(false, m_fileFilterHelper.includeFile(di));
		SetDiffItem(_T("abc\\def"), _T("svv"), _T("svv"), false, di);
		EXPECT_EQ(false, m_fileFilterHelper.includeDir(di));

	}

	TEST_F(FileFilterHelperTest, Error)
	{
		m_fileFilterHelper.SetMask(_T("fp:error include"));
		EXPECT_EQ((size_t)8, m_fileFilterHelper.GetErrorList().size());
	}

}  // namespace
