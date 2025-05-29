#include "pch.h"
#include <gtest/gtest.h>
#include "FilterEngine/FilterEngine.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "PathContext.h"
#include "Poco/DateTimeParser.h"

namespace
{
	// The fixture for testing paths functions.
	class FilterEngineTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		FilterEngineTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~FilterEngineTest()
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

	TEST_F(FilterEngineTest, Parse1)
	{
		return;
		PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
		CDiffContext ctxt(paths, 0);
		FilterContext fc(&ctxt);
		DIFFITEM di;
		int tdz = 0;
		di.diffFileInfo[0].filename = L"Alice.txt";
		di.diffFileInfo[0].size = 1000;
		Poco::DateTime dt0 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M", "2025-05-16 15:34:56", tdz);
		di.diffFileInfo[0].mtime = dt0.timestamp();
		di.diffFileInfo[1].filename = L"Alice.txt";
		di.diffFileInfo[1].size = 1100;
		Poco::DateTime dt1 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M:%S", "2025-05-16 15:34:57", tdz);
		di.diffFileInfo[1].mtime = dt1.timestamp();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);

		EXPECT_TRUE(FilterEngine::Parse(L"Size <= 1000", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"allof(Size <= 1000)", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"anyof(Size <= 1000)", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"LeftDate < now()", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"LeftDate != RightDate", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"abs(-100) == 100", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"abs(LeftSize - RightSize) == (1100 - 1000)", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"LeftSize <= 100 * (1 + 9)", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"LeftSize < 200 + 20 * 40", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"LeftName = \"Alice.txt\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"RightName = \"Bob.txt\"", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"LeftName CONTAINS \"alice\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"RightName contains \".txt\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"RightName matches \"a.*t\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"123 == 123", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2 < 3", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2 <= 3", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"3 <= 3", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"3 > 2", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"3 >= 2", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"3 >= 3", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-27\" == d\"2025-05-27\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-27\" < d\"2025-05-28\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-27\" <= d\"2025-05-28\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28\" <= d\"2025-05-28\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28\" > d\"2025-05-27\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28\" >= d\"2025-05-27\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28\" >= d\"2025-05-28\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"\"abc\" == \"abc\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"\"abb\" < \"abc\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"\"abb\" <= \"abc\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"\"abc\" <= \"abc\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"\"abc\" > \"abb\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"\"abc\" >= \"abb\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"\"abc\" >= \"abc\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.48.2\" == v\"002.016.048.002\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.48.2\" < v\"2.16.49\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.48.2\" <= v\"2.16.49\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.49\" <= v\"2.16.49\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.49\" > v\"2.16.48.2\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.49\" >= v\"2.16.48.2\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.49\" >= v\"2.16.49\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"1weeks == 7days", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1week == 7day", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1w == 7d", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1days == 24hours", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1day == 24hour", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1d == 24hr", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1d == 24h", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1hours == 60minutes", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1hour == 60minute", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1h == 60min", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1h == 60m", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1minutes == 60seconds", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1minute == 60second", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1min == 60sec", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1m == 60s", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1seconds == 1000milliseconds", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1second == 1000millisecond", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1sec == 1000msec", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1s == 1000ms", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"2hours == 60minutes", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2hour == 60minute", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2h == 60min", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2h == 60m", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2minutes == 60seconds", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2minute == 60second", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2min == 60sec", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2m == 60s", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2seconds == 1000milliseconds", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2second == 1000millisecond", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2sec == 1000msec", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2s == 1000ms", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"1KB == 1024", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1MB == 1024KB", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1GB == 1024MB", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"1TB == 1024GB", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"2KB == 1024", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2MB == 1024KB", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2GB == 1024MB", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"2TB == 1024GB", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.48.2\" == v\"002.016.048.002\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-27\" == d\"2025-05-27 00:00:00\"", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28\" == d\"2025-05-21\" + 1week", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28\" == d\"2025-05-27\" + 1day", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28 12:34:56\" == d\"2025-05-28 11:34:56\" + 1hour", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28 12:34:56\" == d\"2025-05-28 12:33:56\" + 1minute", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28 12:34:56\" == d\"2025-05-28 12:34:55\" + 1second", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28 12:34:56\" == d\"2025-05-28 12:34:55\" + 1000millisecond", fc));
		EXPECT_TRUE(FilterEngine::Evaluate(fc, di));

		EXPECT_TRUE(FilterEngine::Parse(L"v\"2.16.48.3\" == v\"002.016.048.002\"", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-28\" == d\"2025-05-27 00:00:00\"", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-29\" == d\"2025-05-21\" + 1week", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-29\" == d\"2025-05-27\" + 1day", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-29 12:34:56\" == d\"2025-05-28 11:34:56\" + 1hour", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-29 12:34:56\" == d\"2025-05-28 12:33:56\" + 1minute", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-29 12:34:56\" == d\"2025-05-28 12:34:55\" + 1second", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
		EXPECT_TRUE(FilterEngine::Parse(L"d\"2025-05-29 12:34:56\" == d\"2025-05-28 12:34:55\" + 1000millisecond", fc));
		EXPECT_FALSE(FilterEngine::Evaluate(fc, di));
	}

	TEST_F(FilterEngineTest, ParseError)
	{
		PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
		CDiffContext ctxt(paths, 0);
		FilterContext fc(&ctxt);
//		EXPECT_FALSE(FilterEngine::Parse(L"LeftDate $ a", fc));
		EXPECT_FALSE(FilterEngine::Parse(L"LeftName = \"aaa", fc));
//		EXPECT_FALSE(FilterEngine::Parse(L"LeftSize = 100 RightSize < 100", fc));
	}

}  // namespace

