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
#include "Logger.h"

using Poco::SharedMemory;
using Poco::FileOutputStream;
using Poco::ByteOrder;
using Poco::Exception;
using Poco::Buffer;

////////////////////////////////////////////////////////////////////////////////

static void *GetVariantArrayData(VARIANT& array, unsigned& size)
{
	char * parrayData;
	SafeArrayAccessData(array.parray, reinterpret_cast<void**>(&parrayData));
	LONG ubound, lbound;
	SafeArrayGetLBound(array.parray, 1, &lbound);
	SafeArrayGetUBound(array.parray, 1, &ubound);
	size = static_cast<unsigned>(ubound - lbound + 1);
	return parrayData;
}

void storageForPlugins::Initialize()
{
	SysFreeString(m_bstr);
	m_bstr = nullptr;
	VariantClear(&m_array);
	m_tempFilenameDst.clear();
}

void storageForPlugins::SetDataFile(const String& filename, bool bOverwrite /*= false*/) 
{
	FileTextEncoding encoding;
	encoding.SetUnicoding(ucr::NONE);
	encoding.SetCodepage(0);
	SetDataFileEncoding(filename, encoding, bOverwrite); 
}
void storageForPlugins::SetDataFileEncoding(const String& filename, const FileTextEncoding& encoding, bool bOverwrite /*= false*/)
{
	m_filename = filename;
	m_nChangedValid = 0;
	m_nChanged = 0;
	m_bCurrentIsFile = true;
	m_bOverwriteSourceFile = bOverwrite;
	m_codepage = encoding.m_codepage;
	m_nBomSize = encoding.m_bom ? ucr::getBomSize(encoding.m_unicoding) : 0;
	m_fileEncoding = encoding;
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
				RootLogger::Error(e.displayText());
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
			RootLogger::Error(e.displayText());
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
				RootLogger::Error(e.displayText());
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

void storageForPlugins::ValidateInternal(bool bNewIsFile, bool bNewBufferIsUnicode)
{
	assert (m_bCurrentIsFile != bNewIsFile || m_bCurrentBufferIsUnicode != bNewBufferIsUnicode);

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
		if (bNewIsFile || m_bCurrentBufferIsUnicode != bNewBufferIsUnicode)
		{
			if (m_bCurrentBufferIsUnicode)
			{
				SysFreeString(m_bstr);
				m_bstr = nullptr;
			}
			else
				VariantClear(&m_array);
		}

	m_bCurrentBufferIsUnicode = bNewBufferIsUnicode;
	m_bCurrentIsFile = bNewIsFile;
	if (bNewBufferIsUnicode)
	{
		m_codepage = ucr::CP_UCS2LE;
		m_nBomSize = 2;	
	}
	else
	{
		m_codepage = 0;
		m_nBomSize = 0;
	}
}

BSTR * storageForPlugins::GetDataBufferUnicode()
{
	assert(m_codepage != 0);

	if (!m_bCurrentIsFile && m_bCurrentBufferIsUnicode)
		return &m_bstr;

	assert(m_bCurrentIsFile);

	unsigned nchars;
	char * pchar;

	try
	{
		{
			std::unique_ptr<SharedMemory> pshmIn;
			// Get source data
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

const tchar_t *storageForPlugins::GetDataFile()
{
	assert(m_bCurrentIsFile);
	return m_filename.c_str();
}


VARIANT * storageForPlugins::GetDataBufferBytes()
{
	assert(m_bCurrentIsFile || !m_bCurrentBufferIsUnicode);

	if (!m_bCurrentIsFile)
		return &m_array;

	unsigned nchars;
	char * pchar;

	try
	{
		{
			std::unique_ptr<SharedMemory> pshmIn;
			// Get source data
			// Init filedata struct and open file as memory mapped (in file)
			TFile fileIn(m_filename);
			try
			{
				pshmIn.reset(new SharedMemory(fileIn, SharedMemory::AM_READ));

				pchar = pshmIn->begin();
				nchars = static_cast<unsigned>(pshmIn->end() - pchar);
			}
			catch (...)
			{
				if (!fileIn.isDevice() && fileIn.getSize() > 0)
					return nullptr;
				pchar = "";
				nchars = 0;
			}

			// Compute the dest size (in bytes)
			int textForeseenSize = nchars; 
			int textRealSize = textForeseenSize;

			// allocate the memory
			SAFEARRAYBOUND rgsabound = {static_cast<ULONG>(textForeseenSize), 0};
			m_array.vt = VT_UI1 | VT_ARRAY;
			m_array.parray = SafeArrayCreate(VT_UI1, 1, &rgsabound);
			char * parrayData;
			SafeArrayAccessData(m_array.parray, (void**)&parrayData);

			// fill in the data
			std::memcpy(parrayData, pchar, nchars);

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

bool storageForPlugins::SaveAsFile(String& filename)
{
	bool arrayAccessed = false;

	if (m_bCurrentIsFile)
	{
		filename = m_filename;
		return true;
	}

	try
	{
		unsigned nchars = 0;
		char* pchar = nullptr;

		if (m_bCurrentBufferIsUnicode)
		{
			pchar = reinterpret_cast<char*>(m_bstr);
			nchars = SysStringLen(m_bstr) * sizeof(wchar_t);
		}
		else
		{
			pchar = static_cast<char*>(GetVariantArrayData(m_array, nchars));
			arrayAccessed = true;
		}

		GetDestFileName();
		TFile fileOut(m_tempFilenameDst);

		const auto& encoding = m_fileEncoding;

		if (encoding.m_unicoding == ucr::UCS4LE ||
			encoding.m_unicoding == ucr::UCS4BE)
		{
			if (arrayAccessed)
				SafeArrayUnaccessData(m_array.parray);

			GetLastValidFile(filename);
			return false;
		}

		if (!m_bCurrentBufferIsUnicode)
		{
			fileOut.setSize(nchars);
			{
				SharedMemory shmOut(fileOut, SharedMemory::AM_WRITE);
				char* dst = static_cast<char*>(shmOut.begin());

				std::memcpy(dst, pchar, nchars);
			}
		}
		else
		{
			int textRealSize = 0;

			const int bomSize = encoding.m_bom
				? ucr::getBomSize(encoding.m_unicoding)
				: 0;

			const int textForeseenSize = static_cast<int>(nchars * 3) + 1;

			fileOut.setSize(bomSize + textForeseenSize);

			{
				SharedMemory shmOut(fileOut, SharedMemory::AM_WRITE);
				char* dst = static_cast<char*>(shmOut.begin());

				if (bomSize > 0)
					ucr::writeBom(dst, encoding.m_unicoding);

				dst += bomSize;

				bool lossy = false;

				textRealSize = ucr::CrossConvert(
					pchar,
					nchars,
					dst,
					textForeseenSize,
					m_codepage,
					encoding.m_codepage,
					&lossy);

				if (textRealSize <= 0 && nchars > 0)
				{
					if (arrayAccessed)
					{
						SafeArrayUnaccessData(m_array.parray);
						arrayAccessed = false;
					}

					GetLastValidFile(filename);
					return false;
				}
			}

			fileOut.setSize(bomSize + textRealSize);
		}

		if (arrayAccessed)
		{
			SafeArrayUnaccessData(m_array.parray);
			arrayAccessed = false;
		}

		ValidateInternal(true, false);

		filename = m_filename;
		return true;
	}
	catch (...)
	{
		if (arrayAccessed)
		{
			try
			{
				SafeArrayUnaccessData(m_array.parray);
			}
			catch (...)
			{
			}
		}

		GetLastValidFile(filename);
		return false;
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
