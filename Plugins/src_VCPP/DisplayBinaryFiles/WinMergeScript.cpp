// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "DisplayBinaryFiles.h"
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
	*pVal = SysAllocString(L"Transform a binary file so WinMerge can display it - save not possible");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	*pVal = SysAllocString(L"\\.exe$;\\.dll$;\\.ocx$;\\.doc$;\\.xls$");
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

	while (len)
	{
		int curlen = len;
		if (curlen > 65536)
			curlen = 65536;
		input.read(buffer, curlen);
		for (int i = 0 ; i < curlen ; i++)
			if (buffer[i] == 0)
				buffer[i] = 0x20;
		output.write(buffer, curlen);
		len -= curlen;
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
