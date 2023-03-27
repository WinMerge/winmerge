/**
 *  @file   unicoder.h
 *  @author Perry Rapp, Creator, 2003-2004
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-02-20 (Perry Rapp)
 *
 *  @brief  Declaration of utility unicode conversion routines
 */
#pragma once

#include "UnicodeString.h"

namespace ucr
{

/**
 * @brief A simple buffer struct.
 */
struct buffer
{
	unsigned char * ptr; /**< Pointer to a buffer. */
	size_t capacity; /**< Buffer's size in bytes. */
	size_t size; /**< Size of the data in the buffer, <= capacity. */

	explicit buffer(size_t initialSize);
	~buffer();
	void resize(size_t newSize);
};

enum
{
	CP_UTF_8  = 65001,
	CP_UCS2LE = 1200,
	CP_UCS2BE = 1201,
#ifdef _UNICODE
	CP_TCHAR = CP_UCS2LE
#else
	CP_TCHAR = 0
#endif
};

/** @brief Known Unicode encodings. */
enum UNICODESET : char
{
	NONE = 0,  /**< No unicode. */
	UCS2LE,    /**< UCS-2 / UTF-16 little endian. */
	UCS2BE,    /**< UCS-2 / UTF-16 big endian. */
	UTF8,      /**< UTF-8. */
	UCS4LE,    /**< UTF-32 little endian */
	UCS4BE,    /**< UTF-32 big-endian */
};

int Ucs4_to_Utf8(unsigned unich, unsigned char * utf8);
int Utf8len_fromLeadByte(unsigned char ch);
int Utf8len_fromCodepoint(unsigned ch);
/**
 * @brief How many bytes will it take to write string as UTF-8 ?
 *
 * @param size size argument as filemapping are not 0 terminated
 *
 * @bug Fails for files larger than 2gigs
 */
template <typename C>
size_t Utf8len_of_string(const C* text, size_t size)
{
	size_t len = 0;
	for (size_t i = 0; i < size; ++i)
	{
		int chlen = Utf8len_fromCodepoint(text[i]);
		if (chlen < 1) chlen = 1;
		len += chlen;
	}
	return len;
}
size_t stringlen_of_utf8(const char* text, size_t size);
unsigned GetUtf8Char(unsigned char * str);
int to_utf8_advance(unsigned u, unsigned char * &lpd);
void maketchar(String & ch, unsigned unich, bool & lossy);
int writeBom(void* dest, UNICODESET unicoding);
int getBomSize(UNICODESET unicoding);
unsigned get_unicode_char(unsigned char * ptr, UNICODESET unicoding, int codepage=0);
bool maketstring(String & line, const char* lpd, size_t len, int codepage, bool * lossy);
void maketchar(String & ch, unsigned unich, bool & lossy, unsigned codepage);
unsigned byteToUnicode(unsigned char ch);
unsigned byteToUnicode(unsigned char ch, unsigned codepage);
void getInternalEncoding(UNICODESET * unicoding, int * codepage);

// generic function to do all conversions
bool convert(UNICODESET unicoding1, int codepage1, const unsigned char * src, size_t srcbytes, UNICODESET unicoding2, int codepage2, buffer * dest);
bool convert(int codepage1, const unsigned char * src, int srcbytes, int codepage2, buffer * dest);

unsigned char *convertTtoUTF8(buffer * dest, const tchar_t *src, int srcbytes = -1);
unsigned char *convertTtoUTF8(const tchar_t *src, int srcbytes = -1);
tchar_t *convertUTF8toT(buffer * dest, const char* src, int srcbytes = -1);
tchar_t *convertUTF8toT(const char* src, int srcbytes = -1);
void dealloc(void *ptr);

String toTString(const std::wstring& str);
String toTString(const std::string& str);
void toUTF16(const String& tstr, std::wstring& wstr);
inline std::wstring toUTF16(const String& tstr)
{
#ifdef UNICODE
	return tstr;
#else
	std::wstring wstr;
	toUTF16(tstr, wstr);
	return wstr;
#endif
}

void toUTF8(const String& tstr, std::string& u8str);
std::string toUTF8(const String& tstr);
std::string toSystemCP(const std::string& str);
std::string toSystemCP(const std::wstring& str);
std::string toThreadCP(const std::string& str);
std::string toThreadCP(const std::wstring& str);

int CrossConvert(const char* src, unsigned srclen, char* dest, unsigned destsize, int cpin, int cpout, bool * lossy);
#ifndef UNICODE
String CrossConvertToStringA(const char* src, unsigned srclen, int cpin, int cpout, bool * lossy);
#endif

bool CheckForInvalidUtf8(const char *pBuffer, size_t size);

UNICODESET DetermineEncoding(const unsigned char *pBuffer, uint64_t size, bool * pBom);

int getDefaultCodepage();
void setDefaultCodepage(int cp);

bool EqualCodepages(int cp1, int cp2);

} // namespace ucr
