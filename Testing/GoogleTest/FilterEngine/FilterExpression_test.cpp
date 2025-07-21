#include "pch.h"
#include <gtest/gtest.h>
#include "FilterEngine/FilterExpression.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "PathContext.h"
#include "Poco/DateTimeParser.h"
#include "Poco/Timezone.h"
#include "Environment.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "MergeApp.h"

struct FilterTestParam { bool optimize; };

// The fixture for testing paths functions.
class FilterExpressionTest : public ::testing::TestWithParam<FilterTestParam> {};

TEST_P(FilterExpressionTest, Literals)
{
	// Test case for evaluating filter expressions with literal values.
	// This function sets up a test context, initializes test data, and verifies
	// the behavior of the FilterExpression class.

	// Initialize path context and diff context for the test.
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);

	// Set up DIFFITEM object with file information and timestamps.
	DIFFITEM di;
	int tzd;
	di.diffFileInfo[0].filename = L"Alice.txt";
	di.diffFileInfo[0].size = 1000;
	Poco::DateTime dt0 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M", "2025-05-16 15:34:56", tzd);
	dt0 -= Poco::Timezone::tzd() * 1000;
	di.diffFileInfo[0].mtime = dt0.timestamp();
	di.diffFileInfo[1].filename = L"Alice.txt";
	di.diffFileInfo[1].size = 1100;
	Poco::DateTime dt1 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M:%S", "2025-05-16 15:34:57", tzd);
	dt1 -= Poco::Timezone::tzd() * 1000;
	di.diffFileInfo[1].mtime = dt1.timestamp();

	// Set side flags for the diff code.
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	// Initialize FilterExpression and set the diff context.
	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	// Verify that the filter expression correctly parses and evaluates literals.
	EXPECT_TRUE(fe.Parse("123 == 123"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123 == 101 + 11 * 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123 == 134 - 22 / 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123 == 121 + 8 % 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123 == 123"));
	EXPECT_TRUE(fe.Evaluate(di));
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
	EXPECT_TRUE(fe.Parse("d\"2025-05-27\" - d\"2025-05-25\" == 2days"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("d\"2025-05-27\" == d\"2025/05/27\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("d\"2025-05-27\" == d\"2025.05.27\""));
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
	EXPECT_TRUE(fe.Parse("\"abcde\" == \"ab\" + \"cde\""));
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
	EXPECT_TRUE(fe.Parse("\"abc\" = \"abd\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" contains \"ab\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" contains \"bc\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" contains \"abc\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" recontains \"a.c\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" recontains \"[a-c]\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" like \"a?c\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" like \"a*c\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" matches \"a.*c\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("\"abc\" matches \"^a.*\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 0, 4) = \"abcd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 0, 5) = \"abcd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 0, 0) = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 0, 3) = \"abc\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 2, 2) = \"cd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -1, 2) = \"d\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -2, 2) = \"cd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -5, 2) = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 0, -3) = \"a\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 1, -2) = \"b\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -3, -2) = \"b\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -3, -3) = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -3, -4) = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 0) = \"abcd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 2) = \"cd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", 4) = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -1) = \"d\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("length(\"abcd\") = 4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("length(\"\") = 0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"abcd\", \"ab\", \"cd\") = \"cdcd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"\", \"ab\", \"cd\") = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"abcd\", \"cd\", \"\") = \"ab\""));
	EXPECT_TRUE(fe.Evaluate(di));

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

	EXPECT_TRUE(fe.Parse("1weeks == 10days - 3days"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1week == 6days + 1days"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1w == 7d"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1days == 24hours"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.25days == 6hours"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1day == 24hour"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1d == 24hr"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1d == 24h"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1hours == 60minutes"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.5hours == 30minutes"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1hour == 60minute"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1h == 60min"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.5h == 30min"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1h == 60m"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1minutes == 60seconds"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.1minutes == 6seconds"));
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
	EXPECT_TRUE(fe.Parse("1kb == 1024"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.5KB == 512"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1MB == 1024KB"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.5MB == 512KB"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1GB == 1024MB"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.5GB == 512MB"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1TB == 1024GB"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("0.5TB == 512GB"));
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
	EXPECT_TRUE(fe.Parse("d\"2025-05-27\" == d\"2025/05/27 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("d\"27-may-2025\" == d\"2025.05.27 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("d\"2025-05-27T00:00:00+00:00\" == d\"2025-05-27T00:00:00Z\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("d\"2025-05-27T12:34:56+00:00\" == d\"2025-05-27T21:34:56+09:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("d\"20250527T123456+0000\" == d\"20250527T213456+0900\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("d\"2025-05-27T01:02:03+09:00\" != d\"2025-05-27T01:02:03z\""));
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

	EXPECT_TRUE(fe.Parse("startOfWeek(d\"2025-07-12 12:34:56\") == d\"2025-07-06 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("startOfWeek(\"2025-07-12 12:34:56\") == d\"2025-07-06 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("startOfMonth(d\"2025-07-12 12:34:56\") == d\"2025-07-01 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("startOfMonth(\"2025-07-12 12:34:56\") == d\"2025-07-01 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("startOfYear(d\"2025-07-12 12:34:56\") == d\"2025-01-01 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("startOfYear(\"2025-07-12 12:34:56\") == d\"2025-01-01 00:00:00\""));
	EXPECT_TRUE(fe.Evaluate(di));

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

TEST_P(FilterExpressionTest, FileAttributes)
{
	PathContext paths(L"C:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src", L"E:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	int tzd;
	di.diffFileInfo[0].path = L"abc";
	di.diffFileInfo[0].filename = L"Alice.txt";
	di.diffFileInfo[0].size = 1000;
	di.diffFileInfo[0].flags.attributes = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE;
	Poco::DateTime dt0 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M:%S", "2025-05-16 15:34:56", tzd);
	dt0.makeUTC(Poco::Timezone::tzd());
	di.diffFileInfo[0].mtime = dt0.timestamp();
	di.diffFileInfo[0].ctime = dt0.timestamp();
	di.diffFileInfo[0].encoding.SetCodepage(65001);
	di.diffFileInfo[0].version.SetFileVersion(0x00020010, 0x00300002);
	di.diffFileInfo[1].path = L"abc";
	di.diffFileInfo[2].path = L"abc";
	di.diffFileInfo[2].filename = L"Alice.txt";
	di.diffFileInfo[2].size = 1100;
	di.diffFileInfo[2].flags.attributes = FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE;
	Poco::DateTime dt1 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M:%S", "2025-05-16 15:34:57", tzd);
	dt1.makeUTC(Poco::Timezone::tzd());
	di.diffFileInfo[2].mtime = dt1.timestamp();
	di.diffFileInfo[2].ctime = dt1.timestamp();
	di.diffFileInfo[2].encoding.SetCodepage(65001);
	di.diffFileInfo[2].version.SetFileVersion(0x00020010, 0x00300002);
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(2);
	di.nsdiffs = 3;
	di.nidiffs = 2;

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

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
	EXPECT_TRUE(fe.Parse("LeftDate != MiddleDate"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftCreationTime != RightCreationTime"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftCreationTime != MiddleCreationTime"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("abs(-100) == 100"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("abs(LeftSize - RightSize) == (1100 - 1000)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftSize <= 100 * (1 + 9)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftSize < 200 + 20 * 40"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleSize != 0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftName = \"Alice.txt\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightName = \"Bob.txt\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftName CONTAINS \"alice\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightName contains \".txt\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightName not contains \".txt\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightName matches \"a.*t\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightName not matches \"a.*t\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightName like \"a?ice.*t\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightName not like \"a?ice.*t\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleName != \"Alice.txt\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftExtension = \"txt\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleExtension != \"txt\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftFolder = \"abc\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleFolder = \"abc\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftDateStr = \"2025-05-16\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleDateStr != \"2025-05-16\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("DiffCode != 0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("Differences = 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("IgnoredDiffs = 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftFullPath = \"C:\\dev\\winmerge\\src\\abc\\Alice.txt\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleFullPath != \"D:\\dev\\winmerge\\src\\abc\\Alice.txt\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightAttributes > 0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleAttributes != 0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftAttrStr = \"RHSA\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleAttrStr != \"RHSA\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftCodepage = 65001"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleCodepage != 65001"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftEncoding = \"UTF-8\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleEncoding != \"UTF-8\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftVersion = v\"2.16.48.2\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleVersion != v\"2.16.48.2\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftExists"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightExists"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("MiddleExists"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("Not MiddleExists"));
	EXPECT_TRUE(fe.Evaluate(di));
}

TEST_P(FilterExpressionTest, Content1)
{
	const String dir = paths::ConcatPath(env::GetProgPath(), L"..\\TestData");
	PathContext paths(dir, dir);
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	di.diffFileInfo[1].filename = L"LeftAndRight.WinMerge";
	di.diffFileInfo[1].path = L"";
	di.diffcode.setSideFlag(1);
	
	GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);

	EXPECT_TRUE(fe.Parse("RightContent = RightContent"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent != LeftContent"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent contains \"UTF-8\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftContent contains \"UTF-8\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent recontains \"xml.*UTF-8\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent not recontains \"xml.*UTF-8\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("LeftContent recontains \"xml.*UTF-8\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("linecount(RightContent) = 7"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, 0, 1) contains \"<?xml version='1.0' encoding='UTF-8'?>\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, 1, 2) contains \"paths\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, -1, 1) contains \"</project>\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, -1, -1) contains \"</project>\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, -1, -2) contains \"</project>\""));
	EXPECT_FALSE(fe.Evaluate(di));
}

TEST_P(FilterExpressionTest, ContentEmpty)
{
	const String dir = paths::ConcatPath(env::GetProgPath(), L"..\\..\\Data\\Compare");
	PathContext paths(paths::ConcatPath(dir, _T("dir1")), paths::ConcatPath(dir, _T("dir2")));
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	di.diffFileInfo[1].filename = L"file123_0.txt";
	di.diffFileInfo[1].path = L"";
	di.diffcode.setSideFlag(1);
	
	GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);

	EXPECT_TRUE(fe.Parse("RightContent = RightContent"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent != LeftContent"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent contains \"UTF-8\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent recontains \"xml.*UTF-8\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("linecount(RightContent) = 0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, 0, 1) contains \"<?xml version='1.0' encoding='UTF-8'?>\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, 1, 2) contains \"paths\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, -1, 1) contains \"</project>\""));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("sublines(RightContent, -1, -1) contains \"</project>\""));
	EXPECT_FALSE(fe.Evaluate(di));
}

TEST_P(FilterExpressionTest, ParseError)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	EXPECT_FALSE(fe.Parse("LeftDate $ a"));
	EXPECT_EQ(FILTER_ERROR_UNKNOWN_CHAR, fe.errorCode);
	EXPECT_FALSE(fe.Parse("LeftName = \"aaa"));
	EXPECT_EQ(FILTER_ERROR_UNTERMINATED_STRING, fe.errorCode);
	EXPECT_FALSE(fe.Parse("LeftSize = 100 RightSize < 100"));
	EXPECT_EQ(FILTER_ERROR_SYNTAX_ERROR, fe.errorCode);
	EXPECT_FALSE(fe.Parse("LeftDate = d\"2025-13-32 25:60:61\""));
	EXPECT_EQ(FILTER_ERROR_INVALID_LITERAL, fe.errorCode);
	EXPECT_FALSE(fe.Parse("aaa(1234)"));
	EXPECT_EQ(FILTER_ERROR_UNDEFINED_IDENTIFIER, fe.errorCode);
	EXPECT_FALSE(fe.Parse("aaa = 1234"));
	EXPECT_EQ(FILTER_ERROR_UNDEFINED_IDENTIFIER, fe.errorCode);
	EXPECT_FALSE(fe.Parse("abs()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("abs(1, 2)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("anyof()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("anyof(true, true)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("allof()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("allof(true, true)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("allequal()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("length()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("length(\"abc\", \"def\")"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("substr()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("substr(\"abc\")"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("substr(\"abc\", 0, 1, 2)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("linecount()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("linecount(\"abc\", \"def\")"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("sublines()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("sublines(\"abc\")"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("sublines(\"abc\", 0, 1, 2)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("replace()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("replace(\"abc\", \"def\", \"ghi\", 2)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("today(1)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("now(1)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("startOfWeek()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("startOfWeek(now(), now())"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("startOfMonth()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("startOfMonth(now(), now())"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("startOfYear()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("startOfYear(now(), now())"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	if (fe.optimize) {
		EXPECT_FALSE(fe.Parse("LeftName matches \"[[\""));
		EXPECT_EQ(FILTER_ERROR_INVALID_REGULAR_EXPRESSION, fe.errorCode);
	}
	EXPECT_FALSE(fe.Parse("abs(1)) + abc(2)"));
	EXPECT_EQ(FILTER_ERROR_SYNTAX_ERROR, fe.errorCode);
	EXPECT_FALSE(fe.Parse(")LeftSize == 1"));
	EXPECT_EQ(FILTER_ERROR_SYNTAX_ERROR, fe.errorCode);
}

TEST_P(FilterExpressionTest, Test1)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	int tzd = 0;
	di.diffFileInfo[0].filename = L"Alice.txt";
	di.diffFileInfo[0].size = 1000;
	Poco::DateTime dt0 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M", "2025-05-16 15:34:56", tzd);
	dt0.makeUTC(Poco::Timezone::tzd());
	di.diffFileInfo[0].mtime = dt0.timestamp();
	di.diffFileInfo[1].filename = L"Alice.txt";
	di.diffFileInfo[1].size = 1100;
	Poco::DateTime dt1 = Poco::DateTimeParser::parse("%Y-%m-%d %H:%M:%S", "2025-05-16 15:34:57", tzd);
	dt1.makeUTC(Poco::Timezone::tzd());
	di.diffFileInfo[1].mtime = dt1.timestamp();
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	EXPECT_TRUE(fe.Parse("1 or true"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1 or false"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("true or 1"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("true or 1"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1 or 1"));
	EXPECT_FALSE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("1 and true"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1 and false"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("true and 1"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("true and 1"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1 and 1"));
	EXPECT_FALSE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("not 1"));
	EXPECT_FALSE(fe.Evaluate(di));
}

INSTANTIATE_TEST_SUITE_P(
	OptimizationCases,
	FilterExpressionTest,
	::testing::Values(
		FilterTestParam{ true },
		FilterTestParam{ false }
	)
);

