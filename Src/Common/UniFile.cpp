/**
 *  @file   UniFile.cpp
 *  @author Perry Rapp, Creator, 2003-2006
 *  @author Kimmo Varis, 2004-2006
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-12-07 (Kimmo Varis)
 *
 *  @brief Implementation of Unicode enabled file classes (Memory-mapped reader class, and Stdio replacement class)
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

/* The MIT License
Copyright (c) 2003 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdafx.h"
#include <sys/stat.h>
#include "UniFile.h"
#include "unicoder.h"
#include "codepage.h"
#include "paths.h" // paths_GetLongPath()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
	m_filesize = 0;
	m_filepath = _T("");
	m_filename = _T("");
	m_lineno = -1;
	m_readbom = false;
	m_unicoding = ucr::NONE;
	m_charsize = 1;
	m_codepage = getDefaultCodepage();
	m_txtstats.clear();
}

/**
 * @brief Get file status into member variables
 *
 * Reads filestatus (size and full path) of a file.
 * @return true on success, false on failure.
 * @note Function sets filesize member to zero, and status as read
 * also when failing. That is needed so caller doesn't need to waste
 * time checking if file already exists (and ignores return value).
 */
bool UniLocalFile::DoGetFileStatus()
{
	struct _stati64 fstats = {0};
	m_statusFetched = -1;
	m_lastError.ClearError();

	m_filepath = paths_GetLongPath(m_filepath.c_str());

	if (_tstati64(m_filepath.c_str(), &fstats) == 0)
	{
		m_filesize = fstats.st_size;
		if (m_filesize == 0)
		{
			// if m_filesize equals zero, the file size is really zero or the file is a symbolic link.
			// use GetCompressedFileSize() to get the file size of the symbolic link target whether the file is symbolic link or not.
			// if the file is not symbolic link, GetCompressedFileSize() will return zero.
			// NOTE: GetCompressedFileSize() returns error for pre-W2K windows versions
			DWORD dwFileSizeLow, dwFileSizeHigh;
			dwFileSizeLow = GetCompressedFileSize(m_filepath.c_str(), &dwFileSizeHigh);
			if (GetLastError() == 0)
				m_filesize = ((__int64)dwFileSizeHigh << 32) + dwFileSizeLow;
		}
		m_statusFetched = 1;

		return true;
	}
	else
	{
		m_filesize = 0;
		m_statusFetched = 1; // Yep, done for this file still
		LastError(_T("_tstati64"), 0);
		return false;
	}
}

/** @brief Record an API call failure */
void UniLocalFile::LastError(LPCTSTR apiname, int syserrnum)
{
	m_lastError.ClearError();

	m_lastError.apiname = apiname;
	m_lastError.syserrnum = syserrnum;
}

/** @brief Record a custom error */
void UniLocalFile::LastErrorCustom(LPCTSTR desc)
{
	m_lastError.ClearError();

	m_lastError.desc = desc;
}

/////////////
// UniMemFile
/////////////

UniMemFile::UniMemFile()
: m_handle(INVALID_HANDLE_VALUE)
, m_hMapping(INVALID_HANDLE_VALUE)
, m_base(NULL)
, m_data(NULL)
, m_current(NULL)
{
}

void UniMemFile::Close()
{
	Clear();
	if (m_base)
	{
		UnmapViewOfFile(m_base);
		m_base = 0;
	}
	m_data = NULL;
	m_current = NULL;
	if (m_hMapping != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hMapping);
		m_hMapping = INVALID_HANDLE_VALUE;
	}
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_handle);
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

/** @brief Is it currently attached to a file ? */
bool UniMemFile::IsOpen() const
{
	// We don't test the handle here, because we allow "opening" empty file
	// but memory-mapping doesn't work on that, so that uses a special state
	// of no handle, but linenumber of 0
	return m_lineno >= 0;
}

/** @brief Get file status into member variables */
bool UniMemFile::GetFileStatus()
{
	if (!IsOpen()) return false;
	return DoGetFileStatus();
}

/** @brief Open file for generic read-only access */
bool UniMemFile::OpenReadOnly(LPCTSTR filename)
{
	DWORD dwOpenAccess = GENERIC_READ;
	DWORD dwOpenShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwOpenCreationDispostion = OPEN_EXISTING;
	DWORD dwMappingProtect = PAGE_READONLY;
	DWORD dwMapViewAccess = FILE_MAP_READ;
	return Open(filename, dwOpenAccess, dwOpenShareMode, dwOpenCreationDispostion, dwMappingProtect, dwMapViewAccess);
}

