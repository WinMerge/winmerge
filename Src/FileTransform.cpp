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
 *  @file FileTransform.cpp
 *
 *  @brief Implementation of file transformations
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include "FileTransform.h"
#include "Plugins.h"
#include "UnicodeUtf8.h"
#include "files.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL bUnpackerMode = UNPACK_AUTO;

/**
 * @brief Category of transformation : define the transformation events
 *
 * @note USER categories are calls to scriptlets, or VB ActiveX DLL, or VC COM DLL
 * Use text definition : if you add one, nothing to do ; 
 * if you change one, you just have change the dll/scripts for that event
 */
extern LPCWSTR TransformationCategories[] = 
{
	L"NO_TRANSFORMATION",
	L"PREDIFFING",
	L"CONTEXT_MENU",
	L"BUFFER_PACK_UNPACK",
	L"FILE_PACK_UNPACK",
	L"SORT_DIRDIFF",
	NULL,		// last empty : necessary
};




////////////////////////////////////////////////////////////////////////////////
// write data helpers
static BOOL GetTempFileName_WM(LPTSTR tempFilepath)
{
	TCHAR tempDir[MAX_PATH] = _T("");
	if (!GetTempPath(countof(tempDir), tempDir))
		return FALSE;

	return (GetTempFileName(tempDir, _T ("_WM"), 0, tempFilepath));
}

static BOOL WriteBackBufferDontFree(CString & filepath, LPSTR pszBuf, UINT nBufSize, BOOL & bMayOverwrite)
{
	// write back in a new temp file, or overwrite the temp file
	TCHAR tempFilepath[MAX_PATH];
	if (bMayOverwrite == FALSE)
	{
		if (!GetTempFileName_WM(tempFilepath))
			return FALSE;
	}
	else
	{
		_tcsncpy(tempFilepath, filepath, filepath.GetLength()+1);
	}

	MAPPEDFILEDATA fileDataOut = {0};
	_tcsncpy(fileDataOut.fileName, tempFilepath, lstrlen(tempFilepath)+1);
	fileDataOut.bWritable = TRUE;
	fileDataOut.dwOpenFlags = CREATE_ALWAYS;
	fileDataOut.dwSize = nBufSize;
	BOOL bOpenSuccess = files_openFileMapped(&fileDataOut);

	if (bOpenSuccess)
	{
		CopyMemory(fileDataOut.pMapBase, pszBuf, nBufSize);

		files_closeFileMapped(&fileDataOut, nBufSize, FALSE);
		// change the filepath only if everything works
		filepath = tempFilepath;
		bMayOverwrite = TRUE;
	}

	return bOpenSuccess;
}
static BOOL WriteBackBuffer(CString & filepath, LPSTR pszBuf, UINT nBufSize, BOOL & bMayOverwrite)
{
	BOOL bSuccess = WriteBackBufferDontFree(filepath, (LPSTR) pszBuf, nBufSize, bMayOverwrite);
	free(pszBuf);
	return bSuccess;
}
// warning : if we create a WriteBackBuffer for LPWSTR, the compiler can not distinguish both...
static BOOL WriteBackBuffer(CString & filepath, BSTR bstrBuffer, UINT nBufSize, BOOL & bMayOverwrite)
{
	BOOL bSuccess = WriteBackBufferDontFree(filepath, (LPSTR) bstrBuffer, nBufSize*sizeof(WCHAR), bMayOverwrite);
	SysFreeString(bstrBuffer);
	return bSuccess;
}
static BOOL WriteBackBuffer(CString & filepath, SAFEARRAY * arrayBuffer, UINT nBufSize, BOOL & bMayOverwrite)
{
	BOOL bSuccess = WriteBackBufferDontFree(filepath, (LPSTR) arrayBuffer->pvData, nBufSize, bMayOverwrite);
	SafeArrayDestroy(arrayBuffer);
	return bSuccess;
}

