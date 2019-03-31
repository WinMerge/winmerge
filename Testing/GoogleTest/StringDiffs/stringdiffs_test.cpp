#include "pch.h"
#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(""), _T(""), false, 0, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// First string empty
	TEST_F(StringDiffsTest, EmptyFirst)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(""), _T("abcde"), false, 0, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Second string empty
	TEST_F(StringDiffsTest, EmptySecond)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T(""), false, 0, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Default1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), false, 0, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, DefaultDiffererentLen)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcdef"), _T("abcde"), false, 0, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, DefaultWhitespaceEnd)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde "), _T("abcde"), false, 0, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, Caseignore1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), true, 0, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, CaseignoreCase1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcde"), true, 0, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde "), _T("abcde"), true, 0, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T(" abcde"), true, 0, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcde abcde"), false, 1, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde abcde"), _T("  abcde abcde"), false, 1, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace5)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde abcde"), _T("	abcde abcde"), false, 1, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	// Whitespace at begin of first string is a difference
	TEST_F(StringDiffsTest, IgnoreWhitespace6)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde abcde"), _T("abcde	abcde"), false, 1, 0, false);
		EXPECT_EQ(1, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore whitespace change, words, word-level
	TEST_F(StringDiffsTest, IgnoreWhitespace7)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcde	abcde"), false, 1, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), true, 2, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde"), _T("abcde"), true, 2, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace3)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("	abcde"), _T("abcde"), true, 2, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace4)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T(" abcde"), _T("  abcde"), true, 2, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace5)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcdeabcde"), true, 2, 0, false);
/// FIXME:
// 		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace6)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde abcde"), _T("abcde	abcde"), true, 2, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, no case sensitivity, ignore all whitespace, words, word-level
	TEST_F(StringDiffsTest, IgnoreAllWhitespace7)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde\tabcde"), _T("abcde	abcde"), true, 2, 0, false);
		EXPECT_EQ(0, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, words, word-level
	TEST_F(StringDiffsTest, WordBreak2Words1)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde fghij"), _T("abcde fghij"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij"), _T("abcde fGhij"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij"), _T("ABcde fGhij"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fgHIj klmno"), _T("abcde fghij klmno"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij klmno"), _T("abcde fGHij klmno"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcDE fGHij klmno"), _T("abcde fghij klmno"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij klmno"), _T("abcDE fGHij klmno"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde fghij KLmno"), _T("abcDE fghij klmno"), true, 0, 0, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde,fghij"), _T("ABcde,fghij"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fghij"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fGHij"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fghij,klmno"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fGHij,klmno"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fGHij,klmNO"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij,klmno"), _T("abcde,fghij,klmNO"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde:fghij"), _T("abcde:fghij"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fghij"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde:fghij"), _T("abcde:fghij"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("Abcde,fghij"), _T("abcde,fghij"), true, 0, 1, false);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("abcde"), _T("abcde"), true, 0, 0, true);
		EXPECT_TRUE(diffs.size() == 0);
	}

	// Identical strings, case sensitivity, no whitespace, words, byte-level
	TEST_F(StringDiffsTest, ByteLevel2)
	{
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcde"), true, 0, 0, true);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBCde"), _T("abcde"), true, 0, 0, true);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abCde"), true, 0, 0, true);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcDe"), _T("abcde"), true, 0, 0, true);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcDe"), true, 0, 0, true);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcdE"), _T("abcde"), true, 0, 0, true);
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
		std::vector<strdiff::wdiff> diffs = strdiff::ComputeWordDiffs(_T("aBcde"), _T("abcdE"), true, 0, 0, true);
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

}  // namespace
