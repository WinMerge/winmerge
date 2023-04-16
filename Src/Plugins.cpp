/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file Plugins.cpp
 *
 *  @brief Support for VBS Scriptlets, VB ActiveX DLL, VC++ COM DLL
 */ 

#include "pch.h"
#include "Plugins.h"
#define POCO_NO_UNWINDOWS 1
#include <vector>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <set>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <Poco/RegularExpression.h>
#include <windows.h>
#include "MergeApp.h"
#include "unicoder.h"
#include "FileFilterMgr.h"
#include "lwdisp.h"
#include "resource.h"
#include "Exceptions.h"
#include "RegKey.h"
#include "paths.h"
#include "Environment.h"
#include "FileFilter.h"
#include "coretools.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"

using std::vector;
using Poco::RegularExpression;
using Poco::FastMutex;
using Poco::ScopedLock;

/**
 * @brief Category of transformation : define the transformation events
 *
 * @note USER categories are calls to scriptlets, or VB ActiveX DLL, or VC COM DLL
 * Use text definition : if you add one, nothing to do ; 
 * if you change one, you just have change the dll/scripts for that event
 */
const wchar_t *TransformationCategories[] = 
{
	L"BUFFER_PREDIFF",
	L"FILE_PREDIFF",
	L"EDITOR_SCRIPT",
	L"BUFFER_PACK_UNPACK",
	L"FILE_PACK_UNPACK",
	L"FILE_FOLDER_PACK_UNPACK",
	L"URL_PACK_UNPACK",
	nullptr,		// last empty : necessary
};

static vector<String> theScriptletList;
/// Need to lock the *.sct so the user can't delete them
static vector<HANDLE> theScriptletHandleList;
static bool scriptletsLoaded=false;
static FastMutex scriptletsSem;
static std::unordered_map<String, std::unordered_map<String, String>> customSettingsMap;

template<class T> struct AutoReleaser
{
	explicit AutoReleaser(T *ptr) : p(ptr) {}
	~AutoReleaser() { if (p!=nullptr) p->Release(); }
	T *p;
};

////////////////////////////////////////////////////////////////////////////////

namespace plugin
{

/**
 * @brief Check for the presence of Windows Script
 *
 * .sct plugins require this optional component
 */
bool IsWindowsScriptThere()
{
	CRegKeyEx keyFile;
	if (!keyFile.QueryRegMachine(_T("SOFTWARE\\Classes\\scriptletfile\\AutoRegister")))
		return false;

	String filename = keyFile.ReadString(_T(""), _T(""));
	keyFile.Close();
	if (filename.empty())
		return false;

	return (paths::DoesPathExist(filename) == paths::IS_EXISTING_FILE);
}

////////////////////////////////////////////////////////////////////////////////
// scriptlet/activeX support for function names

// list the function IDs and names in a script or activeX dll
int GetFunctionsFromScript(IDispatch *piDispatch, vector<String>& namesArray, vector<int>& IdArray, INVOKEKIND wantedKind)
{
	UINT iValidFunc = 0;
	if (piDispatch != nullptr)
	{
		ITypeInfo *piTypeInfo=nullptr;
		unsigned  iTInfo = 0; // 0 for type information of IDispatch itself
		LCID  lcid=0; // locale for localized method names (ignore if no localized names)
		if (SUCCEEDED(piDispatch->GetTypeInfo(iTInfo, lcid, &piTypeInfo)))
		{
			TYPEATTR *pTypeAttr=nullptr;
			if (SUCCEEDED(piTypeInfo->GetTypeAttr(&pTypeAttr)))
			{
				// allocate arrays for the returned structures
				// the names array is NUL terminated
				namesArray.resize(pTypeAttr->cFuncs+1);
				IdArray.resize(pTypeAttr->cFuncs+1);

				UINT iMaxFunc = pTypeAttr->cFuncs - 1;
				for (UINT iFunc = 0 ; iFunc <= iMaxFunc ; ++iFunc)
				{
					UINT iFuncDesc = iMaxFunc - iFunc;
					FUNCDESC *pFuncDesc;
					if (SUCCEEDED(piTypeInfo->GetFuncDesc(iFuncDesc, &pFuncDesc)))
					{
						// exclude properties
						// exclude IDispatch inherited methods
						if (pFuncDesc->invkind & wantedKind && !(pFuncDesc->wFuncFlags & 1))
						{
							BSTR bstrName;
							UINT cNames;
							if (SUCCEEDED(piTypeInfo->GetNames(pFuncDesc->memid,
								&bstrName, 1, &cNames)))
							{
								IdArray[iValidFunc] = pFuncDesc->memid;
								namesArray[iValidFunc] = ucr::toTString(bstrName);
								iValidFunc ++;
							}
							SysFreeString(bstrName);
						}
						piTypeInfo->ReleaseFuncDesc(pFuncDesc);
					}
				}
				piTypeInfo->ReleaseTypeAttr(pTypeAttr);
			}
			piTypeInfo->Release();
		}
	}
	return iValidFunc;
}

int GetMethodsFromScript(IDispatch *piDispatch, vector<String>& namesArray, vector<int> &IdArray)
{
	return GetFunctionsFromScript(piDispatch, namesArray, IdArray, INVOKE_FUNC);
}
int GetPropertyGetsFromScript(IDispatch *piDispatch, vector<String>& namesArray, vector<int> &IdArray)
{
	return GetFunctionsFromScript(piDispatch, namesArray, IdArray, INVOKE_PROPERTYGET);
}


/** 
 * @return ID of the function or -1 if no function with this index
 */
int GetMethodIDInScript(LPDISPATCH piDispatch, int methodIndex)
{
	int fncID;

	vector<String> namesArray;
	vector<int> IdArray;
	const int nFnc = GetMethodsFromScript(piDispatch, namesArray, IdArray);

	if (methodIndex < nFnc)
	{
		fncID = IdArray[methodIndex];
	}
	else
	{
		fncID = -1;
	}
	
	return fncID;
}

}

////////////////////////////////////////////////////////////////////////////////
// find scripts/activeX for an event : each event is assigned to a subdirectory 


/**
 * @brief Get a list of scriptlet file
 *
 * @return Returns an array of LPSTR
 */
