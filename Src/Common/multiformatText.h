/** 
 * @file  FileTransform.h
 *
 * @brief Declaration file for class storageForPlugins
 *
 * @date  Created: 2003-11-24
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef __MULTIFORMATTEXT_H__
#define __MULTIFORMATTEXT_H__

// CComBSTR wraps BSTR initialize/copy
// but function arguments/return value is BSTR as &CComBSTR is a pointer to the internal BSTR
#include <atlbase.h>    // for CComBSTR



/**
 * @brief Storage for data to be processed by plugins. May return data
 * as file or buffer, as ansi or unicode. 
 * First you must load some file into the object. "data" is the content of this
 * file. May be text text (then you may convert ansi<->unicode), or anything.
 * Final save : for text, the format ansi/unicode is the same as the original file.
 *
 * @todo : export final save as UTF-8 for unicode optimization (avoid save+reload the file)
 */
class storageForPlugins
{
public:
	~storageForPlugins()
	{
		if (!tempFilenameDst)
			::DeleteFile(tempFilenameDst);
		bstr.Empty();
		array.Clear();
	}

	/// Get data as unicode buffer (BSTR)
	BSTR * GetDataBufferUnicode();
	/// Get data as ansi buffer (safearray of unsigned char)
	COleSafeArray * GetDataBufferAnsi();
	/// Get data as file (saved as UCS-2 with BOM)
	LPCTSTR GetDataFileUnicode();
	/// Get data as file (saved as Ansi)
	LPCTSTR GetDataFileAnsi();
	/// Get a temporary filename, to be used to save the transformed data 
	LPCTSTR GetDestFileName();
	/// validation for data retrieved by GetDataFileAnsi/GetDataFileUnicode
	void ValidateNewFile();
	/// validation for data retrieved by GetDataBufferAnsi/GetDataBufferUnicode
	void ValidateNewBuffer();

	/// Initial load
	void SetDataFileUnknown(LPCTSTR filename, BOOL bOverwrite = FALSE);
	/// Set codepage to use for ANSI<->UNICODE conversions
	void SetCodepage(int code) { codepage = code; };
	/// Initial load
	void SetDataFileAnsi(LPCTSTR filename, BOOL bOverwrite = FALSE);
	/// Initial load
	void SetDataFileUnicode(LPCTSTR filename, BOOL bOverwrite = FALSE);
	/// Final save, same format as the original file
	BOOL SaveAsFile(CString & filename)
	{
		LPCTSTR newFilename;
		if (bOriginalIsUnicode)
			newFilename = GetDataFileUnicode();
		else
			newFilename = GetDataFileAnsi();
		if (newFilename == NULL)
		{
			GetLastValidFile(filename);
			return FALSE;
		}
		filename = newFilename;
		return TRUE;
	}
	/// Get the last valid file after an error
	/// Warning : the format may be different from the original one
	void GetLastValidFile(CString & filename)
	{
		if (!tempFilenameDst)
			::DeleteFile(tempFilenameDst);
		tempFilenameDst.Empty();
		filename = this->filename;
	}

	/// return number of transformation until now
	int & GetNChanged() { return nChanged; };
	/// return number of valid transformation until now
	int & GetNChangedValid() { return nChangedValid; };
	/// return format of original data
	int GetOriginalMode() { return bOriginalIsUnicode; };

private:
	void Initialize();
	void ValidateInternal(BOOL bNewIsFile, BOOL bNewIsUnicode);

	// original data mode ANSI/UNICODE
	int bOriginalIsUnicode;

	// current format of data : BUFFER/FILE, ANSI/UNICODE
	int m_bCurrentIsUnicode;
	int m_bCurrentIsFile;
	// can we overwrite the current file (different from original file when nChangedValid>=1)
	BOOL bOverwriteSourceFile;	
	// number of valid transformation since load
	int nChangedValid;
	// data storage when mode is BUFFER UNICODE
	CComBSTR bstr;
	// data storage when mode is BUFFER ANSI
	COleSafeArray array;
	// data storage when mode is FILE
	CString filename;
	// error during conversion ?
	BOOL bError;
	// codepage for ANSI mode
	int codepage;

	// temporary number of transformations, transformed by caller
	int nChanged;
	// temporary destination filename
	CString tempFilenameDst;
};


// other conversion functions

/// Convert any unicode file to UCS-2LE
BOOL UnicodeFileToOlechar(CString & filepath, LPCTSTR filepathDst, int & nFileChanged);
/// Convert UCS-2LE file to UTF-8 (for diffutils)
BOOL OlecharToUTF8(CString & filepath, LPCTSTR filepathDst, int & nFileChanged, BOOL bWriteBOM);


#endif //__MULTIFORMATTEXT_H__