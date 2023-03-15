/**
 *  @file   UniFile.cpp
 *  @author Perry Rapp, Creator, 2003-2006
 *  @author Kimmo Varis, 2004-2006
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-12-07 (Kimmo Varis)
 *
 *  @brief Implementation of Unicode enabled file classes.
 *  Classes include memory-mapped reader class and Stdio replacement class.
 */

/* The MIT License
Copyright (c) 2003 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "pch.h"
#include "UniFile.h"
#include <cstdio>
#include <cassert>
#include <memory>
#include <Poco/SharedMemory.h>
#include <Poco/Exception.h>
#include "UnicodeString.h"
#include "unicoder.h"
#include "paths.h" // paths::GetLongbPath()
#include "TFile.h"
#include "cio.h"
#include <windows.h>

using Poco::SharedMemory;
using Poco::Exception;

static void Append(String &strBuffer, const tchar_t *pchTail, size_t cchTail,
		size_t cchBufferMin = 1024);

/**
 * @brief The constructor.
 */
UniFile::UniError::UniError()
{
	ClearError();
}

/////////////
// UniLocalFile
/////////////

/** @brief Create disconnected UniLocalFile, but with name */
UniLocalFile::UniLocalFile()
{
	Clear();
}

/** @brief Reset all variables to empty */
void UniLocalFile::Clear()
{
	m_statusFetched = 0;
	m_lineno = -1;
	m_filesize = 0;
	m_filepath.clear();
	m_filename.clear();
	m_charsize = 1;
	m_codepage = ucr::getDefaultCodepage();
	m_txtstats.clear();
	m_unicoding = ucr::NONE;
	m_bom = false;
	m_bUnicodingChecked = false;
	m_bUnicode = false;
}

/**
 * @brief Get file status into member variables
 *
 * Reads file's status (size and full path).
 * @return true on success, false on failure.
 * @note Function sets filesize member to zero, and status as read
 * also when failing. That is needed so caller doesn't need to waste
 * time checking if file already exists (and ignores return value).
 */
bool UniLocalFile::DoGetFileStatus()
{
	m_statusFetched = -1;
	m_lastError.ClearError();

	m_filepath = paths::GetLongPath(m_filepath);

	try
	{
		m_filesize = TFile(m_filepath).getSize();
		if (m_filesize == 0)
		{
			// if m_filesize equals zero, the file size is really zero or the file is a symbolic link.
			// use GetCompressedFileSize() to get the file size of the symbolic link target whether the file is symbolic link or not.
			// if the file is not symbolic link, GetCompressedFileSize() will return zero.
			// NOTE: GetCompressedFileSize() returns error for pre-W2K windows versions
			DWORD dwFileSizeLow, dwFileSizeHigh;
			
			dwFileSizeLow = GetCompressedFileSize(TFile(m_filepath).wpath().c_str(), &dwFileSizeHigh);
			if (GetLastError() == 0)
				m_filesize = ((int64_t)dwFileSizeHigh << 32) + dwFileSizeLow;
		}
		m_statusFetched = 1;

		return true;
	}
	catch (Exception& e)
	{
		m_filesize = 0;
		m_statusFetched = 1; // Yep, done for this file still
		LastErrorCustom(ucr::toTString(e.displayText()));
		return false;
	}
}

/**
 * @brief Checks if the file is an unicode file.
 * This function Checks if the file is recognized unicode file. This detection
 * includes reading possible BOM bytes and trying to detect UTF-8 files
 * without BOM bytes.
 * @return true if file is an unicode file, false otherwise.
 */
bool UniLocalFile::IsUnicode()
{
	if (!m_bUnicodingChecked)
		m_bUnicode = ReadBom();
	return m_bUnicode;
}

/** @brief Record a custom error */
void UniLocalFile::LastErrorCustom(const String& desc)
{
	m_lastError.ClearError();

	m_lastError.desc = desc;
}

/////////////
// UniMemFile
/////////////

UniMemFile::UniMemFile()
		: m_hMapping(nullptr)
		, m_base(nullptr)
		, m_data(nullptr)
		, m_current(nullptr)
{
}