static void GetScriptletsAt(const String& sSearchPath, const String& extension, vector<String>& scriptlets )
{
	WIN32_FIND_DATA ffi;
	String strFileSpec = paths::ConcatPath(sSearchPath, _T("*") + extension);
	HANDLE hff = FindFirstFile(strFileSpec.c_str(), &ffi);
	
	if (  hff != INVALID_HANDLE_VALUE )
	{
		do
		{
			if (!(ffi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				strFileSpec = paths::ConcatPath(sSearchPath, ffi.cFileName);
				if (strFileSpec.substr(strFileSpec.length() - extension.length()) == extension) // excludes *.sct~ files
					scriptlets.push_back(strFileSpec);  
			}
		}
		while (FindNextFile(hff, &ffi));
		FindClose(hff);
	}
}

PluginInfo::PluginInfo()
	: m_lpDispatch(nullptr)
	, m_filters(NULL)
	, m_bAutomatic(false)
	, m_nFreeFunctions(0)
	, m_disabled(false)
	, m_hasArgumentsProperty(false)
	, m_hasVariablesProperty(false)
	, m_hasPluginOnEventMethod(false)
	, m_bAutomaticDefault(false)
{	
}

PluginInfo::~PluginInfo()
{
	if (m_lpDispatch != nullptr)
	{
		if (m_hasPluginOnEventMethod)
			plugin::InvokePluginOnEvent(EVENTID_TERMINATE, m_lpDispatch);
		m_lpDispatch->Release();
	}
}

void PluginInfo::LoadFilterString()
{
	m_filters.clear();

	String sLine(m_filtersText);
	String sPiece;

	while(1)
	{
		String::size_type pos = sLine.rfind(';');
		sPiece = sLine.substr(pos+1);
		if (pos == String::npos)
			pos = 0;
		sLine = sLine.substr(0, pos);
		if (sPiece.empty())
			break;

		sPiece = strutils::makeupper(strutils::trim_ws_begin(sPiece));

		int re_opts = 0;
		std::string regexString = ucr::toUTF8(sPiece);
		re_opts |= RegularExpression::RE_UTF8;
		try
		{
			m_filters.emplace_back(std::make_shared<FileFilterElement>(regexString, re_opts, true));
		}
		catch (...)
		{
			// TODO:
		}
	};
}


bool PluginInfo::TestAgainstRegList(const String& szTest) const
{
	if (m_filters.empty() || szTest.empty())
		return false;

	String sLine = szTest;
	String sPiece;

	while(!sLine.empty())
	{
		String::size_type pos = sLine.rfind('|');
		sPiece = sLine.substr(pos+1);
		if (pos == String::npos)
			pos = 0;
		sLine = sLine.substr(0, pos);
		sPiece = strutils::makeupper(strutils::trim_ws_begin(sPiece));

		if (::TestAgainstRegList(&m_filters, sPiece))
			return true;
	};

	return false;
}

std::optional<StringView> PluginInfo::GetExtendedPropertyValue(const String& name) const
{
	for (auto& item : strutils::split(m_extendedProperties, ';'))
	{
		auto keyvalue = strutils::split(item, '=');
		if (keyvalue[0] == name)
		{
			if (keyvalue.size() > 1)
				return keyvalue[1];
			else
				return _("");
		}
	}
	return {};
}

/**
 * @brief Log technical explanation, in English, of script error
 */
static void
ScriptletError(const String & scriptletFilepath, const tchar_t *szError)
{
	String msg = _T("Plugin scriptlet error <")
		+ scriptletFilepath
		+ _T("> ")
		+ szError;
    LogErrorString(msg);
}

static String
Decode(const String& text)
{
	String result;
	for (size_t i = 0; i < text.length(); ++i)
	{
		if (text[i] == '%')
		{
			if (i < text.length() - 1 && text[i + 1] == '%')
			{
				result += '%';
				i++;
			}
			else if (i < text.length() - 2 && text[i + 1] == '3' && text[i + 2] == 'A')
			{
				result += ':';
				i += 2;
			}
			else if (i < text.length() - 2 && text[i + 1] == '7' && text[i + 2] == 'C')
			{
				result += '|';
				i += 2;
			}
			else
				result += text[i];
		}
		else
			result += text[i];
	}
	return result;
}

static String
Encode(const String& text)
{
	String result;
	for (size_t i = 0; i < text.length(); ++i)
	{
		switch (text[i])
		{
		case '%': result += _T("%%");  break;
		case ':': result += _T("%3A"); break;
		case '|': result += _T("%7C"); break;
		default:  result += text[i];   break;
		}
	}
	return result;
}

static std::unordered_map<String, std::unordered_map<String, String>> GetCustomSettingsMap()
{
	std::unordered_map<String, std::unordered_map<String, String>> map;
	const String& text = GetOptionsMgr()->GetString(OPT_PLUGINS_CUSTOM_SETTINGS_LIST);
	for (const auto& nameAndKeyValues : strutils::split(text, '\t'))
	{
		auto nv = strutils::split(nameAndKeyValues, '=');
		if (nv.size() == 2)
		{
			String nv0{ nv[0].data(), nv[0].length() };
			String nv1{ nv[1].data(), nv[1].length() };
			map.insert_or_assign(nv0, std::unordered_map<String, String>{});
			for (const auto& keyValue : strutils::split(nv1, '|'))
			{
				const auto kv = strutils::split(keyValue, ':');
				String kv0{ kv[0].data(), kv[0].length() };
				String kv1{ kv.size() > 1 ? Decode({kv[1].data(), kv[1].length()}) : _T("") };
				map[nv0].insert_or_assign(kv0, kv1);
			}
		}
	}
	map.insert_or_assign(_T("||initialized||"), std::unordered_map<String, String>{});
	return map;
}

static String GetCustomSetting(const String& name, const String& key, const String& default)
{
	FastMutex::ScopedLock lock(scriptletsSem);
	if (customSettingsMap.empty())
		customSettingsMap = GetCustomSettingsMap();
	if (customSettingsMap.find(name) != customSettingsMap.end()
		&& customSettingsMap[name].find(key) != customSettingsMap[name].end())
	{
		return strutils::to_str(customSettingsMap[name][key]);
	}
	return default;
}

/**
 * @brief Tiny structure that remembers current scriptlet & event info for calling Log
 */
struct ScriptInfo
{
	ScriptInfo(const String & scriptletFilepath)
		: m_scriptletFilepath(scriptletFilepath)
	{
	}
	void Log(const tchar_t *szError)
	{
		ScriptletError(m_scriptletFilepath, szError);
	}
	const String & m_scriptletFilepath;
};

/**
 * @brief Try to load a plugin
 *
 * @return 1 if loaded plugin successfully, negatives for errors
 */
int PluginInfo::MakeInfo(const String & scriptletFilepath, IDispatch *lpDispatch)
{
	// set up object in case we need to log info
	ScriptInfo scinfo(scriptletFilepath);

	// Ensure that interface is released if any bad exit or exception
	AutoReleaser<IDispatch> drv(lpDispatch);

	std::vector<String> propNamesArray;
	std::vector<String> methodNamesArray;
	std::vector<int> IdArray;
	const int nPropFnc = plugin::GetPropertyGetsFromScript(lpDispatch, propNamesArray, IdArray);
	const int nMethodFnc = plugin::GetMethodsFromScript(lpDispatch, methodNamesArray, IdArray);
	propNamesArray.resize(nPropFnc);
	methodNamesArray.resize(nMethodFnc);
	auto SearchScriptForDefinedProperties = [&propNamesArray](const tchar_t* name) -> bool
	{ return std::find(propNamesArray.begin(), propNamesArray.end(), name) != propNamesArray.end(); };
	auto SearchScriptForMethodName = [&methodNamesArray](const tchar_t* name) -> bool
	{ return std::find(methodNamesArray.begin(), methodNamesArray.end(), name) != methodNamesArray.end(); };

	// Is this plugin for this transformationEvent ?
	VARIANT ret;
	// invoke mandatory method get PluginEvent
	VariantInit(&ret);
	if (!SearchScriptForDefinedProperties(L"PluginEvent"))
	{
		scinfo.Log(_T("PluginEvent method missing"));
		return -20; // error
	}
	HRESULT h = ::invokeW(lpDispatch, &ret, L"PluginEvent", opGet[0], nullptr);
	if (FAILED(h) || ret.vt != VT_BSTR)
	{
		scinfo.Log(	_T("Error accessing PluginEvent method"));
		return -30; // error
	}
	m_event = ucr::toTString(ret.bstrVal);

	VariantClear(&ret);

	// plugins PREDIFF or PACK_UNPACK : functions names are mandatory
	// Check that the plugin offers the requested functions
	// set the mode for the events which uses it
	bool bFound = true;
	if (m_event == _T("BUFFER_PREDIFF"))
	{
		bFound &= SearchScriptForMethodName(L"PrediffBufferW");
	}
	else if (m_event == _T("FILE_PREDIFF"))
	{
		bFound &= SearchScriptForMethodName(L"PrediffFile");
	}
	else if (m_event == _T("BUFFER_PACK_UNPACK"))
	{
		bFound &= SearchScriptForMethodName(L"UnpackBufferA");
		bFound &= SearchScriptForMethodName(L"PackBufferA");
	}
	else if (m_event == _T("FILE_PACK_UNPACK"))
	{
		bFound &= SearchScriptForMethodName(L"UnpackFile");
		bFound &= SearchScriptForMethodName(L"PackFile");
	}
	else if (m_event == _T("FILE_FOLDER_PACK_UNPACK") || m_event == _T("URL_PACK_UNPACK"))
	{
		bFound &= SearchScriptForMethodName(L"IsFolder");
		bFound &= SearchScriptForMethodName(L"UnpackFile");
		bFound &= SearchScriptForMethodName(L"PackFile");
		bFound &= SearchScriptForMethodName(L"UnpackFolder");
		bFound &= SearchScriptForMethodName(L"PackFolder");
	}
	if (!bFound)
	{
		// error (Plugin doesn't support the method as it claimed)
		scinfo.Log(_T("Plugin doesn't support the method as it claimed"));
		return -40; 
	}

	// plugins EDITOR_SCRIPT : functions names are free
	// there may be several functions inside one script, count the number of functions
	if (m_event == _T("EDITOR_SCRIPT"))
	{
		m_nFreeFunctions = static_cast<int>(methodNamesArray.size());
		if (m_nFreeFunctions == 0)
			// error (Plugin doesn't offer any method, what is this ?)
			return -50;
	}


	// get optional property PluginDescription
	if (SearchScriptForDefinedProperties(L"PluginDescription"))
	{
		h = ::invokeW(lpDispatch, &ret, L"PluginDescription", opGet[0], nullptr);
		if (FAILED(h) || ret.vt != VT_BSTR)
		{
			scinfo.Log(_T("Plugin had PluginDescription property, but error getting its value"));
			return -60; // error (Plugin had PluginDescription property, but error getting its value)
		}
		m_description = ucr::toTString(ret.bstrVal);
	}
	else
	{
		// no description, use filename
		m_description = paths::FindFileName(scriptletFilepath);
	}
	VariantClear(&ret);

	// get PluginFileFilters
	bool hasPluginFileFilters = false;
	if (SearchScriptForDefinedProperties(L"PluginFileFilters"))
	{
		h = ::invokeW(lpDispatch, &ret, L"PluginFileFilters", opGet[0], nullptr);
		if (FAILED(h) || ret.vt != VT_BSTR)
		{
			scinfo.Log(_T("Plugin had PluginFileFilters property, but error getting its value"));
			return -70; // error (Plugin had PluginFileFilters property, but error getting its value)
		}
		m_filtersTextDefault = ucr::toTString(ret.bstrVal);
		hasPluginFileFilters = true;
	}
	else
	{
		m_bAutomatic = false;
		m_filtersTextDefault = _T(".");
	}
	VariantClear(&ret);

	// get optional property PluginIsAutomatic
	if (SearchScriptForDefinedProperties(L"PluginIsAutomatic"))
	{
		h = ::invokeW(lpDispatch, &ret, L"PluginIsAutomatic", opGet[0], nullptr);
		if (FAILED(h) || ret.vt != VT_BOOL)
		{
			scinfo.Log(_T("Plugin had PluginIsAutomatic property, but error getting its value"));
			return -80; // error (Plugin had PluginIsAutomatic property, but error getting its value)
		}
		m_bAutomaticDefault = !!ret.boolVal;
	}
	else
	{
		if (hasPluginFileFilters && m_event != _T("EDITOR_SCRIPT"))
		{
			scinfo.Log(_T("Plugin had PluginFileFilters property, but lacked PluginIsAutomatic property"));
			// PluginIsAutomatic property is mandatory for Plugins with PluginFileFilters property
			return -90;
		}
		// default to false when Plugin doesn't have property
		m_bAutomaticDefault = false;
	}
	VariantClear(&ret);

	// get optional property PluginUnpackedFileExtenstion
	if (SearchScriptForDefinedProperties(L"PluginUnpackedFileExtension"))
	{
		h = ::invokeW(lpDispatch, &ret, L"PluginUnpackedFileExtension", opGet[0], nullptr);
		if (FAILED(h) || ret.vt != VT_BSTR)
		{
			scinfo.Log(_T("Plugin had PluginUnpackedFileExtension property, but error getting its value"));
			return -100; // error (Plugin had PluginUnpackedFileExtension property, but error getting its value)
		}
		m_ext = ucr::toTString(ret.bstrVal);
	}
	else
	{
		m_ext.clear();
	}
	VariantClear(&ret);

	// get optional property PluginExtendedProperties
	if (SearchScriptForDefinedProperties(_T("PluginExtendedProperties")))
	{
		h = ::invokeW(lpDispatch, &ret, L"PluginExtendedProperties", opGet[0], nullptr);
		if (FAILED(h) || ret.vt != VT_BSTR)
		{
			scinfo.Log(_T("Plugin had PluginExtendedProperties property, but error getting its value"));
			return -110; // error (Plugin had PluginExtendedProperties property, but error getting its value)
		}
		m_extendedProperties = ucr::toTString(ret.bstrVal);

	}
	else
	{
		m_extendedProperties.clear();
	}
	VariantClear(&ret);

	// get optional property PluginArguments
	if (SearchScriptForDefinedProperties(L"PluginArguments"))
	{
		h = ::invokeW(lpDispatch, &ret, L"PluginArguments", opGet[0], nullptr);
		if (FAILED(h) || ret.vt != VT_BSTR)
		{
			scinfo.Log(_T("Plugin had PluginArguments property, but error getting its value"));
			return -120; // error (Plugin had PluginArguments property, but error getting its value)
		}
		m_argumentsDefault = ucr::toTString(ret.bstrVal);
		m_hasArgumentsProperty = true;
	}
	else
	{
		m_argumentsDefault.clear();
		m_hasArgumentsProperty = false;
	}

	// get optional property PluginVariables
	m_hasVariablesProperty = SearchScriptForDefinedProperties(L"PluginVariables");

	// get optional method PluginOnEvent
	m_hasPluginOnEventMethod = SearchScriptForMethodName(L"PluginOnEvent");
	if (m_hasPluginOnEventMethod)
	{
		if (!plugin::InvokePluginOnEvent(EVENTID_INITIALIZE, lpDispatch))
		{
			scinfo.Log(_T("Plugin had PluginOnEvent method, but an error occurred while calling the method"));
			return -130; // error (Plugin had PluginOnEvent method, but an error occurred while calling the method)
		}
	}

	// keep the filename
	m_name = paths::FindFileName(scriptletFilepath);

	// Clear the autorelease holder
	drv.p = nullptr;

	m_lpDispatch = lpDispatch;

	m_filepath = scriptletFilepath;

	return 1;
}

/**
 * @brief Try to load a plugin
 *
 * @return 1 if loaded plugin successfully, negatives for errors
 */
int PluginInfo::LoadPlugin(const String & scriptletFilepath)
{
	// Search for the class "WinMergeScript"
	LPDISPATCH lpDispatch = CreateDispatchBySource(scriptletFilepath.c_str(), L"WinMergeScript");
	if (lpDispatch == nullptr)
	{
		// set up object in case we need to log info
		ScriptInfo scinfo(scriptletFilepath);
		scinfo.Log(_T("WinMergeScript entry point not found"));
		return -10; // error
	}

	return MakeInfo(scriptletFilepath, lpDispatch);
}

static void ReportPluginLoadFailure(const String & scriptletFilepath)
{
	AppErrorMessageBox(strutils::format(_T("Exception loading plugin\r\n%s"), scriptletFilepath));
}

/**
 * @brief Guard call to LoadPlugin with Windows SEH to trap GPFs
 *
 * @return same as LoadPlugin (1=has event, 0=doesn't have event, errors are negative)
 */
static int LoadPluginWrapper(PluginInfo & plugin, const String & scriptletFilepath)
{
	SE_Handler seh;
	try
	{
		return plugin.LoadPlugin(scriptletFilepath);
	}
	catch (SE_Exception&)
	{
		ReportPluginLoadFailure(scriptletFilepath);
	}
	return false;
}

/**
 * @brief Return list of all candidate plugins in module path
 *
 * Computes list only the first time, and caches it.
 * Lock the plugins *.sct (.ocx and .dll are locked when the interface is created)
 */
static vector<String>& LoadTheScriptletList()
{
	FastMutex::ScopedLock lock(scriptletsSem);
	if (!scriptletsLoaded)
	{
		bool enabledWSH = plugin::IsWindowsScriptThere();
		if (!enabledWSH)
			LogErrorString(_T("\n  .sct plugins disabled (Windows Script Host not found)"));

		for (const auto path : {
				paths::ConcatPath(env::GetProgPath(), _T("MergePlugins")),
				env::ExpandEnvironmentVariables(_T("%APPDATA%\\WinMerge\\MergePlugins")) })
		{
			if (enabledWSH)
			{
				GetScriptletsAt(path, _T(".sct"), theScriptletList);	// VBS/JVS scriptlet
				GetScriptletsAt(path, _T(".wsc"), theScriptletList);	// VBS/JVS scriptlet
			}
			GetScriptletsAt(path, _T(".ocx"), theScriptletList);		// VB COM object
			GetScriptletsAt(path, _T(".dll"), theScriptletList);		// VC++ COM object
		}
		scriptletsLoaded = true;

		// lock the *.sct to avoid them being deleted/moved away
		for (size_t i = 0 ; i < theScriptletList.size() ; i++)
		{
			String scriptlet = theScriptletList.at(i);
			if (scriptlet.length() > 4 && 
				  (strutils::compare_nocase(scriptlet.substr(scriptlet.length() - 4), _T(".sct")) != 0
				|| strutils::compare_nocase(scriptlet.substr(scriptlet.length() - 4), _T(".wsc")) != 0))
			{
				// don't need to lock this file
				theScriptletHandleList.push_back(nullptr);
				continue;
			}

			HANDLE hFile;
			hFile=CreateFile(scriptlet.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
				0, nullptr);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				theScriptletList.erase(theScriptletList.begin() + i);
				i --;
			}
			else
			{
				theScriptletHandleList.push_back(hFile);
			}
		}
	}
	return theScriptletList;
}
/**
 * @brief Delete the scriptlet list and delete locks to *.sct
 *
 * Allow to load it again
 */
