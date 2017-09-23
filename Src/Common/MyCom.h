// MyCom.h
// This file originates from 7-Zip 9.20, and is copyrighted and distributed by
// Igor Pavlov under the terms of the GNU Lesser General Public License (LGPL).
// It has been modified by WinMerge Team.

#pragma once

#ifndef RINOK
#define RINOK(x) { HRESULT __result_ = (x); if (__result_ != S_OK) return __result_; }
#endif

template <class T>
class CMyComPtr
{
  T* _p;
public:
  // typedef T _PtrClass;
  CMyComPtr() { _p = NULL;}
  explicit CMyComPtr(T* p) {if ((_p = p) != NULL) p->AddRef(); }
  explicit CMyComPtr(const CMyComPtr<T>& lp)
  {
    if ((_p = lp._p) != NULL)
      _p->AddRef();
  }
  ~CMyComPtr() { if (_p) _p->Release(); }
  void Release() { if (_p) { _p->Release(); _p = NULL; } }
  operator T*() const { return _p; }
  // T& operator*() const {  return *_p; }
  T** operator&() { return &_p; }
  T* operator->() const { return _p; }
  T* operator=(T* p)
  {
    if (p != 0)
      p->AddRef();
    if (_p)
      _p->Release();
    _p = p;
    return p;
  }
  bool operator!() const { return (_p == NULL); }
  // bool operator==(T* pT) const {  return _p == pT; }
  // Compare two objects for equivalence
  void Attach(T* p2)
  {
    Release();
    _p = p2;
  }
  T* Detach()
  {
    T* pt = _p;
    _p = NULL;
    return pt;
  }
  #ifdef _WIN32
  HRESULT CoCreateInstance(REFCLSID rclsid, REFIID iid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
  {
    return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, iid, (void**)&_p);
  }
  #endif
  /*
  HRESULT CoCreateInstance(LPCOLESTR szProgID, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
  {
    CLSID clsid;
    HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
    ATLASSERT(_p == NULL);
    if (SUCCEEDED(hr))
      hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&_p);
    return hr;
  }
  */
  template <class Q>
  HRESULT QueryInterface(REFGUID iid, Q** pp) const
  {
    return _p->QueryInterface(iid, (void**)pp);
  }
};

//////////////////////////////////////////////////////////

inline HRESULT StringToBstr(LPCOLESTR src, BSTR *bstr)
{
  *bstr = ::SysAllocString(src);
  return (*bstr != 0) ? S_OK : E_OUTOFMEMORY;
}