void UniMemFile::Close()
{
	Clear();
	m_base = nullptr;
	m_data = nullptr;
	m_current = nullptr;
	if (m_hMapping != nullptr)
	{
		delete m_hMapping;
		m_hMapping = nullptr;
	}
}

/** @brief Get file status into member variables */
bool UniMemFile::GetFileStatus()
{
	if (!IsOpen()) return false;
	return DoGetFileStatus();
}

/** @brief Open file for generic read-only access */
bool UniMemFile::OpenReadOnly(const String& filename)
{
	return Open(filename, AM_READ);
}

/** @brief Open file for generic read-write access */
bool UniMemFile::Open(const String& filename)
{
	return Open(filename, AM_WRITE);
}

/** @brief Open file with specified arguments */
bool UniMemFile::Open(const String& filename, AccessMode mode)
{
	// We use an internal workhorse to make it easy to close on any error
	if (!DoOpen(filename, mode))
	{
		Close();
		return false;
	}
	return true;
}

/** @brief Internal implementation of Open */
bool UniMemFile::DoOpen(const String& filename, AccessMode mode)
{
	Close();

	m_filename = filename;
	m_filepath = m_filename; // TODO: Make canonical ?

	try
	{
		TFile file(filename);
		try
		{
			m_hMapping = new SharedMemory(file, static_cast<SharedMemory::AccessMode>(mode));
		}
		catch (Exception&)
		{
			if (file.getSize() == 0)
			{
				m_lineno = 0;
				return true;
			}
			throw;
		}
	}
	catch (Exception& e)
	{
		LastErrorCustom(ucr::toTString(e.displayText()));
		m_hMapping = nullptr;
		return false;
	}

	m_lineno = 0; // GetFileStatus requires file be "open", which means nonnegative line number
	if (!GetFileStatus())
		return false;
	m_lineno = -1;

#ifndef _WIN64
	unsigned sizehi = (unsigned)(m_filesize >> 32);
	unsigned sizelo = (unsigned)(m_filesize & 0xFFFFFFFF);

	if (sizehi || sizelo > 0x7FFFFFFF)
	{
		LastErrorCustom(_T("UniMemFile cannot handle files over 2 gigabytes"));
		return false;
	}
#endif

	if (m_filesize == 0)
	{
		// Allow opening empty file, but memory mapping doesn't work on such
		// m_base and m_current are 0 from the Close call above
		// so ReadString will correctly return empty EOF immediately
		m_lineno = 0;
		return true;
	}

	m_base = reinterpret_cast<unsigned char *>(m_hMapping->begin());
	m_data = m_base;
	m_current = m_base;
	m_lineno = 0;

	return true;
}

/**
 * @brief Check for Unicode BOM (byte order mark) at start of file
 *
 * @note This code only checks for UCS-2LE, UCS-2BE, and UTF-8 BOMs (no UCS-4).
 */
bool UniMemFile::ReadBom()
{
	if (!IsOpen())
		return false;

	unsigned char * lpByte = m_base;
	m_charsize = 1;
	bool unicode = true;
	bool bom = false;

	m_unicoding = ucr::DetermineEncoding(lpByte, m_filesize, &bom);
	switch (m_unicoding)
	{
	case ucr::UCS2LE:
		m_codepage = ucr::CP_UCS2LE;
		m_charsize = 2;
		m_data = lpByte + 2;
		break;
	case ucr::UCS2BE:
		m_codepage = ucr::CP_UCS2BE;
		m_charsize = 2;
		m_data = lpByte + 2;
		break;
	case ucr::UTF8:
		m_codepage = ucr::CP_UTF_8;
		m_data = lpByte + (bom ? 3 : 0);
		break;
	default:
		m_data = m_base;
		unicode = false;
		break;
	}

	m_bom = bom;
	m_current = m_data;
	m_bUnicodingChecked = true;
	return unicode;
}

/**
 * @brief Read one (DOS or UNIX or Mac) line. Do not include eol chars.
 * @param [out] line Line read.
 * @param [out] lossy `true` if there were lossy encoding.
 * @return `true` if there is more lines to read, `false` when last line is read.
 */
