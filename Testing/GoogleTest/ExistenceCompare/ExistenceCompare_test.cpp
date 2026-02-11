#include "pch.h"
#include <gtest/gtest.h>
#include <Poco/Timestamp.h>
#include "DiffContext.h"
#include "DiffItem.h"
#include "DiffWrapper.h"
#include "CompareEngines/ExistenceCompare.h"

namespace
{
	// The fixture for testing paths functions.
	class ExistenceCompareTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		ExistenceCompareTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~ExistenceCompareTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	TEST_F(ExistenceCompareTest, Identical)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_EXISTENCE);
		CompareEngines::ExistenceCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_EXISTENCE);
		CompareEngines::ExistenceCompare tsc3(ctxt3);

		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		EXPECT_EQ(int(DIFFCODE::SAME), tsc.CompareFiles(di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(int(DIFFCODE::SAME), tsc3.CompareFiles(di));
	}

	TEST_F(ExistenceCompareTest, UniqueFile)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_EXISTENCE);
		CompareEngines::ExistenceCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_EXISTENCE);
		CompareEngines::ExistenceCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::SAME), tsc.CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF), tsc.CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(int(DIFFCODE::DIFF), tsc.CompareFiles(di));

		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::SAME), tsc3.CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), tsc3.CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), tsc3.CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), tsc3.CompareFiles(di));


		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), tsc3.CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), tsc3.CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), tsc3.CompareFiles(di));
	}

}  // namespace
