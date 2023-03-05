/**
 *  @file   unicoder.cpp
 *  @author Perry Rapp, Creator, 2003-2006
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-02-20 (Perry Rapp)
 *
 *  @brief  Implementation of utility unicode conversion routines
 */

/* The MIT License
Copyright (c) 2003 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pch.h"
#include "unicoder.h"
#include <windows.h>
#include <cassert>
#include <memory>
#include <Poco/UnicodeConverter.h>
#include "UnicodeString.h"
#include "ExConverter.h"

using Poco::UnicodeConverter;

namespace ucr
{

// store the default codepage as specified by user in options
static int f_nDefaultCodepage = GetACP();


# pragma warning(push)          // Saves the current warning state.
# pragma warning(disable:4244)  // Temporarily disables warning 4244: "conversion from 'int' to 'char', possible loss of data"
/**
 * @brief Convert unicode codepoint to UTF-8 byte string
 *
 * utf8 must be a 7+ byte buffer
 * returns length of byte string written
 * Does not zero-terminate!
 */
int Ucs4_to_Utf8(unsigned unich, unsigned char * utf8)
{
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
}
# pragma warning(pop)           // Restores the warning state.

/**
 * @brief Gets a length of UTF-8 character in bytes.
 * @param [in] ch The character for which to get the length.
 * @return Byte length of UTF-8 character, -1 if invalid.
 */
int Utf8len_fromLeadByte(unsigned char ch)
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
int Utf8len_fromCodepoint(unsigned ch)
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
 * @brief How many chars in this UTF-8 string ?
 *
 * @param size size argument as filemapping are not 0 terminated
 *
 * @bug Fails for files larger than 2gigs
 */
size_t stringlen_of_utf8(const char* text, size_t size)
{
	size_t len = 0;
	for (size_t i = 0; i < size;)
	{
		int chlen = Utf8len_fromLeadByte(text[i]);
		if (chlen < 1) chlen = 1;
		i += chlen;
		len ++;
	}
	return len;
}

/**
 * @brief Read UTF-8 character and return as Unicode
 */
unsigned GetUtf8Char(unsigned char * str)
{
	/* test short cases first, as probably much more common */
	if (!(*str & 0x80 && *str & 0x40))
	{
		return str[0];
	}
	if (!(*str & 0x20))
	{
		unsigned ch = ((str[0] & 0x1F) << 6)
				+ (str[1] & 0x3F);
		return ch;
	}
	if (!(*str & 0x10))
	{
		unsigned ch = ((str[0] & 0x0f) << 12)
				+ ((str[1] & 0x3F) << 6)
				+ (str[2] & 0x3F);
		return ch;
	}
	if (!(*str & 0x08))
	{
		unsigned ch = ((str[0] & 0x0F) << 18)
				+ ((str[1] & 0x3F) << 12)
				+ ((str[2] & 0x3F) << 6)
				+ (str[3] & 0x3F);
		return ch;
	}
	if (!(*str & 0x04))
	{
		unsigned ch = ((str[0] & 0x0F) << 24)
				+ ((str[1] & 0x3F) << 18)
				+ ((str[2] & 0x3F) << 12)
				+ ((str[3] & 0x3F) << 6)
				+ (str[4] & 0x3F);
		return ch;
	}
	else
	{
		unsigned ch = ((str[0] & 0x0F) << 30)
				+ ((str[1] & 0x3F) << 24)
				+ ((str[2] & 0x3F) << 18)
				+ ((str[3] & 0x3F) << 12)
				+ ((str[4] & 0x3F) << 6)
				+ (str[5] & 0x3F);
		return ch;
	}
}


# pragma warning(push)          // Saves the current warning state.
# pragma warning(disable:4244)  // Temporarily disables warning 4244: "conversion from 'int' to 'char', possible loss of data"
/**
 * @brief Write unicode codepoint u out as UTF-8 to lpd, and advance lpd
 *
 * Returns number of bytes written (or -1 for error, in which case it writes '?')
 */
int to_utf8_advance(unsigned u, unsigned char * &lpd)
{
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
		*lpd++ = 0xFC + (u >> 30);
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
}
# pragma warning(pop)           // Restores the warning state.

/**
 * @brief convert character passed (Unicode codepoint) to a tchar_t (set lossy flag if imperfect conversion)
 */
