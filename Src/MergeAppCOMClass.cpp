#include "pch.h"
#include <windows.h>
#include "MergeAppCOMClass.h"
#include "OptionsMgr.h"
#include "MergeApp.h"
#include "Logger.h"
#include "resource.h"

MergeAppCOMClass::MergeAppCOMClass()
	: MyDispatch(static_cast<IMergeApp*>(this))
{
}

MergeAppCOMClass::~MergeAppCOMClass()
{
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::Translate(BSTR text, BSTR* pRet)
{
	*pRet = SysAllocString(I18n::tr(text).c_str());
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetOption(BSTR name, VARIANT varDefault, VARIANT* pRet)
{
	VARIANT varResult;
	VariantInit(&varResult);
	auto value = GetOptionsMgr()->Get(name);
	if (value.GetType() == varprop::VT_NULL)
	{
		const VARIANT* pvar = ((varDefault.vt & VT_BYREF) != 0) ? varDefault.pvarVal : &varDefault;
		switch (pvar->vt)
		{
		case VT_BOOL:
			GetOptionsMgr()->InitOption(name, pvar->boolVal);
			break;
		case VT_I2:
			GetOptionsMgr()->InitOption(name, pvar->iVal);
			break;
		case VT_I4:
			GetOptionsMgr()->InitOption(name, pvar->intVal);
			break;
		case VT_BSTR:
			GetOptionsMgr()->InitOption(name, pvar->bstrVal);
			break;
		default:
			*pRet = varResult;
			return S_OK;;
		}
		value = GetOptionsMgr()->Get(name);
	}
	switch (value.GetType())
	{
	case varprop::VT_BOOL:
		varResult.vt = VT_BOOL;
		varResult.boolVal = value.GetBool();
		break;
	case varprop::VT_INT:
		varResult.vt = VT_I4;
		varResult.intVal = value.GetInt();
		break;
	case varprop::VT_STRING:
		varResult.vt = VT_BSTR;
		varResult.bstrVal = SysAllocString(value.GetString().c_str());
		break;
	case varprop::VT_NULL:
		varResult.vt = VT_NULL;
		break;
	default:
		varResult.vt = VT_EMPTY;
		break;
	}
	*pRet = varResult;
	return S_OK;;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::SaveOption(BSTR name, VARIANT varValue)
{
	auto value = GetOptionsMgr()->Get(name);
	const VARIANT* pvar = ((varValue.vt & VT_BYREF) != 0) ? varValue.pvarVal : &varValue;
	if (value.GetType() == varprop::VT_NULL)
	{
		switch (varValue.vt)
		{
		case VT_BOOL:
			GetOptionsMgr()->InitOption(name, pvar->boolVal);
			break;
		case VT_I2:
			GetOptionsMgr()->InitOption(name, pvar->iVal);
			break;
		case VT_I4:
			GetOptionsMgr()->InitOption(name, pvar->intVal);
			break;
		case VT_BSTR:
			GetOptionsMgr()->InitOption(name, pvar->bstrVal);
			break;
		default:
			return S_OK;
		}
		value = GetOptionsMgr()->Get(name);
	}
	switch (value.GetType())
	{
	case varprop::VT_BOOL:
		if (pvar->vt == VT_BOOL)
			GetOptionsMgr()->SaveOption(name, pvar->boolVal);
		return S_OK;
	case varprop::VT_INT:
		if (pvar->vt == VT_I2)
			GetOptionsMgr()->SaveOption(name, pvar->iVal);
		else if (pvar->vt == VT_I4)
			GetOptionsMgr()->SaveOption(name, pvar->intVal);
		return S_OK;
	case varprop::VT_STRING:
		if (pvar->vt == VT_BSTR)
			GetOptionsMgr()->SaveOption(name, pvar->bstrVal);
		return S_OK;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::MsgBox(BSTR prompt, VARIANT varButtons, VARIANT varTitle, int* pRet)
{
	VARIANT varButtons2, varTitle2;
	VariantInit(&varButtons2);
	VariantInit(&varTitle2);
	if (FAILED(VariantChangeType(&varButtons2, &varButtons, 0, VT_I4)))
	{
		varButtons2.vt = VT_I4;
		varButtons2.intVal = 0;
	}
	if (FAILED(VariantChangeType(&varTitle2, &varTitle, 0, VT_BSTR)))
	{
		varTitle2.vt = VT_BSTR;
		varTitle2.bstrVal = SysAllocString(L"");
	}
	int ans = MessageBox(reinterpret_cast<HWND>(AppGetMainHWND()), prompt, varTitle2.bstrVal, varButtons2.intVal);
	VariantClear(&varButtons2);
	VariantClear(&varTitle2);
	*pRet = ans;
	return S_OK;
}

INT_PTR CALLBACK MergeAppCOMClass::InputBoxProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
	case WM_INITDIALOG:
	{
		SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
		auto* pThis = reinterpret_cast<MergeAppCOMClass*>(lParam);
		SetWindowText(hWnd, pThis->m_inputBoxTitle.c_str());
		SetDlgItemText(hWnd, IDC_INPUTBOX_PROMPT, pThis->m_inputBoxPrompt.c_str());
		SetDlgItemText(hWnd, IDC_INPUTBOX_EDIT, pThis->m_inputBoxText.c_str());
		return TRUE;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR value[2048]{};
			auto* pThis = reinterpret_cast<MergeAppCOMClass*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			GetDlgItemText(hWnd, IDC_INPUTBOX_EDIT, value, sizeof(value) / sizeof(value[0]));
			pThis->m_inputBoxText = value;
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

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::InputBox(BSTR prompt, VARIANT varTitle, VARIANT varDefault, BSTR* pRet)
{
	VARIANT varTitle2, varDefault2;
	VariantInit(&varTitle2);
	VariantInit(&varDefault2);
	if (FAILED(VariantChangeType(&varTitle2, &varTitle, 0, VT_BSTR)))
	{
		varTitle2.vt = VT_BSTR;
		varTitle2.bstrVal = SysAllocString(L"");
	}
	if (FAILED(VariantChangeType(&varDefault2, &varDefault, 0, VT_BSTR)))
	{
		varDefault2.vt = VT_BSTR;
		varDefault2.bstrVal = SysAllocString(L"");
	}
	m_inputBoxPrompt = prompt;
	m_inputBoxTitle = varTitle2.bstrVal;
	m_inputBoxText = varDefault2.bstrVal;
	VariantClear(&varTitle2);
	VariantClear(&varDefault2);
	INT_PTR ans = DialogBoxParam(nullptr, MAKEINTRESOURCE(IDD_INPUTBOX), reinterpret_cast<HWND>(AppGetMainHWND()), InputBoxProc, reinterpret_cast<LPARAM>(this));
	if (ans == IDOK)
	{
		*pRet = SysAllocString(m_inputBoxText.c_str());
		return S_OK;
	}
	*pRet = SysAllocString(L"");
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::LogError(BSTR text)
{
	RootLogger::Error(text);
	return S_OK;
}

