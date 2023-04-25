#include "StdAfx.h"
#include "MergeAppCOMClass.h"
#include "LanguageSelect.h"
#include "OptionsMgr.h"
#include "MergeApp.h"

static PARAMDATA paramData_Translate[] =
{ { L"text", VT_BSTR, }, };
static PARAMDATA paramData_GetOption[] =
{ { L"name", VT_BSTR }, { L"defaultValue", VT_VARIANT|VT_BYREF }, };
static PARAMDATA paramData_SaveOption[] =
{ { L"name", VT_BSTR }, { L"value", VT_VARIANT|VT_BYREF }, };
static METHODDATA methodData_MergeApp[] =
{
	{ L"Translate",  paramData_Translate,  DISPID_Translate,  3, CC_STDCALL, 1, DISPATCH_METHOD, VT_BSTR },
	{ L"GetOption",  paramData_GetOption,  DISPID_GetOption,  4, CC_STDCALL, 2, DISPATCH_METHOD, VT_VARIANT },
	{ L"SaveOption", paramData_SaveOption, DISPID_SaveOption, 5, CC_STDCALL, 2, DISPATCH_METHOD, VT_NULL },
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

VARIANT STDMETHODCALLTYPE MergeAppCOMClass::GetOption(BSTR name, const VARIANT& varDefault)
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

void STDMETHODCALLTYPE MergeAppCOMClass::SaveOption(BSTR name, const VARIANT& varValue)
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