void maketchar(String & ch, unsigned unich, bool & lossy)
{
	// NB: Windows always draws in CP_ACP, not CP_THREAD_ACP, so we must use CP_ACP as an internal codepage

	maketchar(ch, unich, lossy, CP_ACP);
}

/**
 * @brief convert character passed (Unicode codepoint) to a tchar_t (set lossy flag if imperfect conversion)
 */
void maketchar(String & ch, unsigned unich, bool & lossy, unsigned codepage)
{
#ifdef _UNICODE
	if (unich < 0x10000)
	{
		ch = static_cast<tchar_t>(unich);
		return;
	}
	else if (unich < 0x110000)
	{
		ch = static_cast<tchar_t>(((unich - 0x10000)/0x400 + 0xd800));
		ch += static_cast<tchar_t>(((unich % 0x400) + 0xdc00));
		return;
	}
	lossy = true;
	ch = '?';
	return;
#else
	if (unich < 0x80)
	{
		ch = (tchar_t)unich;
		return;
	}
	wchar_t wch = (wchar_t)unich;
	if (!lossy)
	{
		// So far it isn't lossy, so try for lossless conversion
		char outch[3] = {0};
		BOOL defaulted = FALSE;
		DWORD flags = WC_NO_BEST_FIT_CHARS;
		if (WideCharToMultiByte(codepage, flags, &wch, 1, outch, sizeof(outch), nullptr, &defaulted)
				&& !defaulted)
		{
			ch = outch;
			return;
		}
		lossy = true;
	}
	// already lossy, so make our best shot
	DWORD flags = WC_COMPOSITECHECK + WC_DISCARDNS + WC_SEPCHARS + WC_DEFAULtchar_t;
	tchar_t outbuff[16];
	int n = WideCharToMultiByte(codepage, flags, &wch, 1, outbuff, sizeof(outbuff) - 1, nullptr, nullptr);
	if (n > 0)
	{
		outbuff[n] = 0;
		ch = outbuff;
		return;
	}
	ch = _T("?");
#endif
}

/**
 * @brief convert 8-bit character input to Unicode codepoint and return it
 */
unsigned byteToUnicode(unsigned char ch)
{
	// NB: Windows always draws in CP_ACP, not CP_THREAD_ACP, so we must use CP_ACP as an internal codepage
	return byteToUnicode(ch, CP_ACP);
}

/**
 * @brief convert 8-bit character input to Unicode codepoint and return it
 */
unsigned byteToUnicode(unsigned char ch, unsigned codepage)
{

	if (ch < 0x80)
		return ch;

	DWORD flags = 0;
	wchar_t wbuff;
	int n = MultiByteToWideChar(codepage, flags, (const char*) & ch, 1, &wbuff, 1);
	if (n > 0)
		return wbuff;
	else
		return '?';
}

/**
 * @brief Return encoding used for tchar_t & String
 */
void getInternalEncoding(UNICODESET * unicoding, int * codepage)
{
#ifdef _UNICODE
	*unicoding = UCS2LE;
	*codepage = CP_UCS2LE;
#else
	// NB: Windows always draws in CP_ACP, not CP_THREAD_ACP, so we must use CP_ACP as an internal codepage
	*unicoding = NONE;
	*codepage = CP_ACP;
#endif
}

/**
 * @brief Write appropriate BOM (Unicode byte order marker)
 * returns #bytes written
 */
int writeBom(void* dest, UNICODESET unicoding)
{
	unsigned char * lpd = reinterpret_cast<unsigned char *>(dest);
	// write Unicode byte order marker (BOM)
	if (unicoding == UCS2LE)
	{
		*lpd++ = 0xFF;
		*lpd++ = 0xFE;
		return 2;
	}
	else if (unicoding == UCS2BE)
	{
		*lpd++ = 0xFE;
		*lpd++ = 0xFF;
		return 2;
	}
	else if (unicoding == UTF8)
	{
		*lpd++ = 0xEF;
		*lpd++ = 0xBB;
		*lpd++ = 0xBF;
		return 3;
	}
	return 0;
}

int getBomSize(UNICODESET unicoding)
{
	if (unicoding == UCS2LE)
		return 2;
	else if (unicoding == UCS2BE)
		return 2;
	else if (unicoding == UTF8)
		return 3;
	return 0;
}

/**
 * @brief Extract character from pointer, handling UCS-2 codesets
 *  This does not handle MBCS or UTF-8 codepages correctly!
 *  Client should not use this except for Unicode or SBCS codepages.
 */
