// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "IgnoreFieldsTab.h"
#include "WinMergeScript.h"

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


/** 
 * @brief Get the name of the current dll
 */
LPTSTR GetDllFilename(LPTSTR name, int len)
{
	// careful for the last char, the doc does not give this detail
	name[len] = 0;
	GetModuleFileName(_Module.GetModuleInstance(), name, len-1);
	// find last backslash
	TCHAR * lastslash = _tcsrchr(name, _T('//'));
	if (lastslash == 0)		
		lastslash = name;
	else
		lastslash ++;
	TCHAR * lastslash2 = _tcsrchr(lastslash, _T('\\'));
	if (lastslash2 == 0)		
		lastslash2 = name;
	else
		lastslash2 ++;
	if (lastslash2 != name)
		strcpy(name, lastslash2);
	return name;
}


int CreateArrayFromFilename(int (* value)[2])
{
	TCHAR name[256+1];
	GetDllFilename(name, 256);

	// first pass : prepare the chunks
	int nValue = 0;
	TCHAR * token = _tcstok(name, _T(",_"));
	while( token != NULL )
	{
		nValue ++;
		/* Get next token: */
		token = _tcstok( NULL, _T(",_") );
	}

	// pass first chunk 
	nValue --;

	if (value == 0)
		// just return the number of values
		return nValue;

	if (nValue == 0)
		return nValue;

	token = name + _tcslen(name) + 1;	// pass first chunk 
	int i;
	for (i = 0 ; i < nValue ; i++)
	{
		value[i][0] = _tcstol(token, &token, 10);
		while (*token != 0 && !_istdigit(*token))
			token ++;
		if (token[0] == 0)
		{
			value[i][1] = value[i][0];
		}
		else
		{
			value[i][1] = _tcstol(token, &token, 10);
		}
		token = token + _tcslen(token) + 1;
	}

	return nValue;
}


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"BUFFER_PREDIFF");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
	*pVal = SysAllocString(L"Ignore some fields - ignored fields list from the plugin name");
	return S_OK;
}

// not used yet
STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	*pVal = SysAllocString(L"\\.txt$");
	return S_OK;
}

// not used yet
STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL *pVal)
{
	*pVal = VARIANT_TRUE;
	return S_OK;
}


STDMETHODIMP CWinMergeScript::PrediffBufferW(BSTR *pText, INT *pSize, VARIANT_BOOL *pbChanged, VARIANT_BOOL *pbHandled)
{
	WCHAR * pBeginText = *pText;
	long nSize = *pSize;
	WCHAR * pEndText = pBeginText + nSize;


	int nExcludedRanges = CreateArrayFromFilename(NULL);
	int (* aExcludedRanges)[2] = new int[nExcludedRanges][2];
	if (aExcludedRanges == NULL)
		nExcludedRanges = 0;
	else
		nExcludedRanges = CreateArrayFromFilename(aExcludedRanges);

	if (nExcludedRanges == 0)
	{
		*pbChanged = VARIANT_FALSE;
		*pbHandled = VARIANT_TRUE;
		return S_OK;
	}

	// first column is 1 for user, but is 0 here
	int i;
	for (i = 0 ; i < nExcludedRanges ; i++)
	{
		aExcludedRanges[i][0] --;
		aExcludedRanges[i][1] --;
	}


	WCHAR * pDst = pBeginText;
	WCHAR * columnBegin;

	// change this to change the delimiter between fields
	const WCHAR delimiter = L'\t';

	// order of the column in the current line
	int iColumn = 0;
	// next excluded column range in the current line
	int nextExcludedRange = 0;

	for (columnBegin = pBeginText; columnBegin < pEndText ; )
	{

		// search for the end of the column (columnEnd = first excluded character)
		WCHAR * columnEnd = columnBegin;
		while (columnEnd < pEndText &&
				*columnEnd != L'\n' &&
				*columnEnd != L'\r' &&
				*columnEnd != delimiter)
			columnEnd ++;

		// determine the status of this column
		if (nextExcludedRange < nExcludedRanges && iColumn > aExcludedRanges[nextExcludedRange][1])
			nextExcludedRange ++;
		BOOL bIsColumnIncluded = TRUE;
		if (nextExcludedRange < nExcludedRanges && iColumn >= aExcludedRanges[nextExcludedRange][0])
			bIsColumnIncluded = FALSE;

		// copy the characters of included columns
		if (bIsColumnIncluded)
		{
			wcsncpy(pDst, columnBegin, columnEnd - columnBegin);
			pDst += columnEnd - columnBegin;
		}

		// advance the cursor
		columnBegin = columnEnd;

		// keep possible tabulation (only one)
		if (columnBegin < pEndText)
			if (*columnBegin == delimiter)
			{
				// copy the tabulation
				*pDst = delimiter;
				pDst ++;
				// advance the cursor
				columnBegin ++;
				// next column
				iColumn ++;
			}

		// keep possible EOL characters 
		WCHAR * eolEnd = columnBegin;
		while (eolEnd < pEndText && (*eolEnd == L'\n' || *eolEnd == L'\r'))
		{
			eolEnd ++;
		}

		if (eolEnd > columnBegin)
		{
			// copy the EOL characters
			wcsncpy(pDst, columnBegin, eolEnd - columnBegin);
			pDst += eolEnd - columnBegin;
			// advance the cursor
			columnBegin = eolEnd;
			// reset the column counter
			iColumn = 0;
			nextExcludedRange = 0;
		}
	}


	delete [] aExcludedRanges;

	// set the new size
	*pSize = pDst - pBeginText;

	if (*pSize == nSize)
		*pbChanged = VARIANT_FALSE;
	else
		*pbChanged = VARIANT_TRUE;

	*pbHandled = VARIANT_TRUE;
	return S_OK;
}
