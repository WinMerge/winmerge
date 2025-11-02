#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "stringdiffs.h"

using std::vector;

namespace
{
	// The fixture for testing string differencing functions.
	class StringDiffsTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		StringDiffsTest()
		{
			// You can do set-up work for each test	here.
			strdiff::Init();
		}

		virtual ~StringDiffsTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
			strdiff::Close();
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

	// strdiff::ComputeWordDiffs() parameters are:
	// String & str1 - the first string to compare
	// String & str2 - the second string to compare
	// bool case_sensitive - is the compare case-sensitive?
	// int whitespace - do we ignore whitespace and how
	// int breakType - Space (0) or punctuations (1) break
	// bool byte_level - are we word (false) or byte-level (true) diffing
	// std::vector<strdiff::wdiff> * pDiffs - resultting diff list

	// Both strings empty
	TEST_F(StringDiffsTest, EmptyBoth)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(""), _T(""), false, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// First string empty
	TEST_F(StringDiffsTest, EmptyFirst)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(""), _T("abcde"), false, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Second string empty
	TEST_F(StringDiffsTest, EmptySecond)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T(""), false, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Default1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), false, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, DefaultDiffererentLen)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcdef"), _T("abcde"), false, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, DefaultWhitespaceEnd)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde "), _T("abcde"), false, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Caseignore1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, CaseignoreCase1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde "), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T(" abcde"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcde abcde"), false, strdiff::EOL_STRICT, 1, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde abcde"), _T("  abcde abcde"), false, strdiff::EOL_STRICT, 1, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace5)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde abcde"), _T("	abcde abcde"), false, strdiff::EOL_STRICT, 1, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	// Whitespace at begin of first string is a difference
	TEST_F(StringDiffsTest, IgnoreWhitespace6)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde abcde"), _T("abcde	abcde"), false, strdiff::EOL_STRICT, 1, false, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace7)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcde	abcde"), false, strdiff::EOL_STRICT, 1, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), true, strdiff::EOL_STRICT, 2, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde"), _T("abcde"), true, strdiff::EOL_STRICT, 2, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("	abcde"), _T("abcde"), true, strdiff::EOL_STRICT, 2, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde"), _T("  abcde"), true, strdiff::EOL_STRICT, 2, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace5)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcdeabcde"), true, strdiff::EOL_STRICT, 2, false, 0, false);
