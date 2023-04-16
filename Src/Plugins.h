/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file Plugins.h
 *
 *  @brief Declaration file for VBS Scriptlets, VB ActiveX DLL, VC++ COM DLL
 */ 
#pragma once

#include <Poco/Foundation.h>
#include <string>
#include <vector>
#include <optional>
#include <windows.h>
#include <oleauto.h>
#include <memory>
#include "UnicodeString.h"

struct FileFilterElement;
typedef std::shared_ptr<FileFilterElement> FileFilterElementPtr;

/**
 * @brief List of transformation categories (events)
 *
 * @note If you add some event, you have to complete this array in FileTransform.cpp
 */
extern const wchar_t *TransformationCategories[];

enum { EVENTID_INITIALIZE, EVENTID_TERMINATE };

/** 
 * @brief Information structure for a plugin
 */
class PluginInfo
{
public:
	PluginInfo();
	~PluginInfo();

	int LoadPlugin(const String & scriptletFilepath);
	int MakeInfo(const String & scriptletFilepath, IDispatch *pDispatch);

	/// Parse the filter string (only for files), and create the filters
	void LoadFilterString();
	/**
	 * @brief Does the plugin handles this(ese) filename(s) ?
	 *
	 * @param szTest String of filenames, delimited with '|'
	 */
	bool TestAgainstRegList(const String& szTest) const;

	std::optional<StringView> GetExtendedPropertyValue(const String& name) const;

public:
	String      m_filepath;
	LPDISPATCH  m_lpDispatch;
	String      m_name; // usually filename, except for special cases (like auto or no)
	String      m_ext;
	String      m_extendedProperties;
	String      m_arguments;
	String      m_argumentsDefault;
	String      m_filtersText;
	String      m_filtersTextDefault;
	String      m_description;
	String      m_event;
	bool        m_bAutomatic;
	bool        m_bAutomaticDefault;
	bool        m_disabled;
	bool        m_hasArgumentsProperty;
	bool        m_hasVariablesProperty;
	bool        m_hasPluginOnEventMethod;
	std::vector<FileFilterElementPtr> m_filters;
	/// only for plugins with free function names (EDITOR_SCRIPT)
	int         m_nFreeFunctions;

private:
	PluginInfo( const PluginInfo& other ) = delete; // non construction-copyable
	PluginInfo& operator=( const PluginInfo& ) = delete; // non copyable
};

typedef std::shared_ptr<PluginInfo> PluginInfoPtr;

typedef std::vector<PluginInfoPtr> PluginArray;
typedef std::shared_ptr<PluginArray> PluginArrayPtr;

/**
 * @brief Cache for the scriptlets' interfaces during the life of a thread. 
 * One instance and only one for each thread (necessary for VB)
 *
 * @note Never create CScriptsOfThread directly : use the class CAssureScriptsForThread
 * to guarantee unicity
 */
class CScriptsOfThread
{
friend class CAssureScriptsForThread;
friend class CAllThreadsScripts;
public:
	IDispatch* GetHostObject() const { return m_pHostObject; };
	void SetHostObject(IDispatch* pHostObject);
	PluginArray * GetAvailableScripts(const wchar_t *transformationEvent);
	PluginInfo * GetAutomaticPluginByFilter(const wchar_t *transformationEvent, const String& filteredText);
	PluginInfo * GetPluginByName(const wchar_t *transformationEvent, const String& name);
	PluginInfo * GetPluginInfo(LPDISPATCH piScript);
	void SaveSettings();

	void FreeAllScripts();
	void ReloadAllScripts();

protected:
	CScriptsOfThread();
	~CScriptsOfThread();
	void Lock()	  { m_nLocks ++; };
	bool Unlock()	{ m_nLocks --; return (m_nLocks == 0); };
	/// Tell if this scripts is the one for main thread (by convention, the first in the repository)
	bool bInMainThread();

private:
	unsigned m_nLocks;
	unsigned long m_nThreadId;
	/// Result of CoInitialize
	HRESULT hrInitialize;
	int nTransformationEvents;
	std::map<String, PluginArrayPtr> m_aPluginsByEvent;
	IDispatch* m_pHostObject;
};


/**
 * @brief Repository of CScriptsOfThread
 */
class CAllThreadsScripts
{
friend class CAssureScriptsForThread;
protected:
	static void Add(CScriptsOfThread * scripts);
	static void Remove(CScriptsOfThread * scripts);
	static CScriptsOfThread * GetActiveSetNoAssert();
public:
	/// main public function : get the plugins array for the current thread
	static CScriptsOfThread * GetActiveSet();
	/// by convention, the scripts for main thread must be created before all others
	static bool bInMainThread(CScriptsOfThread * scripts);
	using InternalPluginLoaderFuncPtr = bool (*)(std::map<String, PluginArrayPtr>& aPluginsByEvent, String& errmsg);
	static InternalPluginLoaderFuncPtr GetInternalPluginsLoader() { return m_funcInternalPluginsLoader; }
	static void RegisterInternalPluginsLoader(InternalPluginLoaderFuncPtr func) { m_funcInternalPluginsLoader = func; }
	static void ReloadCustomSettings();
	static void ReloadAllScripts();
private:
	// fixed size array, advantage : no mutex to allocate/free
	static std::vector<CScriptsOfThread *> m_aAvailableThreads;
	static inline InternalPluginLoaderFuncPtr m_funcInternalPluginsLoader = nullptr;
};