/** @brief Open file for generic read-write access */
bool UniMemFile::Open(LPCTSTR filename)
{
	DWORD dwOpenAccess = GENERIC_WRITE;
	DWORD dwOpenShareMode = 0;
	DWORD dwOpenCreationDispostion = OPEN_EXISTING;
	DWORD dwMappingProtect = PAGE_READWRITE;
	DWORD dwMapViewAccess = FILE_MAP_WRITE;
	return Open(filename, dwOpenAccess, dwOpenShareMode, dwOpenCreationDispostion, dwMappingProtect, dwMapViewAccess);
}

/** @brief Open file with specified arguments */
bool UniMemFile::Open(LPCTSTR filename, DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess)
{
	// We use an internal workhorse to make it easy to close on any error
	if (!DoOpen(filename, dwOpenAccess, dwOpenShareMode, dwOpenCreationDispostion, dwMappingProtect, dwMapViewAccess))
	{
		Close();
		return false;
	}
	return true;
}

/** @brief Internal implementation of Open */
bool UniMemFile::DoOpen(LPCTSTR filename, DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess)
{
	Close();

	m_filename = filename;
	m_filepath = m_filename; // TODO: Make canonical ?

	m_handle = CreateFile(m_filename.c_str(), dwOpenAccess, dwOpenShareMode, NULL, dwOpenCreationDispostion, 0, 0);
	if (m_handle == INVALID_HANDLE_VALUE)
	{
		LastError(_T("CreateFile"), GetLastError());
		return false;
	}
	m_lineno = 0; // GetFileStatus requires file be "open", which means nonnegative line number
	if (!GetFileStatus())
		return false;
	m_lineno = -1;

	DWORD sizehi = (DWORD)(m_filesize >> 32);
	DWORD sizelo = (DWORD)(m_filesize & 0xFFFFFFFF);

	if (sizehi || sizelo > 0x7FFFFFFF)
	{
		LastErrorCustom(_T("UniMemFile cannot handle files over 2 gigabytes"));
		return false;
	}

	if (sizelo == 0)
	{
		// Allow opening empty file, but memory mapping doesn't work on such
		// m_base and m_current are 0 from the Close call above
		// so ReadString will correctly return empty EOF immediately
		m_lineno = 0;
		return true;
	}

	LPSECURITY_ATTRIBUTES lpAttributes = NULL; // default security
	LPCTSTR lpName = NULL; // nameless mapping
	m_hMapping = CreateFileMapping(m_handle, lpAttributes, dwMappingProtect, sizehi, sizelo, lpName);
	if (!m_hMapping)
	{
		LastError(_T("CreateFileMapping"), GetLastError());
		return false;
	}


	DWORD dwFileOffsetHigh = 0;
	DWORD dwFileOffsetLow = 0;
	SIZE_T dwNumberOfBytesToMap = sizelo;
	m_base = (LPBYTE)MapViewOfFile(m_hMapping, dwMapViewAccess, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
	if (!m_base)
	{
		LastError(_T("MapViewOfFile"), GetLastError());
		return false;
	}
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
	if (!IsOpen()) return false;

	unsigned char * lpByte = m_base;
	m_current = m_data = m_base;
	m_charsize = 1;
	if (m_filesize >= 2)
	{
		if (lpByte[0] == 0xFF && lpByte[1] == 0xFE)
		{
			m_unicoding = ucr::UCS2LE;
			m_charsize = 2;
			m_data = lpByte+2;
		}
		else if (lpByte[0] == 0xFE && lpByte[1] == 0xFF)
		{
			m_unicoding = ucr::UCS2BE;
			m_charsize = 2;
			m_data = lpByte+2;
		}
	}
	if (m_filesize >=3)
	{
		if (lpByte[0] == 0xEF && lpByte[1] == 0xBB && lpByte[2] == 0xBF)
		{
			m_unicoding = ucr::UTF8;
			m_data = lpByte+3;
		}
	}
	m_readbom = true;
	m_current = m_data;
	return (m_data != m_base);
}

/**
 * @brief Read one (DOS or UNIX or Mac) line. Do not include eol chars.
 * @param [out] line Line read.
 * @param [out] lossy TRUE if there were lossy encoding.
 * @return TRUE if there is more lines to read, TRUE when last line is read.
 */
BOOL UniMemFile::ReadString(CString & line, bool * lossy)
{
	CString eol;
	BOOL ok = ReadString(line, eol, lossy);
	return ok;
}

/**
 * @brief Append characters to string and exponentially grow buffer as needed
 */
static int Append(CString &strBuffer, int cchHead, LPCTSTR pchTail, int cchTail, int cchBufferMin = 1024)
{
	int cchBuffer = strBuffer.GetLength();
	int cchLength = cchHead + cchTail;
	while (cchBuffer < cchLength)
	{
		cchBuffer += cchBuffer;
		if (cchBuffer < cchBufferMin)
			cchBuffer = cchBufferMin;
	}
	CopyMemory(strBuffer.GetBufferSetLength(cchBuffer) + cchHead, pchTail, cchTail * sizeof(TCHAR));
	return cchLength;
}

/**
 * @brief Record occurrence of binary zero to stats
 */
static void RecordZero(UniFile::txtstats & txstats, int offset)
{
	++txstats.nzeros;
	if (txstats.first_zero == -1)
		txstats.first_zero = offset;
	txstats.last_zero = offset;
}

/**
 * @brief Read one (DOS or UNIX or Mac) line.
 * @param [out] line Line read.
 * @param [out] eol EOL bytes read (if any).
 * @param [out] lossy TRUE if there were lossy encoding.
 * @return TRUE if there is more lines to read, TRUE when last line is read.
 */
BOOL UniMemFile::ReadString(CString & line, CString & eol, bool * lossy)
{
	line = _T("");
	eol = _T("");
	int cchLine = 0;
	LPCTSTR pchLine = (LPCTSTR)m_current;
	
	// shortcut methods in case file is in the same encoding as our CStrings
	
#ifdef _UNICODE
	if (m_unicoding == ucr::UCS2LE)
	{
		// If there aren't any wchars left in the file, return FALSE to indicate EOF
		if (m_current - m_base + 1 >= m_filesize)
			return FALSE;
		// Loop through wchars, watching for eol chars or zero
		while (m_current - m_base + 1 < m_filesize)
		{
			wchar_t wch = *(wchar_t *)m_current;
			int wch_offset = (m_current - m_base);
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
				CopyMemory(line.GetBufferSetLength(cchLine), pchLine, cchLine * sizeof(TCHAR));
				return TRUE;
			}
			if (!wch)
			{
				RecordZero(m_txtstats, wch_offset);
				CopyMemory(line.GetBufferSetLength(cchLine), pchLine, cchLine * sizeof(TCHAR));
				return TRUE;
			}
			++cchLine;
		}
		CopyMemory(line.GetBufferSetLength(cchLine), pchLine, cchLine * sizeof(TCHAR));
		return TRUE;
	}
#else
	if (m_unicoding == ucr::NONE && EqualCodepages(m_codepage, getDefaultCodepage()))
	{
		// If there aren't any bytes left in the file, return FALSE to indicate EOF
		if (m_current - m_base >= m_filesize)
			return FALSE;
		// Loop through chars, watching for eol chars or zero
		while (m_current - m_base < m_filesize)
		{
			char ch = *m_current;
			int ch_offset = (m_current - m_base);
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
				CopyMemory(line.GetBufferSetLength(cchLine), pchLine, cchLine * sizeof(TCHAR));
				return TRUE;
			}
			if (!ch)
			{
				RecordZero(m_txtstats, ch_offset);
				CopyMemory(line.GetBufferSetLength(cchLine), pchLine, cchLine * sizeof(TCHAR));
				return TRUE;
			}
			++cchLine;
		}
		CopyMemory(line.GetBufferSetLength(cchLine), pchLine, cchLine * sizeof(TCHAR));
		return TRUE;
	}
