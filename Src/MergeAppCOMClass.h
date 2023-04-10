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
		: m_pDispatch(nullptr)
	{
		ITypeInfo* pTypeInfo = nullptr;
		if (SUCCEEDED(CreateDispTypeInfo(idata, LOCALE_SYSTEM_DEFAULT, &pTypeInfo)))
		{
			IUnknown* pUnknown = nullptr;
			if (SUCCEEDED(CreateStdDispatch(nullptr, pThis, pTypeInfo, &pUnknown)))
			{
				pUnknown->QueryInterface(&m_pDispatch);
				pUnknown->Release();
			}
			pTypeInfo->Release();
		}
	}
	MyDispatch(const MyDispatch&) = delete;
	virtual ~MyDispatch()
	{
		if (m_pDispatch)
			m_pDispatch->Release();
	}
	MyDispatch& operator=(const MyDispatch&) = delete;

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
	{
		return m_pDispatch->QueryInterface(riid, ppvObject);
	}

	ULONG STDMETHODCALLTYPE AddRef(void) override
	{
		return m_pDispatch->AddRef();
	}

	ULONG STDMETHODCALLTYPE Release(void) override
	{
		ULONG c = m_pDispatch->Release();
		if (c == 0)
			delete this;
		return c;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override
	{
		return m_pDispatch->GetTypeInfoCount(pctinfo);
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override
	{
		return m_pDispatch->GetTypeInfo(iTInfo, lcid, ppTInfo);
	}

	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
	{
		return m_pDispatch->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
	{
		return m_pDispatch->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
protected:
	IDispatch* m_pDispatch;
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