class CMyComBSTR
{
public:
  BSTR m_str;
  CMyComBSTR(): m_str(NULL) {}
  explicit CMyComBSTR(LPCOLESTR src) { m_str = ::SysAllocString(src); }
  // Move constructor
  explicit CMyComBSTR(BSTR *bstr): m_str(*bstr) { *bstr = NULL; }
  // CMyComBSTR(int nSize) { m_str = ::SysAllocStringLen(NULL, nSize); }
  CMyComBSTR(UINT nSize, LPCOLESTR sz) { m_str = ::SysAllocStringLen(sz, nSize); }
  CMyComBSTR(const CMyComBSTR& src) { m_str = src.MyCopy(); }
  /*
  CMyComBSTR(REFGUID src)
  {
    LPOLESTR szGuid;
    StringFromCLSID(src, &szGuid);
    m_str = ::SysAllocString(szGuid);
    CoTaskMemFree(szGuid);
  }
  */
  ~CMyComBSTR() { ::SysFreeString(m_str); }
  CMyComBSTR& operator=(const CMyComBSTR& src)
  {
    if (m_str != src.m_str)
    {
      if (m_str)
        ::SysFreeString(m_str);
      m_str = src.MyCopy();
    }
    return *this;
  }
  CMyComBSTR& operator=(LPCOLESTR src)
  {
    ::SysReAllocString(&m_str, src);
    return *this;
  }
  CMyComBSTR& operator+=(LPCOLESTR src)
  {
    UINT len = Length();
    if (::SysReAllocStringLen(&m_str, NULL, len + ::lstrlenW(src)))
      ::StrCpyW(m_str + len, src);
    return *this;
  }
  CMyComBSTR SplitAt(LPCWSTR pattern)
  {
    BSTR str = NULL;
    if (LPCWSTR match = StrStrW(m_str, pattern))
    {
      str = SysAllocString(match + lstrlenW(pattern));
      if (str)
        SysReAllocStringLen(&m_str, NULL, static_cast<UINT>(match - m_str));
    }
    return CMyComBSTR(&str);
  }
  bool operator == (const CMyComBSTR &src)
  {
    return VarBstrCmp(m_str, src.m_str, LOCALE_USER_DEFAULT, 0) == VARCMP_EQ;
  }
  UINT Length() const { return ::SysStringLen(m_str); }
  operator BSTR() const { return m_str; }
  BSTR* operator&() { return &m_str; }
  BSTR MyCopy() const
  {
    UINT byteLen = ::SysStringByteLen(m_str);
    return ::SysAllocStringByteLen(reinterpret_cast<LPCSTR>(m_str), byteLen);
  }
  /*
  void Attach(BSTR src) { m_str = src; }
  BSTR Detach()
  {
    BSTR s = m_str;
    m_str = NULL;
    return s;
  }
  */
  void Empty()
  {
    ::SysFreeString(m_str);
    m_str = NULL;
  }
  bool operator!() const {  return (m_str == NULL); }
};

class CMyClipFormat
{
private:
	const CLIPFORMAT cf;
public:
	operator CLIPFORMAT() const { return cf; }
	explicit CMyClipFormat(CLIPFORMAT cf) : cf(cf) { }
	explicit CMyClipFormat(LPCTSTR cfstr) : cf(RegisterClipboardFormat(cfstr)) { }
};

class CMyFormatEtc : public FORMATETC
{
public:
	HRESULT QueryGetData(IDataObject *pDataObj)
	{
		return pDataObj->QueryGetData(this);
	}
	HRESULT GetData(IDataObject *pDataObj, STGMEDIUM *pStgMedium)
	{
		return pDataObj->GetData(this, pStgMedium);
	}
	CMyFormatEtc(CMyClipFormat cf, LONG lindex = -1, DWORD tymed = TYMED_HGLOBAL)
	{
		FORMATETC::cfFormat = cf;
		FORMATETC::ptd = NULL;
		FORMATETC::dwAspect = DVASPECT_CONTENT;
		FORMATETC::lindex = lindex;
		FORMATETC::tymed = tymed;
	}
};

class CMyStgMedium : public STGMEDIUM
{
public:
	CMyStgMedium()
	{
		ZeroMemory(this, sizeof *this);
	}
	~CMyStgMedium()
	{
		ReleaseStgMedium(this);
	}
};

class CMyVariant : public VARIANT
{
public:
	CMyVariant() { VariantInit(this); }
	~CMyVariant() { VariantClear(this); }
	explicit CMyVariant(short value)
	{
		V_VT(this) = VT_I2;
		V_I2(this) = value;
	}
	explicit CMyVariant(long value)
	{
		V_VT(this) = VT_I4;
		V_I4(this) = value;
	}
	explicit CMyVariant(bool value)
	{
		V_VT(this) = VT_BOOL;
		V_BOOL(this) = value ? VARIANT_TRUE : VARIANT_FALSE;
	}
	explicit CMyVariant(LPCOLESTR value)
	{
		V_VT(this) = VT_BSTR;
		V_BSTR(this) = SysAllocString(value);
	}
	explicit CMyVariant(LPCOLESTR value, UINT len)
	{
		V_VT(this) = VT_BSTR;
		V_BSTR(this) = SysAllocStringLen(value, len);
	}
	explicit CMyVariant(IDispatch *value)
	{
		V_VT(this) = VT_DISPATCH;
		V_DISPATCH(this) = value;
		if (value)
			value->AddRef();
	}
	explicit CMyVariant(const CMyVariant &src)
	{
		VariantInit(this);
#if _MSC_VER < 1500 
		VariantCopy(this, const_cast<CMyVariant*>(&src));
#else
		VariantCopy(this, &src);
#endif
	}
	void Clear()
	{
		VariantClear(this);
	}
	HRESULT ChangeType(VARTYPE vt)
	{
		return VariantChangeType(this, this, 0, vt);
	}
};