#endif

	if (m_current - m_base + (m_charsize-1) >= m_filesize)
		return FALSE;

	// Handle 8-bit strings in line chunks because of multibyte codings (eg, 936)
	if (m_unicoding == ucr::NONE)
	{
		bool eof=true;
		LPBYTE eolptr=0;
		for (eolptr = m_current; (eolptr - m_base + (m_charsize-1) < m_filesize); ++eolptr)
		{
			if (*eolptr == '\n' || *eolptr == '\r')
			{
				eof=false;
				break;
			}
			if (*eolptr == 0)
			{
				int offset = (eolptr - m_base);
				RecordZero(m_txtstats, offset);
			}
		}
		line = ucr::maketstring((LPCSTR)m_current, eolptr-m_current, m_codepage, lossy);
		if (lossy && *lossy)
			++m_txtstats.nlosses;
		if (!eof)
		{
			eol += (TCHAR)*eolptr;
			++m_lineno;
			if (*eolptr == '\r')
			{
				if (eolptr - m_base + (m_charsize-1) < m_filesize && eolptr[1] == '\n')
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
		m_current = eolptr + eol.GetLength();
		// TODO: What do we do if save was lossy ?
		return !eof;
	}

	while (m_current - m_base + (m_charsize-1) < m_filesize)
	{
		UINT ch=0;
		UINT utf8len=0;
		bool doneline=false;

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
				utf8len=1;
			}
			else
			{
				ch = ucr::GetUtf8Char(m_current);
			}
		}
		else
		{
			ch = ucr::get_unicode_char(m_current, (ucr::UNICODESET)m_unicoding, m_codepage);
			if (!ch)
				doneline = true;
		}
		// convert from Unicode codepoint to TCHAR string
		// could be multicharacter if decomposition took place, for example
		bool lossy = false; // try to avoid lossy conversion
		CString sch = ucr::maketchar(ch, lossy);
		if (lossy)
			++m_txtstats.nlosses;
		if (sch.GetLength() >= 1)
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
				// For UTF-8, this ch will be wrong if character is non-ASCII
				// but we only check it against \n here, so it doesn't matter
				UINT ch = ucr::get_unicode_char(m_current+m_charsize, (ucr::UNICODESET)m_unicoding);
				if (ch == '\n')
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
			doneline = true;
			int offset = (m_current - m_base);
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
			if (!eol.IsEmpty())
				++m_lineno;
			line.ReleaseBuffer(cchLine);
			return TRUE;
		}
		cchLine = Append(line, cchLine, sch, sch.GetLength());
	}
	line.ReleaseBuffer(cchLine);
	return TRUE;
}

