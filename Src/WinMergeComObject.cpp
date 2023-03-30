#include "StdAfx.h"
#include "WinMergeComObject.h"
#include "LanguageSelect.h"
#include "Merge.h"

WinMergeComObject::WinMergeComObject()
	: m_nRef(0)
{
	static PARAMDATA paramData_Translate[] =
	{ {L"text", VT_BSTR}, };
	static METHODDATA methodData_WinMergeObject[] =
	{
		{ L"Translate",                   paramData_Translate,    DISPID_Translate,                   0, CC_STDCALL, 1, DISPATCH_METHOD,      VT_BSTR },
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

HRESULT STDMETHODCALLTYPE WinMergeComObject::WinMergeComObject::QueryInterface(REFIID riid, void** ppvObject)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE WinMergeComObject::AddRef(void)
{
	return ++m_nRef;
}

ULONG STDMETHODCALLTYPE WinMergeComObject::Release(void)
{
	if (--m_nRef == 0)
	{
		delete this;
		return 0;
	}
	return m_nRef;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetTypeInfoCount(UINT* pctinfo)
{
	*pctinfo = static_cast<UINT>(m_methodData.size());
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	*ppTInfo = this;
	(*ppTInfo)->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
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

HRESULT STDMETHODCALLTYPE WinMergeComObject::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
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
			BSTR text = pDispParams->rgvarg[0].bstrVal;
			BSTR* pbstrResult = &pVarResult->bstrVal;
			hr = Translate(text, pbstrResult);
			pVarResult->vt = VT_BSTR;
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

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetTypeAttr(TYPEATTR** ppTypeAttr)
{
	auto* pTypeAttr = new TYPEATTR();
	pTypeAttr->cFuncs = static_cast<WORD>(m_methodData.size());
	pTypeAttr->typekind = TKIND_DISPATCH;
	pTypeAttr->cbAlignment = 8;
	*ppTypeAttr = pTypeAttr;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetTypeComp(ITypeComp** ppTComp)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetFuncDesc(UINT index, FUNCDESC** ppFuncDesc)
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

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetVarDesc(UINT index, VARDESC** ppVarDesc)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetNames(MEMBERID memid, BSTR* rgBstrNames, UINT cMaxNames, UINT* pcNames)
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

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetRefTypeOfImplType(UINT index, HREFTYPE* pRefType)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetImplTypeFlags(UINT index, INT* pImplTypeFlags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetIDsOfNames(LPOLESTR* rgszNames, UINT cNames, MEMBERID* pMemId)
{
	return GetIDsOfNames(IID_NULL, rgszNames, cNames, 0, pMemId);
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return reinterpret_cast<IDispatch*>(pvInstance)->Invoke(memid, IID_NULL, 0, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetDocumentation(MEMBERID memid, BSTR* pBstrName, BSTR* pBstrDocString, DWORD* pdwHelpContext, BSTR* pBstrHelpFile)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetDllEntry(MEMBERID memid, INVOKEKIND invKind, BSTR* pBstrDllName, BSTR* pBstrName, WORD* pwOrdinal)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetRefTypeInfo(HREFTYPE hRefType, ITypeInfo** ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::AddressOfMember(MEMBERID memid, INVOKEKIND invKind, PVOID* ppv)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::CreateInstance(IUnknown* pUnkOuter, REFIID riid, PVOID* ppvObj)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetMops(MEMBERID memid, BSTR* pBstrMops)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::GetContainingTypeLib(ITypeLib** ppTLib, UINT* pIndex)
{
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE WinMergeComObject::ReleaseTypeAttr(TYPEATTR* pTypeAttr)
{
	delete pTypeAttr;
}

void STDMETHODCALLTYPE WinMergeComObject::ReleaseFuncDesc(FUNCDESC* pFuncDesc)
{
	delete[] pFuncDesc->lprgelemdescParam;
	delete pFuncDesc;
}

void STDMETHODCALLTYPE WinMergeComObject::ReleaseVarDesc(VARDESC* pVarDesc)
{
}

HRESULT STDMETHODCALLTYPE WinMergeComObject::Translate(BSTR text, BSTR* pbstrResult)
{
	std::wstring translated_str;
	theApp.m_pLangDlg->TranslateString(text, translated_str);
	*pbstrResult = SysAllocString(translated_str.c_str());
	return S_OK;
}