unsigned get_unicode_char(unsigned char * ptr, UNICODESET codeset, int codepage)
{
	unsigned ch;
	switch (codeset)
	{
	case UCS2LE:
		ch = *((WORD *)ptr);
		break;
	case UCS2BE:
		ch = (ptr[0] << 8) + ptr[1];
		break;
	default:
		// TODO: How do we recognize valid codepage ?
		// if not, use byteToUnicode(*ptr)
		ch = byteToUnicode(*ptr, codepage);
	}
	return ch;
}

/**
 * @brief Convert series of bytes (8-bit chars) to tchar_ts.
 *
 * @param [out] str String returned.
 * @param [in] lpd Original byte array to convert.
 * @param [in] len Length of the original byte array.
 * @param [in] codepage Codepage used.
 * @param [out] lossy Was conversion lossy?
 * @return true if conversion succeeds, false otherwise.
 * @todo This doesn't inform the caller whether translation was lossy
 *  In fact, this doesn't even know. Probably going to have to make
 *  two passes, the first with MB_ERR_INVALID_CHARS. Ugh. :(
 */
bool maketstring(String & str, const char* lpd, size_t len, int codepage, bool * lossy)
{
	if (!len)
	{
		str.clear();
		return true;
	}

	int defcodepage = getDefaultCodepage();
	
	// 0 is a valid value (CP_ACP)!
	if (codepage == -1)
		codepage = defcodepage;

#ifdef UNICODE
	// Convert input to Unicode, using specified codepage
	// tchar_t is wchar_t, so convert into String (str)
	DWORD flags = MB_ERR_INVALID_CHARS;
	size_t wlen = len * 2 + 6;
	assert(wlen < INT_MAX);

	try
	{
		str.resize(wlen);
	}
	catch (std::bad_alloc&)
	{
		// Not enough memory - exit
		return false;
	}

	LPWSTR wbuff = &*str.begin();
	if (codepage == CP_ACP || IsValidCodePage(codepage))
	{
		int n = MultiByteToWideChar(codepage, flags, lpd, static_cast<int>(len), wbuff, static_cast<int>(wlen - 1));
		if (n)
		{
			/*
			NB: MultiByteToWideChar is documented as only zero-terminating
			if input was zero-terminated, but it appears that it can
			zero-terminate even if input wasn't.
			So we check if it zero-terminated and adjust count accordingly.
			*/
			//>2007-01-11 jtuc: We must preserve an embedded zero even if it is
			// the last input character. As we don't expect MultiByteToWideChar to
			// add a zero that does not originate from the input string, it is a
			// good idea to ASSERT that the assumption holds.
			if (wbuff[n-1] == 0 && lpd[len-1] != 0)
			{
				//assert(false);
				*lossy = true;
				--n;
			}
			try
			{
				str.resize(n);
			}
			catch (std::bad_alloc&)
			{
				// Not enough memory - exit
				return false;
			}
			return true;
		}
		else
		{
			if (GetLastError() == ERROR_INVALID_FLAGS)
			{
				n = MultiByteToWideChar(codepage, 0, lpd, static_cast<int>(len), wbuff, static_cast<int>(wlen-1));
				if (n)
				{
					/* NB: MultiByteToWideChar is documented as only zero-terminating 
					if input was zero-terminated, but it appears that it can 
					zero-terminate even if input wasn't.
					So we check if it zero-terminated and adjust count accordingly.
					*/
					if (wbuff[n-1] == 0 && lpd[len-1] != 0)
					{
						//assert(false);
						*lossy = true;
						--n;
					}
					try
					{
						str.resize(n);
					}
					catch (std::bad_alloc&)
					{
						// Not enough memory - exit
						return false;
					}
					return true;
				}
			}
			if (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)
			{
				*lossy = true;
				flags = 0;
				// wlen & wbuff are still fine
				n = MultiByteToWideChar(codepage, flags, lpd, static_cast<int>(len), wbuff, static_cast<int>(wlen-1));
				if (n)
				{
					try
					{
						str.resize(n);
					}
					catch (std::bad_alloc&)
					{
						// Not enough memory - exit
						return false;
					}
					return true;
				}
			}
			str = _T("?");
			return true;
		}
	}
	else
	{
		IExconverter *pexconv = Exconverter::getInstance();
		if (pexconv != nullptr)
		{
			size_t n = wlen;
			if (pexconv->convertToUnicode(codepage, lpd, &len, wbuff, &n))
			{
				try
				{
					str.resize(n);
				}
				catch (std::bad_alloc&)
				{
					// Not enough memory - exit
					return false;
				}
				return true;
			}
			else
			{
				*lossy = true;
				str = _T("?");
			}
			return true;
		}
		else
		{
			*lossy = true;
			str = _T("?");
		}
		return true;
	}

#else
	int dstcodepage = IsValidCodePage(defcodepage) ? defcodepage : GetACP();

	if (EqualCodepages(codepage, dstcodepage))
	{
		// trivial case, they want the bytes in the file interpreted in our current codepage
		// Only caveat is that input (lpd) is not zero-terminated
		str = String(lpd, len);
		return true;
	}

	if (codepage == CP_ACP || IsValidCodePage(codepage))
	{
		str = CrossConvertToStringA(lpd, len, codepage, dstcodepage, lossy);
		if (*lossy)
			str = _T("?");
		return true;
	}
	else
	{
		IExconverter *pexconv = Exconverter::getInstance();
		if (pexconv != nullptr)
		{		
			size_t n = len * 6 + 6;
			try
			{
				str.resize(n);
			}
			catch (std::bad_alloc&)
			{
				// Not enough memory - exit
				return false;
			}
			char *buff = &*str.begin();
			pexconv->convert(codepage, dstcodepage, (const unsigned char *)lpd, &len, (unsigned char *)buff, &n);
			if (n)
			{
				try
				{
					str.resize(n);
				}
				catch (std::bad_alloc&)
				{
					// Not enough memory - exit
					return false;
				}
			}
			else
				str = _T("?");
		}
		else
			str = _T("?");		
		return true;
	}
#endif
}

