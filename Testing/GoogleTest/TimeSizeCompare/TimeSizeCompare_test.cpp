#include "pch.h"
#include <gtest/gtest.h>
#include <Poco/Timestamp.h>
#include "DiffContext.h"
#include "PathContext.h"
#include "DiffItem.h"
#include "DiffWrapper.h"
#include "CompareEngines/TimeSizeCompare.h"

namespace
{
	// The fixture for testing paths functions.
	class TimeSizeCompareTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		TimeSizeCompareTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~TimeSizeCompareTest()
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

	TEST_F(TimeSizeCompareTest, SizeCompare_SameSize)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_SIZE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_SIZE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 0;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[2].size = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, SizeCompare_DifferentSize)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_SIZE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_SIZE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, SizeCompare_UniqueFile)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_SIZE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_SIZE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 2;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, SizeCompare_TimeError)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_SIZE);
		ctxt.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_SIZE);
		ctxt3.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_SameTime)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		CDiffContext ctxti(paths, CMP_DATE);
		ctxti.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsci(ctxti);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		CDiffContext ctxt3i(paths3, CMP_DATE);
		ctxt3i.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc3i(ctxt3i);
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		tsci.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		tsci.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1 * 1000 * 1000);
		tsc3i.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1 * 1000 * 1000);
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3i.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1 * 1000 * 1000);
		tsc3i.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1 * 1000 * 1000);
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3i.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_DifferentTime)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		CDiffContext ctxti(paths, CMP_DATE);
		ctxti.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsci(ctxti);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		CDiffContext ctxt3i(paths3, CMP_DATE);
		ctxt3i.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc3i(ctxt3i);
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff + 1000 * 1000);
		tsci.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff + 1000 * 1000);
		tsci.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[1].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff + 1000 * 1000);
		di.diffFileInfo[2].mtime = di.diffFileInfo[1].mtime;
		tsc3i.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_UniqueFile)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 2 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 2 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 2 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_TimeError)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE);
		ctxt.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE);
		ctxt3.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp::TIMEVAL_MIN;
		di.diffFileInfo[1].mtime = Poco::Timestamp::TIMEVAL_MIN;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = Poco::Timestamp::TIMEVAL_MIN;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp::TIMEVAL_MIN;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = Poco::Timestamp::TIMEVAL_MIN;
		di.diffFileInfo[1].mtime = Poco::Timestamp::TIMEVAL_MIN;
		di.diffFileInfo[2].mtime = Poco::Timestamp::TIMEVAL_MIN;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = Poco::Timestamp::TIMEVAL_MIN;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = Poco::Timestamp::TIMEVAL_MIN;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = Poco::Timestamp::TIMEVAL_MIN;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_Same)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE_SIZE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE_SIZE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	
		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_Different)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE_SIZE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE_SIZE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	
		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1000 * 1000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[1].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffFileInfo[0].size = 2;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_UniqueFile)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE_SIZE);
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE_SIZE);
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 2 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 2 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 2 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_TimeError)
	{
		PathContext paths;
		paths.SetLeft(_T(""));
		paths.SetRight(_T(""));
		CDiffContext ctxt(paths, CMP_DATE_SIZE);
		ctxt.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T(""));
		paths3.SetMiddle(_T(""));
		paths3.SetRight(_T(""));
		CDiffContext ctxt3(paths3, CMP_DATE_SIZE);
		ctxt3.m_bIgnoreSmallTimeDiff = true;
		CompareEngines::TimeSizeCompare tsc3(ctxt3);
		DIFFITEM di;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		tsc.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1 * 1000 * 1000;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[1].mtime = 1 * 1000 * 1000;
		di.diffFileInfo[2].mtime = 0;
		tsc3.CompareFiles(di);
		EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::CMPERR), di.diffcode.diffcode & (DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
	}

}  // namespace
