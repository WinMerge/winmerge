// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "RCLocalizationHelper.h"
#include "WinMergeScript.h"

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"PREDIFFING");
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

STDMETHODIMP CWinMergeScript::DiffingPreprocessW(BSTR *pText, INT *pSize, VARIANT_BOOL *pbChanged, VARIANT_BOOL *pbHandled)
{
	WCHAR * text = *pText;
	long nSize = *pSize;

	int iSrc, iDst;
	int bPrend = 1;
	for (iSrc = 0, iDst = 0 ; iSrc < nSize ; iSrc++)
	{
		if (text[iSrc] == L'"')
		{
			bPrend = 1 - bPrend;
			continue;
		}
		if (text[iSrc] == L'\n' || text[iSrc] == L'\r')
		{
			bPrend = 1;
			text[iDst ++] = text[iSrc];
			continue;
		}
		if (text[iSrc] >= L'0' && text[iSrc] <= L'9')
		{
			if (iDst == 0 || iswspace(text[iDst-1]) || text[iDst-1] == L',')
			{
				if (text[iSrc] == L'0' && iSrc+1 < nSize && text[iSrc+1] == L'x')
					iSrc += 2;
				while (iSrc < nSize && (text[iSrc] >= L'0' && text[iSrc] <= L'9'))
					iSrc ++;
				iSrc --;
				continue;
			}
		}

		if (bPrend)
			text[iDst ++] = text[iSrc];
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





STDMETHODIMP CWinMergeScript::DiffingPreprocessA(SAFEARRAY **pBuffer, INT *pSize, VARIANT_BOOL *pbChanged, VARIANT_BOOL *pbHandled)
{
	// Now lock the array for editing and get a pointer to the raw elements
	char *text; 
	SafeArrayAccessData(*pBuffer, (void**)&text);

	long nSize = *pSize;

	int iSrc, iDst;
	int bPrend = 1;
	for (iSrc = 0, iDst = 0 ; iSrc < nSize ; iSrc++)
	{
		if (text[iSrc] == '"')
		{
			bPrend = 1 - bPrend;
			continue;
		}
		if (text[iSrc] == '\n' || text[iSrc] == '\r')
		{
			bPrend = 1;
			text[iDst ++] = text[iSrc];
			continue;
		}
		if (text[iSrc] >= '0' && text[iSrc] <= '9')
		{
			if (iDst == 0 || iswspace(text[iDst-1]) || text[iDst-1] == ',')
			{
				if (text[iSrc] == '0' && iSrc+1 < nSize && text[iSrc+1] == 'x')
					iSrc += 2;
				while (iSrc < nSize && (text[iSrc] >= '0' && text[iSrc] <= '9'))
					iSrc ++;
				iSrc --;
				continue;
			}
		}

		if (bPrend)
			text[iDst ++] = text[iSrc];
	}

	// Unlock the array
	SafeArrayUnaccessData(*pBuffer);

	// set the new size
	*pSize = iDst;

	if (iDst == nSize)
		*pbChanged = VARIANT_FALSE;
	else
		*pbChanged = VARIANT_TRUE;

	*pbHandled = VARIANT_TRUE;
	return S_OK;
}
