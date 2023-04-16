#include "stdafx.h"
#include "Common.h"
#include <atlstr.h>
#include <utility>

constexpr int DISPID_Translate = 1;
constexpr int DISPID_GetOption = 2;
constexpr int DISPID_SaveOption = 3;

static std::pair<CString, CString> SplitKeyValueName(BSTR bstrName)
{
	CString key = _T("Software\\Thingamahoochie\\WinMerge\\");
	CString valueName;
	for (const wchar_t* p = bstrName; *p; ++p)
	{
		if (*p == '/')
		{
			key += CString(bstrName, static_cast<int>(p - bstrName));
			valueName = p + 1;
			break;
		}
	}
	return std::make_pair(key, valueName);
}

HRESULT MergeApp_GetOption(IDispatch* pDispatch, BSTR bstrName, VARIANT& varDefault, VARIANT* pvarResult)
{
	VariantClear(pvarResult);
	if (!pDispatch)
	{
		HRESULT hr = VariantCopy(pvarResult, &varDefault);
		if (FAILED(hr))
			return hr;
		auto key = SplitKeyValueName(bstrName);
		CRegKey reg;
		if (reg.Open(HKEY_CURRENT_USER, key.first, KEY_READ) != ERROR_SUCCESS)
			return S_OK;
		DWORD dwType = 0;
		ULONG nBytes = 0;
		if (reg.QueryValue(key.second, &dwType, nullptr, &nBytes) != ERROR_SUCCESS)
			return S_OK;
		BYTE* pValue = new BYTE[nBytes];
		reg.QueryValue(key.second, &dwType, pValue, &nBytes);
		switch (dwType)
		{
		case REG_DWORD:
			pvarResult->vt = VT_I4;
			pvarResult->intVal = *reinterpret_cast<DWORD*>(pValue);
			break;
		case REG_SZ:
			pvarResult->vt = VT_BSTR;
			pvarResult->bstrVal = SysAllocString(reinterpret_cast<wchar_t*>(pValue));
			break;
		}
		delete[] pValue;
		return S_OK;
	}
	VARIANT varName{ VT_BSTR };
	varName.bstrVal = bstrName;
	VARIANT varDefaultRef;
	varDefaultRef.vt = VT_VARIANT | VT_BYREF;
	varDefaultRef.pvarVal = &varDefault;
	CComDispatchDriver drv(pDispatch);
	return drv.Invoke2(DISPID_GetOption, &varName, &varDefaultRef, pvarResult);
}

CString MergeApp_GetOptionString(IDispatch* pDispatch, const CString& sName, const CString& sDefault)
{
	CComBSTR bstrName{ sName };
	CComVariant varDefault{ sDefault };
	CComVariant varResult;
	MergeApp_GetOption(pDispatch, bstrName, varDefault, &varResult);
	varResult.ChangeType(VT_BSTR);
	return varResult.bstrVal;
}

HRESULT MergeApp_SaveOption(IDispatch* pDispatch, BSTR bstrName, VARIANT& varValue)
{
	if (!pDispatch)
	{
		auto key = SplitKeyValueName(bstrName);
		CRegKey reg;
		if (reg.Create(HKEY_CURRENT_USER, key.first, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WRITE) != ERROR_SUCCESS)
			return E_INVALIDARG;
		ULONG nBytes = 0;
		const VARIANT* pvar = ((varValue.vt & VT_BYREF) != 0) ? varValue.pvarVal : &varValue;
		switch (pvar->vt)
		{
		case VT_BOOL:
			reg.SetDWORDValue(key.second, pvar->boolVal);
			break;
		case VT_I2:
			reg.SetDWORDValue(key.second, pvar->iVal);
			break;
		case VT_I4:
			reg.SetDWORDValue(key.second, pvar->intVal);
			break;
		case VT_BSTR:
			reg.SetStringValue(key.second, pvar->bstrVal);
			break;
		}
		return S_OK;
	}
	VARIANT varName{ VT_BSTR };
	VARIANT varValueRef{ VT_VARIANT | VT_BYREF };
	varName.bstrVal = bstrName;
	varValueRef.pvarVal = &varValue;
	CComDispatchDriver drv(pDispatch);
	return drv.Invoke2(DISPID_SaveOption, &varName, &varValueRef);
}

BSTR MergeApp_Translate(IDispatch* pDispatch, BSTR bstrText)
{
	if (!pDispatch)
		return SysAllocString(bstrText);
	VARIANT varResult;
	VARIANT varText{ VT_BSTR };
	varText.bstrVal = bstrText;
	VariantInit(&varResult);
	CComDispatchDriver drv(pDispatch);
	HRESULT hr = drv.Invoke1(DISPID_Translate, &varText, &varResult);
	if (FAILED(hr))
		return SysAllocString(bstrText);
	return varResult.bstrVal;
}

HRESULT MergeApp_SaveOptionString(IDispatch* pDispatch, const CString& sName, const CString& sValue)
{
	CComBSTR bstrName{ sName };
	CComVariant varValue{ sValue };
	return MergeApp_SaveOption(pDispatch, bstrName, varValue);
}

void TranslateDialog(HWND h, IDispatch* pDispatch)
{
	if (!pDispatch)
		return;
	CComDispatchDriver drv(pDispatch);
	UINT gw = GW_CHILD;
	do
	{
		constexpr size_t textbuf_size = 512;
		wchar_t text[textbuf_size];
		::GetWindowTextW(h, text, textbuf_size);
		CComVariant varResult;
		CComVariant varText(text);
		if (SUCCEEDED(drv.Invoke1(DISPID_Translate, &varText, &varResult)))
			::SetWindowTextW(h, varResult.bstrVal);
		h = ::GetWindow(h, gw);
		gw = GW_HWNDNEXT;
	} while (h != nullptr);
}
