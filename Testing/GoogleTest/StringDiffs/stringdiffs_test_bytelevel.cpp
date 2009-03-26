#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>
#include <vector>
#include "stringdiffs.h"

using std::vector;

namespace
{
	// The fixture for testing stringdiff.
	class StringDiffsTestByte : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		StringDiffsTestByte()
		{
			// You can do set-up work for each test	here.
			sd_Init();
		}

		virtual ~StringDiffsTestByte()
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


	TEST_F(StringDiffsTestByte, ByteLevel19b3)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(",;");
		sd_ComputeWordDiffs( "				wsprintf(buf, _T(left=  %s,   %d,%d, right=  %s,   %d,%d \n),",
				"					if (len2 < 50)",
				true, 0, 1, true, &diffs);
		EXPECT_EQ(10, diffs.size());
	}

	// Identical strings, case sensitivity, no whitespace, punctuations, byte-level
	// Second word is different
	TEST_F(StringDiffsTestByte, ByteLevel19b2)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(",;");
		sd_ComputeWordDiffs( "	while (1)", "	for (;;)", true, 0, 1, true, &diffs);
		EXPECT_EQ(3, diffs.size());
		wdiff *pDiff;
		if (diffs.size() >= 1 )
		{
			pDiff = diffs[0];
			EXPECT_EQ(1, pDiff->start[0]);
			EXPECT_EQ(5, pDiff->end[0]);
			EXPECT_EQ(1, pDiff->start[1]);
			EXPECT_EQ(3, pDiff->end[1]);
		}	
		if (diffs.size() >=2 )
		{
			pDiff = diffs[1];
			EXPECT_EQ(8, pDiff->start[0]);
			EXPECT_EQ(9, pDiff->end[0]);
			EXPECT_EQ(5, pDiff->start[1]);
			EXPECT_EQ(4, pDiff->end[1]);
		}	
		if (diffs.size() >=3 )
		{
			pDiff = diffs[2];
			EXPECT_EQ(10, pDiff->start[0]);
			EXPECT_EQ(9, pDiff->end[0]);
			EXPECT_EQ(6, pDiff->start[1]);
			EXPECT_EQ(8, pDiff->end[1]);
		}	
	}
	// Identical strings, case sensitivity, no whitespace, punctuations, byte-level
	// Second word is different
	TEST_F(StringDiffsTestByte, ByteLevel19b1)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(",");
		sd_ComputeWordDiffs( "abcdef,abccef,abcdef,", "abcdef,abcdef,abcdef,",
				true, 0, 1, true, &diffs);
		EXPECT_EQ(1, diffs.size());
		wdiff *pDiff;
		if (diffs.size() >= 1 )
		{
			pDiff = diffs[0];
			EXPECT_EQ(10, pDiff->start[0]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->start[1]);
			EXPECT_EQ(10, pDiff->end[1]);
		}	
	}
	// NoneIdentical strings, case sensitivity, no whitespace, punctuations, byte-level
	// Second word is different
	TEST_F(StringDiffsTestByte, ByteLevel20c)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(".,;:()[]{}!@#\"$%^&*~+-=<>\'/\\|");
		sd_ComputeWordDiffs("/**", "		// remove empty records on both side",
				true, 0, 1, true, &diffs);
		EXPECT_EQ(4, diffs.size());
		wdiff *pDiff;
		if (diffs.size() >= 1 )
		{
			pDiff = diffs[0];
			EXPECT_EQ(0, pDiff->start[0]);
			EXPECT_EQ(-1, pDiff->end[0]);
			EXPECT_EQ(0, pDiff->start[1]);
			EXPECT_EQ(1, pDiff->end[1]);
		}	
		if (diffs.size() >=2 )
		{
			pDiff = diffs[1];
			EXPECT_EQ(1, pDiff->start[0]);
			EXPECT_EQ(1, pDiff->end[0]);
			EXPECT_EQ(3, pDiff->start[1]);
			EXPECT_EQ(3, pDiff->end[1]);
		}	
		if (diffs.size() >=3 )
		{
			pDiff = diffs[2];
			EXPECT_EQ(2, pDiff->start[0]);
			EXPECT_EQ(2, pDiff->end[0]);
			EXPECT_EQ(4, pDiff->start[1]);
			EXPECT_EQ(4, pDiff->end[1]);
		}	
		if (diffs.size() >=4 )
		{
			pDiff = diffs[3];
			EXPECT_EQ(3, pDiff->start[0]);
			EXPECT_EQ(2, pDiff->end[0]);
			EXPECT_EQ(5, pDiff->start[1]);
			EXPECT_EQ(37, pDiff->end[1]);
		}	
		if (diffs.size() >=5 )
		{
			pDiff = diffs[4];
			EXPECT_EQ(40, pDiff->start[0]);
			EXPECT_EQ(40, pDiff->end[0]);
			EXPECT_EQ(37, pDiff->start[1]);
			EXPECT_EQ(37, pDiff->end[1]);
		}	
	}
	// NoneIdentical strings, case sensitivity, no whitespace, punctuations, byte-level
	// Second word is different
	TEST_F(StringDiffsTestByte, ByteLevel20a)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(".,;:()[]{}!@#\"$%^&*~+-=<>\'/\\|");
		sd_ComputeWordDiffs(",;+ der abcdef,der,Thomas,abcdef,abcdef,;",
				",;+ der abcdef,Thomas,accdgf,abcdef,-+",
				true, 0, 1, true, &diffs);
		EXPECT_EQ(4, diffs.size());
		wdiff *pDiff;
		if (diffs.size() >= 1 )
		{
			pDiff = diffs[0];
			EXPECT_EQ(15, pDiff->start[0]);
			EXPECT_EQ(18, pDiff->end[0]);
			EXPECT_EQ(15, pDiff->start[1]);
			EXPECT_EQ(14, pDiff->end[1]);
		}	
		if (diffs.size() >=2 )
		{
			pDiff = diffs[1];
			EXPECT_EQ(27, pDiff->start[0]);
			EXPECT_EQ(30, pDiff->end[0]);
			EXPECT_EQ(23, pDiff->start[1]);
			EXPECT_EQ(26, pDiff->end[1]);
		}	
		if (diffs.size() >=3 )
		{
			pDiff = diffs[2];
			EXPECT_EQ(40, pDiff->start[0]);
			EXPECT_EQ(40, pDiff->end[0]);
			EXPECT_EQ(36, pDiff->start[1]);
			EXPECT_EQ(36, pDiff->end[1]);
		}	
		if (diffs.size() >=4 )
		{
			pDiff = diffs[3];
			EXPECT_EQ(41, pDiff->start[0]);
			EXPECT_EQ(40, pDiff->end[0]);
			EXPECT_EQ(37, pDiff->start[1]);
			EXPECT_EQ(37, pDiff->end[1]);
		}	
	}

	// NoneIdentical strings, case sensitivity, no whitespace, punctuations, word-level
	// Second word is different
	TEST_F(StringDiffsTestByte, ByteLevel19a)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(",");
		sd_ComputeWordDiffs("abcdef,abcdef,abcdef,", "abcdef,abccef,abcdef,",
				true, 0, 1, false, &diffs);
		EXPECT_EQ(1, diffs.size());
		wdiff *pDiff;
		if (diffs.size() >= 1 )
		{
			pDiff = diffs[0];
			EXPECT_EQ(7, pDiff->start[0]);
			EXPECT_EQ(12, pDiff->end[0]);
			EXPECT_EQ(7, pDiff->start[1]);
			EXPECT_EQ(12, pDiff->end[1]);
		}	
	}
	// Identical strings, case sensitivity, no whitespace, punctuations, byte-level
	// Second word is different

	TEST_F(StringDiffsTestByte, ByteLevel19b)
	{
		std::vector<wdiff*> diffs;
		sd_SetBreakChars(",");
		sd_ComputeWordDiffs( "abcdef,abccef,abcdef,", "abcdef,abcdef,abcdef,",
				true, 0, 1, true, &diffs);
		EXPECT_EQ(1, diffs.size());
		wdiff *pDiff;
		if (diffs.size() >= 1 )
		{
			pDiff = diffs[0];
			EXPECT_EQ(10, pDiff->start[0]);
			EXPECT_EQ(10, pDiff->end[0]);
			EXPECT_EQ(10, pDiff->start[1]);
			EXPECT_EQ(10, pDiff->end[1]);
		}	
	}

}  // namespace