/**
 * @brief Write one line (doing any needed conversions)
 */
BOOL UniMemFile::WriteString(const CString & line)
{
	ASSERT(0); // unimplemented -- currently cannot write to a UniMemFile!
	return FALSE;
}

/////////////
// UniStdioFile
/////////////

UniStdioFile::UniStdioFile()
: m_fp(0)
, m_data(0)
{
	m_pucrbuff = new ucr::buffer(128);
}

UniStdioFile::~UniStdioFile()
{
	Close();
	delete (ucr::buffer *)m_pucrbuff;
	m_pucrbuff = 0;
}

void UniStdioFile::Close()
{
	if (IsOpen())
	{
		fclose(m_fp);
		m_fp = 0;
	}
	m_statusFetched = 0;
	m_filesize = 0;
	// preserve m_filepath
	// preserve m_filename
	m_data = 0;
	m_lineno = -1;
	m_readbom = false;
	// preserve m_unicoding
	// preserve m_charsize
	// preserve m_codepage
	m_txtstats.clear();
}

/** @brief Is it currently attached to a file ? */
bool UniStdioFile::IsOpen() const
{
	return m_fp != 0;
}

/** @brief Get file status into member variables */
bool UniStdioFile::GetFileStatus()
{
	if (IsOpen()) return false; // unfortunately we'll hit our lock

	return DoGetFileStatus();
}

bool UniStdioFile::OpenReadOnly(LPCTSTR filename)
{
	return Open(filename, _T("rb"));
}
bool UniStdioFile::OpenCreate(LPCTSTR filename)
{
	return Open(filename, _T("w+b"));
}
bool UniStdioFile::OpenCreateUtf8(LPCTSTR filename)
{
	if (!OpenCreate(filename)) 
		return false;
	SetUnicoding(ucr::UTF8);
	return true;
	
}
bool UniStdioFile::Open(LPCTSTR filename, LPCTSTR mode)
{
	if (!DoOpen(filename, mode))
	{
		Close();
		return false;
	}
	return true;
}

bool UniStdioFile::DoOpen(LPCTSTR filename, LPCTSTR mode)
{
	Close();
	
	m_filepath = filename;
	m_filename = filename; // TODO: Make canonical ?

	// Fails if file doesn't exist (when we are creating new file)
	// But we don't care since size is set to 0 anyway.
	GetFileStatus();

	m_fp = _tfopen(m_filepath.c_str(), mode);
	if (!m_fp)
		return false;

	DWORD sizehi = (DWORD)(m_filesize >> 32);

	if (sizehi)
	{
		// TODO: We could do this in MSC_VER 7+ I think

		LastErrorCustom(_T("UniStdioFile cannot handle files over 4 gigabytes"));
		return false;
	}

	m_lineno = 0;
	return true;
}

