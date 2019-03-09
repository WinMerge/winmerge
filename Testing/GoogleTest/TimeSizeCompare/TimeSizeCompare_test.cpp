#include "pch.h"
#include <gtest/gtest.h>
#include <Poco/Timestamp.h>
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
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 0;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[2].size = 0;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_SIZE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, SizeCompare_DifferentSize)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 0;
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_SIZE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, SizeCompare_UniqueFile)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffcode.setSideNone();
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 2;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_SIZE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, SizeCompare_TimeError)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_SIZE, 2, di));

		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_SIZE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_SIZE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_SameTime)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;

		tsc.SetAdditionalOptions(false);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 2, di));

		tsc.SetAdditionalOptions(true);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 2, di));

		tsc.SetAdditionalOptions(true);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		tsc.SetAdditionalOptions(false);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 3, di));

		tsc.SetAdditionalOptions(true);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 3, di));

		tsc.SetAdditionalOptions(true);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 3, di));

		tsc.SetAdditionalOptions(true);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 3, di));

		tsc.SetAdditionalOptions(true);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff - 1);
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_DifferentTime)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff + 1);
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime - (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff + 1);
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1;
		di.diffFileInfo[2].mtime = di.diffFileInfo[1].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + (Poco::Timestamp::resolution() * CompareEngines::TimeSizeCompare::SmallTimeDiff + 1);
		di.diffFileInfo[2].mtime = di.diffFileInfo[1].mtime;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_UniqueFile)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 2;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 2;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 2;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_DATE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeCompare_TimeError)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE, 2, di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_Same)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));
	
		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_Different)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));
	
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));
	
		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 1;
		di.diffFileInfo[2].mtime = di.diffFileInfo[1].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffFileInfo[0].size = 2;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_UniqueFile)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::SAME, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 2;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 2;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 2;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 0;
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(false);
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));
	}

	TEST_F(TimeSizeCompareTest, TimeSizeCompare_TimeError)
	{
		CompareEngines::TimeSizeCompare tsc;
		DIFFITEM di;

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE_SIZE, 2, di));

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
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 0;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 0;
		di.diffFileInfo[2].mtime = 1;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[0].mtime = 1;
		di.diffFileInfo[1].mtime = 1;
		di.diffFileInfo[2].mtime = 0;
		tsc.SetAdditionalOptions(true);
		EXPECT_EQ(DIFFCODE::CMPERR, tsc.CompareFiles(CMP_DATE_SIZE, 3, di));
	}

}  // namespace
