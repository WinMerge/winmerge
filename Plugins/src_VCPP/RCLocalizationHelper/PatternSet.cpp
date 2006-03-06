/** 
 * @file  PatternSet.cpp
 *
 * @brief Implementation of PatternSet class
 *
 * This applied an arbitrary number of regular expression transformations
 * either to make lines empty or to change them.
 *
 * This uses the CRegExp class from WinMerge Src\Common.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#include "stdafx.h"
#include <stdio.h>
#include "PatternSet.h"
#include "widestr.h"
#include "PluginError.h"
#include "resource.h"
#include "../../../Src/Common/RegExp.h"


PatternSet::PatternSet()
: m_ignoreRegexps(0)
, m_subRegexps(0)
, m_replaceStrings(0)
, m_ignoreLinesCount(0)
, m_substitutionsCount(0)
{
}

bool
PatternSet::loadPatterns(HINSTANCE hinst)
{
	clear();

	// Find ini file (just change .exe extension of dll to .ini)

	TCHAR inipath[MAX_PATH] = _T("");
	if (!GetModuleFileName(hinst, inipath, sizeof(inipath)/sizeof(inipath[0])))
		return false;

	int inilen = _tcslen(inipath);
	if (inilen < 5) return false;

	LPTSTR ext = &inipath[inilen-4];
	if (0 != _tcsicmp(ext, _T(".dll")))
	{
		PluginError(IDS_EXT_NOT_DLL);
		return false;
	}

	_tcscpy(ext, _T(".ini"));

	m_ignoreQuotes = !!GetPrivateProfileInt(_T("Options"), _T("IgnoreQuotes"), 1, inipath);

	m_ignoreLinesCount = GetPrivateProfileInt(_T("IgnoreLines"), _T("count"), 0, inipath);
	if (m_ignoreLinesCount<0 || m_ignoreLinesCount > 9999)
	{
		PluginErrorFmt(IDS_BAD_IGNORE_LINES_COUNT, m_ignoreLinesCount);
		m_ignoreLinesCount = 0;
		return false;
	}
	if (m_ignoreLinesCount)
	{
		m_ignoreRegexps = new CRegExp[m_ignoreLinesCount];
		for (int i=0; i<m_ignoreLinesCount; ++i)
		{
			TCHAR valuename[] = _T("line.9999");
			TCHAR linestr[512] = _T("");
			_stprintf(valuename, _T("line.%d"), i+1);
			if (!GetPrivateProfileString(_T("IgnoreLines"), valuename, _T(""), linestr, sizeof(linestr)/sizeof(linestr[0]), inipath))
			{
				PluginErrorFmt(IDS_MISSING_IGNORE_ENTRY, valuename);
				return false;
			}
			if (!m_ignoreRegexps[i].RegComp(linestr))
			{
				PluginErrorFmt(IDS_BAD_EXP, linestr);
				return false;
			}
		}
	}

	m_substitutionsCount = GetPrivateProfileInt(_T("Substitutions"), _T("count"), 0, inipath);
	if (m_substitutionsCount<0 || m_substitutionsCount > 9999)
	{
		PluginErrorFmt(IDS_BAD_SUBS_COUNT, m_substitutionsCount);
		m_substitutionsCount = 0;
		return false;
	}
	if (m_substitutionsCount)
	{
		m_subRegexps = new CRegExp[m_substitutionsCount];
		m_replaceStrings = new LPTSTR[m_substitutionsCount];
		for (int i=0; i<m_substitutionsCount; ++i)
		{
			m_replaceStrings[i] = 0;
		}
		for (i=0; i<m_substitutionsCount; ++i)
		{
			TCHAR valuename[] = _T("line.9999");
			TCHAR linestr[512] = _T("");
			_stprintf(valuename, _T("source.%d"), i+1);
			if (!GetPrivateProfileString(_T("Substitutions"), valuename, _T(""), linestr, sizeof(linestr)/sizeof(linestr[0]), inipath))
			{
				PluginErrorFmt(IDS_MISSING_SUBS_ENTRY, valuename);
				return false;
			}
			if (!m_subRegexps[i].RegComp(linestr))
			{
				PluginErrorFmt(IDS_BAD_EXP, linestr);
				return false;
			}

			_stprintf(valuename, _T("dest.%d"), i+1);
			if (GetPrivateProfileString(_T("Substitutions"), valuename, _T(""), linestr, sizeof(linestr)/sizeof(linestr[0]), inipath))
			{
				m_replaceStrings[i] = _tcsdup(linestr);
			}
			else
			{
				m_replaceStrings[i] = _tcsdup(_T(""));
			}

		}
	}

	return true;
}


PatternSet::~PatternSet()
{
	clear();
}

void
PatternSet::clear()
{
	if (m_ignoreRegexps)
	{
		delete[] m_ignoreRegexps;
		m_ignoreRegexps = 0;
	}
	if (m_subRegexps)
	{
		delete[] m_subRegexps;
		m_subRegexps = 0;
	}
	if (m_replaceStrings)
	{
		for (int i=0; i<m_substitutionsCount; ++i)
		{
			if (m_replaceStrings[i])
			{
				free(m_replaceStrings[i]);
				m_replaceStrings[i] = 0;
			}
		}
		delete [] m_replaceStrings;
		m_replaceStrings = 0;
	}
}

bool
PatternSet::processLine(widestr & wstr)
{
	USES_CONVERSION;

	for (int i=0; i<m_ignoreLinesCount; ++i)
	{
		LPCTSTR teststr = OLE2CT(wstr.string());
		CRegExp & prex = m_ignoreRegexps[i];
		
		if (prex.RegFind(teststr) >= 0)
		{
			wstr.makeEmpty();
			return true;
		}
	}

	bool matched=false;
	int j=0;
	for (i=0; i<m_substitutionsCount; ++i)
	{
		CRegExp & prex = m_subRegexps[i];
		LPCTSTR replpat = m_replaceStrings[i];
		
		LPCTSTR teststr = OLE2CT(wstr.string());
		int start = prex.RegFind(teststr);
		if (start >= 0)
		{
			int len = prex.GetFindLen();
			LPCTSTR replstr = prex.GetReplaceString(replpat);
			int newlen = wstr.length() - len + _tcslen(replstr);
			widestr wstr2(newlen);
			wstr2.set(wstr.string(), start);
			wstr2.append(T2COLE(replstr));
			LPCWSTR tail = wstr.mid(start+len);
			wstr2.append(tail);
			if (wcscmp(wstr.string(), wstr2.string()) != 0)
				matched = true;
			wstr.set(wstr2.string());
			if (j<15)
			{
				// retry this substitution
				++j;
				--i;
			}
			else
			{
				// Prevent more than 15 matches in one line
				// to prevent mistaken infinite loops
			}
		}
	}

	return matched;
}