/**
 *  @file unicoder.cpp
 *
 *  @brief Implementation of utility unicode conversion routines
 *
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is not in older platform sdk versions
#ifndef WC_NO_BEST_FIT_CHARS
#define WC_NO_BEST_FIT_CHARS        0x00000400
#endif

namespace ucr {

// current OS version
static bool f_osvi_fetched=false;
static OSVERSIONINFO f_osvi;

/**
 * @brief fetch current OS version into file level variable & set flag
 */
static void
fetch_verinfo()
{
	memset(&f_osvi, 0, sizeof(f_osvi));
	f_osvi.dwOSVersionInfoSize = sizeof(f_osvi);
	GetVersionEx(&f_osvi);
	f_osvi_fetched = true;
}

/**
 * @brief Convert unicode codepoint to UTF-8 byte string
 *
 * utf8 must be a 7+ byte buffer
 * returns length of byte string written
 * Does not zero-terminate!
 */
int
Ucs4_to_Utf8(UINT unich, unsigned char * utf8)
{
#pragma warning(disable: 4244) // possible loss of data due to type conversion
	if (unich <= 0x7f)
	{
		utf8[0] = (unsigned char)unich;
		return 1;
	}
	else if (unich <= 0x7ff)
	{
		utf8[0] = 0xc0 + (unich >> 6);
		utf8[1] = 0x80 + (unich & 0x3f);
		return 2;
	}
	else if (unich <= 0xffff)
	{
		utf8[0] = 0xe0 + (unich >> 12);
		utf8[1] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[2] = 0x80 + (unich & 0x3f);
		return 3;
	}
	else if (unich <= 0x1fffff)
	{
		utf8[0] = 0xf0 + (unich >> 18);
		utf8[1] = 0x80 + ((unich >> 12) & 0x3f);
		utf8[2] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[3] = 0x80 + (unich & 0x3f);
		return 4;
	}
	else if (unich <= 0x3ffffff)
	{
		utf8[0] = 0xf8 + (unich >> 24);
		utf8[1] = 0x80 + ((unich >> 18) & 0x3f);
		utf8[2] = 0x80 + ((unich >> 12) & 0x3f);
		utf8[3] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[4] = 0x80 + (unich & 0x3f);
		return 5;
	}
	else if (unich <= 0x7fffffff)
	{
		utf8[0] = 0xfc + (unich >> 30);
		utf8[1] = 0x80 + ((unich >> 24) & 0x3f);
		utf8[2] = 0x80 + ((unich >> 18) & 0x3f);
		utf8[3] = 0x80 + ((unich >> 12) & 0x3f);
		utf8[4] = 0x80 + ((unich >> 6) & 0x3f);
		utf8[5] = 0x80 + (unich & 0x3f);
		return 6;
	}
	else
	{
		// Invalid Unicode codepoint (high bit was set)
		// TODO: What do we do ?
		utf8[0] = '?';
		return 1;
	}
#pragma warning(default: 4244) // possible loss of data due to type conversion
}

/**
 * @brief return byte length of UTF-8 character from its initial character (-1 if invalid)
 */
int
Utf8len_fromLeadByte(unsigned char ch)
{
	if (ch < 0x80) return 1;
	if (ch < 0xC0) return -1;
	if (ch < 0xE0) return 2;
	if (ch < 0xF0) return 3;
	if (ch < 0xF8) return 4;
	if (ch < 0xFC) return 5;
	if (ch < 0xFE) return 6;
	return -1;
}

/**
 * @brief return #bytes required to represent Unicode codepoint as UTF-8 
 */
int
Utf8len_fromCodepoint(UINT ch)
{
	if (ch <= 0x7F) return 1;
	if (ch <= 0x7FF) return 2;
	if (ch <= 0xFFFF) return 3;
	if (ch <= 0x1FFFFF) return 4;
	if (ch <= 0x3FFFFFF) return 5;
	if (ch <= 0x7FFFFFFF) return 6;
	return -1;
}

/**
 * @brief How many bytes will it take to write string as UTF-8 ? 
 *
 * @bug Fails for files larger than 2gigs
 */
UINT
Utf8len_of_string(const CString & text)
{
	UINT len=0;
	for (int i=0; i<text.GetLength(); ++i)
	{
		int chlen = Utf8len_fromCodepoint(text[i]);
		if (chlen < 1) chlen = 1;
		len += chlen;
	}
	return len;
}

/**
 * @brief Read UTF-8 character and return as Unicode
 */
