/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
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

/** 
 * @brief Information structure for a plugin
 */
class PluginInfo
{
public:
	PluginInfo()
		: m_lpDispatch(nullptr), m_filters(NULL), m_bAutomatic(false), m_nFreeFunctions(0), m_disabled(false)
	{	
	}

	~PluginInfo()
	{
		if (m_lpDispatch!=nullptr)
			m_lpDispatch->Release();
	}

	int LoadPlugin(const String & scriptletFilepath, const wchar_t *transformationEvent);

	/// Parse the filter string (only for files), and create the filters
	void LoadFilterString();
	/**
	 * @brief Does the plugin handles this(ese) filename(s) ?
	 *
	 * @param szTest String of filenames, delimited with '|'
	 */
	bool TestAgainstRegList(const String& szTest) const;

public:
	String      m_filepath;
	LPDISPATCH  m_lpDispatch;
	String      m_name; // usually filename, except for special cases (like auto or no)
	String      m_filtersText;
	String      m_description;
	bool        m_bAutomatic;
	bool        m_disabled;
	std::vector<FileFilterElementPtr> m_filters;
	/// only for plugins with free function names (EDITOR_SCRIPT)
	int         m_nFreeFunctions;

private:
	PluginInfo( const PluginInfo& other ); // non construction-copyable
	PluginInfo& operator=( const PluginInfo& ); // non copyable
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
	PluginArray * GetAvailableScripts(const wchar_t *transformationEvent);
	PluginInfo * GetAutomaticPluginByFilter(const wchar_t *transformationEvent, const String& filteredText);
	PluginInfo * GetPluginByName(const wchar_t *transformationEvent, const String& name);
	PluginInfo * GetPluginInfo(LPDISPATCH piScript);
	void SaveSettings();

	void FreeAllScripts();
	void FreeScriptsForEvent(const wchar_t *transformationEvent);

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
	std::vector<PluginArrayPtr> m_aPluginsByEvent;
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
private:
	// fixed size array, advantage : no mutex to allocate/free
	static std::vector<CScriptsOfThread *> m_aAvailableThreads;
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
	CAssureScriptsForThread();
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
 * @brief Get the number of methods in the script
 * @note For free function scripts (EDITOR_SCRIPT)
 */
int CountMethodsInScript(LPDISPATCH piDispatch);

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
 * @brief Call the plugin "IsFolder" method, event FILE_FOLDER_PACK_UNPACK
 */
bool InvokeIsFolder(const String& file, IDispatch *piScript);
/**
 * @brief Call the plugin "UnpackFolder" method, event FILE_FOLDER_PACK_UNPACK
 */
bool InvokeUnpackFolder(const String& fileSource, const String& folderDest, int & nChanged, IDispatch *piScript, int & subCode);
/**
 * @brief Call the plugin "PackFolder" method, event FILE_FOLDER_PACK_UNPACK
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

}
