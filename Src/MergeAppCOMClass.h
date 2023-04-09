#pragma once

#include <oleauto.h>

class MyTypeInfo;

enum
{
	DISPID_Translate = 1,
	DISPID_GetOption = 2,
	DISPID_SaveOption = 3,
};

class MyDispatch : public IDispatch
{
public:
	MyDispatch(METHODDATA* pMethodData, size_t methodDataCount);
	MyDispatch(const MyDispatch&) = delete;
	virtual ~MyDispatch();
	MyDispatch& operator=(const MyDispatch&) = delete;

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
	ULONG STDMETHODCALLTYPE AddRef(void) override;
	ULONG STDMETHODCALLTYPE Release(void) override;
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override;
	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;
	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) = 0;
protected:
	ULONG m_cRef;
	MyTypeInfo* m_pTypeInfo;
};

class MergeAppCOMClass : public MyDispatch
{
public:
	MergeAppCOMClass();
	MergeAppCOMClass(const MergeAppCOMClass&) = delete;
	virtual ~MergeAppCOMClass();
	MergeAppCOMClass& operator=(const MergeAppCOMClass&) = delete;

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;
	HRESULT STDMETHODCALLTYPE Translate(BSTR text, BSTR* pbstrResult);
	HRESULT STDMETHODCALLTYPE GetOption(BSTR name, const VARIANT& varDefault, VARIANT* pvarResult);
	HRESULT STDMETHODCALLTYPE SaveOption(BSTR name, const VARIANT& varValue);
};

