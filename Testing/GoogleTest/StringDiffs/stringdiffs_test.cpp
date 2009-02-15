#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include "stringdiffs.h"

using std::vector;

namespace
{
	// The fixture for testing paths functions.
	class StringDiffsTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		StringDiffsTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~StringDiffsTest()
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

	TEST_F(StringDiffsTest, Identical1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("Boo", "Boo", true, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}


}  // namespace

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
