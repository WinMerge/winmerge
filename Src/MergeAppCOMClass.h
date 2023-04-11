#pragma once

#include <oleauto.h>

enum
{
	DISPID_Translate = 1,
	DISPID_GetOption = 2,
	DISPID_SaveOption = 3,
};


template<class T>
class MyDispatch : public T, public IDispatch
{
public:
	MyDispatch(INTERFACEDATA* idata, void* pThis)
		: m_cRef(0), m_pTypeInfo(nullptr)
	{
		ITypeInfo* pTypeInfo = nullptr;
		if (SUCCEEDED(CreateDispTypeInfo(idata, LOCALE_SYSTEM_DEFAULT, &pTypeInfo)))
			m_pTypeInfo = pTypeInfo;
	}

	virtual ~MyDispatch()
	{
		if (m_pTypeInfo)
			m_pTypeInfo->Release();
	}

	MyDispatch(const MyDispatch&) = delete;
	MyDispatch& operator=(const MyDispatch&) = delete;

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
	{
		return E_NOTIMPL;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) override
	{
		return InterlockedIncrement(&m_cRef);
	}

	ULONG STDMETHODCALLTYPE Release(void) override
	{
		ULONG cRef = InterlockedDecrement(&m_cRef);
		if (cRef == 0)
			delete this;
		return cRef;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override
	{
		*pctinfo = 1;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override
	{
		ppTInfo = &m_pTypeInfo;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
	{
		return m_pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
	}

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
	{
		return m_pTypeInfo->Invoke(this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
protected:
	ULONG m_cRef;
	ITypeInfo* m_pTypeInfo;
};

struct IMergeApp : public IUnknown
{
	virtual BSTR STDMETHODCALLTYPE Translate(BSTR text) = 0;
	virtual VARIANT STDMETHODCALLTYPE GetOption(BSTR name, const VARIANT& varDefault) = 0;
	virtual void STDMETHODCALLTYPE SaveOption(BSTR name, const VARIANT& varValue) = 0;
};

class MergeAppCOMClass : public MyDispatch<IMergeApp>
{
public:
	MergeAppCOMClass();
	MergeAppCOMClass(const MergeAppCOMClass&) = delete;
	virtual ~MergeAppCOMClass();
	MergeAppCOMClass& operator=(const MergeAppCOMClass&) = delete;

	BSTR STDMETHODCALLTYPE Translate(BSTR text) override;
	VARIANT STDMETHODCALLTYPE GetOption(BSTR name, const VARIANT& varDefault) override;
	void STDMETHODCALLTYPE SaveOption(BSTR name, const VARIANT& varValue) override;
};

