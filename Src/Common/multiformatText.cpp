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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include "unicoder.h"
#include "multiformatText.h"
#include "files.h"
#include "UniFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////

void storageForPlugins::Initialize()
{
	bstr.Empty();
	array.Clear();
	tempFilenameDst = _T("");
}

void storageForPlugins::SetDataFileAnsi(LPCTSTR filename, BOOL bOverwrite /*= FALSE*/) 
{
	this->filename = filename;
	nChangedValid = 0;
	nChanged = 0;
	bOriginalIsUnicode = FALSE;
	m_bCurrentIsUnicode = FALSE;
	m_bCurrentIsFile = TRUE;
	bOverwriteSourceFile = bOverwrite;
	codepage = ucr::getDefaultCodepage();
	Initialize();
}
void storageForPlugins::SetDataFileUnicode(LPCTSTR filename, BOOL bOverwrite /*= FALSE*/)
{
	this->filename = filename;
	nChangedValid = 0;
	nChanged = 0;
	bOriginalIsUnicode = TRUE;
	m_bCurrentIsUnicode = TRUE;
	m_bCurrentIsFile = TRUE;
	bOverwriteSourceFile = bOverwrite;
	codepage = ucr::getDefaultCodepage();
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
	if (tempFilenameDst.IsEmpty())
	{
		TCHAR tempDir[MAX_PATH] = _T("");
		if (!GetTempPath(countof(tempDir), tempDir))
			return NULL;

		if (!GetTempFileName(tempDir, _T ("_WM"), 0, tempFilenameDst.GetBuffer(MAX_PATH)))
		{
			tempFilenameDst.ReleaseBuffer();
			tempFilenameDst.Empty();
			return NULL;
		}
		tempFilenameDst.ReleaseBuffer();
	}
	return tempFilenameDst;
}


void storageForPlugins::ValidateNewFile()
{
	// changed data are : file, nChanged
	// nChanged passed as pointer so already upToDate
	// now update file
	if (nChangedValid == nChanged)
	{
		// plugin succeeded, but nothing changed, just delete the new file
		::DeleteFile(tempFilenameDst);
		// we may reuse the temp filename
		// tempFilenameDst.Empty();
	}
	else
	{
		nChangedValid = nChanged;
		if (bOverwriteSourceFile)
		{
			::DeleteFile(filename);
			::MoveFile(tempFilenameDst, filename);
		}
		else
		{
			// do not delete the original file name
			filename = tempFilenameDst;
			// for next transformation, we may overwrite/delete the source file
			bOverwriteSourceFile = TRUE;
		}
		tempFilenameDst.Empty();
	}
}
void storageForPlugins::ValidateNewBuffer()
{
	// changed data are : buffer, nChanged
	// passed as pointers so already upToDate
	nChangedValid = nChanged;
}

////////////////////////////////////////////////////////////////////////////////

void storageForPlugins::ValidateInternal(BOOL bNewIsFile, BOOL bNewIsUnicode)
{
	ASSERT (m_bCurrentIsFile != bNewIsFile || m_bCurrentIsUnicode != bNewIsUnicode);

	// if we create a file, we remove the remaining previous file 
	if (bNewIsFile)
	{
		if (bOverwriteSourceFile)
		{
			::DeleteFile(filename);
			::MoveFile(tempFilenameDst, filename);
		}
		else
		{
			// do not delete the original file name
			filename = tempFilenameDst;
			// for next transformation, we may overwrite/delete the source file
			bOverwriteSourceFile = TRUE;
		}
		tempFilenameDst.Empty();
	}

	// old memory structures are freed
	if (!m_bCurrentIsFile)
		// except if the old data have been in situ replaced by new ones
		if (bNewIsFile || m_bCurrentIsUnicode != bNewIsUnicode)
		{
			if (m_bCurrentIsUnicode)
				bstr.Empty();
			else
				array.Clear();
		}

	m_bCurrentIsUnicode = bNewIsUnicode;
	m_bCurrentIsFile = bNewIsFile;
}

LPCTSTR storageForPlugins::GetDataFileUnicode()
{
	if (m_bCurrentIsFile && m_bCurrentIsUnicode)
		return filename;

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile)
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcsncpy(fileDataIn.fileName, filename, filename.GetLength()+1);
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
			pwchar = BSTR(bstr);
			nchars = bstr.Length();
		}
		else
		{
			array.AccessData((void**)&pchar);
			nchars = array.GetOneDimSize();
		}
	}

	// Compute the dest size (in bytes)
	int textForeseenSize = nchars * sizeof(WCHAR) + 6; // from unicoder.cpp maketstring
	int textRealSize = textForeseenSize;

	// Init filedata struct and open file as memory mapped (out file)
	GetDestFileName();
	MAPPEDFILEDATA fileDataOut = {0};
	_tcscpy(fileDataOut.fileName, tempFilenameDst);
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
			textRealSize = MultiByteToWideChar(codepage, flags, pchar, nchars, (WCHAR*)((char*)fileDataOut.pMapBase+bom_bytes), textForeseenSize-1)
				             * sizeof(WCHAR);
		}
		// size may have changed
		files_closeFileMapped(&fileDataOut, textRealSize + 2, FALSE);
	}

	// Release pointers to source data
	if (m_bCurrentIsFile)
		files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		array.UnaccessData();

	if (!bOpenSuccess)
		return NULL;

	if ((textRealSize == 0) && (textForeseenSize > 0))
	{
		// conversion error
		::DeleteFile(tempFilenameDst);
		return NULL;
	}

	ValidateInternal(TRUE, TRUE);
	return filename;
}


