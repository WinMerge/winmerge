#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include "stringdiffs.h"


using std::vector;

namespace
{
	// The fixture for testing String differencing functions.
	// These tests test adding/removing chars.
	class StringDiffsAddsTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		StringDiffsAddsTest()
		{
			// You can do set-up work for each test	here.
			sd_Init();
		}

		virtual ~StringDiffsAddsTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
			sd_Close();
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
	// int breakType - Space (0) or punctuations (1) break
	// bool byte_level - are we word (false) or byte-level (true) diffing
	// std::vector<wdiff*> * pDiffs - resultting diff list

	// Char added to begin
	TEST_F(StringDiffsAddsTest, AddBeginFirst1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcdefgh", "1abcdefgh",
				false, 0, 0, false, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(7, pDiff->end[0]);
			EXPECT_EQ(8, pDiff->end[1]);
		}
	}

	// Char added to begin
	TEST_F(StringDiffsAddsTest, AddBeginFirst2)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("1abcdefgh", "abcdefgh",
				false, 0, 0, false, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(8, pDiff->end[0]);
			EXPECT_EQ(7, pDiff->end[1]);
		}
	}

	// Char added to begin
	TEST_F(StringDiffsAddsTest, AddBeginFirstChar1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcdefgh", "1abcdefgh",
				false, 0, 0, true, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(-1, pDiff->end[0]);
			EXPECT_EQ(0, pDiff->end[1]);
		}
	}

	// Char added to begin
	TEST_F(StringDiffsAddsTest, AddBeginFirstChar2)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("1abcdefgh", "abcdefgh",
				false, 0, 0, true, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(0, pDiff->end[0]);
			EXPECT_EQ(-1, pDiff->end[1]);
		}
	}

	// Char added to end
	TEST_F(StringDiffsAddsTest, AddEndFirst1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcdefgh", "abcdefgh1",
				false, 0, 0, false, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(7, pDiff->end[0]);
			EXPECT_EQ(8, pDiff->end[1]);
		}
	}

	// Char added to end
	TEST_F(StringDiffsAddsTest, AddEndFirst2)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcdefgh1", "abcdefgh",
				false, 0, 0, false, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(8, pDiff->end[0]);
			EXPECT_EQ(7, pDiff->end[1]);
		}
	}

	// Char added to end
	TEST_F(StringDiffsAddsTest, AddEndFirstChar1)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcdefgh", "abcdefgh1",
				false, 0, 0, true, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(8, pDiff->start[1]);
			EXPECT_EQ(-1, pDiff->end[0]);
			EXPECT_EQ(8, pDiff->end[1]);
		}
	}

	// Char added to end
	TEST_F(StringDiffsAddsTest, AddEndFirstChar2)
	{
		std::vector<wdiff*> diffs;
		sd_ComputeWordDiffs("abcdefgh1", "abcdefgh",
				false, 0, 0, true, &diffs);
		EXPECT_EQ(1, diffs.size());
		if (diffs.size() >= 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(8, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(8, pDiff->end[0]);
			EXPECT_EQ(-1, pDiff->end[1]);
		}
	}
	// exchange string left right side only
	TEST_F(StringDiffsAddsTest, ExchangeString1a)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(".,;:()[]{}!@#\"$%^&*~+-=<>\'/\\|");
		sd_ComputeWordDiffs("N2130   _RobOk=_INT B_AND 512                   ;Roboter bereit",
			"N2040   _RobOk=_INT B_AND 'B1000000000'          ;Roboter bereit",
				false, 1, 0, true, &diffs);
		EXPECT_EQ(2, diffs.size());
		if (diffs.size() > 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(2, pDiff->start[0]);
			EXPECT_EQ(2, pDiff->start[1]);
			EXPECT_EQ(3, pDiff->end[0]);
			EXPECT_EQ(3, pDiff->end[1]);
			pDiff = diffs[1];
			EXPECT_EQ(26, pDiff->start[0]);
			EXPECT_EQ(26, pDiff->start[1]);
			EXPECT_EQ(28, pDiff->end[0]);
			EXPECT_EQ(38, pDiff->end[1]);
		}
	}
	// exchange string left right side only
	TEST_F(StringDiffsAddsTest, ExchangeString1b)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(".,;:()[]{}!@#\"$%^&*~+-=<>\'/\\|");
		sd_ComputeWordDiffs("N2040   _RobOk=_INT B_AND 'B1000000000'          ;Roboter bereit",
			"N2130   _RobOk=_INT B_AND 512                   ;Roboter bereit",
				false, 1, 0, true, &diffs);
		EXPECT_EQ(2, diffs.size());
		if (diffs.size() > 1)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(2, pDiff->start[0]);
			EXPECT_EQ(2, pDiff->start[1]);
			EXPECT_EQ(3, pDiff->end[0]);
			EXPECT_EQ(3, pDiff->end[1]);
			pDiff = diffs[1];
			EXPECT_EQ(26, pDiff->start[0]);
			EXPECT_EQ(26, pDiff->start[1]);
			EXPECT_EQ(38, pDiff->end[0]);
			EXPECT_EQ(28, pDiff->end[1]);
		}
	}

	// exchange string left right side only
	TEST_F(StringDiffsAddsTest, ExchangeString2a)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(".,;:()[]{}!@#\"$%^&*~+-=<>\'/\\|");
		sd_ComputeWordDiffs("N1960 IF(R2941==2) OR (R2941==203))",
			"N1830 IF((R2941==2)   OR (R2941==3)    ",
				false, 1, 1, true, &diffs);
		EXPECT_EQ(4, diffs.size());
		if (diffs.size() > 3)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(2, pDiff->start[0]);
			EXPECT_EQ(2, pDiff->start[1]);
			EXPECT_EQ(3, pDiff->end[0]);
			EXPECT_EQ(3, pDiff->end[1]);
			pDiff = diffs[1];
			EXPECT_EQ(8, pDiff->start[0]);
			EXPECT_EQ(8, pDiff->start[1]);
			EXPECT_EQ(7, pDiff->end[0]);
			EXPECT_EQ(8, pDiff->end[1]);
			pDiff = diffs[2];
			EXPECT_EQ(30, pDiff->start[0]);
			EXPECT_EQ(33, pDiff->start[1]);
			EXPECT_EQ(31, pDiff->end[0]);
			EXPECT_EQ(32, pDiff->end[1]);
			pDiff = diffs[3];
			EXPECT_EQ(34, pDiff->start[0]);
			EXPECT_EQ(35, pDiff->start[1]);
			EXPECT_EQ(34, pDiff->end[0]);
			EXPECT_EQ(38, pDiff->end[1]);
		}
	}
	// exchange string left right side only
	TEST_F(StringDiffsAddsTest, ExchangeString2b)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(".,;:()[]{}!@#\"$%^&*~+-=<>\'/\\|");
		sd_ComputeWordDiffs("N1830 IF((R2941==2)   OR (R2941==3)    ",
			"N1960 IF(R2941==2) OR (R2941==203))",
				false, 1, 1, true, &diffs);
		EXPECT_EQ(4, diffs.size());
		if (diffs.size() > 3)
		{
			wdiff *pDiff = diffs[0];
			EXPECT_EQ(2, pDiff->start[0]);
			EXPECT_EQ(2, pDiff->start[1]);
			EXPECT_EQ(3, pDiff->end[0]);
			EXPECT_EQ(3, pDiff->end[1]);
			pDiff = diffs[1];
			EXPECT_EQ(8, pDiff->start[0]);
			EXPECT_EQ(8, pDiff->start[1]);
			EXPECT_EQ(8, pDiff->end[0]);
			EXPECT_EQ(7, pDiff->end[1]);
			pDiff = diffs[2];
			EXPECT_EQ(33, pDiff->start[0]);
			EXPECT_EQ(30, pDiff->start[1]);
			EXPECT_EQ(32, pDiff->end[0]);
			EXPECT_EQ(31, pDiff->end[1]);
			pDiff = diffs[3];
			EXPECT_EQ(35, pDiff->start[0]);
			EXPECT_EQ(34, pDiff->start[1]);
			EXPECT_EQ(38, pDiff->end[0]);
			EXPECT_EQ(34, pDiff->end[1]);
		}
	}
	// new option third run
	TEST_F(StringDiffsAddsTest, RunThird)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(".,;:()[]{}!@#\"$%^&*~+-=<>\'/\\|");
		sd_ComputeWordDiffs("(sizeof *new);",
			"sizeof(*newob));",
				false, 1, 0, true, &diffs);
		EXPECT_EQ(3, diffs.size());
		wdiff *pDiff;
		if (diffs.size() >= 1)
		{
			pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(0, pDiff->end[0]);
			EXPECT_EQ(-1, pDiff->end[1]);
		}
		if (diffs.size() >= 2)
		{
			pDiff = diffs[1];
			EXPECT_EQ(7, pDiff->start[0]);
			EXPECT_EQ(6, pDiff->start[1]);
			EXPECT_EQ(6, pDiff->end[0]);
			EXPECT_EQ(15, pDiff->end[1]);
		}
		if (diffs.size() >= 3)
		{
			pDiff = diffs[2];
			EXPECT_EQ(7, pDiff->start[0]);
			EXPECT_EQ(16, pDiff->start[1]);
			EXPECT_EQ(13, pDiff->end[0]);
			EXPECT_EQ(15, pDiff->end[1]);
		}
	}
}  // namespace