/**
 * @brief Simple control to add/remove a CScriptsOfThread in the repository. 
 * Create at least one CAssumeScriptsForThread for each thread, including the main one.
 * It's OK to create several CAssumeScriptsForThread for the same thread (if you need scripts in one function 
 * and do not know what happened before the function).
 */
class CAssureScriptsForThread
{
public:
	CAssureScriptsForThread(IDispatch* pHostObject);
	~CAssureScriptsForThread();
};

namespace plugin
{

/**
 * @brief Check for the presence of Windows Script
 *
 * .sct plugins require this optional component
 */
bool IsWindowsScriptThere();
/**
 * @brief Get a list of the function IDs and names in a script or activeX/COM DLL
 *
 * @return Returns the number of functions
 *
 */
int GetMethodsFromScript(LPDISPATCH piDispatch, std::vector<String>& namesArray, std::vector<int>& IdArray);

/**
/**
 * @brief Get the ID of the a free function
 * @param methodOrdinal : index of the free function (0,1,2...)
 */
int GetMethodIDInScript(LPDISPATCH piDispatch, int methodIndex);



// Wrappers to call plugins methods

/**
 * @brief Call the plugin "PrediffBufferW" method, events PREDIFFING
 *
 * @param bstrBuf Overwrite/realloc this buffer
 */
bool InvokePrediffBuffer(BSTR & bstrBuf, int & nChanged, LPDISPATCH piScript);

/** 
 * @brief Call custom plugin functions : text transformation
 */
bool InvokeTransformText(String & text, int & changed, LPDISPATCH piScript, int fncId);

/**
 * @brief Call the plugin "UnpackBufferA" method, event BUFFER_PACK_UNPACK
 *
 * @param pszBuf has unknown format, so a simple char*
 * never owervrites this source buffer
 */
bool InvokeUnpackBuffer(VARIANT & array, int & nChanged, LPDISPATCH piScript, int & subcode);
/**
 * @brief Call the plugin "PackBufferA" method, event BUFFER_PACK_UNPACK
 *
 * @param pszBuf has unknown format, so a simple char*
 * never owervrites this source buffer
 */
bool InvokePackBuffer(VARIANT & array, int & nChanged, LPDISPATCH piScript, int subcode);
/**
 * @brief Call the plugin "UnpackFile" method, event FILE_PACK_UNPACK
 */
bool InvokeUnpackFile(const String& fileSource, const String& fileDest, int & nChanged, LPDISPATCH piScript, int & subCode);
/**
 * @brief Call the plugin "PackFile" method, event FILE_PACK_UNPACK
 */
bool InvokePackFile(const String& fileSource, const String& fileDest, int & nChanged, LPDISPATCH piScript, int subCode);
/**
 * @brief Call the plugin "IsFolder" method, event FILE_FOLDER_PACK_UNPACK or URL_PACK_UNPACK
 */
bool InvokeIsFolder(const String& file, IDispatch *piScript);
/**
 * @brief Call the plugin "UnpackFolder" method, event FILE_FOLDER_PACK_UNPACK or URL_PACK_UNPACK
 */
bool InvokeUnpackFolder(const String& fileSource, const String& folderDest, int & nChanged, IDispatch *piScript, int & subCode);
/**
 * @brief Call the plugin "PackFolder" method, event FILE_FOLDER_PACK_UNPACK or URL_PACK_UNPACK
 */
bool InvokePackFolder(const String& folderSource, const String& fileDest, int & nChanged, IDispatch *piScript, int subCode);
/**
 * @brief Call the plugin "PrediffFile" method, event FILE_PREDIFF
 */
bool InvokePrediffFile(const String& fileSource, const String& fileDest, int & nChanged, LPDISPATCH piScript);
/**
 * @brief Call the plugin "ShowSettingsDialog" method
 */
bool InvokeShowSettingsDialog(LPDISPATCH piScript);

/**
 * @brief Set value to the plugin "PluginArguments" property 
 */
bool InvokePutPluginArguments(const String& args, LPDISPATCH piScript);

/**
 * @brief Set value to the plugin "PluginVariables" property 
 */
bool InvokePutPluginVariables(const String& args, LPDISPATCH piScript);

/**
 * @brief call the plugin "PluginOnEvent" method 
 */
bool InvokePluginOnEvent(int eventType, LPDISPATCH piScript);

}