UINT
GetUtf8Char(unsigned char * str)
{
	/* test short cases first, as probably much more common */
	if (!(*str & 0x80 && *str & 0x40)) {
		return str[0];
	}
	if (!(*str & 0x20)) {
		UINT ch = ((str[0] & 0x1F) << 6)
			+ (str[1] & 0x3F);
		return ch;
	}
	if (!(*str & 0x10)) {
		UINT ch = ((str[0] & 0x0f) << 12)
			+ ((str[1] & 0x3F) << 6)
			+ (str[2] & 0x3F);
		return ch;
	}
	if (!(*str & 0x08)) {
		UINT ch = ((str[0] & 0x0F) << 18)
			+ ((str[1] & 0x3F) << 12)
			+ ((str[2] & 0x3F) << 6)
			+ (str[3] & 0x3F);
		return ch;
	}
	if (!(*str & 0x04)) {
		UINT ch = ((str[0] & 0x0F) << 24)
			+ ((str[1] & 0x3F) << 18)
			+ ((str[2] & 0x3F) << 12)
			+ ((str[3] & 0x3F) << 6)
			+ (str[4] & 0x3F);
		return ch;
	} else {
		UINT ch = ((str[0] & 0x0F) << 30)
			+ ((str[1] & 0x3F) << 24)
			+ ((str[2] & 0x3F) << 18)
			+ ((str[3] & 0x3F) << 12)
			+ ((str[4] & 0x3F) << 6)
			+ (str[5] & 0x3F);
		return ch;
	}
}

/**
 * @brief Write unicode codepoint u out as UTF-8 to lpd, and advance lpd
 *
 * Returns number of bytes written (or -1 for error, in which case it writes '?')
 */
int to_utf8_advance(UINT u, unsigned char * &lpd)
{
#pragma warning(disable: 4244) // possible loss of data due to type conversion
	if (u < 0x80)
	{
		*lpd++ = u;
		return 1;
	}
	else if (u < 0x800)
	{
		*lpd++ = 0xC0 + (u >> 6);
		*lpd++ = 0x80 + (u & 0x3F);
		return 2;
	}
	else if (u < 0x10000)
	{
		*lpd++ = 0xE0 + (u >> 12);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 3;
	}
	else if (u < 0x200000)
	{
		*lpd++ = 0xF0 + (u >> 18);
		*lpd++ = 0x80 + ((u >> 12) & 0x3F);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 4;
	}
	else if (u < 0x4000000)
	{
		*lpd++ = 0xF8 + (u >> 24);
		*lpd++ = 0x80 + ((u >> 18) & 0x3F);
		*lpd++ = 0x80 + ((u >> 12) & 0x3F);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 5;
	}
	else if (u < 0x80000000)
	{
		*lpd++ = 0xF8 + (u >> 30);
		*lpd++ = 0x80 + ((u >> 24) & 0x3F);
		*lpd++ = 0x80 + ((u >> 18) & 0x3F);
		*lpd++ = 0x80 + ((u >> 12) & 0x3F);
		*lpd++ = 0x80 + ((u >> 6) & 0x3F);
		*lpd++ = 0x80 + (u & 0x3F);
		return 6;
	}
	else
	{
		*lpd++ = '?';
		return 1;
	}
#pragma warning(default: 4244) // possible loss of data due to type conversion
}

/**
 * @brief convert character passed (Unicode codepoint) to a TCHAR (set lossy flag if imperfect conversion)
 */
CString maketchar(UINT unich, bool & lossy)
{
	static bool vercheck=false;
	static UINT codepage = CP_ACP;
	if (!vercheck)
	{
		if (!f_osvi_fetched) fetch_verinfo();
		// Need 2000 or better for CP_THREAD_ACP
		if (f_osvi.dwMajorVersion>=5)
			codepage = CP_THREAD_ACP;
		vercheck = true;
	}

	return maketchar(unich, lossy, codepage);
}

/**
 * @brief convert character passed (Unicode codepoint) to a TCHAR (set lossy flag if imperfect conversion)
 */
CString maketchar(UINT unich, bool & lossy, UINT codepage)
{
#ifdef _UNICODE
	if (unich < 0x10000)
	{
		CString s;
		s = (TCHAR)unich;
		return s;
	}
	lossy = TRUE;
	return '?';
#else
	if (unich < 0x80)
	{
		CString s = (TCHAR)unich;
		return s;
	}
	wchar_t wch = (wchar_t)unich;
	if (!lossy)
	{
		static bool vercheck=false;
		static bool has_no_best_fit=false;
		if (!vercheck)
		{
			if (!f_osvi_fetched) fetch_verinfo();
			// Need 2000 (5.x) or 98 (4.10)
			has_no_best_fit = f_osvi.dwMajorVersion>=5 || (f_osvi.dwMajorVersion==4 && f_osvi.dwMinorVersion>=10);
			vercheck = true;
		}
		// So far it isn't lossy, so try for lossless conversion
		TCHAR outch;
		BOOL defaulted=FALSE;
		DWORD flags = has_no_best_fit ? WC_NO_BEST_FIT_CHARS : 0;
		if (WideCharToMultiByte(codepage, flags, &wch, 1, &outch, 1, NULL, &defaulted)
			&& !defaulted)
		{
			CString s = outch;
			return s;
		}
		lossy = TRUE;
	}
	// already lossy, so make our best shot
	DWORD flags = WC_COMPOSITECHECK+WC_DISCARDNS+WC_SEPCHARS+WC_DEFAULTCHAR;
	TCHAR outbuff[16];
	int n = WideCharToMultiByte(codepage, flags, &wch, 1, outbuff, sizeof(outbuff)-1, NULL, NULL);
	if (n>0)
	{
		outbuff[n] =0;
		return outbuff;
	}
	return _T("?");
#endif
}

