#pragma once

#include <oleauto.h>
#include <string>
#include <vector>
#include <map>

class WinMergePluginBase : public IDispatch, public ITypeInfo
{
public:
	enum
	{
		DISPID_PluginEvent = 1,
		DISPID_PluginDescription,
		DISPID_PluginIsAutomatic,
		DISPID_PluginFileFilters,
		DISPID_PluginUnpackedFileExtension,
		DISPID_PluginExtendedProperties,
		DISPID_PluginArguments,
		DISPID_PluginVariables,
		DISPID_PrediffFile,
		DISPID_UnpackFile,
		DISPID_PackFile,
		DISPID_IsFolder,
		DISPID_UnpackFolder,
		DISPID_PackFolder,
		DISPID_ShowSettingsDialog,
	};
	using ScriptFuncPtr = HRESULT(STDMETHODCALLTYPE*)(IDispatch* pDispatch, BSTR bstrText, BSTR* pbstrResult);

	WinMergePluginBase(const std::wstring& sEvent, const std::wstring& sDescription = L"",
		const std::wstring& sFileFilters = L"", const std::wstring& sUnpackedFileExtension = L"", 
		const std::wstring& sExtendedProperties = L"", const std::wstring& sArguments = L"", 
		bool bIsAutomatic = true)
		: m_nRef(0)
		, m_sEvent(sEvent)
		, m_sDescription(sDescription)
		, m_sFileFilters(sFileFilters)
		, m_sUnpackedFileExtension(sUnpackedFileExtension)
		, m_sExtendedProperties(sExtendedProperties)
		, m_sArguments(sArguments)
		, m_bIsAutomatic(bIsAutomatic)
	{
		static PARAMDATA paramData_Prediff[] =
		{ {L"fileSrc", VT_BSTR}, {L"fileDst", VT_BSTR}, {L"pbChanged", VT_BOOL | VT_BYREF}, };
		static PARAMDATA paramData_UnpackFile[] =
		{ {L"fileSrc", VT_BSTR}, {L"fileDst", VT_BSTR}, {L"pbChanged", VT_BOOL | VT_BYREF}, {L"pSubcode", VT_I4 | VT_BYREF}, };
		static PARAMDATA paramData_PackFile[] =
		{ {L"fileSrc", VT_BSTR}, {L"fileDst", VT_BSTR}, {L"pbChanged", VT_BOOL | VT_BYREF}, {L"subcode", VT_I4}, };
		static PARAMDATA paramData_IsFolder[] =
		{ {L"fileSrc", VT_BSTR}, };
		static PARAMDATA paramData_Arguments[] =
		{ {L"args", VT_BSTR}, };
		static PARAMDATA paramData_Variables[] =
		{ {L"vars", VT_BSTR}, };
		static PARAMDATA paramData_UnpackFolder[] =
		{ {L"fileSrc", VT_BSTR}, {L"folderDst", VT_BSTR}, {L"pbChanged", VT_BOOL | VT_BYREF}, {L"pSubcode", VT_I4 | VT_BYREF}, };
		static PARAMDATA paramData_PackFolder[] =
		{ {L"folderSrc", VT_BSTR}, {L"fileDst", VT_BSTR}, {L"pbChanged", VT_BOOL | VT_BYREF}, {L"subcode", VT_I4}, };
		static METHODDATA methodData_FILE_PREDIFF[] =
		{
			{ L"PluginEvent",                 nullptr,                DISPID_PluginEvent,                 0, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginDescription",           nullptr,                DISPID_PluginDescription,           1, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginIsAutomatic",           nullptr,                DISPID_PluginIsAutomatic,           2, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BOOL },
			{ L"PluginFileFilters",           nullptr,                DISPID_PluginFileFilters,           3, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginExtendedProperties",    nullptr,                DISPID_PluginExtendedProperties,    4, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginArguments",             nullptr,                DISPID_PluginArguments,             5, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginArguments",             paramData_Arguments,    DISPID_PluginArguments,             5, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
			{ L"PluginVariables",             nullptr,                DISPID_PluginVariables,             6, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginVariables",             paramData_Variables,    DISPID_PluginVariables,             6, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
			{ L"PrediffFile",                 paramData_Prediff,      DISPID_PrediffFile,                 7, CC_STDCALL, 3, DISPATCH_METHOD,      VT_BOOL },
			{ L"ShowSettingsDialog",          nullptr,                DISPID_ShowSettingsDialog,          8, CC_STDCALL, 0, DISPATCH_METHOD,      VT_VOID },
		};
		static METHODDATA methodData_FILE_PACK_UNPACK[] =
		{
			{ L"PluginEvent",                 nullptr,                DISPID_PluginEvent,                 0, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR  },
			{ L"PluginDescription",           nullptr,                DISPID_PluginDescription,           1, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR  },
			{ L"PluginIsAutomatic",           nullptr,                DISPID_PluginIsAutomatic,           2, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BOOL  },
			{ L"PluginFileFilters",           nullptr,                DISPID_PluginFileFilters,           3, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR  },
			{ L"PluginExtendedProperties",    nullptr,                DISPID_PluginExtendedProperties,    4, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR  },
			{ L"PluginUnpackedFileExtension", nullptr,                DISPID_PluginUnpackedFileExtension, 5, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR  },
			{ L"PluginArguments",             nullptr,                DISPID_PluginArguments,             6, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginArguments",             paramData_Arguments,    DISPID_PluginArguments,             6, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
			{ L"PluginVariables",             nullptr,                DISPID_PluginVariables,             7, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginVariables",             paramData_Variables,    DISPID_PluginVariables,             7, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
			{ L"UnpackFile",                  paramData_UnpackFile,   DISPID_UnpackFile,                  8, CC_STDCALL, 4, DISPATCH_METHOD,      VT_BOOL  },
			{ L"PackFile",                    paramData_PackFile,     DISPID_PackFile ,                   9, CC_STDCALL, 4, DISPATCH_METHOD,      VT_BOOL  },
			{ L"ShowSettingsDialog",          nullptr,                DISPID_ShowSettingsDialog,         10, CC_STDCALL, 0, DISPATCH_METHOD,      VT_VOID  },
		};
		static METHODDATA methodData_FILE_FOLDER_PACK_UNPACK[] =
		{
			{ L"PluginEvent",                 nullptr,                DISPID_PluginEvent,                 0, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginDescription",           nullptr,                DISPID_PluginDescription,           1, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginIsAutomatic",           nullptr,                DISPID_PluginIsAutomatic,           2, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BOOL },
			{ L"PluginFileFilters",           nullptr,                DISPID_PluginFileFilters,           3, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginExtendedProperties",    nullptr,                DISPID_PluginExtendedProperties,    4, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginUnpackedFileExtension", nullptr,                DISPID_PluginUnpackedFileExtension, 5, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginArguments",             nullptr,                DISPID_PluginArguments,             6, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginArguments",             paramData_Arguments,    DISPID_PluginArguments,             6, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
			{ L"PluginVariables",             nullptr,                DISPID_PluginVariables,             7, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginVariables",             paramData_Variables,    DISPID_PluginVariables,             7, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
			{ L"UnpackFile",                  paramData_UnpackFile,   DISPID_UnpackFile,                  8, CC_STDCALL, 4, DISPATCH_METHOD,      VT_BOOL },
			{ L"PackFile",                    paramData_PackFile,     DISPID_PackFile ,                   9, CC_STDCALL, 4, DISPATCH_METHOD,      VT_BOOL },
			{ L"IsFolder",                    paramData_IsFolder,     DISPID_IsFolder,                   10, CC_STDCALL, 1, DISPATCH_METHOD,      VT_BOOL },
			{ L"UnpackFolder",                paramData_UnpackFolder, DISPID_UnpackFolder,               11, CC_STDCALL, 4, DISPATCH_METHOD,      VT_BOOL },
			{ L"PackFolder",                  paramData_PackFolder,   DISPID_PackFolder,                 12, CC_STDCALL, 4, DISPATCH_METHOD,      VT_BOOL },
			{ L"ShowSettingsDialog",          nullptr,                DISPID_ShowSettingsDialog,         13, CC_STDCALL, 0, DISPATCH_METHOD,      VT_VOID },
		};
		static METHODDATA methodData_EDITOR_SCRIPT[] =
		{
			{ L"PluginEvent",                 nullptr,                DISPID_PluginEvent,                 0, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginDescription",           nullptr,                DISPID_PluginDescription,           1, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginFileFilters",           nullptr,                DISPID_PluginFileFilters,           2, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginExtendedProperties",    nullptr,                DISPID_PluginExtendedProperties,    3, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginArguments",             nullptr,                DISPID_PluginArguments,             4, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginArguments",             paramData_Arguments,    DISPID_PluginArguments,             4, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
			{ L"PluginVariables",             nullptr,                DISPID_PluginVariables,             5, CC_STDCALL, 0, DISPATCH_PROPERTYGET, VT_BSTR },
			{ L"PluginVariables",             paramData_Variables,    DISPID_PluginVariables,             5, CC_STDCALL, 1, DISPATCH_PROPERTYPUT, VT_VOID },
		};
		const METHODDATA* pMethodData;
		size_t methodDataCount = 0;
		if (sEvent == L"FILE_PREDIFF")
		{
			methodDataCount = sizeof(methodData_FILE_PREDIFF) / sizeof(methodData_FILE_PREDIFF[0]);
			pMethodData = methodData_FILE_PREDIFF;
		}
		else if (sEvent == L"FILE_PACK_UNPACK")
		{
			methodDataCount = sizeof(methodData_FILE_PACK_UNPACK) / sizeof(methodData_FILE_PACK_UNPACK[0]);
			pMethodData = methodData_FILE_PACK_UNPACK;
		}
		else if (sEvent == L"FILE_FOLDER_PACK_UNPACK" || sEvent == L"URL_PACK_UNPACK")
		{
			methodDataCount = sizeof(methodData_FILE_FOLDER_PACK_UNPACK) / sizeof(methodData_FILE_FOLDER_PACK_UNPACK[0]);
			pMethodData = methodData_FILE_FOLDER_PACK_UNPACK;
		}
		else
		{
			methodDataCount = sizeof(methodData_EDITOR_SCRIPT) / sizeof(methodData_EDITOR_SCRIPT[0]);
			pMethodData = methodData_EDITOR_SCRIPT;
		}
		for (size_t i = 0; i < methodDataCount; ++i)
		{
			auto& methodData = pMethodData[i];
			m_mapNameToIndex.insert_or_assign(methodData.szName, static_cast<int>(m_methodData.size()));
			m_mapDispIdToIndex.insert_or_assign(methodData.dispid, static_cast<int>(m_methodData.size()));
			m_methodData.push_back(methodData);
		}
	}

	virtual ~WinMergePluginBase() {}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override
	{
		return E_NOTIMPL;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) override
	{
		return ++m_nRef;
	}

	ULONG STDMETHODCALLTYPE Release(void) override
	{
		if (--m_nRef == 0)
		{
			delete this;
			return 0;
		}
		return m_nRef;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo) override
	{
		*pctinfo = 1;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override
	{
		*ppTInfo = this;
		(*ppTInfo)->AddRef();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override
	{
		for (unsigned i = 0; i < cNames; ++i)
		{
			auto it = m_mapNameToIndex.find(rgszNames[i]);
			if (it == m_mapNameToIndex.end())
				return DISP_E_UNKNOWNNAME;
			rgDispId[i] = m_methodData[it->second].dispid;
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override
	{
		if (!pDispParams)
			return DISP_E_BADVARTYPE;
		HRESULT hr = E_NOTIMPL;
		if (wFlags & DISPATCH_METHOD)
		{
			switch (dispIdMember)
			{
			case DISPID_PrediffFile:
			{
				BSTR fileSrc = pDispParams->rgvarg[2].bstrVal;
				BSTR fileDst = pDispParams->rgvarg[1].bstrVal;
				VARIANT_BOOL* pbChanged = pDispParams->rgvarg[0].pboolVal;
				VARIANT_BOOL* pbSuccess = &pVarResult->boolVal;
				hr = PrediffFile(fileSrc, fileDst, pbChanged, pbSuccess);
				break;
			}
			case DISPID_UnpackFile:
			{
				BSTR fileSrc = pDispParams->rgvarg[3].bstrVal;
				BSTR fileDst = pDispParams->rgvarg[2].bstrVal;
				VARIANT_BOOL* pbChanged = pDispParams->rgvarg[1].pboolVal;
				INT* pSubcode = pDispParams->rgvarg[0].pintVal;
				VARIANT_BOOL* pbSuccess = &pVarResult->boolVal;
				hr = UnpackFile(fileSrc, fileDst, pbChanged, pSubcode, pbSuccess);
				break;
			}
			case DISPID_PackFile:
			{
				BSTR fileSrc = pDispParams->rgvarg[3].bstrVal;
				BSTR fileDst = pDispParams->rgvarg[2].bstrVal;
				VARIANT_BOOL* pbChanged = pDispParams->rgvarg[1].pboolVal;
				INT subcode = pDispParams->rgvarg[0].intVal;
				VARIANT_BOOL* pbSuccess = &pVarResult->boolVal;
				hr = PackFile(fileSrc, fileDst, pbChanged, subcode, pbSuccess);
				break;
			}
			case DISPID_IsFolder:
			{
				BSTR file = pDispParams->rgvarg[0].bstrVal;
				VARIANT_BOOL* pbSuccess = &pVarResult->boolVal;
				hr = IsFolder(file, pbSuccess);
				break;
			}
			case DISPID_UnpackFolder:
			{
				BSTR fileSrc = pDispParams->rgvarg[3].bstrVal;
				BSTR folderDst = pDispParams->rgvarg[2].bstrVal;
				VARIANT_BOOL* pbChanged = pDispParams->rgvarg[1].pboolVal;
				INT* pSubcode = pDispParams->rgvarg[0].pintVal;
				VARIANT_BOOL* pbSuccess = &pVarResult->boolVal;
				hr = UnpackFolder(fileSrc, folderDst, pbChanged, pSubcode, pbSuccess);
				break;
			}
			case DISPID_PackFolder:
			{
				BSTR folderSrc = pDispParams->rgvarg[3].bstrVal;
				BSTR fileDst = pDispParams->rgvarg[2].bstrVal;
				VARIANT_BOOL* pbChanged = pDispParams->rgvarg[1].pboolVal;
				INT subcode = pDispParams->rgvarg[0].intVal;
				VARIANT_BOOL* pbSuccess = &pVarResult->boolVal;
				hr = PackFolder(folderSrc, fileDst, pbChanged, subcode, pbSuccess);
				break;
			}
			case DISPID_ShowSettingsDialog:
				hr = ShowSettingsDialog(&pVarResult->boolVal);
				break;
			default:
				if (m_mapDispIdToIndex.find(dispIdMember) != m_mapDispIdToIndex.end())
				{
					BSTR bstrText = pDispParams->rgvarg[0].bstrVal;
					pVarResult->vt = VT_BSTR;
					BSTR* pbstrResult = &pVarResult->bstrVal;
					hr = m_mapScriptFuncs[dispIdMember].second(this, bstrText, pbstrResult);
				}
				break;
			}
		}
		else if (wFlags & DISPATCH_PROPERTYGET)
		{
			switch (dispIdMember)
			{
			case DISPID_PluginEvent:
				pVarResult->vt = VT_BSTR;
				hr = get_PluginEvent(&pVarResult->bstrVal);
				break;
			case DISPID_PluginDescription:
				pVarResult->vt = VT_BSTR;
				hr = get_PluginDescription(&pVarResult->bstrVal);
				break;
			case DISPID_PluginIsAutomatic:
				pVarResult->vt = VT_BOOL;
				hr = get_PluginIsAutomatic(&pVarResult->boolVal);
				break;
			case DISPID_PluginFileFilters:
				pVarResult->vt = VT_BSTR;
				hr = get_PluginFileFilters(&pVarResult->bstrVal);
				break;
			case DISPID_PluginUnpackedFileExtension:
				pVarResult->vt = VT_BSTR;
				hr = get_PluginUnpackedFileExtension(&pVarResult->bstrVal);
				break;
			case DISPID_PluginExtendedProperties:
				pVarResult->vt = VT_BSTR;
				hr = get_PluginExtendedProperties(&pVarResult->bstrVal);
				break;
			case DISPID_PluginArguments:
				pVarResult->vt = VT_BSTR;
				hr = get_PluginArguments(&pVarResult->bstrVal);
				break;
			case DISPID_PluginVariables:
				pVarResult->vt = VT_BSTR;
				hr = get_PluginVariables(&pVarResult->bstrVal);
				break;
			}
		}
		else if (wFlags & DISPATCH_PROPERTYPUT)
		{
			switch (dispIdMember)
			{
			case DISPID_PluginArguments:
			{
				BSTR bstrArgs = pDispParams->rgvarg[0].bstrVal;
				hr = put_PluginArguments(bstrArgs);
				break;
			}
			case DISPID_PluginVariables:
			{
				BSTR bstrVars = pDispParams->rgvarg[0].bstrVal;
				hr = put_PluginVariables(bstrVars);
				break;
			}
			}
		}
		if (hr == DISP_E_EXCEPTION && pExcepInfo)
		{
			IErrorInfo* pErrorInfo = nullptr;
			GetErrorInfo(0, &pErrorInfo);
			pErrorInfo->GetDescription(&pExcepInfo->bstrDescription);
			pErrorInfo->GetSource(&pExcepInfo->bstrSource);
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE GetTypeAttr(TYPEATTR** ppTypeAttr) override
	{
		auto* pTypeAttr = new TYPEATTR();
		pTypeAttr->cFuncs = static_cast<WORD>(m_methodData.size());
		pTypeAttr->typekind = TKIND_DISPATCH;
		pTypeAttr->cbAlignment = 8;
		*ppTypeAttr = pTypeAttr;
		return S_OK;
	}
	
	HRESULT STDMETHODCALLTYPE GetTypeComp(ITypeComp** ppTComp) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetFuncDesc(UINT index, FUNCDESC** ppFuncDesc) override
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
			for (int i = 0; i < methodData.cArgs; ++i)
				pFuncDesc->lprgelemdescParam[i].tdesc.vt = methodData.ppdata[i].vt;
		}
		pFuncDesc->elemdescFunc.tdesc.vt = methodData.vtReturn;
		*ppFuncDesc = pFuncDesc;
		return S_OK;
	}
	
	HRESULT STDMETHODCALLTYPE GetVarDesc(UINT index, VARDESC** ppVarDesc) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetNames(MEMBERID memid, BSTR *rgBstrNames, UINT cMaxNames, UINT *pcNames) override
	{
		if (m_mapDispIdToIndex.find(memid) == m_mapDispIdToIndex.end())
			return E_INVALIDARG;
		const METHODDATA& methodData = m_methodData[m_mapDispIdToIndex[memid]];
		for (int i = 0; i < cMaxNames && i < methodData.cArgs + 1; i++)
		{
			if (i == 0)
				rgBstrNames[i] = SysAllocString(methodData.szName);
			else
				rgBstrNames[i] = SysAllocString(methodData.ppdata[i - 1].szName);
			*pcNames = i + 1;
		}
		return S_OK;
	}
	
	HRESULT STDMETHODCALLTYPE GetRefTypeOfImplType(UINT index, HREFTYPE *pRefType) override
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetImplTypeFlags(UINT index, INT *pImplTypeFlags) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetIDsOfNames(LPOLESTR* rgszNames, UINT cNames, MEMBERID* pMemId) override
	{
		return GetIDsOfNames(IID_NULL, rgszNames, cNames, 0, pMemId);
	}
	
	HRESULT STDMETHODCALLTYPE Invoke(PVOID pvInstance, MEMBERID memid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) override
	{
		return reinterpret_cast<IDispatch*>(pvInstance)->Invoke(memid, IID_NULL, 0, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	}
	
	HRESULT STDMETHODCALLTYPE GetDocumentation(MEMBERID memid, BSTR *pBstrName, BSTR *pBstrDocString, DWORD *pdwHelpContext, BSTR *pBstrHelpFile) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetDllEntry(MEMBERID memid, INVOKEKIND invKind, BSTR *pBstrDllName, BSTR *pBstrName, WORD *pwOrdinal)
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetRefTypeInfo(HREFTYPE hRefType, ITypeInfo **ppTInfo) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE AddressOfMember(MEMBERID memid, INVOKEKIND invKind, PVOID *ppv) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown *pUnkOuter, REFIID riid, PVOID *ppvObj) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetMops(MEMBERID memid, BSTR *pBstrMops) override
	{
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE GetContainingTypeLib(ITypeLib **ppTLib, UINT *pIndex) override
	{
		return E_NOTIMPL;
	}
	
	void STDMETHODCALLTYPE ReleaseTypeAttr(TYPEATTR *pTypeAttr) override
	{
		delete pTypeAttr;
	}
	
	void STDMETHODCALLTYPE ReleaseFuncDesc(FUNCDESC *pFuncDesc) override
	{
		delete[] pFuncDesc->lprgelemdescParam;
		delete pFuncDesc;
	}
	
	void STDMETHODCALLTYPE ReleaseVarDesc(VARDESC *pVarDesc) override
	{
	}
	
	virtual HRESULT STDMETHODCALLTYPE PrediffFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, VARIANT_BOOL* pbSuccess)
	{
		*pbSuccess = VARIANT_FALSE;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT subcode, VARIANT_BOOL* pbSuccess)
	{
		*pbSuccess = VARIANT_FALSE;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT* pSubcode, VARIANT_BOOL* pbSuccess)
	{
		*pbSuccess = VARIANT_FALSE;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE IsFolder(BSTR file, VARIANT_BOOL* pbFolder)
	{
		*pbFolder = VARIANT_FALSE;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE PackFolder(BSTR folderSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT subcode, VARIANT_BOOL* pbSuccess)
	{
		*pbSuccess = VARIANT_FALSE;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE UnpackFolder(BSTR fileSrc, BSTR folderDst, VARIANT_BOOL* pbChanged, INT* pSubcode, VARIANT_BOOL* pbSuccess)
	{
		*pbSuccess = VARIANT_FALSE;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE ShowSettingsDialog(VARIANT_BOOL* pbHandled)
	{
		*pbHandled = VARIANT_FALSE;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginEvent(BSTR* pVal)
	{
		*pVal = SysAllocString(m_sEvent.c_str());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginDescription(BSTR* pVal)
	{
		*pVal = SysAllocString(m_sDescription.c_str());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginIsAutomatic(VARIANT_BOOL* pVal)
	{
		*pVal = m_bIsAutomatic ? -1 : 0;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginFileFilters(BSTR* pVal)
	{
		*pVal = SysAllocString(m_sFileFilters.c_str());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginUnpackedFileExtension(BSTR* pVal)
	{
		*pVal = SysAllocString(m_sUnpackedFileExtension.c_str());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginExtendedProperties(BSTR* pVal)
	{
		*pVal = SysAllocString(m_sExtendedProperties.c_str());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginArguments(BSTR* pVal)
	{
		*pVal = SysAllocString(m_sArguments.c_str());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE put_PluginArguments(BSTR val)
	{
		m_sArguments = std::wstring{ val, SysStringLen(val) };
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_PluginVariables(BSTR* pVal)
	{
		*pVal = SysAllocString(m_sVariables.c_str());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE put_PluginVariables(BSTR val)
	{
		m_sVariables = std::wstring{ val, SysStringLen(val) };
		return S_OK;
	}

	bool AddFunction(const std::wstring& name, ScriptFuncPtr pFunc)
	{
		static PARAMDATA paramData_ScriptFunc[] =
		{ {L"text", VT_BSTR} };
		unsigned index = static_cast<int>(m_methodData.size());
		DISPID dispid = static_cast<DISPID>(m_methodData.size()) + 100;
		m_mapNameToIndex.insert_or_assign(name, index);
		m_mapDispIdToIndex.insert_or_assign(dispid, index);
		m_mapScriptFuncs.insert_or_assign(dispid, std::pair{name, pFunc});
		METHODDATA methodData =
		{
			const_cast<OLECHAR *>(m_mapScriptFuncs[dispid].first.c_str()), paramData_ScriptFunc, dispid, index, CC_STDCALL, 1, DISPATCH_METHOD, VT_BSTR
		};
		m_methodData.emplace_back(methodData);
		return true;
	}

protected:
	int m_nRef;
	std::map<std::wstring, int> m_mapNameToIndex;
	std::map<DISPID, int> m_mapDispIdToIndex;
	std::vector<METHODDATA> m_methodData;
	std::map<DISPID, std::pair<std::wstring, ScriptFuncPtr>> m_mapScriptFuncs;
	std::wstring m_sEvent;
	std::wstring m_sDescription;
	std::wstring m_sFileFilters;
	std::wstring m_sUnpackedFileExtension;
	std::wstring m_sExtendedProperties;
	std::wstring m_sArguments;
	std::wstring m_sVariables;
	bool m_bIsAutomatic;
};

