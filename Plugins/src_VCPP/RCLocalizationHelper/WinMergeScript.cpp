// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "RCLocalizationHelper.h"
#include "WinMergeScript.h"
#include "widestr.h"
#include "PatternSet.h"

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"BUFFER_PREDIFF");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
	*pVal = SysAllocString(L"This plugins hides most language/layout differences");
	return S_OK;
}

// not used yet
STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	*pVal = SysAllocString(L"\\.rc$");
	return S_OK;
}

// not used yet
STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL *pVal)
{
	*pVal = VARIANT_TRUE;
	return S_OK;
}

/*
 * Find number of characters from start to end of current line, including start
 * Stop before maxlen characters
 */
static int
GetLineLength(const WCHAR * start, int maxlen)
{
	for (int i=0; (maxlen == -1 || i < maxlen); ++i)
	{
		if (start[i] == '\n' || start[i] == '\r' || start[i] == 0)
			return i;
	}
	return i;
}

STDMETHODIMP CWinMergeScript::PrediffBufferW(BSTR *pText, INT *pSize, VARIANT_BOOL *pbChanged, VARIANT_BOOL *pbHandled)
{
	HINSTANCE hinst = _Module.GetModuleInstance();

	PatternSet ps;
	if (!ps.loadPatterns(hinst))
	{
		*pbChanged = VARIANT_FALSE;
		*pbHandled = VARIANT_FALSE;
		return E_FAIL;
	}

	WCHAR * text = *pText;
	long nSize = *pSize;

	int iSrc=0, iDst=0;

	widestr wstr(512);

	// bQuoting is false when we're processing (& copying text to output)
	// it is true when we're inside a string constant (& not copying text to output)
	bool bQuoting = false;

	while (iSrc < nSize)
	{
		int linelen = GetLineLength(&text[iSrc], nSize - iSrc);
		if (linelen)
		{
			wstr.set(&text[iSrc], linelen);
			if (!bQuoting)
			{
				// Make any pattern changes
				// eg, skipping lines flagged for omission
				// or suppressing numbers (dialog positions)
				//  (codepage declarations, language declarations)
				ps.processLine(wstr);
			}
			for (int i=0; i<wstr.length(); ++i)
			{
				if (ps.shouldIgnoreQuotes() && wstr.at(i) == '"')
				{
					bQuoting = !bQuoting;
					continue; // include neither opening nor closing double quotes
				}
				if (!bQuoting)
					text[iDst++] = wstr.at(i);
			}
			iSrc += linelen;
		}
		// copy all line terminations, even inside quotes
		while (iSrc < nSize && (text[iSrc] == '\n' || text[iSrc] == '\r'))
		{
			text[iDst++] = text[iSrc];
			++iSrc;
		}
	}
	// set the new size
	*pSize = iDst;

	if (iDst == nSize)
		*pbChanged = VARIANT_FALSE;
	else
		*pbChanged = VARIANT_TRUE;

	*pbHandled = VARIANT_TRUE;
	return S_OK;
}