////////////////////////////////////////////////////////////////////////////////
// do nothing packer unpacker
int PackPass (char *& pszBuf, UINT & nBufSize, int & nChanged, int handlerSubcode)
{	
	UNREFERENCED_PARAMETER(pszBuf);
	UNREFERENCED_PARAMETER(nBufSize);
	UNREFERENCED_PARAMETER(nChanged);
	UNREFERENCED_PARAMETER(handlerSubcode);
	return TRUE;
}
int UnpackPass (char *& pszBuf, UINT & nBufSize, int & nChanged, int & handlerSubcode)
{	
	UNREFERENCED_PARAMETER(pszBuf);
	UNREFERENCED_PARAMETER(nBufSize);
	UNREFERENCED_PARAMETER(nChanged);
	UNREFERENCED_PARAMETER(handlerSubcode);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// transformations : packing unpacking




BOOL FileTransform_Packing(CString & filepath, PackingInfo handler)
{
	// no handler : return true
	if (handler.fncPack == NULL && handler.pluginName.IsEmpty())
		return TRUE;

	// Init filedata struct for mapping the file in memory 
	MAPPEDFILEDATA fileData = {0};
	_tcsncpy(fileData.fileName, filepath, filepath.GetLength()+1);
	fileData.bWritable = FALSE;
	fileData.dwOpenFlags = OPEN_EXISTING;

	// Define buffer variables
	char * pszOriginal = NULL;
	char * pszBuf = NULL;
	UINT nBufSize;

	// control values
	int nBufferChanged = 0;
	int nFileChanged = 0;
	BOOL bSuccess = FALSE;

	if (handler.fncPack != NULL)
	{
		// open file mapped
		if (!files_openFileMapped(&fileData))
			return FALSE;
		pszOriginal = (char *)fileData.pMapBase;
		nBufSize = (UINT) fileData.dwSize;
		pszBuf = pszOriginal;

		bSuccess = handler.fncPack(pszBuf, nBufSize, nBufferChanged, handler.subcode);
		// close file mapped
		files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);
	}
	else
	{
		if (handler.bWithFile)
		{
			PluginInfo * plugin = CScriptsOfThread::GetScriptsOfThreads()->GetPluginByName(L"FILE_PACK_UNPACK", handler.pluginName);
			LPDISPATCH piScript = plugin->lpDispatch;
			// we need a temporary name
			TCHAR tempFilepath[MAX_PATH];
			GetTempFileName_WM(tempFilepath);
			int nFileChanged = 0; 
			bSuccess = InvokePackFile(filepath, tempFilepath, nFileChanged, piScript, handler.subcode);
			if (bSuccess && nFileChanged > 0)
				filepath = tempFilepath;
			else
				::DeleteFile(tempFilepath);
		}
		else
		{
			// open file mapped
			if (!files_openFileMapped(&fileData))
				return FALSE;
			pszOriginal = (char *)fileData.pMapBase;
			nBufSize = (UINT) fileData.dwSize;
			pszBuf = pszOriginal;

			PluginInfo * plugin = CScriptsOfThread::GetScriptsOfThreads()->GetPluginByName(L"BUFFER_PACK_UNPACK", handler.pluginName);
			LPDISPATCH piScript = plugin->lpDispatch;
			bSuccess = InvokePackBuffer(pszBuf, nBufSize, nBufferChanged, piScript, handler.subcode);
			// close the file mapping
			files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);
		}
	}

	// error ?
	if (bSuccess == FALSE)
		return FALSE;

	// if the buffer changed, save it before leaving
	ASSERT ((nBufferChanged > 0) == (pszOriginal != pszBuf));
	if (nBufferChanged)
	{
		// always keep the original file
		BOOL bOverwrite = FALSE;
		return WriteBackBuffer(filepath, pszBuf, nBufSize, bOverwrite);
	}
	else
		// file transformation or no change
		return TRUE;
}