/**
 * @brief (ANSI build only) Convert from one 8 bit codepage to another
 */
#ifndef UNICODE
String CrossConvertToStringA(const char* src, unsigned srclen, int cpin, int cpout, bool * lossy)
{
	int wlen = srclen * 2 + 6;
	int clen = wlen * 2 + 6;
	String str;
	str.resize(clen);
	char* cbuff = &*str.begin();
	int nbytes = CrossConvert(src, srclen, cbuff, clen, cpin, cpout, lossy);
	str.resize(nbytes);
	return str;
}
#endif

/**
 * @brief Convert from one 8-bit codepage to another
 *
 * destsize must be at least 2
 */
int CrossConvert(const char* src, unsigned srclen, char* dest, unsigned destsize, int cpin, int cpout, bool * lossy)
{
	assert(destsize > 1);

	// Convert input to Unicode, using specified codepage
	DWORD flags = 0;
	int wlen = srclen * 2 + 6;
	auto wbuff = std::make_unique<wchar_t[]>(wlen);
	int n;
	if (cpin == CP_UCS2LE)
	{
		if (srclen == -1)
			srclen = static_cast<unsigned>(wcslen((wchar_t *)src) * sizeof(wchar_t));
		memcpy(wbuff.get(), src, srclen);
		n = srclen / sizeof(wchar_t);
	}
	else if (cpin == CP_UCS2BE)
	{
		if (srclen == -1)
			srclen = static_cast<unsigned>(wcslen((wchar_t *)src) * sizeof(wchar_t));
		_swab((char *)src, (char *)wbuff.get(), srclen);
		n = srclen / sizeof(wchar_t);
	}
	else
	{
		n = MultiByteToWideChar(cpin, flags, (const char*)src, srclen, wbuff.get(), wlen - 1);
		if (!n)
		{
			int nsyserr = ::GetLastError();
			dest[0] = '?';
			return 1;
		}
	}
	/*
	NB: MultiByteToWideChar is documented as only zero-terminating
	if input was zero-terminated, but it appears that it can
	zero-terminate even if input wasn't.
	So we check if it zero-terminated and adjust count accordingly.
	*/
	if (wbuff[n-1] == 0)
		--n;
	wbuff[n] = 0; // zero-terminate string

	// Now convert to tchar_t (which means defcodepage)
	flags = WC_NO_BEST_FIT_CHARS; // TODO: Think about this
	BOOL defaulted = FALSE;
	BOOL * pdefaulted = &defaulted;
	if (cpout == CP_UTF8)
	{
		flags = 0;
		pdefaulted = nullptr;
	}
	if (cpout == CP_UCS2LE)
	{
		memcpy(dest, wbuff.get(), n * sizeof(wchar_t));
		n = n * sizeof(wchar_t);
		dest[n] = 0;
		dest[n + 1] = 0;
	}
	else if (cpout == CP_UCS2BE)
	{
		_swab((char *)wbuff.get(), dest, n * sizeof(wchar_t));
		n = n * sizeof(wchar_t);
		dest[n] = 0;
		dest[n + 1] = 0;
	}
	else
	{
		n = WideCharToMultiByte(cpout, flags, wbuff.get(), n, dest, destsize - 1, nullptr, pdefaulted);
		if (!n)
		{
			int nsyserr = ::GetLastError();
		}
		dest[n] = 0;
	}
	if (lossy)
		*lossy = !!defaulted;
	return n;
}

