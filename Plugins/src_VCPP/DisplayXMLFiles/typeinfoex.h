#ifndef _NEWOBJECTS_ATL_TYPEINFOEX_H_
#define _NEWOBJECTS_ATL_TYPEINFOEX_H_

// This class is a little changed version of the
// CComTypeInfoHolder class found in the MS ATL
// It supports different ways to obtain type info
// depending on the CComTypeInfoHolder parameter:
// 
//  0   - Acts as the Normal CComTypeInfoHolder i.e. loading from the registry only
//  1   - Loads only from the module resource
//  2   - Try registry and if not found load from the module
//  3   - Try from the module and if not found then from the registry
//
//  Use this class in the IDispatchImpl templates passed as parent
//  classes to your components if you want to create components
//  capable of working with scripts without registration in the
//  system registry
//
//  Michael Elfial, newObjects 2001
//  http://www.newobjects.com

template <int nObtainMethod = 1>
class CComTypeInfoHolderModule {
public:
	const GUID* m_pguid;
	const GUID* m_plibid;
	WORD m_wMajor;
	WORD m_wMinor;

	ITypeInfo* m_pInfo;
	long m_dwRef;
	struct stringdispid
	{
		CComBSTR bstr;
		int nLen;
		DISPID id;
	};
	stringdispid* m_pMap;
	int m_nCount;

public:
	HRESULT GetTI(LCID lcid, ITypeInfo** ppInfo)
	{
		HRESULT hr = S_OK;
		if (m_pInfo == NULL)
			hr = GetTI(lcid);
		*ppInfo = m_pInfo;
		if (m_pInfo != NULL)
		{
			m_pInfo->AddRef();
			hr = S_OK;
		}
		return hr;
	}
	HRESULT GetTI(LCID lcid);
	HRESULT EnsureTI(LCID lcid)
	{
		HRESULT hr = S_OK;
		if (m_pInfo == NULL)
			hr = GetTI(lcid);
		return hr;
	}

	// This function is called by the module on exit
	// It is registered through _Module.AddTermFunc()
	static void __stdcall Cleanup(DWORD dw)
	{
		CComTypeInfoHolder* p = (CComTypeInfoHolder*) dw;
		if (p->m_pInfo != NULL)
			p->m_pInfo->Release();
		p->m_pInfo = NULL;
		delete [] p->m_pMap;
		p->m_pMap = NULL;
	}

	HRESULT GetTypeInfo(UINT /* itinfo */, LCID lcid, ITypeInfo** pptinfo)
	{
		HRESULT hRes = E_POINTER;
		if (pptinfo != NULL)
			hRes = GetTI(lcid, pptinfo);
		return hRes;
	}
	HRESULT GetIDsOfNames(REFIID /* riid */, LPOLESTR* rgszNames, UINT cNames,
		LCID lcid, DISPID* rgdispid)
	{
		HRESULT hRes = EnsureTI(lcid);
		if (m_pInfo != NULL)
		{
			for (int i=0; i<(int)cNames; i++)
			{
				int n = ocslen(rgszNames[i]);
				for (int j=m_nCount-1; j>=0; j--)
				{
					if ((n == m_pMap[j].nLen) &&
						(memcmp(m_pMap[j].bstr, rgszNames[i], m_pMap[j].nLen * sizeof(OLECHAR)) == 0))
					{
						rgdispid[i] = m_pMap[j].id;
						break;
					}
				}
				if (j < 0)
				{
					hRes = m_pInfo->GetIDsOfNames(rgszNames + i, 1, &rgdispid[i]);
					if (FAILED(hRes))
						break;
				}
			}
		}
		return hRes;
	}

	HRESULT Invoke(IDispatch* p, DISPID dispidMember, REFIID /* riid */,
		LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
		EXCEPINFO* pexcepinfo, UINT* puArgErr)
	{
		HRESULT hRes = EnsureTI(lcid);
		if (m_pInfo != NULL)
			hRes = m_pInfo->Invoke(p, dispidMember, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
		return hRes;
	}
	HRESULT LoadNameCache(ITypeInfo* pTypeInfo)
	{
		TYPEATTR* pta;
		HRESULT hr = pTypeInfo->GetTypeAttr(&pta);
		if (SUCCEEDED(hr))
		{
			m_nCount = pta->cFuncs;
			m_pMap = m_nCount == 0 ? 0 : new stringdispid[m_nCount];
			for (int i=0; i<m_nCount; i++)
			{
				FUNCDESC* pfd;
				if (SUCCEEDED(pTypeInfo->GetFuncDesc(i, &pfd)))
				{
					CComBSTR bstrName;
					if (SUCCEEDED(pTypeInfo->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL)))
					{
						m_pMap[i].bstr.Attach(bstrName.Detach());
						m_pMap[i].nLen = SysStringLen(m_pMap[i].bstr);
						m_pMap[i].id = pfd->memid;
					}
					pTypeInfo->ReleaseFuncDesc(pfd);
				}
			}
			pTypeInfo->ReleaseTypeAttr(pta);
		}
		return S_OK;
	}
};

template <int nObtainMethod>
inline HRESULT CComTypeInfoHolderModule<nObtainMethod>::GetTI(LCID lcid)
{
    USES_CONVERSION;
	//If this assert occurs then most likely didn't initialize properly
	ATLASSERT(m_plibid != NULL && m_pguid != NULL);
	ATLASSERT(!::InlineIsEqualGUID(*m_plibid, GUID_NULL) && "Did you forget to pass the LIBID to CComModule::Init?");

	if (m_pInfo != NULL)
		return S_OK;
	HRESULT hRes = E_FAIL;
	EnterCriticalSection(&_Module.m_csTypeInfoHolder);
	if (m_pInfo == NULL)
	{
		ITypeLib* pTypeLib;
        hRes = E_FAIL;
        TCHAR szModule[_MAX_PATH+10];
        LPOLESTR lpszModule;
        // Select the way
        switch (nObtainMethod) {
            case 2: // registry, module
                hRes = LoadRegTypeLib(*m_plibid, m_wMajor, m_wMinor, lcid, &pTypeLib);
            case 1: // module
                if (SUCCEEDED(hRes)) break;
            case 3: // module, registry
                GetModuleFileName(_Module.m_hInstTypeLib, szModule, _MAX_PATH);
                lpszModule = T2OLE(szModule);
                hRes = LoadTypeLib(lpszModule, &pTypeLib);
                if (nObtainMethod == 1) break;
            default:
                if (FAILED(hRes)) {
                    hRes = LoadRegTypeLib(*m_plibid, m_wMajor, m_wMinor, lcid, &pTypeLib);
                }
        }
		
		if (SUCCEEDED(hRes))
		{
			CComPtr<ITypeInfo> spTypeInfo;
			hRes = pTypeLib->GetTypeInfoOfGuid(*m_pguid, &spTypeInfo);
			if (SUCCEEDED(hRes))
			{
				CComPtr<ITypeInfo> spInfo(spTypeInfo);
				CComPtr<ITypeInfo2> spTypeInfo2;
				if (SUCCEEDED(spTypeInfo->QueryInterface(&spTypeInfo2)))
					spInfo = spTypeInfo2;

				LoadNameCache(spInfo);
				m_pInfo = spInfo.Detach();
			}
			pTypeLib->Release();
		}
	}
	LeaveCriticalSection(&_Module.m_csTypeInfoHolder);
	_Module.AddTermFunc(Cleanup, (DWORD)this);
	return hRes;
}

#endif // _NEWOBJECTS_ATL_TYPEINFOEX_H_