BOOL FileTransform_Unpacking(CString & filepath, PackingInfo handler, int * handlerSubcode)
{
	// no handler : return true
	if (handler.fncPack == NULL && handler.pluginName.IsEmpty())
		return TRUE;

	// Init filedata struct for mapping the file in memory 
	MAPPEDFILEDATA fileData = {0};
	_tcsncpy(fileData.fileName, filepath, filepath.GetLength()+1);
	fileData.bWritable = FALSE;
	fileData.dwOpenFlags = OPEN_EXISTING;

	// Define buffer variables
	char * pszOriginal = NULL;
	char * pszBuf = NULL;
	UINT nBufSize;

	// control values
	int nBufferChanged = 0;
	int nFileChanged = 0;
	BOOL bSuccess = FALSE;

	if (handler.fncUnpack != NULL)
	{
		// open file mapped
		if (!files_openFileMapped(&fileData))
			return FALSE;
		pszOriginal = (char *)fileData.pMapBase;
		nBufSize = (UINT) fileData.dwSize;
		pszBuf = pszOriginal;

		bSuccess = handler.fncUnpack(pszBuf, nBufSize, nBufferChanged, handler.subcode);
		// close file mapped
		files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);
	}
	else
	{
		if (handler.bWithFile)
		{
			PluginInfo * plugin = CScriptsOfThread::GetScriptsOfThreads()->GetPluginByName(L"FILE_PACK_UNPACK", handler.pluginName);
			LPDISPATCH piScript = plugin->lpDispatch;
			// we need a temporary name
			TCHAR tempFilepath[MAX_PATH];
			GetTempFileName_WM(tempFilepath);
			int nFileChanged = 0; 
			bSuccess = InvokeUnpackFile(filepath, tempFilepath, nFileChanged, piScript, handler.subcode);
			if (bSuccess && nFileChanged > 0)
				filepath = tempFilepath;
			else
				::DeleteFile(tempFilepath);
		}
		else
		{
			// open file mapped
			if (!files_openFileMapped(&fileData))
				return FALSE;
			pszOriginal = (char *)fileData.pMapBase;
			nBufSize = (UINT) fileData.dwSize;
			pszBuf = pszOriginal;

			PluginInfo * plugin = CScriptsOfThread::GetScriptsOfThreads()->GetPluginByName(L"BUFFER_PACK_UNPACK", handler.pluginName);
			LPDISPATCH piScript = plugin->lpDispatch;
			bSuccess = InvokeUnpackBuffer(pszBuf, nBufSize, nBufferChanged, piScript, handler.subcode);
			// close the file mapping
			files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);
		}
	}

	// error ?
	if (bSuccess == FALSE)
		return FALSE;

	// don't change the handler, but assign the sucode
	*handlerSubcode = handler.subcode;

	// if the buffer changed, write it before leaving
	ASSERT ((nBufferChanged > 0) == (pszOriginal != pszBuf));
	if (nBufferChanged)
	{
		// always keep the original file
		BOOL bOverwrite = FALSE;
		return WriteBackBuffer(filepath, pszBuf, nBufSize, bOverwrite);
	}
	else
		// file transformation or no change
		return TRUE;
}


