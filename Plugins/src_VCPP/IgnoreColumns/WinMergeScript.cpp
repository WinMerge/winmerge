// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "IgnoreColumns.h"
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
	*pVal = SysAllocString(L"Ignore some columns - ignored columns list from the plugin name");
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

	// character position begins at 1 for user, but at 0 here
	int i;
	for (i = 0 ; i < nExcludedRanges ; i++)
	{
		aExcludedRanges[i][0] --;
		aExcludedRanges[i][1] --;
	}


	WCHAR * pDst = pBeginText;
	WCHAR * zoneBegin;
	WCHAR * lineBegin;

	for (zoneBegin = lineBegin = pBeginText; lineBegin < pEndText ; lineBegin = zoneBegin)
	{
		// next excluded range in the current line
		int nextExcludedRange;
		for (nextExcludedRange = 0 ; nextExcludedRange < nExcludedRanges ; nextExcludedRange ++)
		{
			// look for the end of the included zone
			WCHAR * zoneEnd = zoneBegin;
			WCHAR * zoneMaxEnd = lineBegin + aExcludedRanges[nextExcludedRange][0];
			while (zoneEnd < pEndText && zoneEnd < zoneMaxEnd &&
					*zoneEnd != L'\n' && *zoneEnd != L'\r')
				zoneEnd ++;

			// copy the characters of included columns
			wcsncpy(pDst, zoneBegin, zoneEnd - zoneBegin);
			pDst += zoneEnd - zoneBegin;

			// advance the cursor
			zoneBegin = zoneEnd;

			if (zoneEnd < zoneMaxEnd)
				break;

			// look for the end of the excluded zone
			zoneEnd = zoneBegin;
			zoneMaxEnd = lineBegin + aExcludedRanges[nextExcludedRange][1] + 1;
			while (zoneEnd < pEndText && zoneEnd < zoneMaxEnd &&
					*zoneEnd != L'\n' && *zoneEnd != L'\r')
				zoneEnd ++;

			// advance the cursor
			zoneBegin = zoneEnd;

			if (zoneEnd < zoneMaxEnd)
				break;
		}

		if (nextExcludedRange == nExcludedRanges)
		{
			// treat the trailing included zone

			// look for the end of the included zone
			WCHAR * zoneEnd = zoneBegin;
			while (zoneEnd < pEndText &&
					*zoneEnd != L'\n' && *zoneEnd != L'\r')
				zoneEnd ++;

			// copy the characters of included columns
			wcsncpy(pDst, zoneBegin, zoneEnd - zoneBegin);
			pDst += zoneEnd - zoneBegin;

			// advance the cursor
			zoneBegin = zoneEnd;
		}
		
		// keep possible EOL characters 
		WCHAR * eolEnd = zoneBegin;
		while (eolEnd < pEndText && 
			  (*eolEnd == L'\n' || *eolEnd == L'\r'))
		{
			eolEnd ++;
		}

		if (eolEnd > zoneBegin)
		{
			// copy the EOL characters
			wcsncpy(pDst, zoneBegin, eolEnd - zoneBegin);
			pDst += eolEnd - zoneBegin;
			// advance the cursor
			zoneBegin = eolEnd;
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