/// FIXME:
// 		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace6)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcde	abcde"), true, strdiff::EOL_STRICT, 2, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace7)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde\tabcde"), _T("abcde	abcde"), true, strdiff::EOL_STRICT, 2, false, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, WordBreak2Words1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde fghij"), _T("abcde fghij"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	// Last word is different
	TEST_F(StringDiffsTest, WordBreak2Words2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij"), _T("abcde fGhij"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(6, pDiff->begin[0]);
			EXPECT_EQ(6, pDiff->begin[1]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	// Both words are different
	TEST_F(StringDiffsTest, WordBreak2Words3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij"), _T("ABcde fGhij"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(2, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		EXPECT_EQ(0, pDiff->begin[0]);
		EXPECT_EQ(0, pDiff->begin[1]);
		EXPECT_EQ(4, pDiff->end[0]);
		EXPECT_EQ(4, pDiff->end[1]);
		pDiff = &diffs[1];
		EXPECT_EQ(6, pDiff->begin[0]);
		EXPECT_EQ(6, pDiff->begin[1]);
		EXPECT_EQ(10, pDiff->end[0]);
		EXPECT_EQ(10, pDiff->end[1]);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	// Middle word is different
	TEST_F(StringDiffsTest, WordBreak3Words1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fgHIj klmno"), _T("abcde fghij klmno"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(6, pDiff->begin[0]);
			EXPECT_EQ(6, pDiff->begin[1]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	// Middle word is different
	TEST_F(StringDiffsTest, WordBreak3Words2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij klmno"), _T("abcde fGHij klmno"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(6, pDiff->begin[0]);
			EXPECT_EQ(6, pDiff->begin[1]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	// First two words are different
	TEST_F(StringDiffsTest, WordBreak3Words3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcDE fGHij klmno"), _T("abcde fghij klmno"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(2, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		EXPECT_EQ(0, pDiff->begin[0]);
		EXPECT_EQ(0, pDiff->begin[1]);
		EXPECT_EQ(4, pDiff->end[0]);
		EXPECT_EQ(4, pDiff->end[1]);
		pDiff = &diffs[1];
		EXPECT_EQ(6, pDiff->begin[0]);
		EXPECT_EQ(6, pDiff->begin[1]);
		EXPECT_EQ(10, pDiff->end[0]);
		EXPECT_EQ(10, pDiff->end[1]);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	// First two words are different
	TEST_F(StringDiffsTest, WordBreak3Words4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij klmno"), _T("abcDE fGHij klmno"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(2, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		EXPECT_EQ(0, pDiff->begin[0]);
		EXPECT_EQ(0, pDiff->begin[1]);
		EXPECT_EQ(4, pDiff->end[0]);
		EXPECT_EQ(4, pDiff->end[1]);
		pDiff = &diffs[1];
		EXPECT_EQ(6, pDiff->begin[0]);
		EXPECT_EQ(6, pDiff->begin[1]);
		EXPECT_EQ(10, pDiff->end[0]);
		EXPECT_EQ(10, pDiff->end[1]);
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	// First and last words are different generating two diffs
	TEST_F(StringDiffsTest, WordBreak3Words5)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij KLmno"), _T("abcDE fghij klmno"), true, strdiff::EOL_STRICT, 0, false, 0, false);
		EXPECT_EQ(2, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 2)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->end[1]);
			pDiff = &diffs[1];
			EXPECT_EQ(12, pDiff->begin[0]);
			EXPECT_EQ(12, pDiff->begin[1]);
			EXPECT_EQ(16, pDiff->end[0]);
			EXPECT_EQ(16, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// First word is different
	TEST_F(StringDiffsTest, PunctBreak2Words1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde,fghij"), _T("ABcde,fghij"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// First word is different
	TEST_F(StringDiffsTest, PunctBreak2Words2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fghij"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// Both words are different
	TEST_F(StringDiffsTest, PunctBreak2Words3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fGHij"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(2, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 2)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->end[1]);
			pDiff = &diffs[1];
			EXPECT_EQ(6, pDiff->begin[0]);
			EXPECT_EQ(6, pDiff->begin[1]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// First word is different
	TEST_F(StringDiffsTest, PunctBreak3Words1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fghij,klmno"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// First word is different
	TEST_F(StringDiffsTest, PunctBreak3Words2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fGHij,klmno"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(2, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		EXPECT_EQ(0, pDiff->begin[0]);
		EXPECT_EQ(0, pDiff->begin[1]);
		EXPECT_EQ(4, pDiff->end[0]);
		EXPECT_EQ(4, pDiff->end[1]);
		pDiff = &diffs[1];
		EXPECT_EQ(6, pDiff->begin[0]);
		EXPECT_EQ(6, pDiff->begin[1]);
		EXPECT_EQ(10, pDiff->end[0]);
		EXPECT_EQ(10, pDiff->end[1]);
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// All three words are different
	TEST_F(StringDiffsTest, PunctBreak3Words3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fGHij,klmNO"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(3, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		EXPECT_EQ(0, pDiff->begin[0]);
		EXPECT_EQ(0, pDiff->begin[1]);
		EXPECT_EQ(4, pDiff->end[0]);
		EXPECT_EQ(4, pDiff->end[1]);
		pDiff = &diffs[1];
		EXPECT_EQ(6, pDiff->begin[0]);
		EXPECT_EQ(6, pDiff->begin[1]);
		EXPECT_EQ(10, pDiff->end[0]);
		EXPECT_EQ(10, pDiff->end[1]);
		pDiff = &diffs[2];
		EXPECT_EQ(12, pDiff->begin[0]);
		EXPECT_EQ(12, pDiff->begin[1]);
		EXPECT_EQ(16, pDiff->end[0]);
		EXPECT_EQ(16, pDiff->end[1]);
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// First word is different
	TEST_F(StringDiffsTest, PunctBreak3Words4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fghij,klmNO"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(2, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		EXPECT_EQ(0, pDiff->begin[0]);
		EXPECT_EQ(0, pDiff->begin[1]);
		EXPECT_EQ(4, pDiff->end[0]);
		EXPECT_EQ(4, pDiff->end[1]);
		pDiff = &diffs[1];
		EXPECT_EQ(12, pDiff->begin[0]);
		EXPECT_EQ(12, pDiff->begin[1]);
		EXPECT_EQ(16, pDiff->end[0]);
		EXPECT_EQ(16, pDiff->end[1]);
	}

	// Empty custom punct string
	// Match whole strings
	TEST_F(StringDiffsTest, CustomPunct1)
	{
		strdiff::SetBreakChars(_T(""));
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde:fghij"), _T("abcde:fghij"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() > 0)
		{
			strdiff::wdiff *pDiff = &diffs[0];
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->end[1]);
		}
	}

	// Empty custom punct string
	// Match whole strings
	TEST_F(StringDiffsTest, CustomPunct2)
	{
 		strdiff::SetBreakChars(_T(""));
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fghij"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() > 0)
		{
			strdiff::wdiff *pDiff = &diffs[0];
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// First word is different
	TEST_F(StringDiffsTest, CustomPunctBreak1)
	{
 		strdiff::SetBreakChars(_T(":"));
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde:fghij"), _T("abcde:fghij"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, word-level
	// First word is different
	TEST_F(StringDiffsTest, CustomPunctBreak2)
	{
 		strdiff::SetBreakChars(_T(";"));
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fghij"), true, strdiff::EOL_STRICT, 0, false, 1, false);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(0, pDiff->begin[0]);
			EXPECT_EQ(0, pDiff->begin[1]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	TEST_F(StringDiffsTest, ByteLevel1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	TEST_F(StringDiffsTest, ByteLevel2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(1, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->begin[1]);
			EXPECT_EQ(1, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	TEST_F(StringDiffsTest, ByteLevel3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBCde"), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(2, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->begin[1]);
			EXPECT_EQ(2, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	TEST_F(StringDiffsTest, ByteLevel4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abCde"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(2, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->begin[1]);
			EXPECT_EQ(2, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	// NOTE NOTE: This is questionable?
	TEST_F(StringDiffsTest, ByteLevel5)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcDe"), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(3, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(3, pDiff->end[0]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	// NOTE NOTE: This is questionable?
	TEST_F(StringDiffsTest, ByteLevel6)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcDe"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(3, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->begin[1]);
			EXPECT_EQ(3, pDiff->end[1]);
		}	
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	// NOTE NOTE: This is questionable?
	TEST_F(StringDiffsTest, ByteLevel7)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcdE"), _T("abcde"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff;
		if (diffs.size() == 1 )
		{
			pDiff = &diffs[0];
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[1]);
		}
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	// NOTE NOTE: This is questionable?
	TEST_F(StringDiffsTest, ByteLevel8)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcdE"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff *pDiff;
		if (diffs.size() == 1)
		{
            pDiff = &diffs[0];
			EXPECT_EQ(1, pDiff->begin[0]);
			EXPECT_EQ(4, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->begin[1]);
			EXPECT_EQ(4, pDiff->end[1]);
		}
	}

	TEST_F(StringDiffsTest, IgnoreEOL1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abc\r\n"), _T("abc\n"), true, strdiff::EOL_IGNORE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
	}

	TEST_F(StringDiffsTest, IgnoreEOL2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abc\r\n"), _T("abc\r"), true, strdiff::EOL_IGNORE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
	}

	TEST_F(StringDiffsTest, IgnoreEOL3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abc\r\n\r\n"), _T("abc\n\n"), true, strdiff::EOL_IGNORE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
	}

	TEST_F(StringDiffsTest, IgnoreEOL4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abc\r\r"), _T("abc\n\n"), true, strdiff::EOL_IGNORE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
	}

	TEST_F(StringDiffsTest, IgnoreEOL5)
	{
		// GitHub issue #1838 End of line diff is a bit wanky
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abc\r\ndef\r\n"), _T("abc\ndef\n"), true, strdiff::EOL_IGNORE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
		diffs = strdiff::ComputeWordDiffs(_T("  abc  \r\ndef\r\n  "), _T("  abc  \ndef\n  "), true, strdiff::EOL_IGNORE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
	}

	TEST_F(StringDiffsTest, IgnoreLineBreaks)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abc\r\ndef\r\n"), _T("abc\ndef\n"), true, strdiff::EOL_AS_SPACE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
		diffs = strdiff::ComputeWordDiffs(_T("  abc  \r\ndef\r\n  "), _T("  abc  \ndef\n  "), true, strdiff::EOL_AS_SPACE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
		diffs = strdiff::ComputeWordDiffs(_T("  abc  \r\ndef\r\n  "), _T("  abc   def   "), true, strdiff::EOL_AS_SPACE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
		diffs = strdiff::ComputeWordDiffs(_T("  abc   def   "), _T("  abc  \ndef\n  "), true, strdiff::EOL_AS_SPACE, 0, false, 0, true);
		EXPECT_EQ(0, diffs.size());
		diffs = strdiff::ComputeWordDiffs(_T("  abc   def   "), _T("  abc\ndef\n  "), true, strdiff::EOL_AS_SPACE, 1, false, 0, true);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings with numbers
	TEST_F(StringDiffsTest, IgnoreNumbers)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("ab 1"), _T("ab 2"), true, strdiff::EOL_STRICT, 0, false, 0, true);
		EXPECT_EQ(1, diffs.size());
		strdiff::wdiff* pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(3, pDiff->begin[0]);
			EXPECT_EQ(3, pDiff->end[0]);
			EXPECT_EQ(3, pDiff->begin[1]);
			EXPECT_EQ(3, pDiff->end[1]);
		}

		diffs = strdiff::ComputeWordDiffs(_T("ab 1"), _T("ab 2"), true, strdiff::EOL_STRICT, 0, true, 0, true);
		EXPECT_EQ(0, diffs.size());

		diffs = strdiff::ComputeWordDiffs(_T("ab 1234"), _T("ab 2"), true, strdiff::EOL_STRICT, 0, true, 0, true);
		EXPECT_EQ(0, diffs.size());

		diffs = strdiff::ComputeWordDiffs(_T("ab 12 34"), _T("ab 2"), true, strdiff::EOL_STRICT, 0, true, 0, true);
		EXPECT_EQ(1, diffs.size());
		pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(5, pDiff->begin[0]);
			EXPECT_EQ(5, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->begin[1]);
			EXPECT_EQ(3, pDiff->end[1]); // at line end the end is always (begin - 1)
		}

		diffs = strdiff::ComputeWordDiffs(_T("ab 12 34"), _T("ab 2 c"), true, strdiff::EOL_STRICT, 0, true, 0, true);
		EXPECT_EQ(1, diffs.size());
		pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(6, pDiff->begin[0]);
			EXPECT_EQ(7, pDiff->end[0]);
			EXPECT_EQ(5, pDiff->begin[1]);
			EXPECT_EQ(5, pDiff->end[1]);
		}

		diffs = strdiff::ComputeWordDiffs(_T("ab  12 34"), _T("aB 2 c"), false, strdiff::EOL_STRICT, 1, true, 0, true);
		EXPECT_EQ(1, diffs.size());
		pDiff = &diffs[0];
		if (diffs.size() == 1)
		{
			EXPECT_EQ(7, pDiff->begin[0]);
			EXPECT_EQ(8, pDiff->end[0]);
			EXPECT_EQ(5, pDiff->begin[1]);
			EXPECT_EQ(5, pDiff->end[1]);
		}

	}

	TEST_F(StringDiffsTest, CompareCase)
	{
		int result;
		result = strdiff::Compare(_T("abc"), _T("abc"), false, strdiff::EOL_STRICT, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("ABC"), _T("abc"), false, strdiff::EOL_STRICT, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc"), _T("ABC"), false, strdiff::EOL_STRICT, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("aBC"), _T("Bcd"), false, strdiff::EOL_STRICT, 0, false);
		EXPECT_EQ(1, result);
		result = strdiff::Compare(_T("Bcd"), _T("aBC"), false, strdiff::EOL_STRICT, 0, false);
		EXPECT_EQ(-1, result);
	}

	TEST_F(StringDiffsTest, CompareWhiltespaces1)
	{
		int result;
		result = strdiff::Compare(_T("abc def"), _T("abc def"), true, strdiff::EOL_STRICT, 1, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc  def"), _T("abc def"), true, strdiff::EOL_STRICT, 1, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc def"), _T("abc  def"), true, strdiff::EOL_STRICT, 1, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc \tdef"), _T("abc def"), true, strdiff::EOL_STRICT, 1, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc def"), _T("abc \tdef"), true, strdiff::EOL_STRICT, 1, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc def"), _T("abcdef"), true, strdiff::EOL_STRICT, 1, false);
		EXPECT_EQ(1, result);
		result = strdiff::Compare(_T("abcdef"), _T("abc def"), true, strdiff::EOL_STRICT, 1, false);
		EXPECT_EQ(-1, result);
	}

	TEST_F(StringDiffsTest, CompareWhiltespaces2)
	{
		int result;
		result = strdiff::Compare(_T("abc def"), _T("abc def"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc  def"), _T("abc def"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc def"), _T("abc  def"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc \tdef"), _T("abc def"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc def"), _T("abc \tdef"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc def"), _T("abcdef"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abcdef"), _T("abc def"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\tdef"), _T("abcdef"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abcdef"), _T("abc\tdef"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abcde"), _T("abcdef"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(1, result);
		result = strdiff::Compare(_T("abcdef"), _T("abcde"), true, strdiff::EOL_STRICT, 2, false);
		EXPECT_EQ(-1, result);
	}

	TEST_F(StringDiffsTest, CompareEOL)
	{
		int result;
		result = strdiff::Compare(_T("abc\r\n"), _T("abc\r\n") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r\n"), _T("abc\r") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r"), _T("abc\r\n") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r\n"), _T("abc\n") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\n"), _T("abc\r\n") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\n"), _T("abc\r") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r"), _T("abc\n") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(String(_T("abc\r\0"), 5), _T("abc\r\n") , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(-1, result);
		result = strdiff::Compare(_T("abc\r\n"), String(_T("abc\r\0"), 5) , true, strdiff::EOL_IGNORE, 0, false);
		EXPECT_EQ(1, result);
	}

	TEST_F(StringDiffsTest, CompareEOLAsSpace)
	{
		int result;
		result = strdiff::Compare(_T("abc\r\n"), _T("abc\r\n") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r\n"), _T("abc\r") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r"), _T("abc\r\n") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r\n"), _T("abc\n") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\n"), _T("abc\r\n") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\n"), _T("abc\r") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r"), _T("abc\n") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(String(_T("abc\r\0"), 5), _T("abc\r\n") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(-1, result);
		result = strdiff::Compare(_T("abc\r\n"), String(_T("abc\r\0"), 5) , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(1, result);

		result = strdiff::Compare(_T("abc\r\n"), _T("abc ") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc "), _T("abc\r") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc "), _T("abc ") , true, strdiff::EOL_AS_SPACE, 0, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc\r\n"), _T("abc  ") , true, strdiff::EOL_AS_SPACE, 1, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc  "), _T("abc\r\n") , true, strdiff::EOL_AS_SPACE, 1, false);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("abc"), _T("abc\r\n") , true, strdiff::EOL_AS_SPACE, 2, false);
		EXPECT_EQ(0, result);
	}

	TEST_F(StringDiffsTest, CompareNumbers)
	{
		int result;
		result = strdiff::Compare(_T("1a"), _T("2a") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("a123"), _T("a234") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("a"), _T("123a") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("123a"), _T("a") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("123"), _T("") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T(""), _T("123") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(0, result);
		result = strdiff::Compare(_T("123a"), _T("123b") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(1, result);
		result = strdiff::Compare(_T("123b"), _T("123a") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(-1, result);
		result = strdiff::Compare(_T("a123"), _T("b123") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(1, result);
		result = strdiff::Compare(_T("b123"), _T("a123") , true, strdiff::EOL_STRICT, 0, true);
		EXPECT_EQ(-1, result);
	}

}  // namespace