bool UniMemFile::ReadString(String & line, bool * lossy)
{
	String eol;
	return ReadString(line, eol, lossy);
}

bool UniMemFile::ReadStringAll(String& text)
{
	text.clear();
	text.reserve(static_cast<size_t>(m_filesize));

	String tmp, eol;
	bool lossy = false, lossytmp = false, last;
	do
	{
		last = ReadString(tmp, eol, &lossytmp);
		text += tmp;
		text += eol;
		if (lossytmp)
			lossy = true;
	} while (last);
	return lossy;
}

/**
 * @brief Append characters to string.
 * This function appends characters to the string. The storage for the string
 * is grown exponentially to avoid unnecessary allocations and copying.
 * @param [in, out] strBuffer A string to which new characters are appended.
 * @param [in] ccHead Index in the string where new chars are appended.
 * @param [in] pchTail Characters to append.
 * @param [in] cchTail Amount of characters to append.
 * @param [in] cchBufferMin Minimum size for the buffer.
 * @return New length of the string.
 */
static void Append(String &strBuffer, const tchar_t *pchTail,
		size_t cchTail, size_t cchBufferMin)
{
	size_t cchBuffer = strBuffer.capacity();
	size_t cchHead = strBuffer.length();
	size_t cchLength = cchHead + cchTail;
	while (cchBuffer < cchLength)
	{
		assert((cchBufferMin & cchBufferMin - 1) == 0); // must be a power of 2
		cchBuffer &= ~(cchBufferMin - 1); // truncate to a multiple of cchBufferMin
		cchBuffer += cchBuffer;
		if (cchBuffer < cchBufferMin)
			cchBuffer = cchBufferMin;
	}
	strBuffer.reserve(cchBuffer);
	strBuffer.append(pchTail, cchTail);
}

/**
 * @brief Record occurrence of binary zero to stats
 */
static void RecordZero(UniFile::txtstats & txstats, size_t offset)
{
	++txstats.nzeros;
}

/**
 * @brief Read one (DOS or UNIX or Mac) line.
 * @param [out] line Line read.
 * @param [out] eol EOL bytes read (if any).
 * @param [out] lossy `true` if there were lossy encoding.
 * @return true if there is more lines to read, false when last line is read.
 */
