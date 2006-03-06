/** 
 * @file  PatternSet.h
 *
 * @brief Declaration of PatternSet class
 *
 * This applied an arbitrary number of regular expression transformations
 * either to make lines empty or to change them.
 *
 * This uses the CRegExp class from WinMerge Src\Common.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef PatternSet_h_included
#define PatternSet_h_included

class widestr;
class CRegExp;

class PatternSet
{
public:
	PatternSet();
	bool loadPatterns(HINSTANCE hinst);
	~PatternSet();
	bool processLine(widestr & wstr);

	bool shouldIgnoreQuotes() const { return m_ignoreQuotes; }

private:
	void clear();
private:
	CRegExp * m_ignoreRegexps;
	CRegExp * m_subRegexps;
	LPTSTR * m_replaceStrings;
	int m_ignoreLinesCount;
	int m_substitutionsCount;
	bool m_ignoreQuotes;
};


#endif // PatternSet_h_included