// automatic mode
BOOL FileTransform_Unpacking(CString & filepath, CString filteredText, PackingInfo * handler, int * handlerSubcode)
{
	// Init filedata struct for mapping the file in memory 
	MAPPEDFILEDATA fileData = {0};
	_tcsncpy(fileData.fileName, filepath, filepath.GetLength()+1);
	fileData.bWritable = FALSE;
	fileData.dwOpenFlags = OPEN_EXISTING;

	// filename, to test the extension
	CString filename = filepath.Mid(filepath.ReverseFind('\\')+1);

	// get the scriptlet files
	PluginArray * piBufferScriptArray = 
		CScriptsOfThread::GetScriptsOfThreads()->GetAvailableScripts(L"BUFFER_PACK_UNPACK");
	PluginArray * piFileScriptArray = 
		CScriptsOfThread::GetScriptsOfThreads()->GetAvailableScripts(L"FILE_PACK_UNPACK");

	// Define buffer variables
	char * pszOriginal = NULL;
	char * pszBuf = NULL;
	UINT nBufSize;

	// control values
	int nBufferChanged = 0;
	int nFileChanged = 0;
	BOOL bHandled = FALSE;

	// MAIN LOOP : call each handler, 
	// first call : do you handle the file ?
	// second call : so do it, and tell if it was a success 
	// stop as soon as we have a success
	int step;
	for (step = 0 ; bHandled == FALSE && step < 300 ; step ++)
	{

		// steps 0..99 : call plugins which works with file
		if (step < 100)
		{
			if (step % 100 == piFileScriptArray->GetSize())
			{
				step = 99;
				continue;
			}
			PluginInfo & plugin = piFileScriptArray->ElementAt(step % 100);
			if (plugin.bAutomatic == FALSE)
				continue;
			if (plugin.TestAgainstRegList(filteredText) == FALSE)
				continue;

			handler->fncPack = NULL;
			handler->fncUnpack = NULL;
			handler->pluginName = plugin.name;
			handler->bWithFile = TRUE;
			// we need a temporary name
			TCHAR tempFilepath[MAX_PATH];
			GetTempFileName_WM(tempFilepath);
			int nFileChanged = 0; 
			bHandled = InvokeUnpackFile(filepath, tempFilepath, nFileChanged, plugin.lpDispatch, handler->subcode);
			if (bHandled && nFileChanged > 0)
				filepath = tempFilepath;
			else
				::DeleteFile(tempFilepath);
		}

		if (step == 100)
		{
			// we must open the file mapping for steps 100.299
			if (!files_openFileMapped(&fileData))
				return FALSE;
			pszOriginal = (char *)fileData.pMapBase;
			nBufSize = fileData.dwSize;
			pszBuf = pszOriginal;
		}

		// steps 100..199 : call plugins which works with buffer
		if (step >= 100 && step < 200)
		{

			if (step % 100 == piBufferScriptArray->GetSize())
			{
				step = 199;
				continue;
			}
			PluginInfo & plugin = piBufferScriptArray->ElementAt(step % 100);
			if (plugin.bAutomatic == FALSE)
				continue;
			if (plugin.TestAgainstRegList(filteredText) == FALSE)
				continue;

			handler->fncPack = NULL;
			handler->fncUnpack = NULL;
			handler->pluginName = plugin.name;
			handler->bWithFile = FALSE;
			bHandled = InvokeUnpackBuffer(pszBuf, nBufSize, nBufferChanged, plugin.lpDispatch, handler->subcode);
		}

		// step 200..299 : just recognize, no transformation (Unicode/ANSI)
		if (step >= 200 && step < 300)
		{
			// steps 200 : is there a Unicode BOM ?
			if (step == 200)
			{
				handler->fncUnpack = &TestFileFormatForUnicode;
				handler->fncPack = &PackPass;
				// if we find a Unicode BOM, just set bHandled = TRUE
				// and set the subcode according to the Unicode format
				bHandled = TestFileFormatForUnicode(pszBuf, nBufSize, nBufferChanged, handler->subcode);
			}

			// step 299 : last try, let's hope it is simple ANSI
			if (step == 299)
			{
				handler->fncUnpack = &UnpackPass;
				handler->fncPack = &PackPass;
				handler->subcode = 0;
				bHandled = TRUE;
			}
		}

		if (bHandled)
			break;
	}

	// close the file mapping
	if (step >= 100)
		files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);


	// error, no handler for this format
	if (bHandled == FALSE)
		return FALSE;

	// assign the sucode
	*handlerSubcode = handler->subcode;

	// the handler is now defined
	handler->bToBeScanned = FALSE;

	// if the buffer changed, write it before leaving
	ASSERT ((nBufferChanged > 0) == (pszOriginal != pszBuf));
	if (nBufferChanged)
	{
		// always keep the original file
		BOOL bOverwrite = FALSE;
		return WriteBackBuffer(filepath, pszBuf, nBufSize, bOverwrite);
	}
	else
		// file transformation or no change
		return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// normalize Unicode files to the standard for OLECHAR


BOOL FileTransform_NormalizeUnicode(CString & filepath, CString filteredText, int bMayOverwrite)
{
	TCHAR tempFilepath[MAX_PATH] = _T("");
	if (!GetTempFileName_WM(tempFilepath))
		return FALSE;

	int nFileChanged = 0;
	BOOL bSuccess = NormalizeUnicodeFile(filepath, tempFilepath, nFileChanged); 
	if (!bSuccess)
		return FALSE;

	if (nFileChanged)
	{
		// we do not overwrite so we delete the old file
		if (bMayOverwrite)
			::DeleteFile(filepath);
		// and change the filepath if everything works
		filepath = tempFilepath;
	}
	else
		::DeleteFile(tempFilepath);


	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// transformations : preprocess A and W (as few conversions as possible)
    

// use a SAFEARRAY to store the buffer
// the input file must be ANSI
BOOL FileTransform_PreprocessA(CString & filepath, CString filteredText, int bMayOverwrite)
{
	// Copy the file data into a BSTR
	// Init filedata struct and open file as memory mapped 
	MAPPEDFILEDATA fileData = {0};
	_tcsncpy(fileData.fileName, filepath, filepath.GetLength()+1);
	fileData.bWritable = FALSE;
	fileData.dwOpenFlags = OPEN_EXISTING;
	BOOL bSuccess = files_openFileMapped(&fileData);
	if (!bSuccess)
		return FALSE;

	char * pszBuffer = (char *)fileData.pMapBase;
	UINT nBufSize = fileData.dwSize;

	// "just" build a safeArray
	// Now call SafeArrayCreate with type, dimension, and pointer to vector of dimension descriptors 
	UINT nArraySize = nBufSize;
	SAFEARRAYBOUND rgb = { nArraySize, 0 };		
	SAFEARRAY * arrayBuffer = SafeArrayCreate(VT_UI1, 1, &rgb);
	// Now lock the array for editing and get a pointer to the raw elements
	char *rgelems; 
	SafeArrayAccessData(arrayBuffer, (void**)&rgelems);
	CopyMemory(rgelems, pszBuffer, nBufSize);
	// Unlock the array
	SafeArrayUnaccessData(arrayBuffer);

	files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);


	// Get the scriptlet files
	PluginArray * piScriptArray = 
		CScriptsOfThread::GetScriptsOfThreads()->GetAvailableScripts(L"PREDIFFING");

	int nChanged = 0;

	// MAIN TRANSFORMATION LOOP : each step is one transformation
	// abort at the first error
	int step;
	for (step = 0     ; bSuccess && step < 200 ; step ++)
	{
		if (step < 100)	// physical internal transformations 
		{
		}
		else if (step >= 100 && step < 200)	// physical external transformations (plugins)
		{
			if (step % 100 == piScriptArray->GetSize())
			{
				step = 199;
				continue;
			}
			PluginInfo & plugin = piScriptArray->ElementAt(step % 100);
			if (plugin.bAutomatic == FALSE)
				continue;
			if (plugin.TestAgainstRegList(filteredText) == FALSE)
				continue;
			bSuccess = InvokePrediffingSimpleA(arrayBuffer, nArraySize, nChanged, plugin.lpDispatch);
		}
	}


	// an error ?
	if (!bSuccess)
	{
		SafeArrayDestroy(arrayBuffer);
		return FALSE;
	}
	// nothing changed ?
	if (nChanged == 0)
	{
		SafeArrayDestroy(arrayBuffer);
		return TRUE;
	}

	return WriteBackBuffer(filepath, arrayBuffer, nArraySize, bMayOverwrite);
}