/**
 * @brief Buffer constructor.
 * The constructor creates buffer with given size.
 * @param [in] initialSize Buffer's size.
 */
buffer::buffer(size_t initialSize)
{
	size = 0;
	capacity = initialSize;
	ptr = (unsigned char *)calloc(capacity, 1);
}

/**
 * @brief Buffer destructor.
 * Frees the reserved buffer.
 */
buffer::~buffer()
{
	free(ptr);
}

/**
 * @brief Resize the buffer.
 * @param [in] newSize New size of the buffer.
 */
void buffer::resize(size_t newSize)
{
	if (capacity < newSize)
	{
		unsigned char *tmp = static_cast<unsigned char *>(realloc(ptr, newSize));
		if (tmp == nullptr)
			throw std::bad_alloc();
		capacity = newSize;
		ptr = tmp;
	}
}

unsigned char *convertTtoUTF8(buffer * buf, const tchar_t *src, int srcbytes/* = -1*/)
{
	bool bSucceeded;
#ifdef _UNICODE
	bSucceeded = convert(CP_UCS2LE, 
		(unsigned char *)src, (int)((srcbytes < 0) ? wcslen((const wchar_t *)src) * sizeof(wchar_t) : srcbytes),
		CP_UTF8, buf);
#else
	bSucceeded = convert(GetACP(),	
		(unsigned char *)src, (int)((srcbytes < 0) ? strlen((const char *)src) : srcbytes),
		CP_UTF8, buf);
#endif
	if (!bSucceeded)
		*((unsigned char *)buf->ptr) = 0;
	return buf->ptr;
}

unsigned char *convertTtoUTF8(const tchar_t *src, int srcbytes/* = -1*/)
{
	buffer buf(256);
	convertTtoUTF8(&buf, src, srcbytes);
	return (unsigned char *)_strdup((const char *)buf.ptr);
}

tchar_t *convertUTF8toT(buffer * buf, const char *src, int srcbytes/* = -1*/)
{
	bool bSucceeded;
#ifdef _UNICODE
	bSucceeded = convert(CP_UTF8,
		(const unsigned char *)src, (int)((srcbytes < 0) ? strlen((const char *)src) : srcbytes),
		CP_UCS2LE, buf);
#else
	bSucceeded = convert(CP_UTF8,
		(const unsigned char *)src, (int)((srcbytes < 0) ? strlen((const char *)src) : srcbytes),
		GetACP(), buf);
#endif
	if (!bSucceeded)
		*((tchar_t *)buf->ptr) = 0;
	return (tchar_t *)buf->ptr;
}

tchar_t *convertUTF8toT(const char *src, int srcbytes/* = -1*/)
{
	buffer buf(256);
	convertUTF8toT(&buf, src, srcbytes);
	return (tchar_t *)tc::tcsdup((const tchar_t*)buf.ptr);
}

void dealloc(void *ptr)
{
	free(ptr);
}

String toTString(const std::wstring& str)
{
#ifdef UNICODE
	return str;
#else
	return toThreadCP(str);
#endif
}

String toTString(const std::string& str)
{
#ifdef UNICODE
	std::wstring wstr;
	UnicodeConverter::toUTF16(str, wstr);
	return wstr;
#else
	const char *p = convertUTF8toT(str.c_str(), str.length());
	std::string astr = p;
	dealloc((void *)p);
	return astr;
#endif
}

void toUTF16(const String& tstr, std::wstring& wstr)
{
#ifdef UNICODE
	wstr = tstr;
#else
	UnicodeConverter::toUTF16(tstr, wstr);
#endif
}

std::string toUTF8(const String& tstr)
{
	std::string u8str;
	toUTF8(tstr, u8str);
	return u8str;
}

