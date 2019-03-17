#pragma once

#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != nullptr)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        if (nActualItems > 0 && pBuf[nActualItems - 1].m_nCharPos == (pos)) nActualItems--;\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010
#define COOKIE_USER1            0x0020
#define COOKIE_EXT_USER1        0x0040
#define COOKIE_SECTION          0x0080
#define COOKIE_KEY              0x0100
#define COOKIE_EXT_COMMENT2     0x0200
#define COOKIE_USER2            0x0400
#define COOKIE_VARIABLE         0x0800
#define COOKIE_RAWSTRING        0x1000
#define COOKIE_EXT_DEFINITION   0x2000
#define COOKIE_EXT_VALUE        0x4000

#define COOKIE_GET_EXT_COMMENT_DEPTH(cookie) (((cookie) & 0x0F000000) >> 24)
#define COOKIE_SET_EXT_COMMENT_DEPTH(cookie, depth) (cookie) = (((cookie) & 0xF0FFFFFF) | ((depth) << 24))
#define COOKIE_GET_RAWSTRING_NUMBER_COUNT(cookie) (((cookie) & 0xF0000000) >> 28)
#define COOKIE_SET_RAWSTRING_NUMBER_COUNT(cookie, count) (cookie) = (((cookie) & 0x0FFFFFFF) | ((count) << 28))
#define COOKIE_GET_LUA_EQUALS_SIGN_COUNT(cookie) (((cookie) & 0xF0000000) >> 28)
#define COOKIE_SET_LUA_EQUALS_SIGN_COUNT(cookie, count) (cookie) = (((cookie) & 0x0FFFFFFF) | ((count) << 28))

namespace CrystalLineParser
{
//  Syntax coloring overrides
struct TEXTBLOCK
{
	int m_nCharPos;
	int m_nColorIndex;
	int m_nBgColorIndex;
};

DWORD ParseLinePlain(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineAsp(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineBasic(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineBatch(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineC(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineCSharp(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineCss(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineDcl(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineFortran(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineGo(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineHtml(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineIni(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineInnoSetup(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineIS(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineJava(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineLisp(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineLua(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineNsis(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLinePascal(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLinePerl(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLinePhp(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLinePo(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLinePowerShell(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLinePython(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineRexx(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineRsrc(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineRuby(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineRust(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineSgml(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineSh(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineSiod(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineSql(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineTcl(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineTex(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineVerilog(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineVhdl(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
DWORD ParseLineXml(DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);

}
