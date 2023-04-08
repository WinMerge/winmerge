#include "StdAfx.h"
#include "MergeAppCOMClass.h"
#include "LanguageSelect.h"
#include "OptionsMgr.h"
#include "Merge.h"

MergeAppCOMClass::MergeAppCOMClass()
	: m_cRef(0)
{
	static PARAMDATA paramData_Translate[] =
	{ {L"text", VT_BSTR}, };
	static PARAMDATA paramData_GetOption[] =
	{ {L"name", VT_BSTR}, {L"defaultValue", VT_VARIANT}, };
	static PARAMDATA paramData_SaveOption[] =
	{ {L"name", VT_BSTR}, {L"value", VT_VARIANT}, };
	static METHODDATA methodData_WinMergeObject[] =
	{
		{ L"Translate",                   paramData_Translate,    DISPID_Translate,                   0, CC_STDCALL, 1, DISPATCH_METHOD,      VT_BSTR },
		{ L"GetOption",                   paramData_GetOption,    DISPID_GetOption,                   1, CC_STDCALL, 2, DISPATCH_METHOD,      VT_VARIANT },
		{ L"SaveOption",                  paramData_SaveOption,   DISPID_SaveOption,                  2, CC_STDCALL, 2, DISPATCH_METHOD,      VT_VOID },
	};
	size_t methodDataCount = std::size(methodData_WinMergeObject);
	METHODDATA* pMethodData = methodData_WinMergeObject;
	for (size_t i = 0; i < methodDataCount; ++i)
	{
		auto& methodData = pMethodData[i];
		m_mapNameToIndex.insert_or_assign(methodData.szName, static_cast<int>(m_methodData.size()));
		m_mapDispIdToIndex.insert_or_assign(methodData.dispid, static_cast<int>(m_methodData.size()));
		m_methodData.push_back(methodData);
	}
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::MergeAppCOMClass::QueryInterface(REFIID riid, void** ppvObject)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE MergeAppCOMClass::AddRef(void)
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG STDMETHODCALLTYPE MergeAppCOMClass::Release(void)
{
	ULONG ulRefCount = InterlockedDecrement(&m_cRef);
	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}
	return ulRefCount;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetTypeInfoCount(UINT* pctinfo)
{
	*pctinfo = static_cast<UINT>(m_methodData.size());
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	*ppTInfo = this;
	(*ppTInfo)->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	for (unsigned i = 0; i < cNames; ++i)
	{
		auto it = m_mapNameToIndex.find(rgszNames[i]);
		if (it == m_mapNameToIndex.end())
			return DISP_E_UNKNOWNNAME;
		rgDispId[i] = m_methodData[it->second].dispid;
	}
	return S_OK;
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

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetTypeAttr(TYPEATTR** ppTypeAttr)
{
	auto* pTypeAttr = new TYPEATTR();
	pTypeAttr->cFuncs = static_cast<WORD>(m_methodData.size());
	pTypeAttr->typekind = TKIND_DISPATCH;
	pTypeAttr->cbAlignment = 8;
	*ppTypeAttr = pTypeAttr;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetTypeComp(ITypeComp** ppTComp)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetFuncDesc(UINT index, FUNCDESC** ppFuncDesc)
{
	if (index >= m_methodData.size())
		return E_INVALIDARG;
	auto* pFuncDesc = new FUNCDESC();
	const METHODDATA& methodData = m_methodData[index];
	pFuncDesc->funckind = FUNC_DISPATCH;
	pFuncDesc->invkind = static_cast<INVOKEKIND>(methodData.wFlags);
	pFuncDesc->wFuncFlags = 0;
	pFuncDesc->cParams = static_cast<short>(methodData.cArgs);
	pFuncDesc->memid = methodData.dispid;
	pFuncDesc->callconv = methodData.cc;
	if (methodData.cArgs > 0)
	{
		pFuncDesc->lprgelemdescParam = new ELEMDESC[methodData.cArgs];
		for (unsigned i = 0; i < methodData.cArgs; ++i)
			pFuncDesc->lprgelemdescParam[i].tdesc.vt = methodData.ppdata[i].vt;
	}
	pFuncDesc->elemdescFunc.tdesc.vt = methodData.vtReturn;
	*ppFuncDesc = pFuncDesc;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetVarDesc(UINT index, VARDESC** ppVarDesc)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetNames(MEMBERID memid, BSTR* rgBstrNames, UINT cMaxNames, UINT* pcNames)
{
	if (m_mapDispIdToIndex.find(memid) == m_mapDispIdToIndex.end())
		return E_INVALIDARG;
	const METHODDATA& methodData = m_methodData[m_mapDispIdToIndex[memid]];
	for (unsigned i = 0; i < cMaxNames && i < methodData.cArgs + 1; i++)
	{
		if (i == 0)
			rgBstrNames[i] = SysAllocString(methodData.szName);
		else
			rgBstrNames[i] = SysAllocString(methodData.ppdata[i - 1].szName);
		*pcNames = i + 1;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetRefTypeOfImplType(UINT index, HREFTYPE* pRefType)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetImplTypeFlags(UINT index, INT* pImplTypeFlags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetIDsOfNames(LPOLESTR* rgszNames, UINT cNames, MEMBERID* pMemId)
{
	return GetIDsOfNames(IID_NULL, rgszNames, cNames, 0, pMemId);
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return reinterpret_cast<IDispatch*>(pvInstance)->Invoke(memid, IID_NULL, 0, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetDocumentation(MEMBERID memid, BSTR* pBstrName, BSTR* pBstrDocString, DWORD* pdwHelpContext, BSTR* pBstrHelpFile)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetDllEntry(MEMBERID memid, INVOKEKIND invKind, BSTR* pBstrDllName, BSTR* pBstrName, WORD* pwOrdinal)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetRefTypeInfo(HREFTYPE hRefType, ITypeInfo** ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::AddressOfMember(MEMBERID memid, INVOKEKIND invKind, PVOID* ppv)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::CreateInstance(IUnknown* pUnkOuter, REFIID riid, PVOID* ppvObj)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetMops(MEMBERID memid, BSTR* pBstrMops)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::GetContainingTypeLib(ITypeLib** ppTLib, UINT* pIndex)
{
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE MergeAppCOMClass::ReleaseTypeAttr(TYPEATTR* pTypeAttr)
{
	delete pTypeAttr;
}

void STDMETHODCALLTYPE MergeAppCOMClass::ReleaseFuncDesc(FUNCDESC* pFuncDesc)
{
	delete[] pFuncDesc->lprgelemdescParam;
	delete pFuncDesc;
}

void STDMETHODCALLTYPE MergeAppCOMClass::ReleaseVarDesc(VARDESC* pVarDesc)
{
}

HRESULT STDMETHODCALLTYPE MergeAppCOMClass::Translate(BSTR text, BSTR* pbstrResult)
{
	std::wstring translated_str;
	theApp.m_pLangDlg->TranslateString(text, translated_str);
	*pbstrResult = SysAllocString(translated_str.c_str());
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