BSTR * storageForPlugins::GetDataBufferUnicode()
{
	if (!m_bCurrentIsFile && m_bCurrentIsUnicode)
		return &bstr;

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile) 
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcscpy(fileDataIn.fileName, filename);
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
		array.AccessData((void**)&pchar);
		nchars = array.GetOneDimSize();
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
			textRealSize = MultiByteToWideChar(codepage, flags, pchar, nchars, pbstrBuffer, textForeseenSize-1)
				             * sizeof(WCHAR);
		}
		// size may have changed, and we can not reallocate a CComBSTR
		// with append, at least we can force the size
		if (FAILED(bstr.Append(BSTR(tempBSTR), textRealSize/sizeof(WCHAR))))
			bAllocSuccess = FALSE;
	}

	// Release pointers to source data
	if (m_bCurrentIsFile)
		files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		array.UnaccessData();

	if (!bAllocSuccess)
		return NULL;

	ValidateInternal(FALSE, TRUE);
	return &bstr;
}

LPCTSTR storageForPlugins::GetDataFileAnsi()
{
	if (m_bCurrentIsFile && !m_bCurrentIsUnicode)
		return filename;

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile)
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcsncpy(fileDataIn.fileName, filename, filename.GetLength()+1);
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
			pwchar = BSTR(bstr);
			nchars = bstr.Length();
		}
		else
		{
			array.AccessData((void**)&pchar);
			nchars = array.GetOneDimSize();
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
	_tcscpy(fileDataOut.fileName, tempFilenameDst);
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
			textRealSize = WideCharToMultiByte(codepage, flags, pwchar, nchars, (char*)fileDataOut.pMapBase, textForeseenSize, NULL, NULL);
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
		array.UnaccessData();

	if (!bOpenSuccess)
		return NULL;

	if ((textRealSize == 0) && (textForeseenSize > 0))
	{
		// conversion error
		::DeleteFile(tempFilenameDst);
		return NULL;
	}

	ValidateInternal(TRUE, FALSE);
	return filename;
}


COleSafeArray * storageForPlugins::GetDataBufferAnsi()
{
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		return &array;

	MAPPEDFILEDATA fileDataIn = {0};
	UINT nchars;
	CHAR * pchar;
	WCHAR * pwchar;

	// Get source data
	if (m_bCurrentIsFile) 
	{
		// Init filedata struct and open file as memory mapped (in file)
		_tcscpy(fileDataIn.fileName, filename);
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
		pwchar = BSTR(bstr);
		nchars = bstr.Length();
	}

	// Compute the dest size (in bytes)
	int textForeseenSize = nchars; 
	if (m_bCurrentIsUnicode)
		textForeseenSize = nchars * 3; // from unicoder.cpp convertToBuffer
	int textRealSize = textForeseenSize;

	// allocate the memory
	array.CreateOneDim(VT_UI1, textForeseenSize);
	char * parrayData;
	array.AccessData((void**)&parrayData);

	// fill in the data
	if (m_bCurrentIsUnicode)
	{
		// UCS-2 to Ansi conversion, from unicoder.cpp convertToBuffer
		DWORD flags = 0;
		textRealSize = WideCharToMultiByte(codepage, flags, pwchar, nchars, parrayData, textForeseenSize, NULL, NULL);
	}
	else
	{
		CopyMemory(parrayData, pchar, nchars);
	}
	// size may have changed
	array.UnaccessData();
	array.ResizeOneDim(textRealSize);

	// Release pointers to source data
	if (m_bCurrentIsFile)
		files_closeFileMapped(&fileDataIn, 0xFFFFFFFF, FALSE);

	ValidateInternal(FALSE, FALSE);
	return &array;
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
	for (UINT i = 0 ; i < nUcs && nremains > 10; ++i)
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


BOOL UnicodeFileToOlechar(CString & filepath, LPCTSTR filepathDst, int & nFileChanged)
{
	UniMemFile ufile;
	if (!ufile.OpenReadOnly(filepath) || !ufile.ReadBom())
		return TRUE; // not unicode file, nothing to do

	int codeOldBOM = ufile.GetUnicoding();
	if (codeOldBOM == ucr::UCS2LE)
		return TRUE; // unicode UCS-2LE, nothing to do
	// Finished with examing file contents
	ufile.Close();

	// Init filedataIn struct and open file as memory mapped (input)
	BOOL bSuccess;
	MAPPEDFILEDATA fileDataIn = {0};
	_tcsncpy(fileDataIn.fileName, filepath, filepath.GetLength()+1);
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
		nSizeOldBOM = 3;
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
 * Assumes that input file is either Olechar (UCS-2LE), or not Unicode.
 * That is, if it finds a Unicode BOM (byte order mark), then it assumes file
 * is UCS-2LE, which is the Windows standard Unicode encoding.
 * Returns FALSE if file is Unicode but opening it fails.
 * Returns TRUE if file is not Unicode, or if converted file successfully
 */
BOOL OlecharToUTF8(CString & filepath, LPCTSTR filepathDst, int & nFileChanged, BOOL bWriteBOM)
{
	UniMemFile ufile;
	if (!ufile.OpenReadOnly(filepath) || !ufile.ReadBom())
		return TRUE; // not unicode file, nothing to do
	// Finished with examing file contents
	ufile.Close();
	
	// Init filedataIn struct and open file as memory mapped (input)
	BOOL bSuccess;
	MAPPEDFILEDATA fileDataIn = {0};
	_tcsncpy(fileDataIn.fileName, filepath, filepath.GetLength()+1);
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


