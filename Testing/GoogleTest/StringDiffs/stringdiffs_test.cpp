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

	// sd_ComputeWordDiffs() parameters are:
	// String & str1 - the first string to compare
	// String & str2 - the second string to compare
	// bool case_sensitive - is the compare case-sensitive?
	// int whitespace - do we ignore whitespace and how
	// int breakType - word (0) or char (1) break
	// bool byte_level - are we word (false) or byte-level (true) diffing
	// std::vector<wdiff*> * pDiffs - resultting diff list

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Default1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcde", "abcde", false, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, DefaultDiffererentLen)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcdef", "abcde", false, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 1);
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, DefaultWhitespaceEnd)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcde ", "abcde", false, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 1);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Caseignore1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcde", "abcde", true, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, CaseignoreCase1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("aBcde", "abcde", true, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 1);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, CaseignoreWhitespace1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("aBcde ", "abcde", true, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 2);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, CaseignoreWhitespace2)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("aBcde", " abcde", true, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 2);
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, CaseignoreWhitespace3)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcde abcde", "abcde abcde", false, 1, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, CaseignoreWhitespace4)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs(" abcde abcde", "  abcde abcde", false, 1, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, CaseignoreWhitespace5)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs(" abcde abcde", "	abcde abcde", false, 1, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, CaseignoreWhitespace6)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs(" abcde abcde", "abcde	abcde", false, 1, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Identical2)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcde", "abcde", true, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Case-differing strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Identical3)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("aBcde", "abcDe", false, 0, 0, false, &diffs);
		EXPECT_TRUE(diffs.size() == 0);
	}

}  // namespace

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
