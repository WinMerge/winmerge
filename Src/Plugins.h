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
	LPDISPATCH	lpDispatch;
	CString			name;
	CString			filtersText;
	CString			description;
	BOOL				bUnicodeMode;
	BOOL				bAutomatic;
	RegList*		filters;
};



typedef CArray<PluginInfo, PluginInfo&>PluginArray;

/**
 * @brief Cache for the scriptlets' interfaces during the life of a thread
 * Create one and one only CScriptsOfThread for each thread, including the main one.
 *
 * @note Each thread needs its own COM interface (necessary for VB)
 */
class CScriptsOfThread
{
public:
	static CScriptsOfThread * GetScriptsOfThreads()
	{
		int i;
		for (i = 0 ; i < NMAXTHREADS ; i++)
			if (m_aAvailableThreads[i] && m_aAvailableThreads[i]->m_nThreadId == GetCurrentThreadId())
				return m_aAvailableThreads[i];
		ASSERT(0);
		return NULL;
	}

private:
	// fixed size array, advantage : no mutex to allocate/free
	static CScriptsOfThread * m_aAvailableThreads[NMAXTHREADS];
	int nTransformationEvents;

public:
	CScriptsOfThread();

	~CScriptsOfThread();

	PluginArray * GetAvailableScripts(LPCWSTR transformationEvent);
	PluginInfo * GetPluginByName(LPCWSTR transformationEvent, LPCTSTR name);

	/// Get the modes (Unicode/Ansi) supported by all plugins
	BOOL GetUnicodeModeOfScripts(LPCWSTR transformationEvent);
	/// Get the modes (Unicode/Ansi) supported by all plugins related to these files
	BOOL GetUnicodeModeOfScripts(LPCWSTR transformationEvent, LPCTSTR filteredText);

	void FreeAllScripts();
	void FreeScriptsForEvent(LPCWSTR transformationEvent);

protected:
	unsigned long m_nThreadId;
	PluginArray ** m_aPluginsByEvent;

	BOOL bInMainThread();
};





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





// Wrappers to call plugins methods

/**
 * @brief Call the plugin "DiffingPreprocessA" method, events PHYSICAL_PREPROCESS, SYNTAX_PREPROCESS
 *
 * @param arrayBuf Overwrite/realloc this buffer
 */
BOOL InvokePreprocessSimpleA(SAFEARRAY* & arrayBuf, UINT & nBufSize, int & nChanged, LPDISPATCH piScript);
/**
 * @brief Call the plugin "DiffingPreprocessW" method, events PHYSICAL_PREPROCESS, SYNTAX_PREPROCESS
 *
 * @param bstrBuf Overwrite/realloc this buffer
 */
BOOL InvokePreprocessSimpleW(BSTR & bstrBuf, UINT & nBufSize, int & nChanged, LPDISPATCH piScript);

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
BOOL InvokeUnpackBuffer(char *& pszBuf, UINT & nBufSize, int & nChanged, LPDISPATCH piScript, int & subcode);
/**
 * @brief Call the plugin "PackBufferA" method, event BUFFER_PACK_UNPACK
 *
 * @param pszBuf has unknown format, so a simple char*
 * never owervrites this source buffer
 */
BOOL InvokePackBuffer(char *& pszBuf, UINT & nBufSize, int & nChanged, LPDISPATCH piScript, int subCode);
/**
 * @brief Call the plugin "UnpackFile" method, event FILE_PACK_UNPACK
 */
BOOL InvokeUnpackFile(LPCTSTR fileSource, LPCTSTR fileDest, int & nChanged, LPDISPATCH piScript, int & subCode);
/**
 * @brief Call the plugin "PackFile" method, event FILE_PACK_UNPACK
 */
BOOL InvokePackFile(LPCTSTR fileSource, LPCTSTR fileDest, int & nChanged, LPDISPATCH piScript, int subCode);


#endif //__PLUGINS_H__