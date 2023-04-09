#include "StdAfx.h"
#include "TypeInfo.h"

MyTypeInfo::MyTypeInfo(METHODDATA* pMethodData, size_t methodDataCount)
	: m_cRef(0)
{
	for (size_t i = 0; i < methodDataCount; ++i)
	{
		auto& methodData = pMethodData[i];
		m_mapNameToIndex.insert_or_assign(methodData.szName, static_cast<int>(m_methodData.size()));
		m_mapDispIdToIndex.insert_or_assign(methodData.dispid, static_cast<int>(m_methodData.size()));
		m_methodData.push_back(methodData);
	}
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::MyTypeInfo::QueryInterface(REFIID riid, void** ppvObject)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE MyTypeInfo::AddRef(void)
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG STDMETHODCALLTYPE MyTypeInfo::Release(void)
{
	ULONG ulRefCount = InterlockedDecrement(&m_cRef);
	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}
	return ulRefCount;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetTypeAttr(TYPEATTR** ppTypeAttr)
{
	auto* pTypeAttr = new TYPEATTR();
	pTypeAttr->cFuncs = static_cast<WORD>(m_methodData.size());
	pTypeAttr->typekind = TKIND_DISPATCH;
	pTypeAttr->cbAlignment = 8;
	*ppTypeAttr = pTypeAttr;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetTypeComp(ITypeComp** ppTComp)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetFuncDesc(UINT index, FUNCDESC** ppFuncDesc)
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

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetVarDesc(UINT index, VARDESC** ppVarDesc)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetNames(MEMBERID memid, BSTR* rgBstrNames, UINT cMaxNames, UINT* pcNames)
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

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetRefTypeOfImplType(UINT index, HREFTYPE* pRefType)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetImplTypeFlags(UINT index, INT* pImplTypeFlags)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetIDsOfNames(LPOLESTR* rgszNames, UINT cNames, MEMBERID* pMemId)
{
	for (unsigned i = 0; i < cNames; ++i)
	{
		auto it = m_mapNameToIndex.find(rgszNames[i]);
		if (it == m_mapNameToIndex.end())
			return DISP_E_UNKNOWNNAME;
		pMemId[i] = m_methodData[it->second].dispid;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	return reinterpret_cast<IDispatch*>(pvInstance)->Invoke(memid, IID_NULL, 0, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetDocumentation(MEMBERID memid, BSTR* pBstrName, BSTR* pBstrDocString, DWORD* pdwHelpContext, BSTR* pBstrHelpFile)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetDllEntry(MEMBERID memid, INVOKEKIND invKind, BSTR* pBstrDllName, BSTR* pBstrName, WORD* pwOrdinal)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetRefTypeInfo(HREFTYPE hRefType, ITypeInfo** ppTInfo)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::AddressOfMember(MEMBERID memid, INVOKEKIND invKind, PVOID* ppv)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::CreateInstance(IUnknown* pUnkOuter, REFIID riid, PVOID* ppvObj)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetMops(MEMBERID memid, BSTR* pBstrMops)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MyTypeInfo::GetContainingTypeLib(ITypeLib** ppTLib, UINT* pIndex)
{
	return E_NOTIMPL;
}

void STDMETHODCALLTYPE MyTypeInfo::ReleaseTypeAttr(TYPEATTR* pTypeAttr)
{
	delete pTypeAttr;
}

void STDMETHODCALLTYPE MyTypeInfo::ReleaseFuncDesc(FUNCDESC* pFuncDesc)
{
	delete[] pFuncDesc->lprgelemdescParam;
	delete pFuncDesc;
}

void STDMETHODCALLTYPE MyTypeInfo::ReleaseVarDesc(VARDESC* pVarDesc)
{
}

