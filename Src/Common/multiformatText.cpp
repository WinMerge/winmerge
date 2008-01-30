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
 * @file multiformatText.cpp
 *
 * @brief Implementation of class storageForPlugins
 *
 * @date  Created: 2003-11-24
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "unicoder.h"
#include "multiformatText.h"
#include "files.h"
#include "paths.h"
#include "UniFile.h"
#include "codepage.h"
#include "Environment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////

void storageForPlugins::Initialize()
{
	m_bstr.Empty();
	m_array.Clear();
	m_tempFilenameDst = _T("");
}

void storageForPlugins::SetDataFileAnsi(LPCTSTR filename, BOOL bOverwrite /*= FALSE*/) 
{
	m_filename = filename;
	m_nChangedValid = 0;
	m_nChanged = 0;
	m_bOriginalIsUnicode = FALSE;
	m_bCurrentIsUnicode = FALSE;
	m_bCurrentIsFile = TRUE;
	m_bOverwriteSourceFile = bOverwrite;
	m_codepage = getDefaultCodepage();
	Initialize();
}
void storageForPlugins::SetDataFileUnicode(LPCTSTR filename, BOOL bOverwrite /*= FALSE*/)
{
	m_filename = filename;
	m_nChangedValid = 0;
	m_nChanged = 0;
	m_bOriginalIsUnicode = TRUE;
	m_bCurrentIsUnicode = TRUE;
	m_bCurrentIsFile = TRUE;
	m_bOverwriteSourceFile = bOverwrite;
	m_codepage = getDefaultCodepage();
	Initialize();
}
void storageForPlugins::SetDataFileUnknown(LPCTSTR filename, BOOL bOverwrite /*= FALSE*/) 
{
	BOOL bIsUnicode = FALSE;
	UniMemFile ufile;
	if (ufile.OpenReadOnly(filename))
	{
		bIsUnicode = ufile.ReadBom();
		ufile.Close();
	}
	if (bIsUnicode)
		SetDataFileUnicode(filename, bOverwrite);
	else
		SetDataFileAnsi(filename, bOverwrite);
}

LPCTSTR storageForPlugins::GetDestFileName()
{
	if (m_tempFilenameDst.empty())
	{
		m_tempFilenameDst = env_GetTempFileName(env_GetTempPath(), _T ("_WM"));
	}
	return m_tempFilenameDst.c_str();
}


void storageForPlugins::ValidateNewFile()
{
	// changed data are : file, nChanged
	// nChanged passed as pointer so already upToDate
	// now update file
	if (m_nChangedValid == m_nChanged)
	{
		// plugin succeeded, but nothing changed, just delete the new file
		if (!::DeleteFile(m_tempFilenameDst.c_str()))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s")
				, m_tempFilenameDst.c_str(), GetSysError(GetLastError())));
		}
		// we may reuse the temp filename
		// tempFilenameDst.Empty();
	}
	else
	{
		m_nChangedValid = m_nChanged;
		if (m_bOverwriteSourceFile)
		{
			if (!::DeleteFile(m_filename.c_str()))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s")
					, m_filename.c_str(), GetSysError(GetLastError())));
			}
			::MoveFile(m_tempFilenameDst.c_str(), m_filename.c_str());
		}
		else
		{
			// do not delete the original file name
			m_filename = m_tempFilenameDst;
			// for next transformation, we may overwrite/delete the source file
			m_bOverwriteSourceFile = TRUE;
		}
		m_tempFilenameDst.erase();
	}
}
void storageForPlugins::ValidateNewBuffer()
{
	// changed data are : buffer, nChanged
	// passed as pointers so already upToDate
	m_nChangedValid = m_nChanged;
}

////////////////////////////////////////////////////////////////////////////////