bool UniMemFile::ReadString(String & line, String & eol, bool * lossy)
{
	line.clear();
	eol.clear();
	const tchar_t * pchLine = (const tchar_t *)m_current;
	
	// shortcut methods in case file is in the same encoding as our Strings

#ifdef _UNICODE
	if (m_unicoding == ucr::UCS2LE)
	{
		int cchLine = 0;
		// If there aren't any wchars left in the file, return `false` to indicate EOF
		if (m_current - m_base + 1 >= m_filesize)
			return false;
		// Loop through wchars, watching for eol chars or zero
		while (m_current - m_base + 1 < m_filesize)
		{
			wchar_t wch = *(wchar_t *)m_current;
			size_t wch_offset = (m_current - m_base);
			m_current += 2;
			if (wch == '\n' || wch == '\r')
			{
				eol += wch;
				if (wch == '\r')
				{
					if (m_current - m_base + 1 < m_filesize && *(wchar_t *)m_current == '\n')
					{
						eol += '\n';
						m_current += 2;
						++m_txtstats.ncrlfs;
					}
					else
					{
						++m_txtstats.ncrs;
					}
				}
				else
				{
					++m_txtstats.nlfs;
				}
				++m_lineno;
				line.assign(pchLine, cchLine);
				return true;
			}
			if (!wch)
			{
				RecordZero(m_txtstats, wch_offset);
			}
			++cchLine;
		}
		line.assign(pchLine, cchLine);
		return true;
	}
#else
	if (m_unicoding == ucr::NONE && ucr::EqualCodepages(m_codepage, GetACP()))
	{
		int cchLine = 0;
		// If there aren't any bytes left in the file, return `false` to indicate EOF
		if (m_current - m_base >= m_filesize)
			return false;
		// Loop through chars, watching for eol chars or zero
		while (m_current - m_base < m_filesize)
		{
			char ch = *m_current;
			size_t ch_offset = (m_current - m_base);
			++m_current;
			if (ch == '\n' || ch == '\r')
			{
				eol += ch;
				if (ch == '\r')
				{
					if (m_current - m_base < m_filesize && *m_current == '\n')
					{
						eol += '\n';
						++m_current;
						++m_txtstats.ncrlfs;
					}
					else
					{
						++m_txtstats.ncrs;
					}
				}
				else
				{
					++m_txtstats.nlfs;
				}
				++m_lineno;
				line.assign(pchLine, cchLine);
				return true;
			}
			if (!ch)
			{
				RecordZero(m_txtstats, ch_offset);
			}
			++cchLine;
		}
		line.assign(pchLine, cchLine);
		return true;
	}
#endif

	if (m_current - m_base + (m_charsize - 1) >= m_filesize)
		return false;

	// Handle 8-bit strings in line chunks because of multibyte codings (eg, 936)
	if (m_unicoding == ucr::NONE)
	{
		bool eof = true;
		unsigned char *eolptr = nullptr;
		for (eolptr = m_current; (eolptr - m_base + (m_charsize - 1) < m_filesize); ++eolptr)
		{
			if (*eolptr == '\n' || *eolptr == '\r')
			{
				eof = false;
				break;
			}
			
			if (*eolptr == '\x00')
			{
				size_t offset = (eolptr - m_base);
				RecordZero(m_txtstats, offset);
			}
		}
		bool success = ucr::maketstring(line, (const char *)m_current, eolptr-m_current, m_codepage, lossy);
		if (!success)
		{
			return false;
		}
		if (lossy && *lossy)
			++m_txtstats.nlosses;
		if (!eof)
		{
			eol += (tchar_t) * eolptr;
			++m_lineno;
			if (*eolptr == '\r')
			{
				if (eolptr - m_base + m_charsize + (m_charsize - 1) < m_filesize && eolptr[1] == '\n')
				{
					eol += '\n';
					++m_txtstats.ncrlfs;
				}
				else
					++m_txtstats.ncrs;
			}
			else
				++m_txtstats.nlfs;
		}
		m_current = eolptr + eol.length();
		// TODO: What do we do if save was lossy ?
		return !eof;
	}

	while (m_current - m_base + (m_charsize - 1) < m_filesize)
	{
		unsigned ch = 0;
		int  utf8len = 0;
		bool doneline = false;

		if (m_unicoding == ucr::UTF8)
		{
			// check for end in middle of UTF-8 character
			utf8len = ucr::Utf8len_fromLeadByte(*m_current);
			if (m_current - m_base + utf8len > m_filesize)
			{
				ch = '?';
				m_current = m_base + m_filesize;
				doneline = true;
			}
			// Handle bad UTF-8 or UTF-8 outside of UCS-2
			// (Convert bad bytes individually to '?'
			else if (utf8len < 1 || utf8len > 4)
			{
				ch = '?';
				utf8len = 1;
			}
			else
			{
				ch = ucr::GetUtf8Char(m_current);
			}
		}
		else
		{
			ch = ucr::get_unicode_char(m_current, (ucr::UNICODESET)m_unicoding, m_codepage);
		}
		// convert from Unicode codepoint to tchar_t string
		// could be multicharacter if decomposition took place, for example
		bool lossy1 = false; // try to avoid lossy conversion
		String sch;
		ucr::maketchar(sch, ch, lossy1);
		if (lossy1)
			++m_txtstats.nlosses;
		if (sch.length() >= 1)
			ch = sch[0];
		else
			ch = 0;


		if (ch == '\r')
		{
			eol = _T("\r");
			doneline = true;
			bool crlf = false;
			// check for crlf pair
			if (m_current - m_base + 2 * m_charsize - 1 < m_filesize)
			{
				// For UTF-8, this ch1 will be wrong if character is non-ASCII
				// but we only check it against \n here, so it doesn't matter
				unsigned ch1 = ucr::get_unicode_char(m_current + m_charsize, (ucr::UNICODESET)m_unicoding);
				if (ch1 == '\n')
				{
					crlf = true;
				}
			}
			if (crlf)
			{
				eol = _T("\r\n");
				++m_txtstats.ncrlfs;
				// advance an extra character to skip the following lf
				m_current += m_charsize;
			}
			else
			{
				++m_txtstats.ncrs;
			}
		}
		else if (ch == '\n')
		{
			eol = _T("\n");
			doneline = true;
			++m_txtstats.nlfs;
		}
		else if (!ch)
		{
			size_t offset = (m_current - m_base);
			RecordZero(m_txtstats, offset);
		}
		// always advance to next character
		if (m_unicoding == ucr::UTF8)
		{
			m_current += utf8len;
		}
		else
		{
			m_current += m_charsize;
		}
		if (doneline)
		{
			if (!eol.empty())
				++m_lineno;
			return true;
		}
		Append(line, sch.c_str(), sch.length());
	}
	return true;
}

