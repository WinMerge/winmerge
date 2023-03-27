/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 * @file multiformatText.cpp
 *
 * @brief Implementation of class storageForPlugins
 *
 * @date  Created: 2003-11-24
 */ 

#include "pch.h"
#define NOMINMAX
#include "multiformatText.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <memory>
#include <Poco/SharedMemory.h>
#include <Poco/FileStream.h>
#include <Poco/ByteOrder.h>
#include <Poco/Buffer.h>
#include <Poco/Exception.h>
#include "unicoder.h"
#include "ExConverter.h"
#include "paths.h"
#include "UniFile.h"
#include "codepage_detect.h"
#include "Environment.h"
#include "TFile.h"
#include "MergeApp.h"

using Poco::SharedMemory;
using Poco::FileOutputStream;
using Poco::ByteOrder;
using Poco::Exception;
using Poco::Buffer;

////////////////////////////////////////////////////////////////////////////////

static void *GetVariantArrayData(VARIANT& array, unsigned& size)
{
	char * parrayData;
	SafeArrayAccessData(array.parray, (void**)&parrayData);
	LONG ubound, lbound;
	SafeArrayGetLBound(array.parray, 1, &lbound);
	SafeArrayGetUBound(array.parray, 1, &ubound);
	size = ubound - lbound;
	return parrayData;
}

void storageForPlugins::Initialize()
{
	SysFreeString(m_bstr);
	m_bstr = nullptr;
	VariantClear(&m_array);
	m_tempFilenameDst.clear();
}

void storageForPlugins::SetDataFileAnsi(const String& filename, bool bOverwrite /*= false*/) 
{
	FileTextEncoding encoding;
	encoding.SetUnicoding(ucr::NONE);
	encoding.SetCodepage(ucr::getDefaultCodepage());
	SetDataFileEncoding(filename, encoding, bOverwrite); 
}
void storageForPlugins::SetDataFileEncoding(const String& filename, FileTextEncoding encoding, bool bOverwrite /*= false*/)
{
	m_filename = filename;
	m_nChangedValid = 0;
	m_nChanged = 0;
	if (encoding.m_unicoding != ucr::NONE && encoding.m_unicoding != ucr::UTF8)
		m_bOriginalIsUnicode = m_bCurrentIsUnicode = true;
	else
		m_bOriginalIsUnicode = m_bCurrentIsUnicode = false;
	m_bCurrentIsFile = true;
	m_bOverwriteSourceFile = bOverwrite;
	m_codepage = encoding.m_codepage;
	m_nBomSize = encoding.m_bom ? ucr::getBomSize(encoding.m_unicoding) : 0;
	Initialize();
}
void storageForPlugins::SetDataFileUnknown(const String& filename, bool bOverwrite /*= false*/) 
{
	FileTextEncoding encoding = codepage_detect::Guess(filename, 1);
	SetDataFileEncoding(filename, encoding, bOverwrite);
}

