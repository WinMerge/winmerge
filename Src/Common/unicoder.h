/**
 *  @file   unicoder.h
 *  @author Perry Rapp, Creator, 2003-2004
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-02-20 (Perry Rapp)
 *
 *  @brief  Declaration of utility unicode conversion routines
 */
// ID line follows -- this is updated by SVN
// $Id: unicoder.h 6936 2009-07-26 17:17:18Z kimmov $

#ifndef unicoder_h_included
#define unicoder_h_included

#include "UnicodeString.h"
#include "codepage.h"
#include <cstdint>

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

	buffer(size_t initialSize);
	~buffer();
	void resize(size_t newSize);
};

/** @brief Known Unicode encodings. */
enum UNICODESET
#if __cplusplus >= 201103L || _MSC_VER >= 1600
	: char
#endif
{
	NONE = 0,  /**< No unicode. */
	UCS2LE,    /**< UCS-2 / UTF-16 little endian. */
	UCS2BE,    /**< UCS-2 / UTF-16 big endian. */
	UTF8,      /**< UTF-8. */
	UCS4LE,    /**< UTF-32 little endian */
	UCS4BE,    /**< UTF-32 big-endian */
};
String GetUnicodesetName(UNICODESET codeset);

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

unsigned char *convertTtoUTF8(buffer * dest, const TCHAR *src, int srcbytes = -1);
unsigned char *convertTtoUTF8(const TCHAR *src, int srcbytes = -1);
TCHAR *convertUTF8toT(buffer * dest, const char* src, int srcbytes = -1);
TCHAR *convertUTF8toT(const char* src, int srcbytes = -1);
void dealloc(void *ptr);

String toTString(const std::wstring& str);
String toTString(const std::string& str);
void toUTF16(const String& tstr, std::wstring& wstr);
std::wstring toUTF16(const String& tstr);
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

#endif // unicoder_h_included

