// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "WatchBeginningOfLog.h"
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
	*pVal = SysAllocString(L"Fast compare for log files : only first 1/10th");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	*pVal = SysAllocString(L"\\.log$");
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
	ifstream input(W2T(fileSrc), ios::in | ios::binary);
	ofstream output(W2T(fileDst), ios::out | ios::binary);

	input.seekg(0L, ios::end);
	int len = input.tellg();
	input.seekg(0L, ios::beg);

	char buffer[65536];

	len /= 10;
	while (len)
	{
		if (len > 65536)
			input.read(buffer, 65536);
		else
			input.read(buffer, len);
		output.write(buffer, input.gcount());
		len -= input.gcount();
	}

	input.close();
	output.close();

	*pbChanged = VARIANT_TRUE;
	*pbSuccess = VARIANT_TRUE;
	return S_OK;
}



STDMETHODIMP CWinMergeScript::PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT pSubcode, VARIANT_BOOL *pbSuccess)
{
	// always return error so the users knows we can not repack
	*pbChanged = VARIANT_FALSE;
	*pbSuccess = VARIANT_FALSE;
	return S_OK;
}
