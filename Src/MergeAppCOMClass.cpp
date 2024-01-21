#include "pch.h"
#include <Windows.h>
#include "MergeAppCOMClass.h"
#include "LanguageSelect.h"
#include "OptionsMgr.h"
#include "MergeApp.h"
#include "resource.h"

static PARAMDATA paramData_Translate[] =
{ { L"text", VT_BSTR }, };
static PARAMDATA paramData_GetOption[] =
{ { L"name", VT_BSTR }, { L"defaultValue", VT_VARIANT}, };
static PARAMDATA paramData_SaveOption[] =
{ { L"name", VT_BSTR }, { L"value", VT_VARIANT}, };
static PARAMDATA paramData_Log[] =
{ { L"level", VT_I4 }, { L"text", VT_BSTR }, };
static PARAMDATA paramData_MsgBox[] =
{ { L"prompt", VT_BSTR }, { L"buttons", VT_VARIANT}, { L"title", VT_VARIANT}, };
static PARAMDATA paramData_InputBox[] =
{ { L"prompt", VT_BSTR }, { L"title", VT_VARIANT}, { L"default", VT_VARIANT}, };
static METHODDATA methodData_MergeApp[] =
{
	{ L"Translate",  paramData_Translate,  DISPID_Translate,  3, CC_STDCALL, 1, DISPATCH_METHOD, VT_BSTR },
	{ L"GetOption",  paramData_GetOption,  DISPID_GetOption,  4, CC_STDCALL, 2, DISPATCH_METHOD, VT_VARIANT },
	{ L"SaveOption", paramData_SaveOption, DISPID_SaveOption, 5, CC_STDCALL, 2, DISPATCH_METHOD, VT_NULL },
	{ L"Log", paramData_Log, DISPID_Log, 6, CC_STDCALL, 2, DISPATCH_METHOD, VT_NULL },
	{ L"MsgBox", paramData_MsgBox, DISPID_MsgBox, 7, CC_STDCALL, 3, DISPATCH_METHOD, VT_I4 },
	{ L"InputBox", paramData_InputBox, DISPID_InputBox, 8, CC_STDCALL, 3, DISPATCH_METHOD, VT_BSTR },
};

static INTERFACEDATA idata_MergeApp = { methodData_MergeApp, static_cast<UINT>(std::size(methodData_MergeApp)) }; 

MergeAppCOMClass::MergeAppCOMClass()
	: MyDispatch(&idata_MergeApp, static_cast<IMergeApp*>(this))
{
}

MergeAppCOMClass::~MergeAppCOMClass()
{
}

BSTR STDMETHODCALLTYPE MergeAppCOMClass::Translate(BSTR text)
{
	return SysAllocString(tr(text).c_str());
}

VARIANT STDMETHODCALLTYPE MergeAppCOMClass::GetOption(BSTR name, VARIANT varDefault)
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
			return varResult;
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
	return varResult;
}

void STDMETHODCALLTYPE MergeAppCOMClass::SaveOption(BSTR name, VARIANT varValue)
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
			return;
		}
		value = GetOptionsMgr()->Get(name);
	}
	switch (value.GetType())
	{
	case varprop::VT_BOOL:
		if (pvar->vt == VT_BOOL)
			GetOptionsMgr()->SaveOption(name, pvar->boolVal);
		return;
	case varprop::VT_INT:
		if (pvar->vt == VT_I2)
		{
			GetOptionsMgr()->SaveOption(name, pvar->iVal);
			return;
		}
		if (pvar->vt == VT_I4)
		{
			GetOptionsMgr()->SaveOption(name, pvar->intVal);
			return;
		}
		return;
	case varprop::VT_STRING:
		if (pvar->vt == VT_BSTR)
		{
			GetOptionsMgr()->SaveOption(name, pvar->bstrVal);
			return;
		}
		return;
	}
	return;
}

void STDMETHODCALLTYPE MergeAppCOMClass::Log(int level, BSTR text)
{
	LogErrorString(text);
}

int STDMETHODCALLTYPE MergeAppCOMClass::MsgBox(BSTR prompt, VARIANT varButtons, VARIANT varTitle)
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
	return ans;
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

BSTR STDMETHODCALLTYPE MergeAppCOMClass::InputBox(BSTR prompt, VARIANT varTitle, VARIANT varDefault)
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
		return SysAllocString(m_inputBoxText.c_str());
	return SysAllocString(L"");
}

