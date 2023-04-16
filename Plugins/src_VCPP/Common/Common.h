#pragma once

#include <atlbase.h>
#include <atlstr.h>

HRESULT MergeApp_GetOption(IDispatch* pDispatch, BSTR bstrName, VARIANT& varDefault, VARIANT* pvarResult);
CString MergeApp_GetOptionString(IDispatch* pDispatch, const CString& sName, const CString& sDefault);
HRESULT MergeApp_SaveOption(IDispatch* pDispatch, BSTR bstrName, VARIANT& varValue);
HRESULT MergeApp_SaveOptionString(IDispatch* pDispatch, const CString& sName, const CString& sValue);
BSTR MergeApp_Translate(IDispatch* pDispatch, BSTR bstrText);
void TranslateDialog(HWND h, IDispatch* pDispatch);