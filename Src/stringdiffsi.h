/** 
 * @file  stringdiffsi.h
 *
 * @brief Declaration file for class stringdiffs
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef stringdiffsi_h_included
#define stringdiffsi_h_included

/**
 * @brief Class to hold together data needed to implement stringdiffs_Get
 */
class stringdiffs
{
public:
	stringdiffs(const CString & str1, const CString & str2,
		bool case_sensitive, int whitespace, int breakType,
		wdiffarray * pDiffs);

	void BuildWordDiffList();
	void PopulateDiffs();

// Implementation types
private:
	struct word {
		int start; // index of first character of word in original string
		int end;   // index of last character of word in original string
		int hash;
		word(int s=0, int e=0, int h=0) : start(s), end(e), hash(h) { }
		int length() const { return end+1-start; }
	};
	typedef CArray<word, word&> wordarray;


// Implementation methods
private:

	void BuildWordsArray(const CString & str, wordarray * words);
	bool findSync(int *w1, int *w2) const;
	int FindNextMatchInWords2(const word & needword1, int bw2) const;
	int FindNextMatchInWords1(const word & needword2, int bw1) const;

	int hash(const CString & str, int begin, int end) const;
	bool AreWordsSame(const word & word1, const word & word2) const;
	bool caseMatch(TCHAR ch1, TCHAR ch2) const;

// Implementation data
private:
	const CString & m_str1;
	const CString & m_str2;
	bool m_case_sensitive;
	int m_whitespace;
	int m_breakType;
	wdiffarray * m_pDiffs;
	wordarray m_words1;
	wordarray m_words2;
	wdiffarray m_wdiffs;
};


#endif // stringdiffsi_h_included
