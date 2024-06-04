#pragma once

#include <oleauto.h>
#include "MergeAppLib.h"

enum
{
	DISPID_Translate = 1,
	DISPID_GetOption = 2,
	DISPID_SaveOption = 3,
	DISPID_MsgBox = 4,
	DISPID_InputBox = 5,
	DISPID_LogError = 6,
};


template<class T>
class MyDispatch : public T
{
public:
	MyDispatch(void* pThis)
		: m_cRef(0), m_pTypeInfo(nullptr)
	{
		tchar_t path[MAX_PATH];
		ITypeLib* pTypeLib = nullptr;
		ITypeInfo* pTypeInfo = nullptr;
		GetModuleFileName(nullptr, path, sizeof(path) / sizeof(path[0]));
		if (SUCCEEDED(LoadTypeLib(path, &pTypeLib)))
		{
			pTypeLib->GetTypeInfoOfGuid(__uuidof(T), &m_pTypeInfo);
			pTypeLib->Release();
		}
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
		if (riid == IID_IUnknown || riid == IID_IDispatch)
		{
			*ppvObject = static_cast<IDispatch*>(this);
			AddRef();
			return S_OK;
		}
		*ppvObject = nullptr;
		return E_NOINTERFACE;
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
		m_pTypeInfo->AddRef();
		ppTInfo = &m_pTypeInfo;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
	{
		return m_pTypeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
	}

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
	{
		return m_pTypeInfo->Invoke(static_cast<T*>(this), dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
protected:
	ULONG m_cRef;
	ITypeInfo* m_pTypeInfo;
};

class MergeAppCOMClass : public MyDispatch<IMergeApp>
{
public:
	MergeAppCOMClass();
	MergeAppCOMClass(const MergeAppCOMClass&) = delete;
	virtual ~MergeAppCOMClass();
	MergeAppCOMClass& operator=(const MergeAppCOMClass&) = delete;

	HRESULT STDMETHODCALLTYPE Translate(BSTR text, BSTR* pRet) override;
	HRESULT STDMETHODCALLTYPE GetOption(BSTR name, VARIANT varDefault, VARIANT* pRet) override;
	HRESULT STDMETHODCALLTYPE SaveOption(BSTR name, VARIANT varValue) override;
	HRESULT STDMETHODCALLTYPE MsgBox(BSTR prompt, VARIANT varButtons, VARIANT varTitle, int* pRet) override;
	HRESULT STDMETHODCALLTYPE InputBox(BSTR prompt, VARIANT varTitle, VARIANT varDefault, BSTR* pRet) override;
	HRESULT STDMETHODCALLTYPE LogError(BSTR text) override;
private:
	static INT_PTR CALLBACK InputBoxProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
	std::wstring m_inputBoxTitle;
	std::wstring m_inputBoxPrompt;
	std::wstring m_inputBoxText;
};

