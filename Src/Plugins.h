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
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef __PLUGINS_H__
#define __PLUGINS_H__

extern enum TRANSFORMATION_CATEGORY;


const int NMAXTHREADS = 10;

/**
 * @brief Constant : Unicode/ANSI supported mode 
 */
#define SCRIPT_A		1
#define SCRIPT_W		2



class CRegExp;
typedef CTypedPtrList<CPtrList, CRegExp*>RegList;

/** 
 * @brief Information structure for a plugin
 */
class PluginInfo
{
public:
	PluginInfo()
	{	
		lpDispatch = NULL; 
		filters = NULL; 
	};
	/// Parse the filter string (only for files), and create the filters
	void LoadFilterString();
	/**
	 * @brief Does the plugin handles this(ese) filename(s) ?
	 *
	 * @param szTest String of filenames, delimited with '|'
	 */
	BOOL TestAgainstRegList(LPCTSTR szTest);

public:
	LPDISPATCH  lpDispatch;
	CString     name;
	CString     filtersText;
	CString     description;
	BOOL        bUnicodeMode;
	BOOL        bAutomatic;
	RegList*    filters;
	/// only for plugins with free function names (EDITOR_SCRIPT)
	int         nFreeFunctions;
};



typedef CArray<PluginInfo, PluginInfo&>PluginArray;

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
	PluginArray * GetAvailableScripts(LPCWSTR transformationEvent);
	PluginInfo * GetPluginByName(LPCWSTR transformationEvent, LPCTSTR name);
	PluginInfo * GetPluginInfo(LPDISPATCH piScript);

	void FreeAllScripts();
	void FreeScriptsForEvent(LPCWSTR transformationEvent);

protected:
	CScriptsOfThread();
	~CScriptsOfThread();
	void Lock()	  { m_nLocks ++; };
	BOOL Unlock()	{ m_nLocks --; return (m_nLocks == 0); };
	/// Tell if this scripts is the one for main thread (by convention, the first in the repository)
	BOOL bInMainThread();

private:
	unsigned int m_nLocks;
	unsigned long m_nThreadId;
	/// Result of CoInitialize
	HRESULT hrInitialize;
	int nTransformationEvents;
	PluginArray ** m_aPluginsByEvent;
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
	static BOOL bInMainThread(CScriptsOfThread * scripts);
private:
	// fixed size array, advantage : no mutex to allocate/free
	static CScriptsOfThread * m_aAvailableThreads[NMAXTHREADS];
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



/**
 * @brief Check for the presence of Windows Script
 *
 * .sct plugins require this optional component
 */
BOOL IsWindowsScriptThere();
/**
 * @brief Get a list of the function IDs and names in a script or activeX/COM DLL
 *
 * @return Returns the number of functions
 *
 * @note The arrays must be free with delete []
 */
int GetMethodsFromScript(LPDISPATCH piDispatch, BSTR *& namesArray, int *& IdArray);
/**
 * @brief Is a function available in this scriptlet or activeX/COM DLL ?
 *
 * @param functionName name of the function in WCHAR
 */
BOOL SearchScriptForFunctionName(LPDISPATCH piDispatch, WCHAR * functionName);


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
BOOL InvokePrediffBuffer(BSTR & bstrBuf, int & nChanged, LPDISPATCH piScript);

/** 
 * @brief Call custom plugin functions : text transformation
 */
BOOL InvokeTransformText(CString & text, int & changed, LPDISPATCH piScript, int fncId);

/**
 * @brief Call the plugin "UnpackBufferA" method, event BUFFER_PACK_UNPACK
 *
 * @param pszBuf has unknown format, so a simple char*
 * never owervrites this source buffer
 */
BOOL InvokeUnpackBuffer(COleSafeArray & array, int & nChanged, LPDISPATCH piScript, int & subcode);
/**
 * @brief Call the plugin "PackBufferA" method, event BUFFER_PACK_UNPACK
 *
 * @param pszBuf has unknown format, so a simple char*
 * never owervrites this source buffer
 */
BOOL InvokePackBuffer(COleSafeArray & array, int & nChanged, LPDISPATCH piScript, int subcode);
/**
 * @brief Call the plugin "UnpackFile" method, event FILE_PACK_UNPACK
 */
BOOL InvokeUnpackFile(LPCTSTR fileSource, LPCTSTR fileDest, int & nChanged, LPDISPATCH piScript, int & subCode);
/**
 * @brief Call the plugin "PackFile" method, event FILE_PACK_UNPACK
 */
BOOL InvokePackFile(LPCTSTR fileSource, LPCTSTR fileDest, int & nChanged, LPDISPATCH piScript, int subCode);
/**
 * @brief Call the plugin "PrediffFile" method, event FILE_PREDIFF
 */
BOOL InvokePrediffFile(LPCTSTR fileSource, LPCTSTR fileDest, int & nChanged, LPDISPATCH piScript);


#endif //__PLUGINS_H__