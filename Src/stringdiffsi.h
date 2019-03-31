/** 
 * @file  stringdiffsi.h
 *
 * @brief Declaration file for class stringdiffs
 *
 */
#pragma once

#include <vector>

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
	dlbreak, 
	dlinsert,
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
	stringdiffs(const String & str1, const String & str2,
		bool case_sensitive, int whitespace, int breakType,
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
		int length() const { return end+1-start; }
	};

// Implementation methods
private:

	std::vector<word> BuildWordsArray(const String & str);
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
	 * @brief Is this block a break?
	 */
	inline bool IsBreak(const word & word1) const
	{
		return (word1.bBreak == dlbreak || word1.bBreak == dlspace);
	}
	/**
	 * @brief Is this block an empty (insert) one?
	 */
	inline bool IsInsert(const word & word1) const
	{
		return (word1.bBreak == dlinsert);
	}
	bool caseMatch(TCHAR ch1, TCHAR ch2) const;
	bool BuildWordDiffList_DP();
	int dp(std::vector<char> & edscript);
	int onp(std::vector<char> & edscript);
	int snake(int k, int y, bool exchanged);
#ifdef STRINGDIFF_LOGGING
	void debugoutput();
#endif

// Implementation data
private:
	const String & m_str1;
	const String & m_str2;
	bool m_case_sensitive;
	int m_whitespace;
	int m_breakType;
	bool m_matchblock;
	std::vector<wdiff> * m_pDiffs;
	std::vector<word> m_words1;
	std::vector<word> m_words2;
	std::vector<wdiff> m_wdiffs;
};

}