void toUTF8(const String& tstr, std::string& u8str)
{
#ifdef _UNICODE
	const size_t len = tstr.length();
	u8str.resize(len * 3);

	if (len == 0)
		return;

	char *p = &u8str[0];
	for (size_t i = 0; i < len; ++i)
	{
		unsigned uc = tstr[i];
		if (uc >= 0xd800 && uc < 0xdc00)
		{
			if (++i != len)
			{
				wchar_t uc2 = tstr[i];
				uc = ((uc & 0x3ff) << 10) + (uc2 & 0x3ff) + 0x10000;
			}
		}
		p += Ucs4_to_Utf8(uc, reinterpret_cast<unsigned char *>(p));
	}
	u8str.resize(p - &u8str[0]);
#else
	const char *p = (const char *)convertTtoUTF8(tstr.c_str(), tstr.length());
	u8str = p;
	dealloc((void *)p);
#endif
}

bool convert(int codepage1, const unsigned char * src, int srcbytes, int codepage2, buffer * dest)
{
	UNICODESET unicoding[2];
	int codepage[2] = {codepage1, codepage2};

	int i;
	for (i = 0; i < 2; i++)
	{
		switch (codepage[i])
		{
		case CP_UCS2LE:
			unicoding[i] = UCS2LE; break;
		case CP_UCS2BE:
			unicoding[i] = UCS2BE; break;
		case CP_UTF8:
			unicoding[i] = UTF8; break;
		default:
			unicoding[i] = NONE; break;
		}
	}

	return convert(unicoding[0], codepage1, src, srcbytes, unicoding[1], codepage2, dest);
}

/**
 * @brief Convert from one text encoding to another; return false if any lossing conversions
 */
bool convert(UNICODESET unicoding1, int codepage1, const unsigned char * src, size_t srcbytes, UNICODESET unicoding2, int codepage2, buffer * dest)
{
	if (unicoding1 == unicoding2 && (unicoding1 || EqualCodepages(codepage1, codepage2)))
	{
		// simple byte copy
		dest->resize(srcbytes + 2);
		CopyMemory(dest->ptr, src, srcbytes);
		dest->ptr[srcbytes] = 0;
		dest->ptr[srcbytes+1] = 0;
		dest->size = srcbytes;
		return true;
	}
	if ((unicoding1 == UCS2LE && unicoding2 == UCS2BE)
			|| (unicoding1 == UCS2BE && unicoding2 == UCS2LE))
	{
		// simple byte swap
		dest->resize(srcbytes + 2);
		for (size_t i = 0; i < srcbytes; i += 2)
		{
			// Byte-swap into destination
			uint16_t c = *(const uint16_t*)(src + i);
			*(uint16_t*)(dest->ptr + i) = ((c << 8) | (c >> 8));
		}
		dest->ptr[srcbytes] = 0;
		dest->ptr[srcbytes+1] = 0;
		dest->size = srcbytes;
		return true;
	}
	if (unicoding1 != UCS2LE && unicoding2 != UCS2LE)
	{
		// Break problem into two simpler pieces by converting through UCS-2LE
		buffer intermed(dest->capacity + 2);
		bool step1 = convert(unicoding1, codepage1, src, srcbytes, UCS2LE, 0, &intermed);
		bool step2 = convert(UCS2LE, 0, intermed.ptr, intermed.size, unicoding2, codepage2, dest);
		return step1 && step2;
	}
	if (unicoding1 == UCS2LE)
	{
		// From UCS-2LE to 8-bit (or UTF-8)

		// WideCharToMultiByte: lpDefaultChar & lpUsedDefaultChar must be `nullptr` when using UTF-8

		int destcp = (unicoding2 == UTF8 ? CP_UTF8 : codepage2);
		if (destcp == CP_ACP || IsValidCodePage(destcp))
		{
			DWORD flags = 0;
			int bytes = WideCharToMultiByte(destcp, flags, (LPCWSTR)src, static_cast<int>(srcbytes/2), 0, 0, nullptr, nullptr);
			dest->resize(bytes + 2);
			int losses = 0;
			bytes = WideCharToMultiByte(destcp, flags, (LPCWSTR)src, static_cast<int>(srcbytes/2), (char *)dest->ptr, static_cast<int>(dest->capacity), nullptr, nullptr);
			dest->ptr[bytes] = 0;
			dest->ptr[bytes+1] = 0;
			dest->size = bytes;
			return losses==0;
		}
		else
		{
			size_t srcsize = srcbytes / 2;
			size_t dstsize = srcbytes * 6; 
			dest->resize(dstsize + 2);
			IExconverter *pexconv = Exconverter::getInstance();
			if (pexconv != nullptr)
			{
				bool result = pexconv->convertFromUnicode(destcp, (LPWSTR)src, &srcsize, (char *)dest->ptr, &dstsize);
				dest->ptr[dstsize] = 0;
				dest->ptr[dstsize+1] = 0;
				dest->size = dstsize;
				return result;
			}
			else
				return false;
		}
	}
	else
	{
		// From 8-bit (or UTF-8) to UCS-2LE
		int srccp = (unicoding1 == UTF8 ? CP_UTF8 : codepage1);
		if (srccp == CP_ACP || IsValidCodePage(srccp))
		{
			DWORD flags = 0;
			int wchars = MultiByteToWideChar(srccp, flags, (LPCSTR)src, static_cast<int>(srcbytes), 0, 0);
			dest->resize((wchars + 1) *2);
			wchars = MultiByteToWideChar(srccp, flags, (LPCSTR)src, static_cast<int>(srcbytes), (LPWSTR)dest->ptr, static_cast<int>(dest->capacity/2));
			dest->ptr[wchars * 2] = 0;
			dest->ptr[wchars * 2 + 1] = 0;
			dest->size = wchars * 2;
			return true;
		}
		else
		{
			size_t srcsize = srcbytes;
			size_t dstsize = srcbytes; 
			dest->resize((srcbytes + 1) * sizeof(wchar_t));
			IExconverter *pexconv = Exconverter::getInstance();
			if (pexconv != nullptr)
			{
				bool result = pexconv->convertToUnicode(srccp, (LPCSTR)src, &srcsize, (LPWSTR)dest->ptr, &dstsize);
				dest->ptr[dstsize * sizeof(wchar_t)] = 0;
				dest->ptr[dstsize * sizeof(wchar_t) + 1] = 0;
				dest->size = dstsize * sizeof(wchar_t);
				return result;
			}
			else
				return false;
		}
	}
}