static void UnloadTheScriptletList()
{
	FastMutex::ScopedLock lock(scriptletsSem);
	if (scriptletsLoaded)
	{
		for (size_t i = 0 ; i < theScriptletHandleList.size() ; i++)
		{
			HANDLE hFile = theScriptletHandleList.at(i);
			if (hFile != nullptr)
				CloseHandle(hFile);
		}

		theScriptletHandleList.clear();
		theScriptletList.clear();
		scriptletsLoaded = false;
	}
}

/**
 * @brief Remove a candidate plugin from the cache
 */
static void RemoveScriptletCandidate(const String &scriptletFilepath)
{
	for (size_t i=0; i<theScriptletList.size(); ++i)
	{
		if (scriptletFilepath == theScriptletList[i])
		{
			HANDLE hFile = theScriptletHandleList.at(i);
			if (hFile != nullptr)
				CloseHandle(hFile);

			theScriptletHandleList.erase(theScriptletHandleList.begin() + i);
			theScriptletList.erase(theScriptletList.begin() + i);
			return;
		}
	}
}

static void ResolveNameConflict(std::map<std::wstring, PluginArrayPtr> plugins)
{
	std::vector<std::vector<String>> eventsAry = 
	{
		{ L"URL_PACK_UNPACK", L"FILE_FOLDER_PACK_UNPACK", L"FILE_PACK_UNPACK", L"BUFFER_PACK_UNPACK"},
		{ L"FILE_PREDIFF", L"BUFFER_PREDIFF" },
		{ L"EDITOR_SCRIPT"},
	};
	for (const auto& events: eventsAry)
	{
		std::set<String> pluginNames;
		for (const auto& event : events)
		{
			if (plugins.find(event) != plugins.end())
			{
				for (const auto& plugin : *plugins[event])
				{
					String name = paths::RemoveExtension(plugin->m_name);
					if (pluginNames.find(name) != pluginNames.end())
					{
						if (pluginNames.find(plugin->m_name) != pluginNames.end())
						{
							for (int i = 0; ; i++)
							{
								String nameNew = name + strutils::format(_T("(%d)"), i + 2);
								if (pluginNames.find(nameNew) == pluginNames.end())
								{
									name = std::move(nameNew);
									break;
								}
							}
						}
						else
						{
							name = plugin->m_name;
						}
					}
					plugin->m_name = name;
					pluginNames.insert(name);
				}
			}
		}
	}
}

