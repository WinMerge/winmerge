#include "pch.h"
#include <gtest/gtest.h>
#include "FilterEngine/FilterExpression.h"
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

	TEST_F(FilterEngineTest, Literals)
	{
		PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
		CDiffContext ctxt(paths, 0);
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

		FilterExpression fe;
		fe.SetDiffContext(&ctxt);

		EXPECT_TRUE(fe.Parse("123 == 123"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("123 == 124"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2 < 3"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 < 3"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2 <= 3"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 <= 3"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 <= 2"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 > 2"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 > 3"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 >= 2"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 >= 3"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2 >= 3"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 != 3"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("3 != 2"));
		EXPECT_TRUE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" == d\"2025-05-27\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" == d\"2025-05-28\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" < d\"2025-05-28\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" < d\"2025-05-28\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" <= d\"2025-05-28\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" <= d\"2025-05-28\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-29\" <= d\"2025-05-28\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" > d\"2025-05-27\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" > d\"2025-05-28\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" >= d\"2025-05-27\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" >= d\"2025-05-28\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" >= d\"2025-05-29\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" != d\"2025-05-28\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" != d\"2025-05-27\""));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("\"abc\" == \"abc\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abc\" == \"abd\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abb\" < \"abc\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abb\" < \"abb\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abb\" <= \"abc\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abc\" <= \"abc\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abd\" <= \"abc\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abc\" > \"abb\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abc\" > \"abc\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abc\" >= \"abb\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abc\" >= \"abc\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("\"abc\" >= \"abd\""));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("v\"2.16.48.2\" == v\"002.016.048.002\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("v\"2.16.48.2\" < v\"2.16.49\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("v\"2.16.48.2\" <= v\"2.16.49\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("v\"2.16.49\" <= v\"2.16.49\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("v\"2.16.49\" > v\"2.16.48.2\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("v\"2.16.49\" >= v\"2.16.48.2\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("v\"2.16.49\" >= v\"2.16.49\""));
		EXPECT_TRUE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("1weeks == 7days"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1week == 7day"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1w == 7d"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1days == 24hours"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1day == 24hour"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1d == 24hr"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1d == 24h"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1hours == 60minutes"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1hour == 60minute"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1h == 60min"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1h == 60m"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1minutes == 60seconds"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1minute == 60second"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1min == 60sec"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1m == 60s"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1seconds == 1000milliseconds"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1second == 1000millisecond"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1sec == 1000msec"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1s == 1000ms"));
		EXPECT_TRUE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("2hours == 60minutes"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2hour == 60minute"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2h == 60min"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2h == 60m"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2minutes == 60seconds"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2minute == 60second"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2min == 60sec"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2m == 60s"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2seconds == 1000milliseconds"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2second == 1000millisecond"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2sec == 1000msec"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2s == 1000ms"));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("1KB == 1024"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1MB == 1024KB"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1GB == 1024MB"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("1TB == 1024GB"));
		EXPECT_TRUE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("2KB == 1024"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2MB == 1024KB"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2GB == 1024MB"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("2TB == 1024GB"));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" == d\"2025-05-27 00:00:00\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" == d\"2025-05-27 00:00:01\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" < d\"2025-05-27 00:00:01\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" < d\"2025-05-27 00:00:00\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" <= d\"2025-05-27 00:00:00\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" <= d\"2025-05-27 00:00:01\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27 00:00:02\" <= d\"2025-05-27 00:00:01\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27 00:00:01\" > d\"2025-05-27 00:00:00\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27 00:00:01\" > d\"2025-05-27 00:00:01\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27 00:00:01\" >= d\"2025-05-27 00:00:00\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27 00:00:01\" >= d\"2025-05-27 00:00:01\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27 00:00:01\" >= d\"2025-05-27 00:00:02\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" != d\"2025-05-27 00:00:01\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-27\" != d\"2025-05-27 00:00:00\""));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" == d\"2025-05-21\" + 1week"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" == d\"2025-05-27\" + 1day"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28 12:34:56\" == d\"2025-05-28 11:34:56\" + 1hour"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28 12:34:56\" == d\"2025-05-28 12:35:56\" - 1minute"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28 12:34:56\" == d\"2025-05-28 12:34:55\" + 1second"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-28 12:34:56\" == d\"2025-05-28 12:34:57\" - 1000millisecond"));
		EXPECT_TRUE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("d\"2025-05-28\" == d\"2025-05-27 00:00:00\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-29\" == d\"2025-05-21\" + 1week"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-29\" == d\"2025-05-27\" + 1day"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-29 12:34:56\" == d\"2025-05-28 11:34:56\" + 1hour"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-29 12:34:56\" == d\"2025-05-28 12:33:56\" + 1minute"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-29 12:34:56\" == d\"2025-05-28 12:34:55\" + 1second"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("d\"2025-05-29 12:34:56\" == d\"2025-05-28 12:34:55\" + 1000millisecond"));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("TRUE"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("FALSE"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("true"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("false"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("true = TRUE"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("true = false"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("false = FALSE"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("false = true"));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("not TRUE"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("not FALSE"));
		EXPECT_TRUE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("TRUE and TRUE"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("TRUE and FALSE"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("FALSE and TRUE"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("FALSE and FALSE"));
		EXPECT_FALSE(fe.Evaluate(di));

		EXPECT_TRUE(fe.Parse("TRUE OR TRUE"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("TRUE OR FALSE"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("FALSE OR TRUE"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("FALSE OR FALSE"));
		EXPECT_FALSE(fe.Evaluate(di));

	}

	TEST_F(FilterEngineTest, FileAttributes)
	{
		PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
		CDiffContext ctxt(paths, 0);
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

		FilterExpression fe;
		fe.SetDiffContext(&ctxt);

		EXPECT_TRUE(fe.Parse("Size <= 1000"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("allequal(Size)"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("allof(Size <= 1000)"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("anyof(Size <= 1000)"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("LeftDate < now()"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("LeftDate != RightDate"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("abs(-100) == 100"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("abs(LeftSize - RightSize) == (1100 - 1000)"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("LeftSize <= 100 * (1 + 9)"));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("LeftSize < 200 + 20 * 40"));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("LeftName = \"Alice.txt\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("RightName = \"Bob.txt\""));
		EXPECT_FALSE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("LeftName CONTAINS \"alice\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("RightName contains \".txt\""));
		EXPECT_TRUE(fe.Evaluate(di));
		EXPECT_TRUE(fe.Parse("RightName matches \"a.*t\""));
		EXPECT_TRUE(fe.Evaluate(di));
	}

	TEST_F(FilterEngineTest, ParseError)
	{
		PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
		CDiffContext ctxt(paths, 0);
		FilterExpression fe;
		fe.SetDiffContext(&ctxt);
		EXPECT_FALSE(fe.Parse("LeftDate $ a"));
		EXPECT_EQ(FilterExpression::ERROR_UNKNOWN_CHAR, fe.errorCode);
		EXPECT_FALSE(fe.Parse("LeftName = \"aaa"));
		EXPECT_EQ(FilterExpression::ERROR_UNTERMINATED_STRING, fe.errorCode);
		EXPECT_FALSE(fe.Parse("LeftSize = 100 RightSize < 100"));
		EXPECT_EQ(FilterExpression::ERROR_SYNTAX_ERROR, fe.errorCode);
		EXPECT_FALSE(fe.Parse("LeftDate = d\"2025-13-32 25:60:61\""));
		EXPECT_EQ(FilterExpression::ERROR_INVALID_LITERAL, fe.errorCode);
		EXPECT_FALSE(fe.Parse("aaa(1234)"));
		EXPECT_EQ(FilterExpression::ERROR_UNDEFINED_IDENTIFIER, fe.errorCode);
		EXPECT_FALSE(fe.Parse("aaa = 1234"));
		EXPECT_EQ(FilterExpression::ERROR_UNDEFINED_IDENTIFIER, fe.errorCode);
		EXPECT_FALSE(fe.Parse("abs()"));
		EXPECT_EQ(FilterExpression::ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
		EXPECT_FALSE(fe.Parse("abs(1, 2)"));
		EXPECT_EQ(FilterExpression::ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
		EXPECT_FALSE(fe.Parse("LeftName matches \"[[\""));
		EXPECT_EQ(FilterExpression::ERROR_INVALID_REGULAR_EXPRESSION, fe.errorCode);
		EXPECT_FALSE(fe.Parse("abs(1)) + abc(2)"));
		EXPECT_EQ(FilterExpression::ERROR_SYNTAX_ERROR, fe.errorCode);
		EXPECT_FALSE(fe.Parse(")LeftSize == 1"));
		EXPECT_EQ(FilterExpression::ERROR_SYNTAX_ERROR, fe.errorCode);
	}

}  // namespace