/**
 * @brief Convert from Unicode to Ansi using given codepage.
 * @param [in] from String to convert.
 * @param [in] codepage Codepage to use in conversion.
 * @param [out] to Ansi string.
 */
static void convert(const std::wstring& from, unsigned codepage, std::string& to)
{
	int len = WideCharToMultiByte(codepage, 0, from.c_str(), static_cast<int>(from.length()), 0, 0, 0, 0);
	if (len)
	{
		to.resize(len);
		WideCharToMultiByte(codepage, 0, from.c_str(), static_cast<int>(from.length()), &to[0], static_cast<int>(len), nullptr, nullptr);
	}
	else
	{
		to.clear();
	}
}

/**
 * @brief Convert from Unicode to Ansi using system codepage.
 * This function converts Unicode string to ansi string using system codepage.
 * This conversion function should be used when converting strings containing
 * paths. As paths are handled by the system and are not file content.
 * @param [in] str String to convert.
 * @return Ansi string.
 */
std::string toSystemCP(const std::string& str)
{
	return str;
}

std::string toSystemCP(const std::wstring& str)
{
	std::string to;
	convert(str, CP_ACP, to);
	return to;
}

/**
 * @brief Convert from Unicode to Ansi using thread codepage.
 * This function converts Unicode string to ansi string using thread codepage.
 * Thread codepage is practically the codepage WinMerge is using internally.
 * @param [in] str String to convert.
 * @return Ansi string.
 */
std::string toThreadCP(const std::string& str)
{
	return str;
}

std::string toThreadCP(const std::wstring& str)
{
	std::string to;
	convert(str, CP_THREAD_ACP, to);
	return to;
}

// Algorithm originally from:
// TortoiseMerge - a Diff/Patch program
// Copyright (C) 2007 - TortoiseSVN
/**
 * @brief Check for invalid UTF-8 bytes in buffer.
 * This function checks if there are invalid UTF-8 bytes in the given buffer.
 * If such bytes are found, caller knows this buffer is not valid UTF-8 file.
 * @param [in] pBuffer Pointer to begin of the buffer.
 * @param [in] size Size of the buffer in bytes.
 * @return true if invalid bytes found, false otherwise.
 */
