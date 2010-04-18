/** 
 * @file  stringdiffs.h
 *
 * @brief Interface file declaring sd_ComputeWordDiffs (q.v.)
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: stringdiffs.h 6712 2009-05-03 09:37:54Z kimmov $

#ifndef stringdiffs_h_included
#define stringdiffs_h_included

#include "UnicodeString.h"
#include <vector>

/** @brief One difference between two strings */
struct wdiff {
	int begin[3]; // 0-based, eg, begin[0] is from str1
	int end[3]; // 0-based, eg, end[1] is from str2
	wdiff(int s1=0, int e1=0, int s2=0, int e2=0, int s3=0, int e3=0)
	{
		if (s1>e1) e1=s1-1;
		if (s2>e2) e2=s2-1;
		if (s3>e3) e3=s3-1;
		begin[0] = s1;
		begin[1] = s2;
		begin[2] = s3;
		end[0] = e1;
		end[1] = e2;
		end[2] = e3;
	}
	wdiff(const wdiff & src)
	{
		for (int i=0; i<3; ++i)
		{
			begin[i] = src.begin[i];
			end[i] = src.end[i];
		}
	}
};

void sd_Init();
void sd_Close();

void sd_SetBreakChars(const TCHAR *breakChars);

void sd_ComputeWordDiffs(const String str1, const String str2,
	bool case_sensitive, int whitespace, int breakType, bool byte_level,
	std::vector<wdiff*> * pDiffs);
void sd_ComputeWordDiffs(int nStrings, const String str[3], 
                   bool case_sensitive, int whitespace, int breakType, bool byte_level,
				   std::vector<wdiff*> * pDiffs);
bool IsSide0Empty(std::vector<wdiff*> worddiffs, int nLineLengt);
bool IsSide1Empty(std::vector<wdiff*> worddiffs, int nLineLengt);

void sd_ComputeByteDiff(const String& str1, const String& str2,
			bool casitive, int xwhite, 
			int begin[2], int end[2], bool equal);


#endif // stringdiffs_h_included