void storageForPlugins::ValidateInternal(BOOL bNewIsFile, BOOL bNewIsUnicode)
{
	ASSERT (m_bCurrentIsFile != bNewIsFile || m_bCurrentIsUnicode != bNewIsUnicode);

	// if we create a file, we remove the remaining previous file 
	if (bNewIsFile)
	{
		if (m_bOverwriteSourceFile)
		{
			::DeleteFile(m_filename.c_str());
			::MoveFile(m_tempFilenameDst.c_str(), m_filename.c_str());
		}
		else
		{
			// do not delete the original file name
			m_filename = m_tempFilenameDst;
			// for next transformation, we may overwrite/delete the source file
			m_bOverwriteSourceFile = TRUE;
		}
		m_tempFilenameDst.erase();
	}

	// old memory structures are freed
	if (!m_bCurrentIsFile)
		// except if the old data have been in situ replaced by new ones
		if (bNewIsFile || m_bCurrentIsUnicode != bNewIsUnicode)
		{
			if (m_bCurrentIsUnicode)
				m_bstr.Empty();
			else
				m_array.Clear();
		}

	m_bCurrentIsUnicode = bNewIsUnicode;
	m_bCurrentIsFile = bNewIsFile;
}

LPCTSTR storageForPlugins::GetDataFileUnicode()
{
	if (m_bCurrentIsFile && m_bCurrentIsUnicode)
		return m_filename.c_str();

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile)
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcsncpy(fileDataIn.fileName, m_filename.c_str(), MAX_PATH);
		fileDataIn.bWritable = FALSE;
		fileDataIn.dwOpenFlags = OPEN_EXISTING;
		BOOL bSuccess = files_openFileMapped(&fileDataIn);
		if (!bSuccess)
			return NULL;

		pchar = (char *)fileDataIn.pMapBase;
		nchars = fileDataIn.dwSize;
	}
	else
	{
		if (m_bCurrentIsUnicode)
		{
			pwchar = BSTR(m_bstr);
			nchars = m_bstr.Length();
		}
		else
		{
			m_array.AccessData((void**)&pchar);
			nchars = m_array.GetOneDimSize();
		}
	}

	// Compute the dest size (in bytes)
	int textForeseenSize = nchars * sizeof(WCHAR) + 6; // from unicoder.cpp maketstring
	int textRealSize = textForeseenSize;

	// Init filedata struct and open file as memory mapped (out file)
	GetDestFileName();
	MAPPEDFILEDATA fileDataOut = {0};
	_tcsncpy(fileDataOut.fileName, m_tempFilenameDst.c_str(), MAX_PATH);
	fileDataOut.bWritable = TRUE;
	fileDataOut.dwOpenFlags = CREATE_ALWAYS;
	fileDataOut.dwSize = textForeseenSize + 2;  
	BOOL bOpenSuccess = files_openFileMapped(&fileDataOut);
	if (bOpenSuccess)
	{
		int bom_bytes = ucr::writeBom(fileDataOut.pMapBase, ucr::UCS2LE);
		if (m_bCurrentIsUnicode)
		{
			CopyMemory((char*)fileDataOut.pMapBase+bom_bytes, pwchar, nchars * sizeof(WCHAR));
			textRealSize = nchars * sizeof(WCHAR);
		}
		else
		{
			// Ansi to UCS-2 conversion, from unicoder.cpp maketstring
			DWORD flags = 0;
			textRealSize = MultiByteToWideChar(m_codepage, flags, pchar, nchars, 
				(WCHAR*)((char*)fileDataOut.pMapBase+bom_bytes), textForeseenSize-1)
				             * sizeof(WCHAR);
		}
		// size may have changed
		files_closeFileMapped(&fileDataOut, textRealSize + 2, FALSE);
	}

	// Release pointers to source data
	if (m_bCurrentIsFile)
		files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		m_array.UnaccessData();

	if (!bOpenSuccess)
		return NULL;

	if ((textRealSize == 0) && (textForeseenSize > 0))
	{
		// conversion error
		::DeleteFile(m_tempFilenameDst.c_str());
		return NULL;
	}

	ValidateInternal(TRUE, TRUE);
	return m_filename.c_str();
}