static void LoadCustomSettings(std::map<std::wstring, PluginArrayPtr> plugins)
{
	for (const auto& [event, pluginAry] : plugins)
	{
		for (const auto& plugin : *pluginAry)
		{
			String uniqueName = event + _T(".") + plugin->m_name;
			plugin->m_disabled = (GetCustomSetting(uniqueName, _T("disabled"), _T("false"))[0] != 'f');
			plugin->m_bAutomatic = GetCustomSetting(uniqueName, _T("automatic"), plugin->m_bAutomaticDefault ? _T("true") : _T("false"))[0] == 't';
			plugin->m_arguments = GetCustomSetting(uniqueName, _T("arguments"), plugin->m_argumentsDefault);
			plugin->m_filtersText = GetCustomSetting(uniqueName, _T("filters"), plugin->m_filtersTextDefault);
			plugin->LoadFilterString();
		}
	}
}

/** 
 * @brief Get available scriptlets for an event
 *
 * @return Returns an array of valid LPDISPATCH
 */
static std::map<String, PluginArrayPtr> GetAvailableScripts()
{
	vector<String>& scriptlets = LoadTheScriptletList();
	std::map<std::wstring, PluginArrayPtr> plugins;

	std::list<String> badScriptlets;
	for (size_t i = 0; i < scriptlets.size(); i++)
	{
		// Note all the info about the plugin
		PluginInfoPtr plugin(new PluginInfo);

		String scriptletFilepath = scriptlets.at(i);
		int rtn = LoadPluginWrapper(*plugin.get(), scriptletFilepath);
		if (rtn == 1)
		{
			// Plugin has this event
			if (plugins.find(plugin->m_event) == plugins.end())
				plugins[plugin->m_event].reset(new PluginArray);
			plugins[plugin->m_event]->push_back(plugin);
		}
		else if (rtn < 0)
		{
			// Plugin is bad
			badScriptlets.push_back(scriptletFilepath);
		}
	}

	if (CAllThreadsScripts::GetInternalPluginsLoader())
	{
		String errmsg;
		if (!CAllThreadsScripts::GetInternalPluginsLoader()(plugins, errmsg))
			AppErrorMessageBox(errmsg);
	}

	// Remove any bad plugins from the cache
	// This might be a good time to see if the user wants to abort or continue
	while (!badScriptlets.empty())
	{
		RemoveScriptletCandidate(badScriptlets.front());
		badScriptlets.pop_front();
	}

	ResolveNameConflict(plugins);
	LoadCustomSettings(plugins);

	return plugins;
}