// use a BSTR to store the buffer
// the input file must be UCS-2 LE
BOOL FileTransform_PreprocessW(CString & filepath, CString filteredText, int bMayOverwrite)
{
	// Copy the file data into a BSTR
	// Init filedata struct and open file as memory mapped 
	MAPPEDFILEDATA fileData = {0};
	_tcsncpy(fileData.fileName, filepath, filepath.GetLength()+1);
	fileData.bWritable = FALSE;
	fileData.dwOpenFlags = OPEN_EXISTING;
	BOOL bSuccess = files_openFileMapped(&fileData);
	if (!bSuccess)
		return FALSE;

	char * pszBuffer = (char *)fileData.pMapBase;
	UINT nBufSize = fileData.dwSize;

	// don't read the BOM
	pszBuffer += 2;
	nBufSize -= 2;

	UINT nWideBufSize = nBufSize/sizeof(WCHAR);
	// don't use SysAllocString as the buffer is not zero-terminated
	BSTR bstrBuffer = SysAllocStringLen(0, nWideBufSize);
	CopyMemory(bstrBuffer, pszBuffer, nBufSize);

	files_closeFileMapped(&fileData, 0xFFFFFFFF, FALSE);

	// Get the scriptlet files
	PluginArray * piScriptArray = 
		CScriptsOfThread::GetScriptsOfThreads()->GetAvailableScripts(L"PREDIFFING");


	int nChanged = 0;

	// MAIN TRANSFORMATION LOOP : each step is one transformation
	// abort at the first error
	int step;
	for (step = 0     ; bSuccess && step < 200 ; step ++)
	{
		if (step < 100)	// physical internal transformations 
		{
		}
		else if (step >= 100 && step < 200)	// physical external transformations (plugins)
		{
			if (step % 100 == piScriptArray->GetSize())
			{
				step = 199;
				continue;
			}
			PluginInfo & plugin = piScriptArray->ElementAt(step % 100);
			if (plugin.bAutomatic == FALSE)
				continue;
			if (plugin.TestAgainstRegList(filteredText) == FALSE)
				continue;
			bSuccess = InvokePrediffingSimpleW(bstrBuffer, nWideBufSize, nChanged, plugin.lpDispatch);
		}
	}

	// an error ?
	if (!bSuccess)
	{
		SysFreeString(bstrBuffer);
		return FALSE;
	}

	// last transformation
	// change the buffer from UCS-2 to UTF-8 
	if (bSuccess)
	{
		nBufSize = TransformUcs2ToUtf8( bstrBuffer, nWideBufSize, NULL, 0, FALSE, UCS2LE);
		pszBuffer = (LPSTR) malloc(nBufSize);
		TransformUcs2ToUtf8( bstrBuffer, nWideBufSize, pszBuffer, nBufSize, FALSE, UCS2LE);
		SysFreeString(bstrBuffer);

		nChanged ++;
	}

	// nothing changed ? impossible
	ASSERT (nChanged > 0);

	return WriteBackBuffer(filepath, pszBuffer, nBufSize, bMayOverwrite);
}


