// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "DisplayBinaryFiles.h"
#include "WinMergeScript.h"
#include "unicheck.h"
#include <fstream>
using namespace std;

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
	*pVal = SysAllocString(L"\\.exe$;\\.dll$;\\.ocx$");
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

	bool beginning=true;
	// Check for Unicode BOM (byte order mark)
	// Only matter if file has 3 or more bytes
	// Files with <3 bytes are empty if they are UCS-2
	unicodingInfo uninfo;
	while (len)
	{
		int curlen = len;
		if (curlen > 65536)
			curlen = 65536;
		// align on 4byte boundary, in case doing unicode encoding
		if (curlen > 4 && ( (curlen % 4) != 0))
			curlen -= (curlen % 4);
		input.read(buffer, curlen);
		int i=0;
		if (beginning)
		{
			if (CheckForBom(buffer, curlen, &uninfo))
				i += uninfo.bom_width;
			beginning = false;
		}
		char * p1 = (char *)buffer;
		short * p2 = (short *)buffer;
		int * p4 = (int *)buffer;
		for ( ; i < curlen ; i += uninfo.char_width)
		{
			if (i + (uninfo.char_width-1) < curlen)
			{
				int index = i/uninfo.char_width;
				if (uninfo.char_width == 1)
				{
					if (p1[index] == 0)
						p1[index] = 0x20;
				}
				else if (uninfo.char_width == 2)
				{
					if (p2[index] == 0)
						p2[index] = 0x20;
				}
				else // uninfo.char_width == 4
				{
					if (p4[index] == 0)
						p4[index] = 0x20;
				}
			}
		}

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
