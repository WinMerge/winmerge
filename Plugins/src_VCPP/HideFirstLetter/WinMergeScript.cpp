// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "HideFirstLetter.h"
#include "WinMergeScript.h"
#include <fstream.h>

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"FILE_PACK_UNPACK");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
	*pVal = SysAllocString(L"This plugins hides the first non-space character");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	*pVal = SysAllocString(L"\\.nofirst$;\\.hidefirst$");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL *pVal)
{
	*pVal = VARIANT_TRUE;
	return S_OK;
}


STDMETHODIMP CWinMergeScript::UnpackBufferA(SAFEARRAY **pBuffer, INT *pSize, VARIANT_BOOL *pbChanged, INT *pSubcode, VARIANT_BOOL *pbSuccess)
{
	// We don't need it
	return S_OK;
}

STDMETHODIMP CWinMergeScript::PackBufferA(SAFEARRAY **pBuffer, INT *pSize, VARIANT_BOOL *pbChanged, INT subcode, VARIANT_BOOL *pbSuccess)
{
	// We don't need it
	return S_OK;
}

STDMETHODIMP CWinMergeScript::UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT *pSubcode, VARIANT_BOOL *pbSuccess)
{
	USES_CONVERSION;
	ifstream input(W2T(fileSrc), ios::in || ios::binary);
	ofstream output(W2T(fileDst));

	*pSubcode = -1;

	int pos = 0;
	while(1)
	{
		char c = input.get();
		if (!input.good())
			break;
		if (! isspace(c))
		{
			// memorize value and position of the first non-space character
			*pSubcode = pos*0x100 + (int) c;
			break;
		}
		output.put(c);
		pos ++;
	};

	while(1)
	{
		char c = input.get();
		if (!input.good())
			break;
		output.put(c);
	};

	input.close();
	output.close();

	*pbChanged = VARIANT_TRUE;
	*pbSuccess = VARIANT_TRUE;
	return S_OK;
}



STDMETHODIMP CWinMergeScript::PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT pSubcode, VARIANT_BOOL *pbSuccess)
{
	USES_CONVERSION;
	ifstream input(W2T(fileSrc), ios::in || ios::binary);
	ofstream output(W2T(fileDst));

	if (pSubcode == -1)
	{
		*pbChanged = VARIANT_FALSE;
		*pbSuccess = VARIANT_TRUE;
		return S_OK;
	}

	int pos = 0;
	while (pos < (pSubcode/0x100))
	{
		char c = input.get();
		if (!input.good())
			break;
		output.put(c);
		pos ++;
	}

	output.put((char) pSubcode);

	while (1)
	{
		char c = input.get();
		if (!input.good())
			break;
		output.put(c);
	}

	input.close();
	output.close();

	*pbChanged = VARIANT_TRUE;
	*pbSuccess = VARIANT_TRUE;
	return S_OK;
}
