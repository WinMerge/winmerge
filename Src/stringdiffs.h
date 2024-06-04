/** 
 * @file  stringdiffs.h
 *
 * @brief Interface file declaring ComputeWordDiffs (q.v.)
 *
 */
#pragma once

#include "UnicodeString.h"
#include <vector>

namespace strdiff
{

/** @brief One difference between two strings */
struct wdiff {
	std::array<int, 3> begin; // 0-based, eg, begin[0] is from str1
	std::array<int, 3> end; // 0-based, eg, end[1] is from str2
	int op;
	wdiff(int s1=0, int e1=0, int s2=0, int e2=0, int s3=0, int e3=0)
		: begin{s1, s2, s3}
		, op(-1)
	{
		if (s1>e1) e1=s1-1;
		if (s2>e2) e2=s2-1;
		if (s3>e3) e3=s3-1;
		end[0] = e1;
		end[1] = e2;
		end[2] = e3;
	}
};

void Init();
void Close();

void SetBreakChars(const tchar_t *breakChars);

std::vector<wdiff> ComputeWordDiffs(const String& str1, const String& str2,
	bool case_sensitive, bool eol_sensitive, int whitespace, bool ignore_numbers, int breakType, bool byte_level);
std::vector<wdiff> ComputeWordDiffs(int nStrings, const String *str, 
                   bool case_sensitive, bool eol_sensitive, int whitespace, bool ignore_numbers, int breakType, bool byte_level);
int Compare(const String& str1, const String& str2,
	bool case_sensitive, bool eol_sensitive, int whitespace, bool ignore_numbers);

}
