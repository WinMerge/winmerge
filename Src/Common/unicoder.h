/**
 *  @file   unicoder.h
 *  @author Perry Rapp, Creator, 2003
 *  @date   Created: 2003-10
 *  @date   Edited:  2003-10-21 (Perry)
 *
 *  @brief  Declaration of utility unicode conversion routines
 */ 

#ifndef unicoder_h_included
#define unicoder_h_included

namespace ucr {

typedef enum { NONE=0, UCS2LE, UCS2BE, UTF8 } UNICODESET;

int Ucs4_to_Utf8(UINT unich, unsigned char * utf8);
int Utf8len_fromLeadByte(unsigned char ch);
int Utf8len_fromCodepoint(UINT ch);
UINT Utf8len_of_string(const CString & text);
UINT GetUtf8Char(unsigned char * str);
int to_utf8_advance(UINT u, unsigned char * &lpd);
CString maketchar(UINT ch, bool & lossy);
void convertToBuffer(const CString & src, LPVOID dest, UNICODESET codeset);
UINT get_unicode_char(unsigned char * ptr, UNICODESET codeset, int codepage=0);
CString maketstring(unsigned char * lpd, UINT len, int codepage=0);
CString maketchar(UINT unich, bool & lossy);
CString maketchar(UINT unich, bool & lossy, UINT codepage);
UINT byteToUnicode(unsigned char ch);
UINT byteToUnicode(unsigned char ch, UINT codepage);


} // namespace ucr

#endif // unicoder_h_included

