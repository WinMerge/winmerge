#pragma once

#include <oleauto.h>

enum
{
	DISPID_Translate = 1,
	DISPID_GetOption = 2,
	DISPID_SaveOption = 3,
};

class MyDispatch : public IDispatch
{
public:
	MyDispatch(INTERFACEDATA* idata);
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
	ITypeInfo* m_pTypeInfo;
};

struct IMergeApp
{
	virtual HRESULT STDMETHODCALLTYPE Translate(BSTR text, BSTR* pbstrResult) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetOption(BSTR name, const VARIANT& varDefault, VARIANT* pvarResult) = 0;
	virtual HRESULT STDMETHODCALLTYPE SaveOption(BSTR name, const VARIANT& varValue) = 0;
};

class MergeAppCOMClass : public IMergeApp, public MyDispatch
{
public:
	MergeAppCOMClass();
	MergeAppCOMClass(const MergeAppCOMClass&) = delete;
	virtual ~MergeAppCOMClass();
	MergeAppCOMClass& operator=(const MergeAppCOMClass&) = delete;

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;
	HRESULT STDMETHODCALLTYPE Translate(BSTR text, BSTR* pbstrResult) override;
	HRESULT STDMETHODCALLTYPE GetOption(BSTR name, const VARIANT& varDefault, VARIANT* pvarResult) override;
	HRESULT STDMETHODCALLTYPE SaveOption(BSTR name, const VARIANT& varValue) override;
};

