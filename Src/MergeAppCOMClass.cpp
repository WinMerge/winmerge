#include "StdAfx.h"
#include "MergeAppCOMClass.h"
#include "LanguageSelect.h"
#include "OptionsMgr.h"
#include "TypeInfo.h"
#include "MergeApp.h"

static PARAMDATA paramData_Translate[] =
{ {L"text", VT_BSTR}, };
static PARAMDATA paramData_GetOption[] =
{ {L"name", VT_BSTR}, {L"defaultValue", VT_VARIANT}, };
static PARAMDATA paramData_SaveOption[] =
{ {L"name", VT_BSTR}, {L"value", VT_VARIANT}, };
static METHODDATA methodData_MergeApp[] =
{
	{ L"Translate",  paramData_Translate,  DISPID_Translate,  0, CC_STDCALL, 1, DISPATCH_METHOD, VT_BSTR },
	{ L"GetOption",  paramData_GetOption,  DISPID_GetOption,  1, CC_STDCALL, 2, DISPATCH_METHOD, VT_VARIANT },
	{ L"SaveOption", paramData_SaveOption, DISPID_SaveOption, 2, CC_STDCALL, 2, DISPATCH_METHOD, VT_VOID },
};

MyDispatch::MyDispatch(METHODDATA* pMethodData, size_t methodDataCount)
	: m_cRef(0)
	, m_pTypeInfo(new MyTypeInfo(pMethodData, methodDataCount))
{
	m_pTypeInfo->AddRef();
}

MyDispatch::~MyDispatch()
{
	if (m_pTypeInfo)
		m_pTypeInfo->Release();
}

HRESULT STDMETHODCALLTYPE MyDispatch::QueryInterface(REFIID riid, void** ppvObject)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE MyDispatch::AddRef(void)
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG STDMETHODCALLTYPE MyDispatch::Release(void)
{
	ULONG ulRefCount = InterlockedDecrement(&m_cRef);
	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}
	return ulRefCount;
}

HRESULT STDMETHODCALLTYPE MyDispatch::GetTypeInfoCount(UINT* pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyDispatch::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	*ppTInfo = m_pTypeInfo;
	(*ppTInfo)->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyDispatch::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	return m_pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

MergeAppCOMClass::MergeAppCOMClass()
	: MyDispatch(methodData_MergeApp, std::size(methodData_MergeApp))
{
}

MergeAppCOMClass::~MergeAppCOMClass()
{
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	if (!pDispParams)
		return DISP_E_BADVARTYPE;
	HRESULT hr = E_NOTIMPL;
	if (wFlags & DISPATCH_METHOD)
	{
		switch (dispIdMember)
		{
		case DISPID_Translate:
		{
			if (pDispParams->rgvarg[0].vt != VT_BSTR)
				return E_INVALIDARG;
			BSTR text = pDispParams->rgvarg[0].bstrVal;
			BSTR* pbstrResult = &pVarResult->bstrVal;
			hr = Translate(text, pbstrResult);
			pVarResult->vt = VT_BSTR;
			break;
		}
		case DISPID_GetOption:
		{
			if (pDispParams->rgvarg[1].vt != VT_BSTR && pDispParams->rgvarg[1].vt != (VT_BYREF | VT_VARIANT) && pDispParams->rgvarg[1].pvarVal->vt != VT_BSTR)
				return E_INVALIDARG;
			BSTR name = (pDispParams->rgvarg[1].vt & VT_BYREF) ? pDispParams->rgvarg[1].pvarVal->bstrVal : pDispParams->rgvarg[1].bstrVal;
			hr = GetOption(name, pDispParams->rgvarg[0], pVarResult);
			break;
		}
		case DISPID_SaveOption:
		{
			if (pDispParams->rgvarg[1].vt != VT_BSTR && pDispParams->rgvarg[1].vt != (VT_BYREF | VT_VARIANT) && pDispParams->rgvarg[1].pvarVal->vt != VT_BSTR)
				return E_INVALIDARG;
			BSTR name = (pDispParams->rgvarg[1].vt & VT_BYREF) ? pDispParams->rgvarg[1].pvarVal->bstrVal : pDispParams->rgvarg[1].bstrVal;
			hr = SaveOption(name, pDispParams->rgvarg[0]);
			break;
		}
		default:
			break;
		}
	}
	else if (wFlags & DISPATCH_PROPERTYGET)
	{
	}
	else if (wFlags & DISPATCH_PROPERTYPUT)
	{
	}
	if (hr == DISP_E_EXCEPTION && pExcepInfo)
	{
		IErrorInfo* pErrorInfo = nullptr;
		GetErrorInfo(0, &pErrorInfo);
		pErrorInfo->GetDescription(&pExcepInfo->bstrDescription);
		pErrorInfo->GetSource(&pExcepInfo->bstrSource);
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::Translate(BSTR text, BSTR* pbstrResult)
{
	*pbstrResult = SysAllocString(tr(text).c_str());
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetOption(BSTR name, const VARIANT& varDefault, VARIANT* pvarResult)
{
	VariantClear(pvarResult);
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
			return E_INVALIDARG;
		}
		value = GetOptionsMgr()->Get(name);
	}
	switch (value.GetType())
	{
	case varprop::VT_BOOL:
		pvarResult->vt = VT_BOOL;
		pvarResult->boolVal = value.GetBool();
		break;
	case varprop::VT_INT:
		pvarResult->vt = VT_INT;
		pvarResult->intVal = value.GetInt();
		break;
	case varprop::VT_STRING:
		pvarResult->vt = VT_BSTR;
		pvarResult->bstrVal = SysAllocString(value.GetString().c_str());
		break;
	case varprop::VT_NULL:
		pvarResult->vt = VT_NULL;
		break;
	default:
		pvarResult->vt = VT_EMPTY;
		break;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::SaveOption(BSTR name, const VARIANT& varValue)
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
			return E_INVALIDARG;
		}
		value = GetOptionsMgr()->Get(name);
	}
	switch (value.GetType())
	{
	case varprop::VT_BOOL:
		if (pvar->vt == VT_BOOL)
			GetOptionsMgr()->SaveOption(name, pvar->boolVal);
		return E_INVALIDARG;
	case varprop::VT_INT:
		if (pvar->vt == VT_I2)
		{
			GetOptionsMgr()->SaveOption(name, pvar->iVal);
			return S_OK;
		}
		if (pvar->vt == VT_I4)
		{
			GetOptionsMgr()->SaveOption(name, pvar->intVal);
			return S_OK;
		}
		return E_INVALIDARG;
	case varprop::VT_STRING:
		if (pvar->vt == VT_BSTR)
		{
			GetOptionsMgr()->SaveOption(name, pvar->bstrVal);
			return S_OK;
		}
		return E_INVALIDARG;
	}
	return E_INVALIDARG;
}