BSTR * storageForPlugins::GetDataBufferUnicode()
{
	if (!m_bCurrentIsFile && m_bCurrentIsUnicode)
		return &m_bstr;

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile) 
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcsncpy(fileDataIn.fileName, m_filename.c_str(), MAX_PATH);
		fileDataIn.bWritable = FALSE;
		fileDataIn.dwOpenFlags = OPEN_EXISTING;
		BOOL bSuccess = files_openFileMapped(&fileDataIn);
		if (!bSuccess)
			return NULL;

		if (m_bCurrentIsUnicode)
		{
			pwchar = (WCHAR*) ((char*)fileDataIn.pMapBase+2); // pass the BOM
			nchars = (fileDataIn.dwSize-2) / 2;
		}
		else
		{
			pchar = (char *)fileDataIn.pMapBase;
			nchars = fileDataIn.dwSize;
		}
	}
	else
	{
		m_array.AccessData((void**)&pchar);
		nchars = m_array.GetOneDimSize();
	}

	// Compute the dest size (in bytes)
	int textForeseenSize = nchars * sizeof(WCHAR) + 6; // from unicoder.cpp maketstring
	int textRealSize = textForeseenSize;

	// allocate the memory
	CComBSTR tempBSTR(textForeseenSize/sizeof(WCHAR));

	// fill in the data
	WCHAR * pbstrBuffer = (WCHAR*) BSTR(tempBSTR);
	BOOL bAllocSuccess = (pbstrBuffer != NULL);
	if (bAllocSuccess)
	{
		if (m_bCurrentIsUnicode)
		{
			CopyMemory(pbstrBuffer, pwchar, nchars * sizeof(WCHAR));
			textRealSize = nchars * sizeof(WCHAR);
		}
		else
		{
			// Ansi to UCS-2 conversion, from unicoder.cpp maketstring
			DWORD flags = 0;
			textRealSize = MultiByteToWideChar(m_codepage, flags, pchar,
				nchars, pbstrBuffer, textForeseenSize-1) * sizeof(WCHAR);
		}
		// size may have changed, and we can not reallocate a CComBSTR
		// with append, at least we can force the size
		if (FAILED(m_bstr.Append(BSTR(tempBSTR), textRealSize/sizeof(WCHAR))))
			bAllocSuccess = FALSE;
	}

	// Release pointers to source data
	if (m_bCurrentIsFile)
		files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		m_array.UnaccessData();

	if (!bAllocSuccess)
		return NULL;

	ValidateInternal(FALSE, TRUE);
	return &m_bstr;
}

LPCTSTR storageForPlugins::GetDataFileAnsi()
{
	if (m_bCurrentIsFile && !m_bCurrentIsUnicode)
		return m_filename.c_str();

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile)
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcsncpy(fileDataIn.fileName, m_filename.c_str(), MAX_PATH);
		fileDataIn.bWritable = FALSE;
		fileDataIn.dwOpenFlags = OPEN_EXISTING;
		BOOL bSuccess = files_openFileMapped(&fileDataIn);
		if (!bSuccess)
			return NULL;

		pwchar = (WCHAR*) ((char*)fileDataIn.pMapBase+2); // pass the BOM
		nchars = (fileDataIn.dwSize-2) / 2;
	}
	else 
	{
		if (m_bCurrentIsUnicode)
		{
			pwchar = BSTR(m_bstr);
			nchars = m_bstr.Length();
		}
		else
		{
			m_array.AccessData((void**)&pchar);
			nchars = m_array.GetOneDimSize();
		}
	}

	// Compute the dest size (in bytes)
	int textForeseenSize = nchars; 
	if (m_bCurrentIsUnicode)
		textForeseenSize = nchars * 3; // from unicoder.cpp convertToBuffer
	int textRealSize = textForeseenSize;

	// Init filedata struct and open file as memory mapped (out file)
	GetDestFileName();
	MAPPEDFILEDATA fileDataOut = {0};
	_tcsncpy(fileDataOut.fileName, m_tempFilenameDst.c_str(), MAX_PATH);
	fileDataOut.bWritable = TRUE;
	fileDataOut.dwOpenFlags = CREATE_ALWAYS;
	fileDataOut.dwSize = textForeseenSize;  
	BOOL bOpenSuccess = files_openFileMapped(&fileDataOut);
	if (bOpenSuccess)
	{
		if (m_bCurrentIsUnicode)
		{
			// UCS-2 to Ansi conversion, from unicoder.cpp convertToBuffer
			DWORD flags = 0;
			textRealSize = WideCharToMultiByte(m_codepage, flags, pwchar, nchars,
				(char*)fileDataOut.pMapBase, textForeseenSize, NULL, NULL);
		}
		else
		{
			CopyMemory((char*)fileDataOut.pMapBase, pchar, nchars);
		}
		// size may have changed
		files_closeFileMapped(&fileDataOut, textRealSize, FALSE);
	}

	// Release pointers to source data
	if (m_bCurrentIsFile)
		files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		m_array.UnaccessData();

	if (!bOpenSuccess)
		return NULL;

	if ((textRealSize == 0) && (textForeseenSize > 0))
	{
		// conversion error
		::DeleteFile(m_tempFilenameDst.c_str());
		return NULL;
	}

	ValidateInternal(TRUE, FALSE);
	return m_filename.c_str();
}


