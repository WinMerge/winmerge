// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "EditBinaryFiles.h"
#include "WinMergeScript.h"
#include "Unpacker.h"
#include "Common.h"

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"FILE_PACK_UNPACK");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
	*pVal = SysAllocString(L"Transform a binary file for editing with WinMerge");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	// There are not natural extensions for this plugin
	*pVal = SysAllocString(L"");
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
	LPCWSTR srcFilepath = fileSrc;
	LPCWSTR destFilepath = fileDst;

	*pbChanged = VARIANT_FALSE;
	*pbSuccess = VARIANT_FALSE;

	if (Unpack(srcFilepath, destFilepath, m_pMergeApp))
	{
		*pbChanged = VARIANT_TRUE;
		*pbSuccess = VARIANT_TRUE;
	}

	return S_OK;
}


STDMETHODIMP CWinMergeScript::PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT pSubcode, VARIANT_BOOL *pbSuccess)
{
	LPCWSTR srcFilepath = fileSrc;
	LPCWSTR destFilepath = fileDst;

	*pbChanged = VARIANT_FALSE;
	*pbSuccess = VARIANT_FALSE;

	if (Pack(srcFilepath, destFilepath, m_pMergeApp))
	{
		*pbChanged = VARIANT_TRUE;
		*pbSuccess = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CWinMergeScript::ShowSettingsDialog(VARIANT_BOOL *pbHandled)
{
	*pbHandled = VARIANT_FALSE;
	return E_NOTIMPL;
}

STDMETHODIMP CWinMergeScript::PluginOnEvent(int iEventType, IDispatch* pDispatch)
{
	m_pMergeApp = pDispatch;
	return S_OK;
}
