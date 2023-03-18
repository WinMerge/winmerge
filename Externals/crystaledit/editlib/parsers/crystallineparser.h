#pragma once

#include "../utils/ctchar.h"
#include <cassert>

#define ISXKEYWORD(keywordlist, key, keylen) CrystalLineParser::IsXKeyword(key, keylen, keywordlist, sizeof(keywordlist)/sizeof(keywordlist[0]), tc::tcsncmp)
#define ISXKEYWORDI(keywordlist, key, keylen) CrystalLineParser::IsXKeyword(key, keylen, keywordlist, sizeof(keywordlist)/sizeof(keywordlist[0]), tc::tcsnicmp)

#ifndef ASSERT
#define ASSERT(condition) assert(condition);
#endif
#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != nullptr)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        if (nActualItems > 0 && pBuf[nActualItems - 1].m_nCharPos == (pos)) nActualItems--;\
        if (nActualItems == 0 || pBuf[nActualItems - 1].m_nColorIndex != (colorindex)){\
            pBuf[nActualItems].m_nCharPos = (pos);\
            pBuf[nActualItems].m_nColorIndex = (colorindex);\
            pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
            nActualItems ++;}}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_ELEMENT          0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010
#define COOKIE_USER1            0x0020
#define COOKIE_EXT_USER1        0x0040
#define COOKIE_BLOCK_STYLE      0x0080
#define COOKIE_BLOCK_SCRIPT     0x0100
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
#define COOKIE_GET_RAWSTRING_DELIMITER(cookie) (((cookie) & 0xFF000000) >> 24)
#define COOKIE_SET_RAWSTRING_DELIMITER(cookie, delimiter) (cookie) = (((cookie) & 0x00FFFFFF) | ((delimiter) << 24))
#define COOKIE_GET_LUA_EQUALS_SIGN_COUNT(cookie) (((cookie) & 0xF0000000) >> 28)
#define COOKIE_SET_LUA_EQUALS_SIGN_COUNT(cookie, count) (cookie) = (((cookie) & 0x0FFFFFFF) | ((count) << 28))

#define SRCOPT_INSERTTABS 1
#define SRCOPT_SHOWTABS 2
#define SRCOPT_BSATBOL 4
#define SRCOPT_SELMARGIN 8
#define SRCOPT_AUTOINDENT 16
#define SRCOPT_BRACEANSI 32
#define SRCOPT_BRACEGNU 64
#define SRCOPT_EOLNDOS 128
#define SRCOPT_EOLNUNIX 256
#define SRCOPT_EOLNMAC 512
#define SRCOPT_FNBRACE 1024
#define SRCOPT_WORDWRAP 2048
#define SRCOPT_TOPMARGIN 4096

namespace CrystalLineParser
{
//  Syntax coloring overrides
struct TEXTBLOCK
{
	int m_nCharPos;
	int m_nColorIndex;
	int m_nBgColorIndex;
};

typedef enum
{
	SRC_PLAIN = 0,
	SRC_ABAP,
	SRC_ASP,
	SRC_AUTOIT,
	SRC_BASIC,
	SRC_BATCH,
	SRC_C,
	SRC_CSHARP,
	SRC_CSS,
	SRC_DCL,
	SRC_DLANG,
	SRC_FORTRAN,
	SRC_GO,
	SRC_HTML,
	SRC_INI,
	SRC_INNOSETUP,
	SRC_INSTALLSHIELD,
	SRC_JAVA,
	SRC_JAVASCRIPT,
	SRC_JSON,
	SRC_LISP,
	SRC_LUA,
	SRC_MATLAB,
	SRC_NSIS,
	SRC_PASCAL,
	SRC_PERL,
	SRC_PHP,
	SRC_PO,
	SRC_POWERSHELL,
	SRC_PYTHON,
	SRC_REXX,
	SRC_RSRC,
	SRC_RUBY,
	SRC_RUST,
	SRC_SGML,
	SRC_SH,
	SRC_SIOD,
	SRC_SMARTY,
	SRC_SQL,
	SRC_TCL,
	SRC_TEX,
	SRC_VERILOG,
	SRC_VHDL,
	SRC_XML,
	SRC_MAX_ENTRY	/* always last entry, used for bound checking */
}
TextType;

// Tabsize is commented out since we have only GUI setting for it now.
// Not removed because we may later want to have per-filetype settings again.
// See ccrystaltextview.cpp for per filetype table initialization.
struct TextDefinition
{
	TextType type;
	tchar_t name[256];
	tchar_t exts[256];
	unsigned (* ParseLineX) (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
	unsigned flags;
//        unsigned tabsize;
	tchar_t opencomment[8];
	tchar_t closecomment[8];
	tchar_t commentline[8];
	unsigned encoding;
};

extern TextDefinition m_SourceDefs[SRC_MAX_ENTRY];

bool IsXKeyword(const tchar_t *pszKey, size_t nKeyLen, const tchar_t *pszKeywordList[], size_t nKeywordListCount, int(*compare)(const tchar_t *, const tchar_t *, size_t));
bool IsXNumber(const tchar_t* pszChars, int nLength);
bool IsHtmlKeyword(const tchar_t *pszChars, int nLength);
bool IsHtmlUser1Keyword(const tchar_t *pszChars, int nLength);
bool IsHtmlUser2Keyword(const tchar_t *pszChars, int nLength);

TextDefinition *GetTextType(const tchar_t *pszExt);
TextDefinition* GetTextType(int index);
void SetExtension(int index, const tchar_t *pszExts);

unsigned ParseLinePlain(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineAbap(unsigned dwCookie, const tchar_t* pszChars, int nLength, TEXTBLOCK* pBuf, int& nActualItems);
unsigned ParseLineAsp(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineAutoIt(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineBasic(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineBatch(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineC(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineCJava(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems, bool (*IsKeyword)(const tchar_t *pszChars, int nLength), bool (*IsUser1Keyword)(const tchar_t *pszChars, int nLength));
unsigned ParseLineCSharp(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineCss(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineDcl(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineDlang(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineFortran(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineGo(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineHtml(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineHtmlEx(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems, int nEmbeddedLanguage);
unsigned ParseLineIni(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineInnoSetup(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineIS(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineJava(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineJavaScript(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineLisp(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineLua(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineMatlab(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineNsis(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLinePascal(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLinePerl(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLinePhp(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLinePhpLanguage(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLinePo(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLinePowerShell(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLinePython(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineRexx(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineRsrc(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineRuby(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineRust(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineSgml(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineSh(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineSiod(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineSmarty(unsigned dwCookie, const tchar_t* pszChars, int nLength, TEXTBLOCK* pBuf, int& nActualItems);
unsigned ParseLineSmartyLanguage(unsigned dwCookie, const tchar_t* pszChars, int nLength, TEXTBLOCK* pBuf, int& nActualItems);
unsigned ParseLineSql(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineTcl(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineTex(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineVerilog(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineVhdl(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);
unsigned ParseLineXml(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems);

}