COleSafeArray * storageForPlugins::GetDataBufferAnsi()
{
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		return &m_array;

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile) 
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcsncpy(fileDataIn.fileName, m_filename.c_str(), MAX_PATH);
		fileDataIn.bWritable = FALSE;
		fileDataIn.dwOpenFlags = OPEN_EXISTING;
		BOOL bSuccess = files_openFileMapped(&fileDataIn);
		if (!bSuccess)
			return NULL;

		if (m_bCurrentIsUnicode)
		{
			pwchar = (WCHAR*) ((char*)fileDataIn.pMapBase+2); // pass the BOM
			nchars = (fileDataIn.dwSize-2) / 2;
		}
		else
		{
			pchar = (char *)fileDataIn.pMapBase;
			nchars = fileDataIn.dwSize;
		}
	}
	else
	{
		pwchar = BSTR(m_bstr);
		nchars = m_bstr.Length();
	}

	// Compute the dest size (in bytes)
	int textForeseenSize = nchars; 
	if (m_bCurrentIsUnicode)
		textForeseenSize = nchars * 3; // from unicoder.cpp convertToBuffer
	int textRealSize = textForeseenSize;

	// allocate the memory
	m_array.CreateOneDim(VT_UI1, textForeseenSize);
	char * parrayData;
	m_array.AccessData((void**)&parrayData);

	// fill in the data
	if (m_bCurrentIsUnicode)
	{
		// UCS-2 to Ansi conversion, from unicoder.cpp convertToBuffer
		DWORD flags = 0;
		textRealSize = WideCharToMultiByte(m_codepage, flags, pwchar, nchars, parrayData, textForeseenSize, NULL, NULL);
	}
	else
	{
		CopyMemory(parrayData, pchar, nchars);
	}
	// size may have changed
	m_array.UnaccessData();
	m_array.ResizeOneDim(textRealSize);

	// Release pointers to source data
	if (m_bCurrentIsFile)
		files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);

	ValidateInternal(FALSE, FALSE);
	return &m_array;
}






//////////////////////////////////////////////////////////////////////////////////
// other conversion functions
// TODO : put in unicoder.cpp or merge in class storageForPlugins

/**
 * @brief Copy UCS-2LE string to UTF-8 string
 *
 * @param nUcs is the size in wide characters of the source string
 * @param nUtf is the size in bytes of the resulting string
 *
 * @return if nUtf = 0, return the size required for the translation buffer
 */
static UINT TransformUcs2ToUtf8(LPCWSTR psUcs, UINT nUcs, LPSTR pcsUtf, UINT nUtf)
{
	if (nUtf == 0)
		// just tell required length
		return ucr::Utf8len_of_string(psUcs, nUcs);

	// the buffer is allocated, output in it directly
	unsigned char * pc = (unsigned char *) pcsUtf;
	int nremains = nUtf;

	// quick way 
	UINT i=0;
	for (i = 0 ; i < nUcs && nremains > 10; ++i)
		nremains -= ucr::to_utf8_advance(psUcs[i], pc);

	// be careful for the end of the buffer, risk of overflow because
	// of the variable length of the UTF-8 character
	unsigned char smallTempBuffer[20];
	int nremainsend = nremains;
	unsigned char * pcTemp = (unsigned char *) smallTempBuffer;
	for ( ; i < nUcs && nremainsend > 0; ++i)
		nremainsend -= ucr::to_utf8_advance(psUcs[i], pcTemp);

	int ncomplement = min(nremains, pcTemp-smallTempBuffer);
	CopyMemory(pc, smallTempBuffer, ncomplement);
	nremains -= ncomplement;

	// return number of written bytes
	return (nUtf - nremains);
}

