#include "pch.h"
#include <gtest/gtest.h>
#include "FilterEngine/FilterExpression.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "PathContext.h"
#include "UniFile.h"
#include "Poco/DateTimeParser.h"
#include "Poco/Timezone.h"
#include "Environment.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "MergeApp.h"

#pragma execution_character_set("utf-8")

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
	EXPECT_TRUE(fe.Parse("123 == 124"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(123, 124) == 124"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125, 126) == array(125, 126)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125, 126) == array(126, 125)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(125, 126), 0) == 125"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(125, 126), 1) == 126"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2 < 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 < 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3, 2) < 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3, 3) < 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(2, 3) < array(3, 3)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3, 3) < array(3, 3)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2 <= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 <= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 <= 2"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2 <= array(3, 1)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 <= array(3, 3)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 <= array(2, 2)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 > 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 > 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3, 2) > 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3, 3) > 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 >= 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 >= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2 >= 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3, 2) >= 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3, 2) >= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(2, 2) >= 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 != 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 != 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125, 126) != 124"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125, 126) != array(126, 125)"));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("123.4 == 123.4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(".1 == 0.1"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1.23 == 123e-2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1.234 == 123.4e-2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123.4 == 101.4 + 11 * 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123 == 134 - 22 / 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123.4 == 121.4 + 8 % 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123.0 == 123"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123 == 123.0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123.4 == 123.4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123.0 == 124"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123 == 124.0"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("123.4 == 124.5"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(123.4, 124.5) == 124.5"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125.6, 126.7) == array(125.6, 126.7)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125.6, 126.7) == array(126.7, 125.6)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(125.6, 126.7), 0) == 125.6"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(125.6, 126.7), 1) == 126.7"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2.9 < 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2 < 2.1"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2.8 < 2.9"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 < 3.4"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3.4, 2.3) < 3.4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3.4, 3.4) < 3.4"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(2.3, 3.4) < array(3.4, 3.4)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3.4, 3.4) < array(3.4, 3.4)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2.9 <= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2 <= 2.1"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2.9 <= 3.0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.0 <= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 <= 3.0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 <= 3.4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.0 <= 2.9"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2.3 <= array(3.4, 1.2)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 <= array(3.4, 3.4)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 <= array(2.3, 2.3)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.1 > 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 > 2.9"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 > 2.3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 > 3.4"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3.4, 2.3) > 2.3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3.4, 3.4) > 3.4"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.1 >= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 >= 2.9"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 >= 2.3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.0 >= 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 >= 3.0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 >= 3.4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("2.3 >= 3.4"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3.4, 2.3) >= 2.3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(3.4, 2.3) >= 3.4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(2.3, 2.3) >= 3.4"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.0 != 3"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 != 3.0"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 != 3.4"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 != 2"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3 != 2.3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("3.4 != 2.3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125.6, 126.7) != 124.5"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(125.6, 126.7) != array(126.7, 125.6)"));
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
	EXPECT_TRUE(fe.Parse("at(array(d\"2025-05-27\", d\"2025-05-28\"), 0) == d\"2025-05-27\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(d\"2025-05-27\", d\"2025-05-28\"), 1) == d\"2025-05-28\""));
	EXPECT_TRUE(fe.Evaluate(di));

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
	EXPECT_TRUE(fe.Parse("substr(array(\"abcd\", \"efghij\"), 2, 2) = array(\"cd\", \"gh\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -1, 2) = \"d\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -2, 2) = \"cd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(\"abcd\", -5, 2) = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(array(\"abcd\"), -2, 2) = array(\"cd\")"));
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
	EXPECT_TRUE(fe.Parse("substr(array(\"abcd\", \"a\"), -1) = array(\"d\", \"a\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(\"abcd\") = 4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(array(\"abcd\", \"a\")) = array(4, 1)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(\"\") = 0"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(array(\"\")) = array(0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(array()) = array()"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"abcd\", \"ab\", \"cd\") = \"cdcd\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"\", \"ab\", \"cd\") = \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"abcd\", \"cd\", \"\") = \"ab\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(array(\"abcd\"), \"cd\", \"\") = array(\"ab\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	// Case-insensitive replace: search pattern matches regardless of case
	EXPECT_TRUE(fe.Parse("replace(\"ABCD\", \"ab\", \"cd\") = \"cdCD\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"TeSt\", \"test\", \"X\") = \"X\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(\"Hello World\", \"hello\", \"Hi\") = \"Hi World\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(array(\"TEST\", \"TeSt\"), \"test\", \"foo\") = array(\"foo\", \"foo\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(\"ab\", \"cd\"), 0) == \"ab\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(\"ab\", \"cd\"), 1) == \"cd\""));
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
	EXPECT_TRUE(fe.Parse("at(array(v\"2.16.49\", v\"2.16.50\"), 0) == v\"2.16.49\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(v\"2.16.49\", v\"2.16.50\"), 1) == v\"2.16.50\""));
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
	EXPECT_TRUE(fe.Parse("toDateStr(d\"2025-07-12 12:34:56\") == \"2025-07-12\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toDateStr(array(d\"2025-07-12 12:34:56\", d\"2025-12-31 23:59:59\")) == array(\"2025-07-12\", \"2025-12-31\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toDateStr(\"2025-07-12 12:34:56\") == \"2025-07-12\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toDateStr(array(\"2025-07-12 12:34:56\", \"2025-12-31 23:59:59\")) == array(\"2025-07-12\", \"2025-12-31\")"));
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
	EXPECT_TRUE(fe.Parse("at(array(false, true), 0) == false"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(false, true), 1) == true"));
	EXPECT_TRUE(fe.Evaluate(di));

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

	EXPECT_TRUE(fe.Parse("array() = array()"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(1) = array(1)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(1, 2, 3) = array(1, 2, 3)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20) = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array() = array(1)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array(1) = array()"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("array() = 1"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1 = array()"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("1 = array(1)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(array(1)), 0) = array(1)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(array(1)), -1) = array(1)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(array(1)), -2) = array(1)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("at(array(array(1)), 1) = array(1)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array())"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(123))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(123, 123, 123))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(123, 123, 124))"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(\"abc\", \"abc\", \"abc\"))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(\"abc\", \"abc\", \"abcd\"))"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(d\"2025-05-27\", d\"2025-05-27\", d\"2025-05-27\"))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(d\"2025-05-27\", d\"2025-05-27\", d\"2025-05-28\"))"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(array(1)), array(array(1)))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(array(1, 2), array(1, 2)))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(array(array(1, 2), array(1, 3)))"));
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
	EXPECT_TRUE(fe.Parse("abs(-123.4) == 123.4"));
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
	EXPECT_TRUE(fe.Parse("allequal(array(RightContent, RightContent))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent != LeftContent"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(Content)"));
	EXPECT_FALSE(fe.Evaluate(di));
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
	EXPECT_TRUE(fe.Parse("allequal(array(RightContent, RightContent))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("RightContent != LeftContent"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allequal(Content)"));
	EXPECT_FALSE(fe.Evaluate(di));
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
	EXPECT_FALSE(fe.Parse("at()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("at(array(1,2))"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("at(array(1,2),0,1)"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("strlen()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("strlen(\"abc\", \"def\")"));
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
	EXPECT_FALSE(fe.Parse("toDateStr()"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("toDateStr(now(), now())"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("isWithin(now(), now())"));
	EXPECT_EQ(FILTER_ERROR_INVALID_ARGUMENT_COUNT, fe.errorCode);
	EXPECT_FALSE(fe.Parse("inRange(now(), now())"));
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

TEST_P(FilterExpressionTest, IsWithinAndInRange)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	// Integer tests for isWithin
	EXPECT_TRUE(fe.Parse("isWithin(5, 1, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(1, 1, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(10, 1, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(0, 1, 10)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(11, 1, 10)"));
	EXPECT_FALSE(fe.Evaluate(di));

	// Integer tests for inRange
	EXPECT_TRUE(fe.Parse("inRange(5, 1, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(1, 1, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(10, 1, 10)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(0, 1, 10)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(11, 1, 10)"));
	EXPECT_FALSE(fe.Evaluate(di));

	// Double tests for isWithin
	EXPECT_TRUE(fe.Parse("isWithin(5.5, 1.0, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(1.0, 1.0, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(10.0, 1.0, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(0.9, 1.0, 10.0)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(10.1, 1.0, 10.0)"));
	EXPECT_FALSE(fe.Evaluate(di));

	// Double tests for inRange
	EXPECT_TRUE(fe.Parse("inRange(5.5, 1.0, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(1.0, 1.0, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(10.0, 1.0, 10.0)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(0.9, 1.0, 10.0)"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(10.1, 1.0, 10.0)"));
	EXPECT_FALSE(fe.Evaluate(di));

	// Mixed int/double tests for isWithin
	EXPECT_TRUE(fe.Parse("isWithin(5, 1.0, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(5.5, 1, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(5, 1, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(5.5, 1.0, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Mixed int/double tests for inRange
	EXPECT_TRUE(fe.Parse("inRange(5, 1.0, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(5.5, 1, 10)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(5, 1, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(5.5, 1, 10.0)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// String tests for isWithin
	EXPECT_TRUE(fe.Parse("isWithin(\"bbb\", \"aaa\", \"ccc\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(\"aaa\", \"aaa\", \"ccc\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(\"ccc\", \"aaa\", \"ccc\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(\"ddd\", \"aaa\", \"ccc\")"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(\"aaa\", \"bbb\", \"ccc\")"));
	EXPECT_FALSE(fe.Evaluate(di));

	// String tests for inRange
	EXPECT_TRUE(fe.Parse("inRange(\"bbb\", \"aaa\", \"ccc\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(\"aaa\", \"aaa\", \"ccc\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(\"ccc\", \"aaa\", \"ccc\")"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(\"ddd\", \"aaa\", \"ccc\")"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(\"aaa\", \"bbb\", \"ccc\")"));
	EXPECT_FALSE(fe.Evaluate(di));

	// DateTime tests for isWithin
	EXPECT_TRUE(fe.Parse("isWithin(d\"2025-05-15\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(d\"2025-05-10\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(d\"2025-05-20\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(d\"2025-05-09\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(d\"2025-05-21\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_FALSE(fe.Evaluate(di));

	// DateTime tests for inRange
	EXPECT_TRUE(fe.Parse("inRange(d\"2025-05-15\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(d\"2025-05-10\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(d\"2025-05-20\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(d\"2025-05-09\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(d\"2025-05-21\", d\"2025-05-10\", d\"2025-05-20\")"));
	EXPECT_FALSE(fe.Evaluate(di));

	// Array tests for isWithin
	EXPECT_TRUE(fe.Parse("isWithin(array(3, 5, 7), 1, 10) == array(true, true, true)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(array(0, 5, 11), 1, 10) == array(false, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("isWithin(array(1, 10), 1, 10) == array(true, true)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Array tests for inRange
	EXPECT_TRUE(fe.Parse("inRange(array(3, 5, 7), 1, 10) == array(true, true, true)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(array(0, 5, 11), 1, 10) == array(false, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(array(1, 10), 1, 10) == array(true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Array tests with strings
	EXPECT_TRUE(fe.Parse("isWithin(array(\"aaa\", \"bbb\", \"ddd\"), \"aaa\", \"ccc\") == array(true, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(array(\"aaa\", \"bbb\", \"ccc\"), \"aaa\", \"ccc\") == array(true, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Array tests with doubles
	EXPECT_TRUE(fe.Parse("isWithin(array(1.5, 5.5, 10.5), 1.0, 10.0) == array(true, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("inRange(array(1.0, 5.5, 10.0), 1.0, 10.0) == array(true, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Tests with anyof
	EXPECT_TRUE(fe.Parse("anyof(isWithin(array(0, 5, 11), 1, 10))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("anyof(inRange(array(0, 5, 11), 1, 10))"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Tests with allof
	EXPECT_TRUE(fe.Parse("allof(isWithin(array(3, 5, 7), 1, 10))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allof(isWithin(array(0, 5, 11), 1, 10))"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allof(inRange(array(3, 5, 7), 1, 10))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allof(inRange(array(0, 5, 11), 1, 10))"));
	EXPECT_FALSE(fe.Evaluate(di));
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

TEST_P(FilterExpressionTest, ConditionalFunctions)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	di.diffFileInfo[0].filename = L"Alice.txt";
	di.diffFileInfo[0].size = 1000;
	di.diffFileInfo[1].filename = L"Bob.txt";
	di.diffFileInfo[1].size = 2000;
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	// if function tests
	EXPECT_TRUE(fe.Parse("if(true, \"yes\", \"no\") == \"yes\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("if(false, \"yes\", \"no\") == \"no\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("if(LeftSize > 500, \"large\", \"small\") == \"large\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("if(LeftSize < 500, \"large\", \"small\") == \"small\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// ifEach function tests
	EXPECT_TRUE(fe.Parse("ifEach(array(true, false, true), \"yes\", \"no\") == array(\"yes\", \"no\", \"yes\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("ifEach(Size > 1500, \"large\", \"small\") == array(\"small\", \"large\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("ifEach(array(true, false), array(\"a\", \"b\"), array(\"c\", \"d\")) == array(\"a\", \"d\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("ifEach(true, \"yes\", \"no\") == \"yes\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// choose function tests
	EXPECT_TRUE(fe.Parse("choose(0, \"zero\", \"one\", \"two\") == \"zero\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("choose(1, \"zero\", \"one\", \"two\") == \"one\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("choose(2, \"zero\", \"one\", \"two\") == \"two\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("choose(-1, \"zero\", \"one\", \"two\") == \"zero\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("choose(5, \"zero\", \"one\", \"two\") == \"two\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// chooseEach function tests
	EXPECT_TRUE(fe.Parse("chooseEach(array(0, 1, 2), \"a\", \"b\", \"c\") == array(\"a\", \"b\", \"c\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("chooseEach(array(0, 0, 1), \"x\", \"y\", \"z\") == array(\"x\", \"x\", \"y\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("chooseEach(array(-1, 5), \"a\", \"b\") == array(\"a\", \"b\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("chooseEach(0, \"a\", \"b\") == \"a\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// andEach function tests
	EXPECT_TRUE(fe.Parse("andEach(true, true) == true"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("andEach(true, false) == false"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("andEach(false, true) == false"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("andEach(false, false) == false"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("andEach(array(true, false, true), true) == array(true, false, true)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("andEach(true, array(true, false, true)) == array(true, false, true)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("andEach(array(true, false, true), array(true, true, false)) == array(true, false, false)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("andEach(Size > 500, Size < 1500) == array(true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// orEach function tests
	EXPECT_TRUE(fe.Parse("orEach(true, true) == true"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("orEach(true, false) == true"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("orEach(false, true) == true"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("orEach(false, false) == false"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("orEach(array(true, false, true), false) == array(true, false, true)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("orEach(false, array(true, false, true)) == array(true, false, true)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("orEach(array(true, false, false), array(false, true, false)) == array(true, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("orEach(Size < 500, Size > 1500) == array(false, true)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// notEach function tests
	EXPECT_TRUE(fe.Parse("notEach(true) == false"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("notEach(false) == true"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("notEach(array(true, false, true)) == array(false, true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("notEach(Size > 1500) == array(true, false)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Combined tests
	EXPECT_TRUE(fe.Parse("anyof(andEach(Size > 500, Size < 1500))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("allof(orEach(Size < 500, Size > 1500))"));
	EXPECT_FALSE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("anyof(notEach(Size > 2500))"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("ifEach(andEach(Size > 500, Size < 1500), \"in range\", \"out of range\") == array(\"in range\", \"out of range\")"));
	EXPECT_TRUE(fe.Evaluate(di));
}

TEST_P(FilterExpressionTest, StringTransformFunctions)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	di.diffFileInfo[0].filename = L"Test.txt";
	di.diffFileInfo[0].size = 1000;
	di.diffFileInfo[1].filename = L"Test.txt";
	di.diffFileInfo[1].size = 1000;
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	// tolower function tests
	EXPECT_TRUE(fe.Parse("tolower(\"HELLO\") == \"hello\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("tolower(\"Hello World\") == \"hello world\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("tolower(\"ABC123\") == \"abc123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("tolower(array(\"HELLO\", \"WORLD\")) == array(\"hello\", \"world\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toupper function tests
	EXPECT_TRUE(fe.Parse("toupper(\"hello\") == \"HELLO\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toupper(\"Hello World\") == \"HELLO WORLD\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toupper(\"abc123\") == \"ABC123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toupper(array(\"hello\", \"world\")) == array(\"HELLO\", \"WORLD\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Japanese character conversion tests (tohalfwidth/tofullwidth)
	EXPECT_TRUE(fe.Parse(u8"tohalfwidth(\"ＡＢＣＤＥ\") == \"ABCDE\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tofullwidth(\"ABCDE\") == \"ＡＢＣＤＥ\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tohalfwidth(\"１２３４５\") == \"12345\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tofullwidth(\"12345\") == \"１２３４５\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tohalfwidth(array(\"ＡＢＣＤＥ\", \"１２３４５\")) == array(\"ABCDE\", \"12345\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tofullwidth(array(\"ABCDE\", \"12345\")) == array(\"ＡＢＣＤＥ\", \"１２３４５\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Japanese hiragana/katakana conversion tests
	EXPECT_TRUE(fe.Parse(u8"tokatakana(\"あいうえお\") == \"アイウエオ\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tohiragana(\"アイウエオ\") == \"あいうえお\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tokatakana(\"かきくけこ\") == \"カキクケコ\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tohiragana(\"カキクケコ\") == \"かきくけこ\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tokatakana(array(\"あいうえお\", \"かきくけこ\")) == array(\"アイウエオ\", \"カキクケコ\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tohiragana(array(\"アイウエオ\", \"カキクケコ\")) == array(\"あいうえお\", \"かきくけこ\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Chinese character conversion tests (simplified/traditional)
	EXPECT_TRUE(fe.Parse(u8"tosimplifiedchinese(\"繁體字\") == \"繁体字\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"totraditionalchinese(\"简体字\") == \"簡體字\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tosimplifiedchinese(\"電腦\") == \"电脑\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"totraditionalchinese(\"计算机\") == \"計算機\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tosimplifiedchinese(array(\"繁體字\", \"電腦\")) == array(\"繁体字\", \"电脑\")"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"totraditionalchinese(array(\"简体字\", \"计算机\")) == array(\"簡體字\", \"計算機\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Combined usage tests
	EXPECT_TRUE(fe.Parse(u8"toupper(tolower(\"HeLLo\")) == \"HELLO\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tohalfwidth(tofullwidth(\"ABC\")) == \"ABC\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tokatakana(tohiragana(\"アイウエオ\")) == \"アイウエオ\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"totraditionalchinese(tosimplifiedchinese(\"繁體字\")) == \"繁體字\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Edge case: empty string
	EXPECT_TRUE(fe.Parse("tolower(\"\") == \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toupper(\"\") == \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("tohalfwidth(\"\") == \"\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("tofullwidth(\"\") == \"\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Mixed content tests
	EXPECT_TRUE(fe.Parse("tolower(\"Test123!@#\") == \"test123!@#\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toupper(\"Test123!@#\") == \"TEST123!@#\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tohalfwidth(\"Ａ１あア\") == \"A1あｱ\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"tokatakana(\"あア\") == \"アア\""));
	EXPECT_TRUE(fe.Evaluate(di));
}

TEST_P(FilterExpressionTest, StringFunctionsWithNonStringArguments)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	di.diffFileInfo[0].filename = L"Test.txt";
	di.diffFileInfo[0].size = 1000;
	di.diffFileInfo[1].filename = L"Test.txt";
	di.diffFileInfo[1].size = 1000;
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	// strlen with non-string arguments
	EXPECT_TRUE(fe.Parse("strlen(123) == 3"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(12345) == 5"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(123.45) == 10")); // "123.450000"
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(true) == 4"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(false) == 5"));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("strlen(array(123, 456)) == array(3, 3)"));
	EXPECT_TRUE(fe.Evaluate(di));

	// substr with non-string first argument
	EXPECT_TRUE(fe.Parse("substr(12345, 1, 3) == \"234\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(123.45, 0, 3) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(true, 0, 2) == \"tr\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("substr(array(123, 456), 1, 2) == array(\"23\", \"56\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// replace with non-string first argument
	EXPECT_TRUE(fe.Parse("replace(12345, \"23\", \"XX\") == \"1XX45\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(123.45, \".\", \",\") == \"123,450000\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(true, \"t\", \"T\") == \"True\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("replace(array(123, 456), \"3\", \"X\") == array(\"12X\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// regexReplace with non-string first argument
	EXPECT_TRUE(fe.Parse("regexReplace(12345, \"[24]\", \"X\") == \"1X3X5\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("regexReplace(123.45, \"\\d\", \"X\") == \"XXX.XXXXXX\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("regexReplace(true, \"[a-z]\", \"X\") == \"XXXX\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("regexReplace(array(123, 456), \"[13]\", \"X\") == array(\"X2X\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toUpper with non-string argument
	EXPECT_TRUE(fe.Parse("toUpper(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toUpper(123.45) == \"123.450000\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toUpper(true) == \"TRUE\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toUpper(false) == \"FALSE\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toUpper(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toLower with non-string argument
	EXPECT_TRUE(fe.Parse("toLower(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toLower(123.45) == \"123.450000\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toLower(true) == \"true\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toLower(false) == \"false\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toLower(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toHalfWidth with non-string argument
	EXPECT_TRUE(fe.Parse("toHalfWidth(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toHalfWidth(true) == \"true\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toHalfWidth(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toFullWidth with non-string argument
	EXPECT_TRUE(fe.Parse("toFullWidth(123) == \"１２３\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse(u8"toFullWidth(array(123, 456)) == array(\"１２３\", \"４５６\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toHiragana with non-string argument
	EXPECT_TRUE(fe.Parse("toHiragana(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toHiragana(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toKatakana with non-string argument
	EXPECT_TRUE(fe.Parse("toKatakana(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toKatakana(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toSimplifiedChinese with non-string argument
	EXPECT_TRUE(fe.Parse("toSimplifiedChinese(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toSimplifiedChinese(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// toTraditionalChinese with non-string argument
	EXPECT_TRUE(fe.Parse("toTraditionalChinese(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("toTraditionalChinese(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// normalizeUnicode with non-string argument
	EXPECT_TRUE(fe.Parse("normalizeUnicode(123) == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("normalizeUnicode(true) == \"true\""));
	EXPECT_TRUE(fe.Evaluate(di));
	EXPECT_TRUE(fe.Parse("normalizeUnicode(array(123, 456)) == array(\"123\", \"456\")"));
	EXPECT_TRUE(fe.Evaluate(di));
}

TEST_P(FilterExpressionTest, ReplaceWithList)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	di.diffFileInfo[0].filename = L"Test.txt";
	di.diffFileInfo[1].filename = L"Test.txt";
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	// Create temporary directory path
	const String tempDir = env::GetTemporaryPath();
	const String replaceListPath = paths::ConcatPath(tempDir, L"test_replacelist.txt");
	const String regexReplaceListPath = paths::ConcatPath(tempDir, L"test_regex_replacelist.txt");

	// Create replace list file
	{
		UniStdioFile file;
		EXPECT_TRUE(file.OpenCreateUtf8(replaceListPath));
		file.WriteBom();
		file.WriteString(L"# Comment line\n");
		file.WriteString(L"apple\t\u308a\u3093\u3054\n"); // りんご
		file.WriteString(L"orange\t\u30aa\u30ec\u30f3\u30b8\n"); // オレンジ
		file.WriteString(L"banana\t\u30d0\u30ca\u30ca\n"); // バナナ
		file.WriteString(L"# Another comment\n");
		file.WriteString(L"grape\t\u3076\u3069\u3046\n"); // ぶどう
		file.Close();
	}

	// Create regex replace list file
	{
		UniStdioFile file;
		EXPECT_TRUE(file.OpenCreateUtf8(regexReplaceListPath));
		file.WriteBom();
		file.WriteString(L"# Regex patterns\n");
		file.WriteString(L"\\d+\t----\n");
		file.WriteString(L"[a-z]+\t****\n");
		file.WriteString(L"\\s+\t|\n");
		file.Close();
	}

	GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);

	// Test replaceWithList function
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"I like apple and orange\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"I like りんご and オレンジ\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"banana is yellow\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"バナナ is yellow\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("replaceWithList(\"grape juice\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"ぶどう juice\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Multiple replacements in one string
	EXPECT_TRUE(fe.Parse("replaceWithList(\"apple orange banana\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"りんご オレンジ バナナ\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// No match
	EXPECT_TRUE(fe.Parse("replaceWithList(\"watermelon\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"watermelon\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Empty string
	EXPECT_TRUE(fe.Parse("replaceWithList(\"\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Array of strings
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(array(\"apple\", \"banana\"), \"" + ucr::toUTF8(replaceListPath) + u8"\") == array(\"りんご\", \"バナナ\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Case-insensitive: uppercase input matches lowercase pattern
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"APPLE\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"りんご\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Case-insensitive: mixed case input
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"Apple is good\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"りんご is good\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Case-insensitive: multiple mixed-case words
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"ORANGE and Banana\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"オレンジ and バナナ\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Case-insensitive: array with mixed case
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(array(\"APPLE\", \"GRAPE\"), \"" + ucr::toUTF8(replaceListPath) + u8"\") == array(\"りんご\", \"ぶどう\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Test regexReplaceWithList function
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"abc123def456\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"****----****----\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"Hello World 2025\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"****|****|----\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"test 123\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"****|----\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Multiple spaces
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"a   b\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"****|****\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// No match
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"!@#$%\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"!@#$%\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Empty string
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Array of strings
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(array(\"abc123\", \"789XYZ789\"), \"" + ucr::toUTF8(regexReplaceListPath) + "\") == array(\"****----\", \"----****----\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Test with non-existent file (should return original string)
	EXPECT_TRUE(fe.Parse("replaceWithList(\"test\", \"nonexistent.txt\") == \"test\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"test\", \"nonexistent.txt\") == \"test\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Cleanup temporary files
	DeleteFile(replaceListPath.c_str());
	DeleteFile(regexReplaceListPath.c_str());
}

TEST_P(FilterExpressionTest, ReplaceWithListAdvanced)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	di.diffFileInfo[0].filename = L"Test.txt";
	di.diffFileInfo[1].filename = L"Test.txt";
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	const String tempDir = env::GetTemporaryPath();
	const String replaceListPath = paths::ConcatPath(tempDir, L"test_replacelist2.txt");
	const String regexReplaceListPath = paths::ConcatPath(tempDir, L"test_regex_replacelist2.txt");

	// Create replace list with edge cases
	{
		UniStdioFile file;
		EXPECT_TRUE(file.OpenCreateUtf8(replaceListPath));
		file.WriteBom();
		file.WriteString(L"# Test special characters\n");
		file.WriteString(L"C++\tC\u30d7\u30e9\u30d7\u30e9\n"); // Cプラプラ
		file.WriteString(L"a\tb\tc\td\n"); // Extra tabs should be ignored
		file.WriteString(L"\n"); // Empty line should be skipped
		file.WriteString(L"test\t\n"); // Replace with empty string
		file.WriteString(L"# Comment at end\n");
		file.Close();
	}

	// Create regex replace list with capturing groups
	{
		UniStdioFile file;
		EXPECT_TRUE(file.OpenCreateUtf8(regexReplaceListPath));
		file.WriteBom();
		file.WriteString(L"# Regex with captures\n");
		file.WriteString(L"(\\d{4})-(\\d{2})-(\\d{2})\t$3/$2/$1\n");
		file.WriteString(L"\\b(\\w+)\\s+\\1\\b\t$1\n"); // Remove duplicate words
		file.Close();
	}

	GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);

	// Test special characters
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"I love C++\", \"" + ucr::toUTF8(replaceListPath) + u8"\") == \"I love Cプラプラ\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Test extra tabs (should only use first two fields)
	EXPECT_TRUE(fe.Parse("replaceWithList(\"a\", \"" + ucr::toUTF8(replaceListPath) + "\") == \"b\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Test replace with empty string
	EXPECT_TRUE(fe.Parse("replaceWithList(\"test123\", \"" + ucr::toUTF8(replaceListPath) + "\") == \"123\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Test regex with date format
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"2025-05-16\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"16/05/2025\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Test regex for removing duplicate words
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"the the cat\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"the cat\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse("regexReplaceWithList(\"hello hello world world\", \"" + ucr::toUTF8(regexReplaceListPath) + "\") == \"hello world\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Combined test with array
	EXPECT_TRUE(fe.Parse("regexReplaceWithList(array(\"2025-05-16\", \"2025-12-31\"), \"" + ucr::toUTF8(regexReplaceListPath) + "\") == array(\"16/05/2025\", \"31/12/2025\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Cleanup
	DeleteFile(replaceListPath.c_str());
	DeleteFile(regexReplaceListPath.c_str());
}

TEST_P(FilterExpressionTest, ReplaceWithListEncoding)
{
	PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
	CDiffContext ctxt(paths, 0);
	DIFFITEM di;
	di.diffFileInfo[0].filename = L"Test.txt";
	di.diffFileInfo[1].filename = L"Test.txt";
	di.diffcode.setSideFlag(0);
	di.diffcode.setSideFlag(1);

	FilterExpression fe;
	fe.SetDiffContext(&ctxt);
	fe.optimize = GetParam().optimize;

	const String tempDir = env::GetTemporaryPath();
	const String replaceListPath = paths::ConcatPath(tempDir, L"test_replacelist_utf8.txt");

	// Create UTF-8 encoded file with Japanese characters
	{
		UniStdioFile file;
		EXPECT_TRUE(file.OpenCreateUtf8(replaceListPath));
		file.WriteBom();
		file.WriteString(L"# \u65e5\u672c\u8a9e\u306e\u30c6\u30b9\u30c8\n"); // 日本語のテスト
		file.WriteString(L"\u3053\u3093\u306b\u3061\u306f\thello\n"); // こんにちは
		file.WriteString(L"\u3055\u3088\u3046\u306a\u3089\tgoodbye\n"); // さようなら
		file.WriteString(L"\u3042\u308a\u304c\u3068\u3046\tthank you\n"); // ありがとう
		file.WriteString(L"\u72ac\tdog\n"); // 犬
		file.WriteString(L"\u732b\tcat\n"); // 猫
		file.Close();
	}

	GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);

	// Test Japanese to English
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"こんにちは\", \"" + ucr::toUTF8(replaceListPath) + "\") == \"hello\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"さようなら\", \"" + ucr::toUTF8(replaceListPath) + "\") == \"goodbye\""));
	EXPECT_TRUE(fe.Evaluate(di));

	EXPECT_TRUE(fe.Parse(u8"replaceWithList(\"私の犬と猫\", \"" + ucr::toUTF8(replaceListPath) + "\") == \"私のdogとcat\""));
	EXPECT_TRUE(fe.Evaluate(di));

	// Array test
	EXPECT_TRUE(fe.Parse(u8"replaceWithList(array(\"こんにちは\", \"ありがとう\"), \"" + ucr::toUTF8(replaceListPath) + "\") == array(\"hello\", \"thank you\")"));
	EXPECT_TRUE(fe.Evaluate(di));

	// Cleanup
	DeleteFile(replaceListPath.c_str());
}

INSTANTIATE_TEST_SUITE_P(
	OptimizationCases,
	FilterExpressionTest,
	::testing::Values(
		FilterTestParam{ true },
		FilterTestParam{ false }
	)
);




