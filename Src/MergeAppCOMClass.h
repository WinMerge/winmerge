#pragma once

#include <oleauto.h>

enum
{
	DISPID_Translate = 1,
	DISPID_GetOption = 2,
	DISPID_SaveOption = 3,
	DISPID_Log = 4,
	DISPID_MsgBox = 5,
	DISPID_InputBox = 6,
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

struct IMergeApp : public IUnknown
{
	virtual BSTR STDMETHODCALLTYPE Translate(BSTR text) = 0;
	virtual VARIANT STDMETHODCALLTYPE GetOption(BSTR name, VARIANT varDefault) = 0;
	virtual void STDMETHODCALLTYPE SaveOption(BSTR name, VARIANT varValue) = 0;
	virtual void STDMETHODCALLTYPE Log(int level, BSTR text) = 0;
	virtual int STDMETHODCALLTYPE MsgBox(BSTR prompt, VARIANT varButtons, VARIANT varTitle) = 0;
	virtual BSTR STDMETHODCALLTYPE InputBox(BSTR prompt, VARIANT varTitle, VARIANT varDefault) = 0;
};

class MergeAppCOMClass : public MyDispatch<IMergeApp>
{
public:
	MergeAppCOMClass();
	MergeAppCOMClass(const MergeAppCOMClass&) = delete;
	virtual ~MergeAppCOMClass();
	MergeAppCOMClass& operator=(const MergeAppCOMClass&) = delete;

	BSTR STDMETHODCALLTYPE Translate(BSTR text) override;
	VARIANT STDMETHODCALLTYPE GetOption(BSTR name, VARIANT varDefault) override;
	void STDMETHODCALLTYPE SaveOption(BSTR name, VARIANT varValue) override;
	void STDMETHODCALLTYPE Log(int level, BSTR text) override;
	int STDMETHODCALLTYPE MsgBox(BSTR prompt, VARIANT varButtons, VARIANT varTitle) override;
	BSTR STDMETHODCALLTYPE InputBox(BSTR prompt, VARIANT varTitle, VARIANT varDefault) override;
private:
	static INT_PTR CALLBACK InputBoxProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
	std::wstring m_inputBoxTitle;
	std::wstring m_inputBoxPrompt;
	std::wstring m_inputBoxText;
};