/**
 * @brief Copy UTF-8 string to UCS-2LE string
 *
 * @return if nUcs = 0, return the size required for the translation buffer
 */
static UINT TransformUtf8ToUcs2(LPCSTR pcsUtf, UINT nUtf, LPWSTR psUcs, UINT nUcs)
{
	if (nUcs == 0)
		// just tell required length
		return ucr::stringlen_of_utf8(pcsUtf, nUtf);

	// the buffer is allocated, output in it directly
	unsigned char * pUtf = (unsigned char * ) pcsUtf;
	LPWSTR pwc = psUcs;
	int nremains = nUcs;

	for (UINT i = 0 ; i < nUtf && nremains > 0; )
	{
		*pwc++ = ucr::GetUtf8Char(pUtf+i);
		nremains --;
		int chlen = ucr::Utf8len_fromLeadByte(pUtf[i]);
		if (chlen < 1) chlen = 1;
		i += chlen;
	}

	// return number of written wchars
	return (nUtf - nremains);
}


BOOL UnicodeFileToOlechar(LPCTSTR filepath, LPCTSTR filepathDst, int & nFileChanged)
{
	UniMemFile ufile;
	if (!ufile.OpenReadOnly(filepath) || !ufile.ReadBom())
		return TRUE; // not unicode file, nothing to do

	int codeOldBOM = ufile.GetUnicoding();
	if (codeOldBOM == ucr::UCS2LE)
		return TRUE; // unicode UCS-2LE, nothing to do
	bool bBom = ufile.HasBom();
	// Finished with examing file contents
	ufile.Close();

	// Init filedataIn struct and open file as memory mapped (input)
	BOOL bSuccess;
	MAPPEDFILEDATA fileDataIn = {0};
	_tcsncpy(fileDataIn.fileName, filepath, MAX_PATH);
	fileDataIn.bWritable = FALSE;
	fileDataIn.dwOpenFlags = OPEN_EXISTING;
	bSuccess = files_openFileMapped(&fileDataIn);
	if (!bSuccess)
		return FALSE;

	char * pszBuf = (char *)fileDataIn.pMapBase;
	UINT nBufSize = fileDataIn.dwSize;

	// first pass : get the size of the destination file
	UINT nSizeOldBOM;
	UINT nchars;
	switch (codeOldBOM)
	{
	case ucr::UTF8:
		nSizeOldBOM = bBom ? 3 : 0;
		nchars = TransformUtf8ToUcs2(pszBuf + nSizeOldBOM, nBufSize - nSizeOldBOM, NULL, 0);
		break;
	case ucr::UCS2BE:
		nSizeOldBOM = 2;
		// same number of characters
		nchars = (nBufSize - nSizeOldBOM)/sizeof(WCHAR);
	}

	UINT nSizeBOM = 2;
	UINT nDstSize = nchars * sizeof(WCHAR); // data size in bytes

	// create the destination file
	MAPPEDFILEDATA fileDataOut = {0};
	_tcsncpy(fileDataOut.fileName, filepathDst, lstrlen(filepathDst)+1);
	fileDataOut.bWritable = TRUE;
	fileDataOut.dwOpenFlags = CREATE_ALWAYS;
	fileDataOut.dwSize = nDstSize + nSizeBOM;
	bSuccess = files_openFileMapped(&fileDataOut);

	// second pass : write the file
	if (bSuccess)
	{
		// write BOM
		ucr::writeBom(fileDataOut.pMapBase, ucr::UCS2LE);

		// write data
		LPWSTR pszWideDst = (LPWSTR) ((char *)fileDataOut.pMapBase+nSizeBOM);
		switch (codeOldBOM)
		{
		case ucr::UTF8:
			TransformUtf8ToUcs2( pszBuf + nSizeOldBOM, nBufSize - nSizeOldBOM, pszWideDst, nchars);
			break;
		case ucr::UCS2BE:
			LPWSTR pszWideBuf = (LPWSTR) (pszBuf + nSizeOldBOM);
			// swap all characters
			UINT i;
			for (i = 0 ; i < nchars ; i++)
			{
				WCHAR wc = pszWideBuf[i];
				wc = ((wc & 0xFF) << 8) + (wc >> 8);
				pszWideDst[i] = wc;
			}
			break;
		}

		files_closeFileMapped(&fileDataOut, nDstSize + nSizeBOM, FALSE);
	}

	files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);

	nFileChanged ++;
	return bSuccess;
}