/**
 * @brief Write one line (doing any needed conversions)
 */
bool UniMemFile::WriteString(const String & line)
{
	assert(false); // unimplemented -- currently cannot write to a UniMemFile!
	return false;
}

/////////////
// UniStdioFile
/////////////

UniStdioFile::UniStdioFile()
		: m_fp(nullptr)
		, m_data(0)
		, m_ucrbuff(128)
{
}

UniStdioFile::~UniStdioFile()
{
	Close();
}

void UniStdioFile::Close()
{
	if (IsOpen())
	{
		fclose(m_fp);
		m_fp = nullptr;
	}
	m_statusFetched = 0;
	m_filesize = 0;
	// preserve m_filepath
	// preserve m_filename
	m_data = 0;
	m_lineno = -1;
	// preserve m_unicoding
	// preserve m_charsize
	// preserve m_codepage
	m_txtstats.clear();
}

/** @brief Get file status into member variables */
bool UniStdioFile::GetFileStatus()
{
	if (IsOpen()) return false; // unfortunately we'll hit our lock

	return DoGetFileStatus();
}

bool UniStdioFile::OpenReadOnly(const String& filename)
{
	return Open(filename, _T("rb"));
}
bool UniStdioFile::OpenCreate(const String& filename)
{
	return Open(filename, _T("w+b"));
}
bool UniStdioFile::OpenCreateUtf8(const String& filename)
{
	if (!OpenCreate(filename))
		return false;
	SetUnicoding(ucr::UTF8);
	return true;

}
bool UniStdioFile::Open(const String& filename, const String& mode)
{
	if (!DoOpen(filename, mode))
	{
		Close();
		return false;
	}
	return true;
}

bool UniStdioFile::DoOpen(const String& filename, const String& mode)
{
	Close();

	m_filepath = filename;
	m_filename = filename; // TODO: Make canonical ?

	// Fails if file doesn't exist (when we are creating new file)
	// But we don't care since size is set to 0 anyway.
	GetFileStatus();

	if (cio::tfopen_s(&m_fp, m_filepath, mode.c_str()) != 0)
		return false;

#ifndef _WIN64
	unsigned sizehi = (unsigned)(m_filesize >> 32);

	if (sizehi)
	{
		// TODO: We could do this in MSC_VER 7+ I think

		LastErrorCustom(_T("UniStdioFile cannot handle files over 4 gigabytes"));
		return false;
	}
#endif

	m_lineno = 0;
	return true;
}

bool UniStdioFile::SetVBuf(int mode, size_t size)
{
	if (!IsOpen())
		return false;
	return setvbuf(m_fp, NULL, mode, size) == 0;
}

/** @brief Record a custom error */
void UniStdioFile::LastErrorCustom(const String& desc)
{
	m_lastError.ClearError();

	m_lastError.desc = desc;
}

/**
 * @brief Check for Unicode BOM (byte order mark) at start of file
 *
 * @note This code only checks for UCS-2LE, UCS-2BE, and UTF-8 BOMs (no UCS-4).
 */