class CMyDispId
{
private:
	DISPID dispid;
public:
	CMyDispId() : dispid(0) { }
	HRESULT Init(IDispatch *pdisp, LPCWSTR name)
	{
		HRESULT hr = pdisp->GetIDsOfNames(IID_NULL, const_cast<LPWSTR *>(&name), 1, LOCALE_NEUTRAL, &dispid);
		return hr;
	}
	HRESULT Call(IDispatch *pdisp, DISPPARAMS &dp, WORD wFlags = DISPATCH_METHOD, VARIANT *pret = 0)
	{
		// DISPATCH_PROPERTYPUT is special as per http://support.microsoft.com/kb/238393
		if (wFlags & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF))
		{
			dp.cNamedArgs = 1;
			static DISPID dispidNamed = DISPID_PROPERTYPUT;
			dp.rgdispidNamedArgs = &dispidNamed;
		}
		EXCEPINFO e;
		memset(&e, 0, sizeof e);
		UINT uErrArg = 0;
		HRESULT hr = pdisp->Invoke(dispid, IID_NULL, LOCALE_NEUTRAL, wFlags, &dp, pret, &e, &uErrArg);
		if (hr == DISP_E_EXCEPTION)
		{
			ICreateErrorInfo *pcerrinfo;
			if (SUCCEEDED(CreateErrorInfo(&pcerrinfo)))
			{
				pcerrinfo->SetGUID(IID_IDispatch);
				pcerrinfo->SetSource(e.bstrSource);
				pcerrinfo->SetDescription(e.bstrDescription);
				pcerrinfo->SetHelpFile(e.bstrHelpFile);
				pcerrinfo->SetHelpContext(e.dwHelpContext);
				IErrorInfo *perrinfo;
				if (SUCCEEDED(pcerrinfo->QueryInterface(&perrinfo)))
				{
					SetErrorInfo(0, perrinfo);
					perrinfo->Release();
				}
				pcerrinfo->Release();
			}
			SysFreeString(e.bstrSource);
			SysFreeString(e.bstrDescription);
			SysFreeString(e.bstrHelpFile);
		}
		return hr;
	}
};

class CMyDispParamsUnnamed : public DISPPARAMS
{
public:
	void Make(VARIANT *rgvarg)
	{
		ZeroMemory(this, sizeof *this);
		DISPPARAMS::rgvarg = rgvarg;
	}
	void Drop()
	{
		while (cArgs)
		{
			--cArgs;
			static_cast<CMyVariant *>(rgvarg++)->CMyVariant::~CMyVariant();
		}
	}
	template<class T>
	CMyDispParamsUnnamed &operator[](const T &value)
	{
		static_cast<CMyVariant *>(--rgvarg)->CMyVariant::CMyVariant(value);
		++cArgs;
		return *this;
	}
};

// This assumption is critical to the sanity of the CMyDispParams<> template:
C_ASSERT(sizeof(CMyDispParamsUnnamed) == sizeof(VARIANT));

template<UINT cArgs = 0>
union CMyDispParams
{
private:
	VARIANT rgvarg[cArgs + 1];
	// NB: rgvarg[0] is overlayed by the DispParamsUnnamed member declared
	// below, and hence invalid and must not be accessed as a VARIANT.
public:
	CMyDispParamsUnnamed Unnamed;
	CMyDispParams() { Unnamed.Make(rgvarg + cArgs + 1); }
	~CMyDispParams() { Unnamed.Drop(); }
};

