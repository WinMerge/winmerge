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
	}

	TEST_F(FilterEngineTest, ParseError)
	{
		PathContext paths(L"D:\\dev\\winmerge\\src", L"D:\\dev\\winmerge\\src");
		CDiffContext ctxt(paths, 0);
		FilterContext fc(&ctxt);
		EXPECT_FALSE(FilterEngine::Parse(L"LeftDate $ a", fc));
		EXPECT_FALSE(FilterEngine::Parse(L"LeftName = \"aaa", fc));
		EXPECT_FALSE(FilterEngine::Parse(L"LeftSize = 100 RightSize < 100", fc));
	}

}  // namespace

