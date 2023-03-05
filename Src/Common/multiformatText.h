/** 
 * @file  multiformatText.h
 *
 * @brief Declaration file for class storageForPlugins
 *
 * @date  Created: 2003-11-24
 */
#pragma once

#include "UnicodeString.h"
#include "unicoder.h"
#include "FileTextEncoding.h"
#include <windows.h>
#include <oleauto.h>

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
	storageForPlugins()
	: m_bstr(nullptr)
	, m_bOriginalIsUnicode(false)
	, m_bCurrentIsUnicode(false)
	, m_bCurrentIsFile(false)
	, m_bOverwriteSourceFile(false)
	, m_nChangedValid(0)
	, m_bError(false)
	, m_codepage(0)
	, m_nBomSize(0)
	, m_nChanged(0)
	{
		VariantInit(&m_array);
	}

	~storageForPlugins()
	{
		if (!m_tempFilenameDst.empty()) // "!m_tempFilenameDst" means "never"
			::DeleteFile(m_tempFilenameDst.c_str());
		if (m_bstr != nullptr)
			SysFreeString(m_bstr);
		VariantClear(&m_array);
	}

	/// Get data as unicode buffer (BSTR)
	BSTR * GetDataBufferUnicode();
	/// Get data as ansi buffer (safearray of unsigned char)
	VARIANT * GetDataBufferAnsi();
	/// Get data as file (saved as UCS-2 with BOM)
	const tchar_t *GetDataFileUnicode();
	/// Get data as file (saved as Ansi)
	const tchar_t *GetDataFileAnsi();
	/// Get a temporary filename, to be used to save the transformed data 
	const tchar_t *GetDestFileName();
	/// validation for data retrieved by GetDataFileAnsi/GetDataFileUnicode
	void ValidateNewFile();
	/// validation for data retrieved by GetDataBufferAnsi/GetDataBufferUnicode
	void ValidateNewBuffer();

	/// Initial load
	void SetDataFileUnknown(const String& filename, bool bOverwrite = false);
	/// Set codepage to use for ANSI<->UNICODE conversions
	void SetCodepage(int code) { m_codepage = code; };
	/// Initial load
	void SetDataFileAnsi(const String& filename, bool bOverwrite = false);
	/// Initial load
	void SetDataFileEncoding(const String& filename, FileTextEncoding encoding, bool bOverwrite = false);
	/// Final save, same format as the original file
	bool SaveAsFile(String & filename)
	{
		const tchar_t *newFilename;
		if (m_bOriginalIsUnicode)
			newFilename = GetDataFileUnicode();
		else
			newFilename = GetDataFileAnsi();
		if (newFilename == nullptr)
		{
			GetLastValidFile(filename);
			return false;
		}
		filename = newFilename;
		return true;
	}
	/// Get the last valid file after an error
	/// Warning : the format may be different from the original one
	void GetLastValidFile(String & filename)
	{
		if (!m_tempFilenameDst.empty())
			::DeleteFile(m_tempFilenameDst.c_str());
		m_tempFilenameDst.erase();
		filename = this->m_filename;
	}

	/// return number of transformation until now
	int & GetNChanged() { return m_nChanged; };
	/// return number of valid transformation until now
	int & GetNChangedValid() { return m_nChangedValid; }
	/// return format of original data
	bool GetOriginalMode() const { return m_bOriginalIsUnicode; }
	const String GetDestFileExtension() const { return m_tempFileExtensionDst; }
	void SetDestFileExtension(const String& ext) { if (!ext.empty() && ext.back() != '/') m_tempFileExtensionDst = ext; }

private:
	void Initialize();
	void ValidateInternal(bool bNewIsFile, bool bNewIsUnicode);

// Implementation data
private:
	// original data mode ANSI/UNICODE
	bool m_bOriginalIsUnicode;

	// current format of data : BUFFER/FILE, ANSI/UNICODE
	bool m_bCurrentIsUnicode;
	bool m_bCurrentIsFile;
	// can we overwrite the current file (different from original file when nChangedValid>=1)
	bool m_bOverwriteSourceFile;	
	// number of valid transformation since load
	int m_nChangedValid;
	// data storage when mode is BUFFER UNICODE
	BSTR m_bstr;
	// data storage when mode is BUFFER ANSI
	VARIANT m_array;
	// data storage when mode is FILE
	String m_filename;
	// error during conversion ?
	bool m_bError;
	// codepage for ANSI mode
	int m_codepage;
	// BOM size
	int m_nBomSize;

	// temporary number of transformations, transformed by caller
	int m_nChanged;
	// temporary destination filename
	String m_tempFilenameDst;
	// temporary destination file extension
	String m_tempFileExtensionDst;
};


// other conversion functions

/// Convert file to UTF-8 (for diffutils)
bool AnyCodepageToUTF8(int codepage, const String& filepath, const String& filepathDst, int & nFileChanged, bool bWriteBOM);