bool UniStdioFile::ReadBom()
{
	if (!IsOpen())
		return false;

	fseek(m_fp, 0, SEEK_SET);

	// Read 8 KB at max for get enough data determining UTF-8 without BOM.
	const size_t max_size = 8 * 1024;
	unsigned char buff[max_size];

	size_t bytes = fread(buff, 1, max_size, m_fp);
	m_charsize = 1;
	bool unicode = true;
	bool bom = false;

	m_unicoding = ucr::DetermineEncoding(buff, bytes, &bom);
	switch (m_unicoding)
	{
	case ucr::UCS2LE:
		m_codepage = ucr::CP_UCS2LE;
		m_charsize = 2;
		m_data = 2;
		break;
	case ucr::UCS2BE:
		m_codepage = ucr::CP_UCS2BE;
		m_charsize = 2;
		m_data = 2;
		break;
	case ucr::UTF8:
		m_codepage = ucr::CP_UTF_8;
		m_data = bom ? 3 : 0;
		break;
	default:
		m_data = 0;
		unicode = false;
		break;
	}

	fseek(m_fp, (long)m_data, SEEK_SET);
	m_bom = bom;
	return unicode;
}

bool UniStdioFile::ReadString(String & line, bool * lossy)
{
	assert(false); // unimplemented -- currently cannot read from a UniStdioFile!
	return false;
}

bool UniStdioFile::ReadString(String & line, String & eol, bool * lossy)
{
	assert(false); // unimplemented -- currently cannot read from a UniStdioFile!
	return false;
}

bool UniStdioFile::ReadStringAll(String & line)
{
	assert(false); // unimplemented -- currently cannot read from a UniStdioFile!
	return false;
}

/** @brief Write BOM (byte order mark) if Unicode file */
int UniStdioFile::WriteBom()
{
	if (m_unicoding == ucr::UCS2LE && m_bom)
	{
		unsigned char bom[] = "\xFF\xFE";
		fseek(m_fp, 0, SEEK_SET);
		fwrite(bom, 1, 2, m_fp);
		m_data = 2;
	}
	else if (m_unicoding == ucr::UCS2BE && m_bom)
	{
		unsigned char bom[] = "\xFE\xFF";
		fseek(m_fp, 0, SEEK_SET);
		fwrite(bom, 1, 2, m_fp);
		m_data = 2;
	}
	else if (m_unicoding == ucr::UTF8 && m_bom)
	{
		unsigned char bom[] = "\xEF\xBB\xBF";
		fseek(m_fp, 0, SEEK_SET);
		fwrite(bom, 1, 3, m_fp);
		m_data = 3;
	}
	else
	{
		m_data = 0;
	}
	return (int)m_data;
}

/**
 * @brief Write one line (doing any needed conversions)
 */
bool UniStdioFile::WriteString(const String & line)
{
	// shortcut the easy cases
#ifdef _UNICODE
	if (m_unicoding == ucr::UCS2LE)
#else
	if (m_unicoding == ucr::NONE && ucr::EqualCodepages(m_codepage, GetACP()))
#endif
	{
		size_t bytes = line.length() * sizeof(tchar_t);
		size_t wbytes = fwrite(line.c_str(), 1, bytes, m_fp);
		if (wbytes != bytes)
			return false;
		return true;
	}

	ucr::UNICODESET unicoding1 = ucr::NONE;
	int codepage1 = 0;
	ucr::getInternalEncoding(&unicoding1, &codepage1); // What String & tchar_ts represent
	const unsigned char * src = (const unsigned char *)line.c_str();
	size_t srcbytes = line.length() * sizeof(tchar_t);
	bool lossy = ucr::convert(unicoding1, codepage1, src, srcbytes, (ucr::UNICODESET)m_unicoding, m_codepage, &m_ucrbuff);
	// TODO: What to do about lossy conversion ?
	size_t wbytes = fwrite(m_ucrbuff.ptr, 1, m_ucrbuff.size, m_fp);
	if (wbytes != m_ucrbuff.size)
		return false;
	return true;
}

int64_t UniStdioFile::GetPosition() const
{
	if (!IsOpen()) return 0;
	return ftell(m_fp);
}