BOOL FileTransform_Preprocess(CString & filepath, CString filteredText, int bMayOverwrite)
{
	int attrs = 0;	
	BOOL bIsUnicode = UnicodeCheck(filepath, &attrs);


	// Unicode file, use mode W (Unicode)
	if (bIsUnicode)
		return FileTransform_PreprocessW(filepath, filteredText, bMayOverwrite);


	// ANSI file : do needed plugins support mode A (not Unicode) ?
	// For automatic mode : test plugins which handle the file extension
	BOOL bMode;
	bMode = CScriptsOfThread::GetScriptsOfThreads()->GetUnicodeModeOfScripts(L"PREDIFFING", filteredText);
	// Yes, call function mode A (quicker, avoid conversions)
	if (bMode & SCRIPT_A)
		return FileTransform_PreprocessA(filepath, filteredText, bMayOverwrite);
	

	// No, convert ANSI to UCS-2 LE and use default mode W
	TCHAR tempFilepath[MAX_PATH] = _T("");
	if (!GetTempFileName_WM(tempFilepath))
		return FALSE;

	int nFileChanged = 0;
	BOOL bSuccess = TransformAnsiToStandardUnicode(filepath, tempFilepath, nFileChanged); 
	if (!bSuccess)
		return FALSE;

	if (nFileChanged)
	{
		// we do not overwrite so we delete the old file
		if (bMayOverwrite)
			::DeleteFile(filepath);
		// and change the filepath if everything works
		filepath = tempFilepath;
	}
	else
		::DeleteFile(tempFilepath);


	// now for sure we can overwrite
	bMayOverwrite = TRUE;

	return FileTransform_PreprocessW(filepath, filteredText, bMayOverwrite);
}



////////////////////////////////////////////////////////////////////////////////
// transformation : TextTransform_Interactive


BOOL TextTransform_Interactive(CString & text, LPCWSTR TransformationEvent,
															 int (*callbackUserChooseFunction) (CStringArray*, void*), void * dataForCallback)
{
	// get an array with the available scripts
	PluginArray * piScriptArray = 
		CScriptsOfThread::GetScriptsOfThreads()->GetAvailableScripts(TransformationEvent);

	// allocate structures : function name, ordinal of script, function Id in script
	CDWordArray iInScriptArray;
	CDWordArray iIdArray;
	CStringArray sNamesArray;

	// fill in these structures
	int nFnc = 0;	
	int iScript;
	for (iScript = 0 ; iScript < piScriptArray->GetSize() ; iScript++)
	{
		PluginInfo & plugin = piScriptArray->ElementAt(iScript);
		LPDISPATCH piScript = plugin.lpDispatch;
		BSTR * scriptNamesArray;
		int * scriptIdsArray;
		int nScriptFnc = GetMethodsFromScript(piScript, scriptNamesArray, scriptIdsArray);
		iInScriptArray.SetSize(nFnc+nScriptFnc);
		iIdArray.SetSize(nFnc+nScriptFnc);
		sNamesArray.SetSize(nFnc+nScriptFnc);

		int iFnc;
		for (iFnc = 0 ; iFnc < nScriptFnc ; iFnc++)
		{
			iInScriptArray[nFnc+iFnc] = iScript;
			iIdArray[nFnc+iFnc] = scriptIdsArray[iFnc];
			// the CString = operator provides the conversion if UNICODE is not defined
			sNamesArray[nFnc+iFnc] = scriptNamesArray[iFnc];
		}

		delete [] scriptNamesArray;
		delete [] scriptIdsArray;
		nFnc += nScriptFnc;
	}

	// now ask the user : let him choose a function name in the sNamesArray
	int iFncChosen = callbackUserChooseFunction(&sNamesArray, dataForCallback);

	if (iFncChosen < 0 || iFncChosen >= nFnc)
		return FALSE;

	// execute the transform operation
	BOOL bChanged = FALSE;
	InvokeTransformText(text, bChanged, piScriptArray->GetAt(iInScriptArray[iFncChosen]).lpDispatch, iIdArray[iFncChosen]);

	return bChanged;
}

