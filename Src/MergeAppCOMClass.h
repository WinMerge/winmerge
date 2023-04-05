#pragma once

#include <oleauto.h>
#include <string>
#include <vector>
#include <map>

class MergeAppCOMClass: public IDispatch, public ITypeInfo
{
public:
	enum
	{
		DISPID_Translate = 1,
		DISPID_GetOption = 2,
		DISPID_SaveOption = 3,
	};

	MergeAppCOMClass();
	MergeAppCOMClass(const MergeAppCOMClass&) = delete;
	MergeAppCOMClass& operator=(const MergeAppCOMClass&) = delete;

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
	ULONG STDMETHODCALLTYPE AddRef(void) override;
	ULONG STDMETHODCALLTYPE Release(void) override;
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;
	HRESULT STDMETHODCALLTYPE GetTypeAttr(TYPEATTR** ppTypeAttr) override;
	HRESULT STDMETHODCALLTYPE GetTypeComp(ITypeComp** ppTComp) override;
	HRESULT STDMETHODCALLTYPE GetFuncDesc(UINT index, FUNCDESC** ppFuncDesc) override;
	HRESULT STDMETHODCALLTYPE GetVarDesc(UINT index, VARDESC** ppVarDesc) override;
	HRESULT STDMETHODCALLTYPE GetNames(MEMBERID memid, BSTR* rgBstrNames, UINT cMaxNames, UINT* pcNames) override;
	HRESULT STDMETHODCALLTYPE GetRefTypeOfImplType(UINT index, HREFTYPE* pRefType) override;
	HRESULT STDMETHODCALLTYPE GetImplTypeFlags(UINT index, INT* pImplTypeFlags) override;
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(LPOLESTR* rgszNames, UINT cNames, MEMBERID* pMemId) override;
	HRESULT STDMETHODCALLTYPE Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;
	HRESULT STDMETHODCALLTYPE GetDocumentation(MEMBERID memid, BSTR* pBstrName, BSTR* pBstrDocString, DWORD* pdwHelpContext, BSTR* pBstrHelpFile) override;
	HRESULT STDMETHODCALLTYPE GetDllEntry(MEMBERID memid, INVOKEKIND invKind, BSTR* pBstrDllName, BSTR* pBstrName, WORD* pwOrdinal);
	HRESULT STDMETHODCALLTYPE GetRefTypeInfo(HREFTYPE hRefType, ITypeInfo** ppTInfo) override;
	HRESULT STDMETHODCALLTYPE AddressOfMember(MEMBERID memid, INVOKEKIND invKind, PVOID* ppv) override;
	HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, PVOID* ppvObj) override;
	HRESULT STDMETHODCALLTYPE GetMops(MEMBERID memid, BSTR* pBstrMops) override;
	HRESULT STDMETHODCALLTYPE GetContainingTypeLib(ITypeLib** ppTLib, UINT* pIndex) override;
	void STDMETHODCALLTYPE ReleaseTypeAttr(TYPEATTR* pTypeAttr) override;
	void STDMETHODCALLTYPE ReleaseFuncDesc(FUNCDESC* pFuncDesc) override;
	void STDMETHODCALLTYPE ReleaseVarDesc(VARDESC* pVarDesc) override;
	HRESULT STDMETHODCALLTYPE Translate(BSTR text, BSTR* pbstrResult);
	HRESULT STDMETHODCALLTYPE GetOption(BSTR name, VARIANT varDefault, VARIANT* pvarResult);
	HRESULT STDMETHODCALLTYPE SaveOption(BSTR name, VARIANT varValue);

protected:
	ULONG m_cRef;
	std::map<std::wstring, int> m_mapNameToIndex;
	std::map<DISPID, int> m_mapDispIdToIndex;
	std::vector<METHODDATA> m_methodData;
};

