/**
 *  @file   unicoder.h
 *  @author Perry Rapp, Creator, 2003-2004
 *  @date   Created: 2003-10
 *  @date   Edited:  2004-03-26 (Laurent)
 *
 *  @brief  Declaration of utility unicode conversion routines
 */ 

#ifndef unicoder_h_included
#define unicoder_h_included

namespace ucr {

struct buffer {
	unsigned char * ptr;
	unsigned int size;
	unsigned int used;

	buffer(unsigned int needed);
	~buffer();
	void resize(unsigned int needed);
};

typedef enum { NONE=0, UCS2LE, UCS2BE, UTF8 } UNICODESET;
CString GetUnicodesetName(UNICODESET codeset);

int Ucs4_to_Utf8(UINT unich, unsigned char * utf8);
int Utf8len_fromLeadByte(unsigned char ch);
int Utf8len_fromCodepoint(UINT ch);
UINT Utf8len_of_string(LPCWSTR text, int size);
UINT stringlen_of_utf8(LPCSTR text, int size);
UINT GetUtf8Char(unsigned char * str);
int to_utf8_advance(UINT u, unsigned char * &lpd);
CString maketchar(UINT ch, bool & lossy);
int writeBom(LPVOID dest, UNICODESET unicoding);
int convertToBuffer(const CString & src, LPVOID dest, UNICODESET unicoding, int codepage);
UINT get_unicode_char(unsigned char * ptr, UNICODESET unicoding, int codepage=0);
CString maketstring(LPCSTR lpd, UINT len, int codepage, bool * lossy);
CString maketchar(UINT unich, bool & lossy);
CString maketchar(UINT unich, bool & lossy, UINT codepage);
UINT byteToUnicode(unsigned char ch);
UINT byteToUnicode(unsigned char ch, UINT codepage);
int getDefaultCodepage();
void getDefaultEncoding(UNICODESET * unicoding, int * codepage);

// generic function to do all conversions
bool convert(UNICODESET unicoding1, int codepage1, const unsigned char * src, int srcbytes, UNICODESET unicoding2, int codepage2, buffer * dest);

int CrossConvert(LPCSTR src, UINT srclen, LPSTR dest, UINT destsize, int cpin, int cpout, bool * lossy);
#ifndef UNICODE
CString CrossConvertToStringA(LPCSTR src, UINT srclen, int cpin, int cpout, bool * lossy);
#endif

} // namespace ucr

#endif // unicoder_h_included

