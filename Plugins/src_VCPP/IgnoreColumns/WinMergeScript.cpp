// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "IgnoreColumns.h"
#include "WinMergeScript.h"
#include "Common.h"
#include "resource.h"
#include <atlstr.h>

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
	TCHAR * lastslash = _tcsrchr(name, '/');
	if (lastslash == 0)		
		lastslash = name;
	else
		lastslash ++;
	TCHAR * lastslash2 = _tcsrchr(lastslash, '\\');
	if (lastslash2 == 0)		
		lastslash2 = name;
	else
		lastslash2 ++;
	if (lastslash2 != name)
		lstrcpy(name, lastslash2);
	return name;
}

CString KeyName()
{
	TCHAR szKeyName[256];
	TCHAR name[256+1];
	GetDllFilename(name, 256);
	lstrcpy(szKeyName, _T("Plugins\\"));
	lstrcat(szKeyName, name);
	return szKeyName;
}

int CreateArrayFromRangeString(const TCHAR *rangestr, int (* value)[2])
{
	TCHAR name[256];

	lstrcpy(name, rangestr);

	if (name[0] == 0)
		return 0;

	// first pass : prepare the chunks
	int nValue = 0;
	TCHAR * token = _tcstok(name, _T(",_"));
	while( token != NULL )
	{
		nValue ++;
		/* Get next token: */
		token = _tcstok( NULL, _T(",_") );
	}

	if (value == 0)
		// just return the number of values
		return nValue;

	token = name;
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

CString CreateRangeStringFromArray(int nExcludedRanges, const int aExcludedRanges[][2])
{
	CString rangestr = _T("");
	for (int i = 0; i < nExcludedRanges; ++i)
	{
		TCHAR value[256];
		if (aExcludedRanges[i][0] > 0 && aExcludedRanges[i][1] > 0)
		{
			if (aExcludedRanges[i][0] == aExcludedRanges[i][1])
			{
				wsprintf(value, _T("%d"), aExcludedRanges[i][0]);
				rangestr += value;
			}
			else
			{
				wsprintf(value, _T("%d-%d"), aExcludedRanges[i][0], aExcludedRanges[i][1]);
				rangestr += value;
			}
			rangestr += _T(",");
		}
	}
	if (!rangestr.IsEmpty())
		rangestr.Delete(rangestr.GetLength() - 1);

	return rangestr;
}

CString CWinMergeScript::GetColumnRangeString()
{
	TCHAR name[256+1];
	GetDllFilename(name, 256);
	CString rangestr;
	TCHAR * token = _tcspbrk(name, _T(",_"));
	if (!token)
		rangestr = MergeApp_GetOptionString(m_pMergeApp, KeyName() + _T("/ColumnRanges"), _T(""));
	else
		rangestr = token + 1;
	
	int nExcludedRanges = CreateArrayFromRangeString(rangestr, NULL);
	int (* aExcludedRanges)[2] = new int[nExcludedRanges][2];
	if (aExcludedRanges == NULL)
		nExcludedRanges = 0;
	else
		nExcludedRanges = CreateArrayFromRangeString(rangestr, aExcludedRanges);

	rangestr = CreateRangeStringFromArray(nExcludedRanges, aExcludedRanges);

	delete[] aExcludedRanges;

	return rangestr;
}


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"BUFFER_PREDIFF");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
	*pVal = SysAllocString(L"Ignore some columns - ignored columns list from the plugin name or the plugin argument");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	*pVal = SysAllocString(L"\\.txt$");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL *pVal)
{
	*pVal = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginArguments(BSTR *pVal)
{
	*pVal = m_bstrArguments.Copy();
	return S_OK;
}

STDMETHODIMP CWinMergeScript::put_PluginArguments(BSTR val)
{
	m_bstrArguments = val;
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginExtendedProperties(BSTR *pVal)
{
	*pVal = SysAllocString(L"MenuCaption=Ignore Columns");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::PrediffBufferW(BSTR *pText, INT *pSize, VARIANT_BOOL *pbChanged, VARIANT_BOOL *pbHandled)
{
	WCHAR * pBeginText = *pText;
	long nSize = *pSize;
	WCHAR * pEndText = pBeginText + nSize;

	int argc = 0;
	wchar_t** argv = CommandLineToArgvW(m_bstrArguments.m_str, &argc);
	CString rangestr = (m_bstrArguments.Length() > 0 && argc > 0) ? argv[0] : GetColumnRangeString();
	if (argv)
		LocalFree(argv);

	int nExcludedRanges = CreateArrayFromRangeString(rangestr, NULL);
	int (* aExcludedRanges)[2] = new int[nExcludedRanges][2];
	if (aExcludedRanges == NULL)
		nExcludedRanges = 0;
	else
		nExcludedRanges = CreateArrayFromRangeString(rangestr, aExcludedRanges);

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
	*pSize = static_cast<int>(pDst - pBeginText);

	if (*pSize == nSize)
		*pbChanged = VARIANT_FALSE;
	else
		*pbChanged = VARIANT_TRUE;

	*pbHandled = VARIANT_TRUE;
	return S_OK;
}

INT_PTR CALLBACK CWinMergeScript::DlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg) {
	case WM_INITDIALOG:
	{
		SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
		auto* pWinMergeScript = reinterpret_cast<CWinMergeScript*>(lParam);
		TranslateDialog(hWnd, pWinMergeScript->m_pMergeApp);
		SetDlgItemText(hWnd, IDC_EDIT1, pWinMergeScript->GetColumnRangeString());
		return TRUE;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR value[512]{};
			GetDlgItemText(hWnd, IDC_EDIT1, value, sizeof(value)/sizeof(TCHAR));
			auto* pWinMergeScript = reinterpret_cast<CWinMergeScript*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			MergeApp_SaveOptionString(pWinMergeScript->m_pMergeApp, KeyName() + _T("/ColumnRanges"), value);
			EndDialog(hWnd, IDOK);
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, IDCANCEL);
		}
		return TRUE;
		break;

	default:
		break;
	}
	return FALSE;
}

STDMETHODIMP CWinMergeScript::ShowSettingsDialog(VARIANT_BOOL *pbHandled)
{
	*pbHandled = 
		(DialogBoxParam(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc,
			reinterpret_cast<LPARAM>(this)) == IDOK);
	return S_OK;
}

STDMETHODIMP CWinMergeScript::PluginOnEvent(int iEventType, IDispatch* pDispatch)
{
	m_pMergeApp = pDispatch;
	return S_OK;
}