const tchar_t *storageForPlugins::GetDestFileName()
{
	if (m_tempFilenameDst.empty())
	{
		m_tempFilenameDst = env::GetTemporaryFileName(env::GetTemporaryPath(), _T ("_WM"));
		if (!m_tempFileExtensionDst.empty())
		{
			String tempFilenameDstNew = m_tempFilenameDst + m_tempFileExtensionDst;
			try
			{
				TFile(m_tempFilenameDst).renameTo(tempFilenameDstNew);
				m_tempFilenameDst = std::move(tempFilenameDstNew);
			}
			catch (Exception& e)
			{
				LogErrorStringUTF8(e.displayText());
			}
		}
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
		try
		{
			TFile(m_tempFilenameDst).remove();
		}
		catch (Exception& e)
		{
			LogErrorStringUTF8(e.displayText());
		}
		// we may reuse the temp filename
		// tempFilenameDst.Empty();
	}
	else
	{
		m_nChangedValid = m_nChanged;
		if (m_bOverwriteSourceFile)
		{
			try
			{
				TFile(m_filename).remove();
				TFile(m_tempFilenameDst).renameTo(m_filename);
			}
			catch (Exception& e)
			{
				LogErrorStringUTF8(e.displayText());
			}
		}
		else
		{
			// do not delete the original file name
			m_filename = m_tempFilenameDst;
			// for next transformation, we may overwrite/delete the source file
			m_bOverwriteSourceFile = true;
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

void storageForPlugins::ValidateInternal(bool bNewIsFile, bool bNewIsUnicode)
{
	assert (m_bCurrentIsFile != bNewIsFile || m_bCurrentIsUnicode != bNewIsUnicode);

	// if we create a file, we remove the remaining previous file 
	if (bNewIsFile)
	{
		if (m_bOverwriteSourceFile)
		{
			try
			{
				TFile(m_filename).remove();
				TFile(m_tempFilenameDst).renameTo(m_filename);
			}
			catch (...)
			{
			}
		}
		else
		{
			// do not delete the original file name
			m_filename = m_tempFilenameDst;
			// for next transformation, we may overwrite/delete the source file
			m_bOverwriteSourceFile = true;
		}
		m_tempFilenameDst.erase();
	}

	// old memory structures are freed
	if (!m_bCurrentIsFile)
		// except if the old data have been in situ replaced by new ones
		if (bNewIsFile || m_bCurrentIsUnicode != bNewIsUnicode)
		{
			if (m_bCurrentIsUnicode)
			{
				SysFreeString(m_bstr);
				m_bstr = nullptr;
			}
			else
				VariantClear(&m_array);
		}

	m_bCurrentIsUnicode = bNewIsUnicode;
	m_bCurrentIsFile = bNewIsFile;
	if (bNewIsUnicode)
	{
		m_codepage = ucr::CP_UCS2LE;
		m_nBomSize = 2;	
	}
	else
	{
		m_codepage = ucr::getDefaultCodepage();
		m_nBomSize = 0;
	}
}

const tchar_t *storageForPlugins::GetDataFileUnicode()
{
	if (m_bCurrentIsFile && m_bCurrentIsUnicode)
		return m_filename.c_str();

	unsigned nchars;
	char * pchar = nullptr;

	try
	{
		{
			std::unique_ptr<SharedMemory> pshmIn;
			// Get source data
			if (m_bCurrentIsFile)
			{
				// Init filedata struct and open file as memory mapped (in file)
				TFile fileIn(m_filename);
				try
				{
					pshmIn.reset(new SharedMemory(fileIn, SharedMemory::AM_READ));
					pchar = pshmIn->begin() + m_nBomSize;
					nchars = static_cast<unsigned>(pshmIn->end() - pchar);
				}
				catch (...)
				{
					if (!fileIn.isDevice() && fileIn.getSize() > 0)
						return nullptr;
					pchar = "";
					nchars = 0;
				}			
			}
			else
			{
				if (m_bCurrentIsUnicode)
				{
					pchar = (char *)m_bstr;
					nchars = SysStringLen(m_bstr) * sizeof(wchar_t);
				}
				else
				{
					pchar = (char *)GetVariantArrayData(m_array, nchars);
				}
			}

			// Compute the dest size (in bytes)
			int textForeseenSize = nchars * sizeof(wchar_t) + 6; // from unicoder.cpp maketstring
			int textRealSize = textForeseenSize;

			// Init filedata struct and open file as memory mapped (out file)
			GetDestFileName();

			TFile fileOut(m_tempFilenameDst);
			fileOut.setSize(textForeseenSize + 2);
			int bom_bytes = 0;
			{
				SharedMemory shmOut(fileOut, SharedMemory::AM_WRITE);
				bom_bytes = ucr::writeBom(shmOut.begin(), ucr::UCS2LE);
				// to UCS-2 conversion, from unicoder.cpp maketstring
				bool lossy;
				textRealSize = ucr::CrossConvert(pchar, nchars, (char *)shmOut.begin()+bom_bytes, textForeseenSize-1, m_codepage, ucr::CP_UCS2LE, &lossy);
			}
			// size may have changed
			fileOut.setSize(textRealSize + bom_bytes);

			// Release pointers to source data
			if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
				SafeArrayUnaccessData(m_array.parray);

			if ((textRealSize == 0) && (textForeseenSize > 0))
			{
				// conversion error
				try { TFile(m_tempFilenameDst).remove(); } catch (...) {}
				return nullptr;
			}
		}
		ValidateInternal(true, true);
		return m_filename.c_str();
	}
	catch (...)
	{
		return nullptr;
	}
}


BSTR * storageForPlugins::GetDataBufferUnicode()
{
	if (!m_bCurrentIsFile && m_bCurrentIsUnicode)
		return &m_bstr;

	unsigned nchars;
	char * pchar;

	try
	{
		{
			std::unique_ptr<SharedMemory> pshmIn;
			// Get source data
			if (m_bCurrentIsFile) 
			{
				// Init filedata struct and open file as memory mapped (in file)
				TFile fileIn(m_filename);
				try
				{
					pshmIn.reset(new SharedMemory(fileIn, SharedMemory::AM_READ));

					pchar = pshmIn->begin() + m_nBomSize;
					nchars = static_cast<unsigned>(pshmIn->end() - pchar);
				}
				catch (...)
				{
					if (!fileIn.isDevice() && fileIn.getSize() > 0)
						return nullptr;
					pchar = "";
					nchars = 0;
				}			
			}
			else
			{
				pchar = (char *)GetVariantArrayData(m_array, nchars);
			}

			// Compute the dest size (in bytes)
			int textForeseenSize = nchars * sizeof(wchar_t) + 6; // from unicoder.cpp maketstring
			int textRealSize = textForeseenSize;

			// allocate the memory
			auto tempBSTR = std::make_unique<wchar_t[]>(textForeseenSize);

			// fill in the data
			wchar_t * pbstrBuffer = tempBSTR.get();
			bool bAllocSuccess = (pbstrBuffer != nullptr);
			if (bAllocSuccess)
			{
				// to UCS-2 conversion, from unicoder.cpp maketstring
				bool lossy;
				textRealSize = ucr::CrossConvert(pchar, nchars, (char *)pbstrBuffer, textForeseenSize-1, m_codepage, ucr::CP_UCS2LE, &lossy);
				SysFreeString(m_bstr);
				m_bstr = SysAllocStringLen(tempBSTR.get(), textRealSize / sizeof(wchar_t));
				if (m_bstr == nullptr)
					bAllocSuccess = false;
			}

			// Release pointers to source data
			if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
				SafeArrayUnaccessData(m_array.parray);

			if (!bAllocSuccess)
				return nullptr;
		}
		ValidateInternal(false, true);
		return &m_bstr;
	}
	catch (...)
	{
		return nullptr;
	}
}

const tchar_t *storageForPlugins::GetDataFileAnsi()
{
	if (m_bCurrentIsFile && !m_bCurrentIsUnicode)
		return m_filename.c_str();

	unsigned nchars;
	char * pchar = nullptr;

	try
	{
		{
			std::unique_ptr<SharedMemory> pshmIn;
			// Get source data
			if (m_bCurrentIsFile)
			{
				// Init filedata struct and open file as memory mapped (in file)
				TFile fileIn(m_filename);
				try
				{
					pshmIn.reset(new SharedMemory(fileIn, SharedMemory::AM_READ));

					pchar = pshmIn->begin()+m_nBomSize; // pass the BOM
					nchars = static_cast<unsigned>(pshmIn->end() - pchar);
				}
				catch (...)
				{
					if (!fileIn.isDevice() && fileIn.getSize() > 0)
						return nullptr;
					pchar = "";
					nchars = 0;
				}
			}
			else 
			{
				if (m_bCurrentIsUnicode)
				{
					pchar  = (char *)m_bstr;
					nchars = SysStringLen(m_bstr) * sizeof(wchar_t);
				}
				else
				{
					pchar = (char *)GetVariantArrayData(m_array, nchars);
				}
			}

			// Compute the dest size (in bytes)
			int textForeseenSize = nchars; 
			if (m_bCurrentIsUnicode)
				textForeseenSize = nchars * 3; // from unicoder.cpp convertToBuffer
			int textRealSize = textForeseenSize;

			// Init filedata struct and open file as memory mapped (out file)
			GetDestFileName();
			TFile fileOut(m_tempFilenameDst);
			fileOut.setSize(textForeseenSize);
			if (textForeseenSize > 0)
			{
				SharedMemory shmOut(fileOut, SharedMemory::AM_WRITE);

				if (m_bCurrentIsUnicode)
				{
					// UCS-2 to Ansi conversion, from unicoder.cpp convertToBuffer
					bool lossy;
					textRealSize = ucr::CrossConvert(pchar, nchars, (char *)shmOut.begin(), textForeseenSize, m_codepage, ucr::getDefaultCodepage(), &lossy);
				}
				else
				{
					std::memcpy(shmOut.begin(), pchar, nchars);
				}
			}
			// size may have changed
			fileOut.setSize(textRealSize);

			// Release pointers to source data
			if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
				SafeArrayUnaccessData(m_array.parray);

			if ((textRealSize == 0) && (textForeseenSize > 0))
			{
				// conversion error
				try { TFile(m_tempFilenameDst).remove(); } catch (...) {}
				return nullptr;
			}
		}
		ValidateInternal(true, false);
		return m_filename.c_str();
	}
	catch (...)
	{
		return nullptr;
	}
}


VARIANT * storageForPlugins::GetDataBufferAnsi()
{
	if (!m_bCurrentIsFile && !m_bCurrentIsUnicode)
		return &m_array;

	unsigned nchars;
	char * pchar;

	try
	{
		{
			std::unique_ptr<SharedMemory> pshmIn;
			// Get source data
			if (m_bCurrentIsFile) 
			{
				// Init filedata struct and open file as memory mapped (in file)
				TFile fileIn(m_filename);
				pshmIn.reset(new SharedMemory(fileIn, SharedMemory::AM_READ));

				pchar = pshmIn->begin() + m_nBomSize;
				nchars = static_cast<unsigned>(pshmIn->end() - pchar);
			}
			else
			{
				pchar  = (char *)m_bstr;
				nchars = SysStringLen(m_bstr) * sizeof(wchar_t);
			}

			// Compute the dest size (in bytes)
			int textForeseenSize = nchars; 
			if (m_bCurrentIsUnicode)
				textForeseenSize = nchars * 3; // from unicoder.cpp convertToBuffer
			int textRealSize = textForeseenSize;

			// allocate the memory
			SAFEARRAYBOUND rgsabound = {static_cast<ULONG>(textForeseenSize), 0};
			m_array.vt = VT_UI1 | VT_ARRAY;
			m_array.parray = SafeArrayCreate(VT_UI1, 1, &rgsabound);
			char * parrayData;
			SafeArrayAccessData(m_array.parray, (void**)&parrayData);

			// fill in the data
			if (m_bCurrentIsUnicode)
			{
				// to Ansi conversion, from unicoder.cpp convertToBuffer
				bool lossy;
				textRealSize = ucr::CrossConvert(pchar, nchars, (char *)parrayData, textForeseenSize, m_codepage, ucr::getDefaultCodepage(), &lossy);
			}
			else
			{
				std::memcpy(parrayData, pchar, nchars);
			}
			// size may have changed
			SafeArrayUnaccessData(m_array.parray);
			SAFEARRAYBOUND rgsaboundnew = {static_cast<ULONG>(textRealSize), 0};
			SafeArrayRedim(m_array.parray, &rgsaboundnew);
		}
		ValidateInternal(false, false);
		return &m_array;
	}
	catch (...)
	{
		return nullptr;
	}
}

template<typename T, bool flipbytes>
inline const T *findNextLine(const T *pstart, const T *pend)
{
	for (const T *p = pstart; p < pend; ++p)
	{
		int ch = flipbytes ? ByteOrder::flipBytes(*p) : *p;
		if (ch == '\n')
			return p + 1;
		else if (ch == '\r')
		{
			if (p + 1 < pend && *(p + 1) == (flipbytes ? ByteOrder::flipBytes('\n') : '\n'))
				return p + 2;
			else
				return p + 1;
		}
	}
	return pend;
}

static const char *findNextLine(ucr::UNICODESET unicoding, const char *pstart, const char *pend)
{
	switch (unicoding)
	{
	case ucr::UCS2LE:
		return (const char *)findNextLine<unsigned short, false>((const unsigned short *)pstart, (const unsigned short *)pend);
	case ucr::UCS2BE:
		return (const char *)findNextLine<unsigned short, true>((const unsigned short *)pstart, (const unsigned short *)pend);
	default:
		return findNextLine<char, false>(pstart, pend);
	}
}

bool AnyCodepageToUTF8(int codepage, const String& filepath, const String& filepathDst, int & nFileChanged, bool bWriteBOM)
{
	UniMemFile ufile;
	if (!ufile.OpenReadOnly(filepath))
		return true;
	ufile.ReadBom();
	ucr::UNICODESET unicoding = ufile.GetUnicoding();
	// Finished with examing file contents
	ufile.Close();

	TFile fileIn(filepath);
	try
	{
		// Init filedataIn struct and open file as memory mapped (input)
		SharedMemory shmIn(fileIn, SharedMemory::AM_READ);

		IExconverter *pexconv = Exconverter::getInstance();

		char * pszBuf = shmIn.begin();
		size_t nBufSize = shmIn.end() - shmIn.begin();
		size_t nSizeOldBOM = 0;
		switch (unicoding)
		{
		case ucr::UTF8:
			nSizeOldBOM = 3;
			break;
		case ucr::UCS2LE:
		case ucr::UCS2BE:
			nSizeOldBOM = 2;
			break;
		}

		const size_t minbufsize = 128 * 1024;

		// create the destination file
		FileOutputStream fout(ucr::toUTF8(filepathDst), std::ios::out|std::ios::binary|std::ios::trunc);
		Buffer<char> obuf(minbufsize);
		int64_t pos = nSizeOldBOM;

		// write BOM
		if (bWriteBOM)
		{
			char bom[4];
			fout.write(bom, ucr::writeBom(bom, ucr::UTF8));
		}

		// write data
		for (;;)
		{
			size_t srcbytes = findNextLine(unicoding, pszBuf + pos + minbufsize, pszBuf + nBufSize) - (pszBuf + pos);
			if (srcbytes == 0)
				break;
			if (srcbytes * 3 > obuf.size())
				obuf.resize(srcbytes * 3 * 2, false);
			size_t destbytes = obuf.size();
			if (pexconv != nullptr)
			{
				size_t srcbytes2 = srcbytes;
				if (!pexconv->convert(codepage, ucr::CP_UTF_8, (const unsigned char *)pszBuf+pos, &srcbytes2, (unsigned char *)obuf.begin(), &destbytes))
					throw "failed to convert file contents to utf-8";
			}
			else
			{
				bool lossy = false;
				destbytes = ucr::CrossConvert((const char *)pszBuf+pos, static_cast<unsigned>(srcbytes), obuf.begin(), static_cast<unsigned>(destbytes), codepage, ucr::CP_UTF_8, &lossy);
			}
			fout.write(obuf.begin(), destbytes);
			pos += srcbytes;
		}

		nFileChanged ++;
		return true;
	}
	catch (...)
	{
		if (fileIn.getSize() == 0)
			return true;
		return false;
	}
}

