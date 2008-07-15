/**
 *  @file   unicoder.h
 *  @author Perry Rapp, Creator, 2003-2004
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-02-20 (Perry Rapp)
 *
 *  @brief  Declaration of utility unicode conversion routines
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#ifndef unicoder_h_included
#define unicoder_h_included

namespace ucr {

/**
 * @brief A simple buffer struct.
 */
struct buffer {
	unsigned char * ptr; /**< Pointer to a buffer. */
	unsigned int capacity; /**< Buffer's size in bytes. */
	unsigned int size; /**< Size of the data in the buffer, <= capacity. */

	buffer(unsigned int initialSize);
	~buffer();
	void resize(unsigned int newSize);
};

/** @brief Known Unicode encodings. */
enum UNICODESET
{
	NONE = 0,  /**< No unicode. */
	UCS2LE,    /**< UCS-2 little endian. */
	UCS2BE,    /**< UCS-2 big endian. */
	UTF8,      /**< UTF-8. */
};

int Ucs4_to_Utf8(UINT unich, unsigned char * utf8);
int Utf8len_fromLeadByte(unsigned char ch);
int Utf8len_fromCodepoint(UINT ch);
UINT Utf8len_of_string(LPCWSTR text, int size);
UINT stringlen_of_utf8(LPCSTR text, int size);
UINT GetUtf8Char(unsigned char * str);
int to_utf8_advance(UINT u, unsigned char * &lpd);
String maketchar(UINT ch, bool & lossy);
int writeBom(LPVOID dest, UNICODESET unicoding);
UINT get_unicode_char(unsigned char * ptr, UNICODESET unicoding, int codepage=0);
String maketstring(LPCSTR lpd, UINT len, int codepage, bool * lossy);
String maketchar(UINT unich, bool & lossy);
String maketchar(UINT unich, bool & lossy, UINT codepage);
UINT byteToUnicode(unsigned char ch);
UINT byteToUnicode(unsigned char ch, UINT codepage);
void getInternalEncoding(UNICODESET * unicoding, int * codepage);

// generic function to do all conversions
bool convert(UNICODESET unicoding1, int codepage1, const unsigned char * src, int srcbytes, UNICODESET unicoding2, int codepage2, buffer * dest);

int CrossConvert(LPCSTR src, UINT srclen, LPSTR dest, UINT destsize, int cpin, int cpout, bool * lossy);
#ifndef UNICODE
String CrossConvertToStringA(LPCSTR src, UINT srclen, int cpin, int cpout, bool * lossy);
#endif

UNICODESET DetermineEncoding(LPBYTE pBuffer, int size, bool * pBom);

} // namespace ucr

bool EqualCodepages(int cp1, int cp2);

#endif // unicoder_h_included

