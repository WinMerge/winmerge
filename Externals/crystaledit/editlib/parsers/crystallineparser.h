#pragma once

#include "../utils/ctchar.h"
#include "../SyntaxColors.h"
#include "../ISyntaxParser.h"
#include "../TextDefinition.h"
#include <cassert>
#include <array>
#include <vector>

#define ISXKEYWORD(keywordlist, key, keylen) CrystalLineParser::IsXKeyword(key, keylen, keywordlist, sizeof(keywordlist)/sizeof(keywordlist[0]), tc::tcsncmp)
#define ISXKEYWORDI(keywordlist, key, keylen) CrystalLineParser::IsXKeyword(key, keylen, keywordlist, sizeof(keywordlist)/sizeof(keywordlist[0]), tc::tcsnicmp)

#ifndef ASSERT
#define ASSERT(condition) assert(condition);
#endif

#define DEFINE_BLOCK(pos, colorindex) CrystalLineParser::defineBlock(pBuf, pos, colorindex)

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

namespace CrystalLineParser
{
// TEXTBLOCK is now defined in ISyntaxParser.h.
// These aliases preserve backward compatibility for all existing code.
using TEXTBLOCK = LangServices::TEXTBLOCK;
using LanguageId = LangServices::LanguageId;

inline void defineBlock(std::vector<TEXTBLOCK>* pBuf, int pos, int colorindex)
{
	if (!pBuf)
		return;
	if (pBuf->empty() || pBuf->back().m_nCharPos <= pos)
	{
		if (pBuf->size() > 0 && pBuf->back().m_nCharPos == pos)
			pBuf->pop_back();
		if (pBuf->empty() || pBuf->back().m_nColorIndex != colorindex)
			pBuf->push_back({ pos, colorindex, static_cast<int>(COLORINDEX_BKGND) });
	}
}

bool IsXKeyword(const tchar_t *pszKey, size_t nKeyLen, const tchar_t *pszKeywordList[], size_t nKeywordListCount, int(*compare)(const tchar_t *, const tchar_t *, size_t));
bool IsXNumber(const tchar_t* pszChars, int nLength);
bool IsHtmlKeyword(const tchar_t *pszChars, int nLength);
bool IsHtmlUser1Keyword(const tchar_t *pszChars, int nLength);
bool IsHtmlUser2Keyword(const tchar_t *pszChars, int nLength);

unsigned ParseLinePlain(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineAbap(unsigned dwCookie, const tchar_t* pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineAda(unsigned dwCookie, const tchar_t* pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineAsp(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineAutoIt(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineBasic(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineBatch(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineC(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineCJava(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf, bool (*IsKeyword)(const tchar_t *pszChars, int nLength), bool (*IsUser1Keyword)(const tchar_t *pszChars, int nLength));
unsigned ParseLineCSharp(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineCss(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineDcl(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineDlang(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineFortran(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineFSharp(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineGo(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineHtml(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineHtmlEx(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf, LangServices::LanguageId nEmbeddedLanguage);
unsigned ParseLineIni(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineInnoSetup(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineIS(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineJava(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineJavaScript(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineLisp(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineLua(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineMatlab(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineNsis(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLinePascal(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLinePerl(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLinePhp(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLinePhpLanguage(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLinePo(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLinePowerShell(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLinePython(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineRexx(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineRsrc(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineRuby(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineRust(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineSgml(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineSh(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineSiod(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineSmarty(unsigned dwCookie, const tchar_t* pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineSmartyLanguage(unsigned dwCookie, const tchar_t* pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineSql(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineTcl(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineTex(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineVerilog(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineVhdl(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
unsigned ParseLineXml(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);

}