/**
 * @brief  If file is OLECHAR, then convert it to UTF-8
 *
 * If file has UCS-2LE BOM (is Olechar), then convert it to UTF-8.
 * (No other file conversions are done here; nothing is done to UCS-2BE files)
 * (UCS-2LE is the Windows standard Unicode encoding)
 *
 * Returns FALSE if file is Unicode but opening it fails.
 * Returns FALSE if file has Unicode BOM but is not UCS-2LE.
 * Returns TRUE if file is not Unicode, or if converted file successfully.
 */
BOOL OlecharToUTF8(LPCTSTR filepath, LPCTSTR filepathDst, int & nFileChanged, BOOL bWriteBOM)
{
	UniMemFile ufile;
	if (!ufile.OpenReadOnly(filepath) || !ufile.ReadBom())
		return TRUE; // not unicode file, nothing to do
	int unicoding = ufile.GetUnicoding();
	// Finished with examing file contents
	ufile.Close();

	// OlecharToUTF8 only converts UCS-2LE files to UTF-8
	if (unicoding != ucr::UCS2LE)
		return FALSE;
	
	// Init filedataIn struct and open file as memory mapped (input)
	BOOL bSuccess;
	MAPPEDFILEDATA fileDataIn = {0};
	_tcsncpy(fileDataIn.fileName, filepath, MAX_PATH);
	fileDataIn.bWritable = FALSE;
	fileDataIn.dwOpenFlags = OPEN_EXISTING;
	bSuccess = files_openFileMapped(&fileDataIn);
	if (!bSuccess)
		return FALSE;

	char * pszBuf = (char *)fileDataIn.pMapBase;
	UINT nBufSize = fileDataIn.dwSize;

	// first pass : get the size of the destination file
	UINT nSizeOldBOM = 2;
	UINT nchars = (nBufSize - nSizeOldBOM)/sizeof(WCHAR);

	UINT nSizeBOM = (bWriteBOM) ? 3 : 0;
	UINT nDstSize = TransformUcs2ToUtf8((WCHAR*)(pszBuf + nSizeOldBOM), nchars, NULL, 0);

	// create the destination file
	MAPPEDFILEDATA fileDataOut = {0};
	_tcsncpy(fileDataOut.fileName, filepathDst, lstrlen(filepathDst)+1);
	fileDataOut.bWritable = TRUE;
	fileDataOut.dwOpenFlags = CREATE_ALWAYS;
	fileDataOut.dwSize = nDstSize + nSizeBOM;
	bSuccess = files_openFileMapped(&fileDataOut);

	// second pass : write the file
	if (bSuccess)
	{
		// write BOM
		if (bWriteBOM)
			ucr::writeBom(fileDataOut.pMapBase, ucr::UTF8);

		// write data
		TransformUcs2ToUtf8((WCHAR*)(pszBuf + nSizeOldBOM), nchars, (char *)fileDataOut.pMapBase+nSizeBOM, nDstSize);

		files_closeFileMapped(&fileDataOut, nDstSize + nSizeBOM, FALSE);
	}

	files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);

	nFileChanged ++;
	return bSuccess;
}