/** @brief Record an API call failure */
void UniStdioFile::LastError(LPCTSTR apiname, int syserrnum)
{
	m_lastError.ClearError();

	m_lastError.apiname = apiname;
	m_lastError.syserrnum = syserrnum;
}

/** @brief Record a custom error */
void UniStdioFile::LastErrorCustom(LPCTSTR desc)
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
	if (!IsOpen()) return false;

	fseek(m_fp, 0, SEEK_SET);

	// Need three bytes for BOM
	unsigned char buff[4];
	int bytes = fread(buff, 1, 3, m_fp);
	unsigned char * lpByte = (unsigned char *)buff;

	m_data = 0;
	m_charsize = 1;
	if (bytes >= 2 && lpByte[0] == 0xFF && lpByte[1] == 0xFE)
	{
		m_unicoding = ucr::UCS2LE;
		m_charsize = 2;
		m_data = 2;
	}
	else if (bytes >= 2 && lpByte[0] == 0xFE && lpByte[1] == 0xFF)
	{
		m_unicoding = ucr::UCS2BE;
		m_charsize = 2;
		m_data = 2;
	}
	else if (bytes >= 3 && lpByte[0] == 0xEF && lpByte[1] == 0xBB && lpByte[2] == 0xBF)
	{
		m_unicoding = ucr::UTF8;
		m_data = 3;
	}
	fseek(m_fp, (long)m_data, SEEK_SET);
	m_readbom = true;
	return (m_data != 0);
}

BOOL UniStdioFile::ReadString(CString & line, bool * lossy)
{
	ASSERT(0); // unimplemented -- currently cannot read from a UniStdioFile!
	return FALSE;
}

BOOL UniStdioFile::ReadString(CString & line, CString & eol, bool * lossy)
{
	ASSERT(0); // unimplemented -- currently cannot read from a UniStdioFile!
	return FALSE;
}
BOOL UniStdioFile::ReadString(sbuffer & sline, bool * lossy)
{
	ASSERT(0); // unimplemented -- currently cannot read from a UniStdioFile!
	return FALSE;
}
BOOL UniStdioFile::ReadString(sbuffer & sline, CString & eol, bool * lossy)
{
	ASSERT(0); // unimplemented -- currently cannot read from a UniStdioFile!
	return FALSE;
}

/** @brief Write BOM (byte order mark) if Unicode file */
int UniStdioFile::WriteBom()
{
	if (m_unicoding == ucr::UCS2LE)
	{
		unsigned char bom[] = "\xFF\xFE";
		fseek(m_fp, 0, SEEK_SET);
		fwrite(bom, 1, 2, m_fp);
		m_data = 2;
	}
	else if (m_unicoding == ucr::UCS2BE)
	{
		unsigned char bom[] = "\xFE\xFF";
		fseek(m_fp, 0, SEEK_SET);
		fwrite(bom, 1, 2, m_fp);
		m_data = 2;
	}
	else if (m_unicoding == ucr::UTF8)
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
BOOL UniStdioFile::WriteString(const CString & line)
{
	// shortcut the easy cases
#ifdef _UNICODE
	if (m_unicoding == ucr::UCS2LE)
#else
	if (m_unicoding == ucr::NONE && EqualCodepages(m_codepage, getDefaultCodepage()))
#endif
	{
		unsigned int bytes = line.GetLength() * sizeof(TCHAR);
		unsigned int wbytes = fwrite((LPCTSTR)line, 1, bytes, m_fp);
		if (wbytes != bytes)
			return FALSE;
		return TRUE;
	}

	ucr::buffer * buff = (ucr::buffer *)m_pucrbuff;
	ucr::UNICODESET unicoding1=ucr::NONE;
	int codepage1=0;
	ucr::getInternalEncoding(&unicoding1, &codepage1); // What CString & TCHARs represent
	const unsigned char * src = (const UCHAR *)(LPCTSTR)line;
	int srcbytes = line.GetLength() * sizeof(TCHAR);
	bool lossy = ucr::convert(unicoding1, codepage1, src, srcbytes, (ucr::UNICODESET)m_unicoding, m_codepage, buff);
	// TODO: What to do about lossy conversion ?
	unsigned int wbytes = fwrite(buff->ptr, 1, buff->used, m_fp);
	if (wbytes != buff->used)
		return FALSE;
	return TRUE;
}

__int64 UniStdioFile::GetPosition() const
{
	if (!IsOpen()) return 0;
	return ftell(m_fp);
}