bool CheckForInvalidUtf8(const char* pBuffer, size_t size)
{
	bool bUTF8 = false;
	for (unsigned char* pb = (unsigned char*)pBuffer, *end = pb + size; pb < end;)
	{
		unsigned c = *pb++;
		
		if (!(c & 0x80)) continue;
		
		if ((c >= 0xF5) || (c == 0xC0) || (c == 0xC1))
			return true;

		uint32_t v = 0x80808000; //1st 0-byte covers scenario if no any next "if" fired at all

		if ((c & 0xE0) == 0xC0)
		{
			if (pb == end)
				return true;
			*reinterpret_cast<unsigned char*>(&v) = *pb++;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			if (pb > end - 2)
				return true;
			*reinterpret_cast<uint16_t*>(&v) = *reinterpret_cast<uint16_t*>(pb);
			pb += 2;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			if (pb > end - 3)
				return true;
			static_assert(sizeof(char) == sizeof(uint8_t), "unexpected char-size");

			*reinterpret_cast<uint16_t*>(&v) = *reinterpret_cast<uint16_t*>(pb);
			reinterpret_cast<uint8_t*>(&v)[2] = pb[2];
			pb += 3;
		}

		if ((v & (0xC0C0C0C0)) != 0x80808080)
			return true;
		bUTF8 = true;
	}
	return !bUTF8;
}

/**
 * @brief Determine encoding from byte buffer.
 * @param [in] pBuffer Pointer to the begin of the buffer.
 * @param [in] size Size of the buffer.
 * @param [out] pBom Returns true if buffer had BOM bytes, false otherwise.
 * @return One of UNICODESET values as encoding.
 * EF BB BF UTF-8
 * FF FE UTF-16, little endian
 * FE FF UTF-16, big endian
 * FF FE 00 00 UTF-32, little endian
 * 00 00 FE FF UTF-32, big-endian
 */
UNICODESET DetermineEncoding(const unsigned char *pBuffer, uint64_t size, bool * pBom)
{
	UNICODESET unicoding = NONE;
	*pBom = false;

	if (size >= 2)
	{
		if (pBuffer[0] == 0xFF && pBuffer[1] == 0xFE)
		{
			unicoding = UCS2LE; //UNI little endian
			*pBom = true;
		}
		else if (pBuffer[0] == 0xFE && pBuffer[1] == 0xFF)
		{
			unicoding = UCS2BE; //UNI big endian
			*pBom = true;
		}
	}
	if (size >= 3)
	{
		if (pBuffer[0] == 0xEF && pBuffer[1] == 0xBB && pBuffer[2] == 0xBF)
		{
			unicoding = UTF8;
			*pBom = true;
		}
	}
	if (size >= 4)
	{
		if (pBuffer[0] == 0xFF && pBuffer[1] == 0xFE &&
				pBuffer[2] == 0x00 && pBuffer[3] == 0x00)
		{
			unicoding = UCS4LE; //UTF-32, little endian
			*pBom = true;
		}
		else if (pBuffer[0] == 0x00 && pBuffer[1] == 0x00 &&
				pBuffer[2] == 0xFE && pBuffer[3] == 0xFF)
		{
			unicoding = UCS4BE; //UTF-32, big endian
			*pBom = true;
		}
	}

	return unicoding;
}

/**
 * @brief Change any special codepage constants into real codepage numbers
 */
static int NormalizeCodepage(int cp)
{
	if (cp == CP_THREAD_ACP) // should only happen on Win2000+
	{
		tchar_t buff[32];
		if (GetLocaleInfo(GetThreadLocale(), LOCALE_IDEFAULTANSICODEPAGE, buff, sizeof(buff) / sizeof(buff[0])))
			cp = tc::ttol(buff);
		else
			// a valid codepage is better than no codepage
			cp = GetACP();
	}
	if (cp == CP_ACP) cp = GetACP();
	if (cp == CP_OEMCP) cp = GetOEMCP();
	return cp;
}

/**
 * @brief Compare two codepages for equality
 */
bool EqualCodepages(int cp1, int cp2)
{
	return (cp1 == cp2)
			|| (NormalizeCodepage(cp1) == NormalizeCodepage(cp2));
}

int getDefaultCodepage()
{
	return f_nDefaultCodepage;
}

void setDefaultCodepage(int cp)
{
	f_nDefaultCodepage = cp;
}

} // namespace ucr