/**
 * @brief convert 8-bit character input to Unicode codepoint and return it
 */
UINT
byteToUnicode (unsigned char ch)
{
	static bool vercheck=false;
	static UINT codepage = CP_ACP;
	if (!vercheck)
	{
		if (!f_osvi_fetched) fetch_verinfo();
		// Need 2000 or better for CP_THREAD_ACP
		if (f_osvi.dwMajorVersion>=5)
			codepage = CP_THREAD_ACP;
		vercheck = true;
	}
	return byteToUnicode(ch, codepage);
}

/**
 * @brief convert 8-bit character input to Unicode codepoint and return it
 */
UINT
byteToUnicode (unsigned char ch, UINT codepage)
{

	if (ch < 0x80)
		return ch;

	DWORD flags = 0;
	wchar_t wbuff;
	int n = MultiByteToWideChar(codepage, flags, (LPCSTR)&ch, 1, &wbuff, 1);
	if (n>0)
		return wbuff;
	else
		return '?';
}

/**
 * @brief Write src string (TCHAR) to destination buffer in codeset specified.
 *
 * This is designed for use writing to memory-mapped file.
 * NB: for loop is deliberately repeated inside the if statements (for speed)
 */
void
convertToBuffer(const CString & src, LPVOID dest, UNICODESET codeset)
{
	unsigned char * lpd = reinterpret_cast<unsigned char *>(dest);
	if (codeset == UCS2LE)
	{
		*lpd++ = 0xFF;
		*lpd++ = 0xFE;
	}
	else if (codeset == UCS2BE)
	{
		*lpd++ = 0xFE;
		*lpd++ = 0xFF;
	}
	else if (codeset == UTF8)
	{
		*lpd++ = 0xEF;
		*lpd++ = 0xBB;
		*lpd++ = 0xBF;
	}
#ifdef _UNICODE
	if (codeset == UCS2LE)
	{
		CopyMemory(lpd, src, src.GetLength() * 2);
		return;
	}
	else if (codeset == UCS2BE)
	{
		for (int i=0; i<src.GetLength(); ++i)
		{
			UINT u = src[i];
			*lpd++ = (unsigned char)(u >> 8);
			*lpd++ = (unsigned char)(u & 0xFF);
		}
	}
	else if (codeset == UTF8)
	{
		for (int i=0; i<src.GetLength(); ++i)
		{
			UINT u = src[i];
			ucr::to_utf8_advance(u, lpd);
		}
	}
	else if (codeset == NONE)
	{
		for (int i=0; i<src.GetLength(); ++i)
		{
			UINT u = src[i];
			if (u <= 0xFF)
				*lpd++ = (unsigned char)u;
			else
				*lpd++ = '?';
		}
	}
	else
	{
		ASSERT(0);
	}
#else
	// We really need the codeset of the 8-bit source

	if (codeset == UCS2LE)
	{
		for (int i=0; i<src.GetLength(); ++i)
		{
			UINT u = src[i];
			*lpd++ = (unsigned char)u;
			*lpd++ = 0;
		}
	}
	else if (codeset == UCS2BE)
	{
		for (int i=0; i<src.GetLength(); ++i)
		{
			UINT u = src[i];
			*lpd++ = 0;
			*lpd++ = (unsigned char)u;
		}
	}
	else if (codeset == UTF8)
	{
		for (int i=0; i<src.GetLength(); ++i)
		{
			UINT u = src[i];
			to_utf8_advance(u, lpd);
		}
	}
	else
	{
		ASSERT(0);
	}
#endif
}

/**
 * @brief Extract character from pointer, handling UCS-2 codesets (doesn't handle UTF-8)
 */
UINT
get_unicode_char(unsigned char * ptr, UNICODESET codeset)
{
	UINT ch;
	switch (codeset)
	{
	case UCS2LE:
		ch = *((WORD *)ptr);
		break;
	case UCS2BE:
		ch = (ptr[0] << 8) + ptr[1];
		break;
	default:
		ch = byteToUnicode(*ptr);
	}
	return ch;
}

} // namespace ucr
