#include "pch.h"
#include <gtest/gtest.h>
#include "FilterEngine/FilterEngine.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "PathContext.h"

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
		FilterEngine::ParseResult result;
		DIFFITEM di;
		di.diffFileInfo[0].filename = L"Alice.txt";
		di.diffFileInfo[0].size = 1000;
		di.diffFileInfo[1].filename = L"Alice.txt";
		di.diffFileInfo[1].size = 1000;

		result = FilterEngine::Parse(L"LeftSize <= 100 * (1 + 9)", ctxt);
		EXPECT_TRUE(FilterEngine::Evaluate(result.root, di));
		result = FilterEngine::Parse(L"LeftSize < 200 + 20 * 40", ctxt);
		EXPECT_FALSE(FilterEngine::Evaluate(result.root, di));

		result = FilterEngine::Parse(L"LeftName = \"Alice.txt\"", ctxt);
		EXPECT_TRUE(FilterEngine::Evaluate(result.root, di));
		result = FilterEngine::Parse(L"RightName = \"Bob.txt\"", ctxt);
		EXPECT_FALSE(FilterEngine::Evaluate(result.root, di));
	}

}  // namespace

