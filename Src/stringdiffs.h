/** 
 * @file  stringdiffs.h
 *
 * @brief Interface file declaring sd_ComputeWordDiffs (q.v.)
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef stringdiffs_h_included
#define stringdiffs_h_included

#include "UnicodeString.h"
#include <vector>

/** @brief One difference between two strings */
struct wdiff {
	int start[2]; // 0-based, eg, start[0] is from str1
	int end[2]; // 0-based, eg, end[1] is from str2
	wdiff(int s1=0, int e1=0, int s2=0, int e2=0)
	{
		if (s1>e1) e1=s1-1;
		if (s2>e2) e2=s2-1;
		start[0] = s1;
		start[1] = s2;
		end[0] = e1;
		end[1] = e2;
	}
	wdiff(const wdiff & src)
	{
		for (int i=0; i<2; ++i)
		{
			start[i] = src.start[i];
			end[i] = src.end[i];
		}
	}
};

void sd_Init();
void sd_Close();

void sd_SetBreakChars(const TCHAR *breakChars);

void sd_ComputeWordDiffs(const String & str1, const String & str2,
		bool case_sensitive, int whitespace, int breakType, bool byte_level,
		std::vector<wdiff*> * pDiffs);

#endif // stringdiffs_h_included
