#include "stdafx.h"
#include "Common.h"

constexpr int DISPID_Translate = 1;

BSTR WinMerge_Translate(IDispatch* pDispatch, BSTR bstrText)
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
