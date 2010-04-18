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

namespace ucr
{

/**
 * @brief A simple buffer struct.
 */
struct buffer
{
	unsigned char * ptr; /**< Pointer to a buffer. */
	unsigned int capacity; /**< Buffer's size in bytes. */
	unsigned int size; /**< Size of the data in the buffer, <= capacity. */

	buffer(unsigned int initialSize);
	~buffer();
	void resize(unsigned int newSize);
};

struct CodePageInfo
{
	int codepage;
	wchar_t desc[256];
    wchar_t fixedWidthFont[256];
    char bGDICharset;
};

struct IExconverter
{
	virtual bool initialize() = 0;
	virtual bool convert(int srcCodepage, int dstCodepage, const unsigned char * src, int * srcbytes, unsigned char * dest, int * destbytes) = 0;
	virtual bool convertFromUnicode(int dstCodepage, const wchar_t * src, int * srcchars, char * dest, int *destbytes) = 0;
	virtual bool convertToUnicode(int srcCodepage, const char * src, int * srcbytes, wchar_t * dest, int *destchars) = 0;
	virtual int detectInputCodepage(int autodetectType, int defcodepage, const char *data, int size) = 0;
	virtual int enumCodePages(CodePageInfo *cpinfo, int count) = 0;
	virtual bool getCodepageFromCharsetName(LPCTSTR pszCharsetName, int *pCodepage) = 0;
	virtual bool getCodepageDescription(int codepage, LPTSTR pszCharsetName) = 0;
	virtual bool isValidCodepage(int codepage) = 0;
	virtual bool getCodePageInfo(int codePage, CodePageInfo *pCodePageInfo) = 0;
};

IExconverter *createConverterMLang();

class IExconverterPtr {
private:
	IExconverter *m_ptr;
public:
	IExconverterPtr(IExconverter *pexconv) { m_ptr = pexconv;	}
	~IExconverterPtr()                     { if (m_ptr) delete m_ptr; }
	operator IExconverter * ( void ) const { return m_ptr ; }
	IExconverter *operator->( void ) const { return m_ptr ; }
};

/** @brief Known Unicode encodings. */
enum UNICODESET
{
	NONE = 0,  /**< No unicode. */
	UCS2LE,    /**< UCS-2 / UTF-16 little endian. */
	UCS2BE,    /**< UCS-2 / UTF-16 big endian. */
	UTF8,      /**< UTF-8. */
	UCS4LE,    /**< UTF-32 little endian */
	UCS4BE,    /**< UTF-32 big-endian */
};
String GetUnicodesetName(UNICODESET codeset);

int Ucs4_to_Utf8(unsigned int unich, unsigned char * utf8);
int Utf8len_fromLeadByte(unsigned char ch);
int Utf8len_fromCodepoint(unsigned int ch);
unsigned int Utf8len_of_string(const wchar_t* text, int size);
unsigned int stringlen_of_utf8(const char* text, int size);
unsigned int GetUtf8Char(unsigned char * str);
int to_utf8_advance(unsigned int u, unsigned char * &lpd);
void maketchar(String & ch, unsigned int unich, bool & lossy);
int writeBom(void* dest, UNICODESET unicoding);
unsigned int get_unicode_char(unsigned char * ptr, UNICODESET unicoding, int codepage=0);
bool maketstring(String & line, const char* lpd, unsigned int len, int codepage, bool * lossy);
void maketchar(String & ch, unsigned int unich, bool & lossy, unsigned int codepage);
unsigned int byteToUnicode(unsigned char ch);
unsigned int byteToUnicode(unsigned char ch, unsigned int codepage);
void getInternalEncoding(UNICODESET * unicoding, int * codepage);

// generic function to do all conversions
bool convert(UNICODESET unicoding1, int codepage1, const unsigned char * src, int srcbytes, UNICODESET unicoding2, int codepage2, buffer * dest);
bool convert(int codepage1, const unsigned char * src, int srcbytes, int codepage2, buffer * dest);

unsigned char *convertTtoUTF8(buffer * dest, LPCTSTR * src, int srcbytes = -1);
unsigned char *convertTtoUTF8(LPCTSTR * src, int srcbytes/* = -1*/);
TCHAR *convertUTF8toT(buffer * dest, const char* * src, int srcbytes = -1);
TCHAR *convertUTF8toT(const char* * src, int srcbytes/* = -1*/);
void dealloc(void *ptr);

int CrossConvert(const char* src, unsigned int srclen, char* dest, unsigned int destsize, int cpin, int cpout, bool * lossy);
#ifndef UNICODE
String CrossConvertToStringA(const char* src, unsigned int srclen, int cpin, int cpout, bool * lossy);
#endif

UNICODESET DetermineEncoding(PBYTE pBuffer, __int64 size, bool * pBom);

} // namespace ucr

bool EqualCodepages(int cp1, int cp2);

#endif // unicoder_h_included