static void FreeAllScripts(PluginArrayPtr& pArray) 
{
	pArray->clear();
	pArray.reset();
}

////////////////////////////////////////////////////////////////////////////////////
// class CScriptsOfThread : cache the interfaces during the thread life

CScriptsOfThread::CScriptsOfThread()
	: m_pHostObject(nullptr)
{
	// count number of events
	int i;
	for (i = 0 ;  ; i ++)
		if (TransformationCategories[i] == nullptr)
			break;
	nTransformationEvents = i;

	// initialize the thread data
	m_nThreadId = GetCurrentThreadId();
	m_nLocks = 0;
	// initialize the plugins pointers
	typedef PluginArray * LPPluginArray;
	// CoInitialize the thread, keep the returned value for the destructor 
	hrInitialize = CoInitialize(nullptr);
	assert(hrInitialize == S_OK || hrInitialize == S_FALSE);
}

CScriptsOfThread::~CScriptsOfThread()
{
	FreeAllScripts();

	if (m_pHostObject)
		m_pHostObject->Release();

	if (hrInitialize == S_OK || hrInitialize == S_FALSE)
		CoUninitialize();
}

bool CScriptsOfThread::bInMainThread()
{
	return (CAllThreadsScripts::bInMainThread(this));
}

void CScriptsOfThread::SetHostObject(IDispatch* pHostObject)
{
	if (m_pHostObject)
		m_pHostObject->Release();
	m_pHostObject = pHostObject;
	if (m_pHostObject)
		m_pHostObject->AddRef();
}

PluginArray * CScriptsOfThread::GetAvailableScripts(const wchar_t *transformationEvent)
{
	if (m_aPluginsByEvent.empty())
		m_aPluginsByEvent = ::GetAvailableScripts();
	if (auto it = m_aPluginsByEvent.find(transformationEvent); it != m_aPluginsByEvent.end())
		return it->second.get();
	// return a pointer to an empty list
	static PluginArray noPlugin;
	return &noPlugin;
}

void CScriptsOfThread::SaveSettings()
{
	if (m_aPluginsByEvent.empty())
		m_aPluginsByEvent = ::GetAvailableScripts();
	std::vector<String> list;
	for (auto [key, pArray] : m_aPluginsByEvent)
	{
		for (const auto& plugin: *pArray)
		{
			std::vector<String> ary;
			if (plugin->m_disabled)
				ary.push_back(_T("disabled"));
			if (plugin->m_filtersTextDefault != plugin->m_filtersText)
				ary.push_back(_T("filters:") + Encode(plugin->m_filtersText));
			if (plugin->m_argumentsDefault != plugin->m_arguments)
				ary.push_back(_T("arguments:") + Encode(plugin->m_arguments));
			if (plugin->m_bAutomaticDefault != plugin->m_bAutomatic)
				ary.push_back(String(_T("automatic:")) + (plugin->m_bAutomatic ? _T("true") : _T("false")));
			if (!ary.empty())
				list.push_back(plugin->m_event + _T(".") + plugin->m_name + _T("=") + strutils::join(ary.begin(), ary.end(), _T("|")));
		}
	}
	GetOptionsMgr()->SaveOption(OPT_PLUGINS_CUSTOM_SETTINGS_LIST, strutils::join(list.begin(), list.end(), _T("\t")));
}

void CScriptsOfThread::FreeAllScripts()
{
	// release all the scripts of the thread
	for (auto [key, pArray] : m_aPluginsByEvent)
		::FreeAllScripts(pArray);

	// force to reload the scriptlet list
	UnloadTheScriptletList();

	m_aPluginsByEvent.clear();
}

void CScriptsOfThread::ReloadAllScripts()
{
	FreeAllScripts();
	m_aPluginsByEvent = ::GetAvailableScripts();
}

PluginInfo *CScriptsOfThread::GetAutomaticPluginByFilter(const wchar_t *transformationEvent, const String& filteredText)
{
	PluginArray * piFileScriptArray = GetAvailableScripts(transformationEvent);
	for (size_t step = 0 ; step < piFileScriptArray->size() ; step ++)
	{
		const PluginInfoPtr & plugin = piFileScriptArray->at(step);
		if (!plugin->m_bAutomatic || plugin->m_disabled)
			continue;
		if (!plugin->TestAgainstRegList(filteredText))
			continue;
		return plugin.get();
	}
	return nullptr;
}

PluginInfo * CScriptsOfThread::GetPluginByName(const wchar_t *transformationEvent, const String& name)
{
	if (m_aPluginsByEvent.empty())
		m_aPluginsByEvent = ::GetAvailableScripts();
	for (auto [key, pArray] : m_aPluginsByEvent)
	{
		if (!transformationEvent || key == transformationEvent)
		{
			for (size_t j = 0; j < pArray->size(); j++)
				if (pArray->at(j)->m_name == name)
					return pArray->at(j).get();
			String name2 = paths::RemoveExtension(name);
			for (size_t j = 0; j < pArray->size(); j++)
				if (pArray->at(j)->m_name == name2)
					return pArray->at(j).get();
		}
	}
	return nullptr;
}

