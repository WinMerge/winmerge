/** 
 * @file  stringdiffsi.h
 *
 * @brief Declaration file for class stringdiffs
 *
 */
#pragma once

#include <vector>
#include "utils/icu.hpp"

// Uncomment this to see stringdiff log messages
// We don't use _DEBUG since stringdiff logging is verbose and slows down WinMerge
//#define STRINGDIFF_LOGGING

namespace strdiff
{

/**
 * @brief kind of diff blocks.
 */
enum
{
	dlword = 0,
	dlspace,
	dleol,
	dlbreak, 
	dlnumber,
};
/**
 * @brief kind of synchronaction
 */
enum
{
	synbegin1 = 0, 
	synbegin2,
	synend1, 
	synend2 
};

struct wdiff;

/**
 * @brief Class to hold together data needed to implement strdiff::ComputeWordDiffs
 */
class stringdiffs
{
public:
	enum EolCompareMode { EOL_STRICT = 0, EOL_IGNORE = 1, EOL_AS_SPACE = 2 };

	stringdiffs(const String & str1, const String & str2,
		bool case_sensitive, EolCompareMode eol_mode, int whitespace, bool ignore_numbers, int breakType,
		std::vector<wdiff> * pDiffs);

	~stringdiffs();

	void BuildWordDiffList();
	void wordLevelToByteLevel();
	void PopulateDiffs();

// Implementation types
private:
	struct word {
		int start; // index of first character of word in original string
		int end;   // index of last character of word in original string
		int hash;
		int bBreak; // Is it a isWordBreak 0 = word -1= whitespace -2 = empty 1 = breakWord
		word(int s = 0, int e = 0, int b = 0, int h = 0) : start(s), end(e), bBreak(b),hash(h) { }
		inline int length() const { return end+1-start; }
	};

// Implementation methods
private:

	void ComputeByteDiff(const String& str1, const String& str2,
			bool casitive, int xwhite, 
			int begin[2], int end[2], bool equal);
	std::vector<word> BuildWordsArray(const String & str) const;
	unsigned Hash(const String & str, int begin, int end, unsigned h ) const;
	bool AreWordsSame(const word & word1, const word & word2) const;
	bool IsWord(const word & word1) const;
	/**
	 * @brief Is this block an space or whitespace one?
	 */
	inline bool IsSpace(const word & word1) const
	{
		return (word1.bBreak == dlspace);
	}
	/**
	 * @brief Is this block a number one?
	 */
	inline bool IsNumber(const word& word1) const
	{
		return (word1.bBreak == dlnumber);
	}
	/**
	 * @brief Is this block an EOL?
	 */
	inline bool IsEOL(const word & word1) const
	{
		return (word1.bBreak == dleol);
	}
	bool BuildWordDiffList_DP();
	int dp(std::vector<char> & edscript);
	int onp(std::vector<char> & edscript);
	int snake(int k, int y, int M, int N, bool exchanged) const;
#ifdef STRINGDIFF_LOGGING
	void debugoutput();
#endif

// Implementation data
private:
	const String & m_str1;
	const String & m_str2;
	int m_whitespace;
	int m_breakType;
	bool m_case_sensitive;
	EolCompareMode m_eol_mode;
	bool m_ignore_numbers = false;
	bool m_matchblock;
	std::vector<wdiff> * m_pDiffs;
	std::vector<word> m_words1;
	std::vector<word> m_words2;
	std::vector<wdiff> m_wdiffs;
};

}