PluginInfo *  CScriptsOfThread::GetPluginInfo(LPDISPATCH piScript)
{
	for (auto [key, pArray] : m_aPluginsByEvent)
	{
		if (pArray == nullptr)
			continue;
		for (size_t j = 0 ; j < pArray->size() ; j++)
			if ((*pArray)[j]->m_lpDispatch == piScript)
				return (*pArray)[j].get();
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////
// class CAllThreadsScripts : array of CScriptsOfThread, one per active thread

std::vector<CScriptsOfThread *> CAllThreadsScripts::m_aAvailableThreads;
FastMutex m_aAvailableThreadsLock;

void CAllThreadsScripts::Add(CScriptsOfThread * scripts)
{
	FastMutex::ScopedLock lock(m_aAvailableThreadsLock);
	// add the thread in the array

	// register in the array
	m_aAvailableThreads.push_back(scripts);
}

void CAllThreadsScripts::Remove(CScriptsOfThread * scripts)
{
	FastMutex::ScopedLock lock(m_aAvailableThreadsLock);
	// unregister from the list
	std::vector<CScriptsOfThread *>::iterator it;
	for (it =  m_aAvailableThreads.begin(); it != m_aAvailableThreads.end(); ++it)
		if ((*it) == scripts)
		{
			m_aAvailableThreads.erase(it);
			break;
		}
}

CScriptsOfThread * CAllThreadsScripts::GetActiveSet()
{
	FastMutex::ScopedLock lock(m_aAvailableThreadsLock);
	unsigned long nThreadId = GetCurrentThreadId();
	for (size_t i = 0 ; i < m_aAvailableThreads.size() ; i++)
		if (m_aAvailableThreads[i] && m_aAvailableThreads[i]->m_nThreadId == nThreadId)
			return m_aAvailableThreads[i];
	assert(false);
	return nullptr;
}

CScriptsOfThread * CAllThreadsScripts::GetActiveSetNoAssert()
{
	FastMutex::ScopedLock lock(m_aAvailableThreadsLock);
	unsigned long nThreadId = GetCurrentThreadId();
	for (size_t i = 0 ; i < m_aAvailableThreads.size() ; i++)
		if (m_aAvailableThreads[i] && m_aAvailableThreads[i]->m_nThreadId == nThreadId)
			return m_aAvailableThreads[i];
	return nullptr;
}

bool CAllThreadsScripts::bInMainThread(CScriptsOfThread * scripts)
{
	FastMutex::ScopedLock lock(m_aAvailableThreadsLock);
	return (scripts == m_aAvailableThreads[0]);
}

void CAllThreadsScripts::ReloadCustomSettings()
{
	{
		FastMutex::ScopedLock lock(scriptletsSem);
		customSettingsMap.clear();
	}
	for (auto& thread : m_aAvailableThreads)
		LoadCustomSettings(thread->m_aPluginsByEvent);
}

void CAllThreadsScripts::ReloadAllScripts()
{
	for (auto& thread : m_aAvailableThreads)
		thread->ReloadAllScripts();
}

////////////////////////////////////////////////////////////////////////////////////
// class CAssureScriptsForThread : control creation/destruction of CScriptsOfThread

CAssureScriptsForThread::CAssureScriptsForThread(IDispatch* pHostObject)
{
	CScriptsOfThread * scripts = CAllThreadsScripts::GetActiveSetNoAssert();
	if (scripts == nullptr)
	{
		scripts = new CScriptsOfThread;
		// insert the script in the repository
		CAllThreadsScripts::Add(scripts);
		scripts->SetHostObject(pHostObject);
	}
	scripts->Lock();
}
CAssureScriptsForThread::~CAssureScriptsForThread()
{
	CScriptsOfThread * scripts = CAllThreadsScripts::GetActiveSetNoAssert();
	if (scripts == nullptr)
		return;
	if (scripts->Unlock())
	{
		delete scripts;
		CAllThreadsScripts::Remove(scripts);
	}
}

////////////////////////////////////////////////////////////////////////////////
// wrap invokes with error handlers

/**
 * @brief Display a message box with the plugin name and the error message
 *
 * @note Use MessageBox instead of AfxMessageBox so we can set the caption.
 * VB/VBS plugins has an internal error handler, and a message box with caption,
 * and we try to reproduce it for other plugins.
 */
static void ShowPluginErrorMessage(IDispatch *piScript, tchar_t* description)
{
	PluginInfo * pInfo = CAllThreadsScripts::GetActiveSet()->GetPluginInfo(piScript);
	assert(pInfo != nullptr);
	assert(description != nullptr);	
	AppErrorMessageBox(strutils::format(_T("%s: %s"), pInfo->m_name, description));
}

/**
 * @brief safe invoke helper (by ordinal)
 *
 * @note Free all variants passed to it (except ByRef ones) 
 */
static HRESULT safeInvokeA(LPDISPATCH pi, VARIANT *ret, DISPID id, LPCCH op, ...)
{
	HRESULT h = E_FAIL;
	SE_Handler seh;
	tchar_t errorText[500];
	bool bExceptionCatched = false;	
#ifdef WIN64
	int nargs = LOBYTE((UINT_PTR)op);
	vector<VARIANT> args(nargs);
	va_list list;
	va_start(list, op);
	for (vector<VARIANT>::iterator it = args.begin(); it != args.end(); ++it)
		*it = va_arg(list, VARIANT);
	va_end(list);
#endif

	try 
	{
#ifdef WIN64
		h = invokeA(pi, ret, id, op, nargs == 0 ? nullptr : &args[0]);
#else
		h = invokeA(pi, ret, id, op, (VARIANT *)(&op + 1));
#endif
	}
	catch(SE_Exception& e) 
	{
		// structured exception are catched here thanks to class SE_Exception
		if (!(e.GetErrorMessage(errorText, 500, nullptr)))
			// don't localize this as we do not localize the known exceptions
			tc::tcslcpy(errorText, _T("Unknown CException"));
		bExceptionCatched = true;
	}
	catch(...) 
	{
		// don't localize this as we do not localize the known exceptions
		tc::tcslcpy(errorText, _T("Unknown C++ exception"));
		bExceptionCatched = true;
	}

	if (bExceptionCatched)
	{
		ShowPluginErrorMessage(pi, errorText);
		// set h to FAILED
		h = E_FAIL;
	}

	return h;
}
/**
 * @brief safe invoke helper (by function name)
 *
 * @note Free all variants passed to it (except ByRef ones) 
 */
static HRESULT safeInvokeW(LPDISPATCH pi, VARIANT *ret, LPCOLESTR silent, LPCCH op, ...)
{
	HRESULT h = E_FAIL;
	SE_Handler seh;
	tchar_t errorText[500];
	bool bExceptionCatched = false;
#ifdef WIN64
	int nargs = LOBYTE((UINT_PTR)op);
	vector<VARIANT> args(nargs);
	va_list list;
	va_start(list, op);
	for (vector<VARIANT>::iterator it = args.begin(); it != args.end(); ++it)
		*it = va_arg(list, VARIANT);
	va_end(list);
#endif
	
	try 
	{
#ifdef WIN64
		h = invokeW(pi, ret, silent, op, nargs == 0 ? nullptr : &args[0]);
#else
		h = invokeW(pi, ret, silent, op, (VARIANT *)(&op + 1));
#endif
	}
	catch(SE_Exception& e) 
	{
		// structured exception are catched here thanks to class SE_Exception
		if (!(e.GetErrorMessage(errorText, 500, nullptr)))
			// don't localize this as we do not localize the known exceptions
			tc::tcslcpy(errorText, _T("Unknown CException"));
		bExceptionCatched = true;
	}
	catch(...) 
	{
		// don't localize this as we do not localize the known exceptions
		tc::tcslcpy(errorText, _T("Unknown C++ exception"));
		bExceptionCatched = true;
	}

	if (bExceptionCatched)
	{
		ShowPluginErrorMessage(pi, errorText);
		// set h to FAILED
		h = E_FAIL;
	}

	return h;
}

////////////////////////////////////////////////////////////////////////////////
// invoke for plugins

namespace plugin
{

/*
 * ----- about VariantClear -----
 * VariantClear is done in safeInvokeW/safeInvokeA except for :
 * - the returned value
 * - BYREF arguments
 * note : BYREF arguments don't need VariantClear if the refered value
 * is deleted in the function destructor. Example :
 * {
 *   int Value;
 *   VARIANT vValue;
 *   vValue.plVal = &vValue;
 *   ...
 */

bool InvokePrediffBuffer(BSTR & bstrBuf, int & nChanged, IDispatch *piScript)
{
	UINT nBufSize = SysStringLen(bstrBuf);

	// prepare the arguments
	// argument text buffer by reference
	VARIANT vpbstrBuf{ VT_BYREF | VT_BSTR };
	vpbstrBuf.pbstrVal = &bstrBuf;
	// argument buffer size by reference
	VARIANT vpiSize{ VT_BYREF | VT_I4 };
	vpiSize.plVal = (long*) &nBufSize;
	// argument flag changed (VT_BOOL is short)
	VARIANT_BOOL changed = 0;
	VARIANT vpboolChanged{ VT_BYREF | VT_BOOL };
	vpboolChanged.pboolVal = &changed;
	// argument return value (VT_BOOL is short)
	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// for VC, if the invoked function changes the buffer address, 
	// it must free the old buffer with SysFreeString
	// VB does it automatically
	// VARIANT_BOOL DiffingPreprocessW(BSTR * buffer, UINT * nSize, VARIANT_BOOL * bChanged)
	HRESULT h = ::safeInvokeW(piScript,	&vboolHandled, L"PrediffBufferW", opFxn[3], 
                            vpboolChanged, vpiSize, vpbstrBuf);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;
	if (bSuccess && changed)
	{
		// remove trailing charracters in the rare case that bstrBuf was not resized 
		if (SysStringLen(bstrBuf) != nBufSize)
			bSuccess = !FAILED(SysReAllocStringLen(&bstrBuf, bstrBuf, nBufSize));
		if (bSuccess)
			nChanged ++;
	}

	// clear the returned variant
	VariantClear(&vboolHandled);

	return 	(bSuccess);
}

bool InvokeUnpackBuffer(VARIANT & array, int & nChanged, IDispatch *piScript, int & subcode)
{
	LONG nArraySize;
	SafeArrayGetUBound(array.parray, 0, &nArraySize);
	++nArraySize;

	// prepare the arguments
	// argument file buffer
	VARIANT vparrayBuf{ VT_BYREF | VT_ARRAY | VT_UI1 };
	vparrayBuf.pparray = &(array.parray);
	// argument buffer size by reference
	VARIANT vpiSize{ VT_BYREF | VT_I4 };
	vpiSize.plVal = (long*) &nArraySize;
	// argument flag changed (VT_BOOL is short)
	VARIANT_BOOL changed = 0;
	VARIANT vpboolChanged{ VT_BYREF | VT_BOOL };
	vpboolChanged.pboolVal = &changed;
	// argument subcode by reference
	VARIANT viSubcode{ VT_BYREF | VT_I4 };
	viSubcode.plVal = (long*) &subcode;
	// argument return value (VT_BOOL is short)
	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// VARIANT_BOOL UnpackBufferA(SAFEARRAY * array, UINT * nSize, VARIANT_BOOL * bChanged, UINT * subcode)
	HRESULT h = ::safeInvokeW(piScript,	&vboolHandled, L"UnpackBufferA", opFxn[4], 
                            viSubcode, vpboolChanged, vpiSize, vparrayBuf);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;
	if (bSuccess && changed)
	{
		// remove trailing charracters if the array was not resized
		LONG nNewArraySize;
		SafeArrayGetUBound(array.parray, 0, &nNewArraySize);
		++nNewArraySize;

		if (nNewArraySize != nArraySize)
		{
			SAFEARRAYBOUND sab = {static_cast<ULONG>(nArraySize), 0};
			SafeArrayRedim(array.parray, &sab);
		}
		nChanged ++;
	}

	// clear the returned variant
	VariantClear(&vboolHandled);

	return 	(bSuccess);
}

bool InvokePackBuffer(VARIANT & array, int & nChanged, IDispatch *piScript, int subcode)
{
	LONG nArraySize;
	SafeArrayGetUBound(array.parray, 0, &nArraySize);
	++nArraySize;

	// prepare the arguments
	// argument file buffer
	VARIANT vparrayBuf{ VT_BYREF | VT_ARRAY | VT_UI1 };
	vparrayBuf.pparray = &(array.parray);
	// argument buffer size by reference
	VARIANT vpiSize{ VT_BYREF | VT_I4 };
	vpiSize.plVal = (long*) &nArraySize;
	// argument flag changed (VT_BOOL is short)
	VARIANT_BOOL changed = 0;
	VARIANT vpboolChanged{ VT_BYREF | VT_BOOL };
	vpboolChanged.pboolVal = &changed;
	// argument subcode
	VARIANT viSubcode{ VT_I4 };
	viSubcode.lVal = subcode;
	// argument return value (VT_BOOL is short)
	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// VARIANT_BOOL PackBufferA(SAFEARRAY * array, UINT * nSize, VARIANT_BOOL * bChanged, UINT subcode)
	HRESULT h = ::safeInvokeW(piScript,	&vboolHandled, L"PackBufferA", opFxn[4], 
                            viSubcode, vpboolChanged, vpiSize, vparrayBuf);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;
	if (bSuccess && changed)
	{
		// remove trailing charracters if the array was not resized
		LONG nNewArraySize;
		SafeArrayGetUBound(array.parray, 0, &nNewArraySize);
		++nNewArraySize;

		if (nNewArraySize != nArraySize)
		{
			SAFEARRAYBOUND sab = {static_cast<ULONG>(nArraySize), 0};
			SafeArrayRedim(array.parray, &sab);
		}
	}

	// clear the returned variant
	VariantClear(&vboolHandled);

	return 	(bSuccess);
}


static bool unpack(const wchar_t *method, const String& source, const String& dest, int & nChanged, IDispatch *piScript, int & subCode)
{
	// argument text  
	VARIANT vbstrSrc{ VT_BSTR };
	vbstrSrc.bstrVal = SysAllocString(ucr::toUTF16(source).c_str());
	// argument transformed text 
	VARIANT vbstrDst{ VT_BSTR };
	vbstrDst.bstrVal = SysAllocString(ucr::toUTF16(dest).c_str());
	// argument subcode by reference
	VARIANT vpiSubcode{ VT_BYREF | VT_I4 };
	vpiSubcode.plVal = (long*) &subCode;
	// argument flag changed (VT_BOOL is short)
	VARIANT_BOOL changed = 0;
	VARIANT vpboolChanged{ VT_BYREF | VT_BOOL };
	vpboolChanged.pboolVal = &changed;
	// argument return value (VT_BOOL is short)
	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// VARIANT_BOOL UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL * bChanged, INT * bSubcode)
	HRESULT h = ::safeInvokeW(piScript,	&vboolHandled, method, opFxn[4], 
	                          vpiSubcode, vpboolChanged, vbstrDst, vbstrSrc);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;
	if (bSuccess && changed)
		nChanged ++;

	// clear the returned variant
	VariantClear(&vboolHandled);

	return 	(bSuccess);
}

static bool pack(const wchar_t *method, const String& source, const String& dest, int & nChanged, IDispatch *piScript, int & subCode)
{
	// argument text  
	VARIANT vbstrSrc{ VT_BSTR };
	vbstrSrc.bstrVal = SysAllocString(ucr::toUTF16(source).c_str());
	// argument transformed text 
	VARIANT vbstrDst{ VT_BSTR };
	vbstrDst.bstrVal = SysAllocString(ucr::toUTF16(dest).c_str());
	// argument subcode
	VARIANT viSubcode{ VT_I4 };
	viSubcode.lVal = subCode;
	// argument flag changed (VT_BOOL is short)
	VARIANT_BOOL changed = 0;
	VARIANT vpboolChanged{ VT_BYREF | VT_BOOL };
	vpboolChanged.pboolVal = &changed;
	// argument return value (VT_BOOL is short)
	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// VARIANT_BOOL PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL * bChanged, INT bSubcode)
	HRESULT h = ::safeInvokeW(piScript,	&vboolHandled, method, opFxn[4], 
	                          viSubcode, vpboolChanged, vbstrDst, vbstrSrc);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;
	if (bSuccess && changed)
		nChanged ++;

	// clear the returned variant
	VariantClear(&vboolHandled);

	return 	(bSuccess);
}

bool InvokeUnpackFile(const String& fileSource, const String& fileDest, int & nChanged, IDispatch *piScript, int & subCode)
{
	return unpack(L"UnpackFile", fileSource, fileDest, nChanged, piScript, subCode);
}

bool InvokePackFile(const String& fileSource, const String& fileDest, int & nChanged, IDispatch *piScript, int subCode)
{
	return pack(L"PackFile", fileSource, fileDest, nChanged, piScript, subCode);
}

bool InvokeUnpackFolder(const String& fileSource, const String& folderDest, int & nChanged, IDispatch *piScript, int & subCode)
{
	return unpack(L"UnpackFolder", fileSource, folderDest, nChanged, piScript, subCode);
}

bool InvokePackFolder(const String& folderSource, const String& fileDest, int & nChanged, IDispatch *piScript, int subCode)
{
	return pack(L"PackFolder", folderSource, fileDest, nChanged, piScript, subCode);
}

bool InvokePrediffFile(const String& fileSource, const String& fileDest, int & nChanged, IDispatch *piScript)
{
	// argument text  
	VARIANT vbstrSrc{ VT_BSTR };
	vbstrSrc.bstrVal = SysAllocString(ucr::toUTF16(fileSource).c_str());
	// argument transformed text 
	VARIANT vbstrDst{ VT_BSTR };
	vbstrDst.bstrVal = SysAllocString(ucr::toUTF16(fileDest).c_str());
	// argument flag changed (VT_BOOL is short)
	VARIANT_BOOL changed = 0;
	VARIANT vpboolChanged{ VT_BYREF | VT_BOOL };
	vpboolChanged.pboolVal = &changed;
	// argument return value (VT_BOOL is short)
	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// VARIANT_BOOL PrediffFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL * bChanged)
	HRESULT h = ::safeInvokeW(piScript,	&vboolHandled, L"PrediffFile", opFxn[3], 
                            vpboolChanged, vbstrDst, vbstrSrc);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;
	if (bSuccess && changed)
		nChanged ++;

	// clear the returned variant
	VariantClear(&vboolHandled);

	return 	(bSuccess);
}


bool InvokeTransformText(String & text, int & changed, IDispatch *piScript, int fncId)
{
	// argument text  
	VARIANT pvPszBuf{ VT_BSTR };
#ifdef _UNICODE
	pvPszBuf.bstrVal = SysAllocStringLen(text.data(), static_cast<unsigned>(text.length()));
#else
	std::wstring wtext = ucr::toUTF16(text);
	pvPszBuf.bstrVal = SysAllocStringLen(wtext.data(), static_cast<unsigned>(wtext.length()));
#endif
	// argument transformed text 
	VARIANT vTransformed{ VT_BSTR };
	vTransformed.bstrVal = nullptr;

	// invoke method by ordinal
	// BSTR customFunction(BSTR text)
	HRESULT h = ::safeInvokeA(piScript, &vTransformed, fncId, opFxn[1], pvPszBuf);

	if (! FAILED(h) && vTransformed.bstrVal)
	{
#ifdef _UNICODE
		text = String(vTransformed.bstrVal, SysStringLen(vTransformed.bstrVal));
#else
		std::wstring wtext2 = std::wstring(vTransformed.bstrVal, SysStringLen(vTransformed.bstrVal));
		text = ucr::toTString(wtext2);
#endif
		changed = true;
	}
	else
		changed = false;

	// clear the returned variant
	VariantClear(&vTransformed);

	return (! FAILED(h));
}

bool InvokeIsFolder(const String& path, IDispatch *piScript)
{
	// argument text  
	VARIANT vbstrPath{ VT_BSTR };
	vbstrPath.bstrVal = SysAllocString(ucr::toUTF16(path).c_str());

	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// VARIANT_BOOL ShowSettingsDialog()
	HRESULT h = ::safeInvokeW(piScript, &vboolHandled, L"IsFolder", opFxn[1], vbstrPath);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;

	// clear the returned variant
	VariantClear(&vboolHandled);

	return (bSuccess);
}

bool InvokeShowSettingsDialog(IDispatch *piScript)
{
	VARIANT vboolHandled{ VT_BOOL };
	vboolHandled.boolVal = false;

	// invoke method by name, reverse order for arguments
	// VARIANT_BOOL ShowSettingsDialog()
	HRESULT h = ::safeInvokeW(piScript, &vboolHandled, L"ShowSettingsDialog", opFxn[0]);
	bool bSuccess = ! FAILED(h) && vboolHandled.boolVal;

	// clear the returned variant
	VariantClear(&vboolHandled);

	return (bSuccess);
}

bool InvokePutPluginArguments(const String& args, LPDISPATCH piScript)
{
	// argument text  
	VARIANT vbstrArgs{ VT_BSTR };
	std::wstring wargs = ucr::toUTF16(args);
	vbstrArgs.bstrVal = SysAllocStringLen(wargs.data(), static_cast<unsigned>(wargs.size()));

	HRESULT h = ::safeInvokeW(piScript, nullptr, L"PluginArguments", opPut[1], vbstrArgs);
	return SUCCEEDED(h);
}

bool InvokePutPluginVariables(const String& vars, LPDISPATCH piScript)
{
	// argument text  
	VARIANT vbstrVars{ VT_BSTR };
	std::wstring wvars = ucr::toUTF16(vars);
	vbstrVars.bstrVal = SysAllocStringLen(wvars.data(), static_cast<unsigned>(wvars.size()));

	HRESULT h = ::safeInvokeW(piScript, nullptr, L"PluginVariables", opPut[1], vbstrVars);
	return SUCCEEDED(h);
}

bool InvokePluginOnEvent(int eventType, LPDISPATCH piScript)
{
	// argument wmobj
	VARIANT vdispHostObject{ VT_DISPATCH };
	vdispHostObject.pdispVal = CAllThreadsScripts::GetActiveSet()->GetHostObject();
	vdispHostObject.pdispVal->AddRef();
	// argument eventType
	VARIANT viEventType{ VT_I4 };
	viEventType.intVal = eventType;

	HRESULT h = ::safeInvokeW(piScript, nullptr, L"PluginOnEvent", opFxn[2], vdispHostObject, viEventType);
	return SUCCEEDED(h);
}

}
