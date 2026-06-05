#pragma once

#include "../utils/ctchar.h"
#include "../SyntaxColors.h"
#include "../ISyntaxParser.h"
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
// TEXTBLOCK and TextType are now defined in ISyntaxParser.h.
// These aliases preserve backward compatibility for all existing code.
using TEXTBLOCK = ISyntaxParser::TEXTBLOCK;
using TextType  = ISyntaxParser::TextType;

// Legacy SRC_* constants as constexpr aliases.
constexpr auto SRC_PLAIN         = ISyntaxParser::TextType::Plain;
constexpr auto SRC_ABAP          = ISyntaxParser::TextType::Abap;
constexpr auto SRC_ADA           = ISyntaxParser::TextType::Ada;
constexpr auto SRC_ASP           = ISyntaxParser::TextType::Asp;
constexpr auto SRC_AUTOIT        = ISyntaxParser::TextType::AutoIt;
constexpr auto SRC_BASIC         = ISyntaxParser::TextType::Basic;
constexpr auto SRC_BATCH         = ISyntaxParser::TextType::Batch;
constexpr auto SRC_C             = ISyntaxParser::TextType::C;
constexpr auto SRC_CSHARP        = ISyntaxParser::TextType::CSharp;
constexpr auto SRC_CSS           = ISyntaxParser::TextType::Css;
constexpr auto SRC_DCL           = ISyntaxParser::TextType::Dcl;
constexpr auto SRC_DLANG         = ISyntaxParser::TextType::Dlang;
constexpr auto SRC_FORTRAN       = ISyntaxParser::TextType::Fortran;
constexpr auto SRC_FSHARP        = ISyntaxParser::TextType::FSharp;
constexpr auto SRC_GO            = ISyntaxParser::TextType::Go;
constexpr auto SRC_HTML          = ISyntaxParser::TextType::Html;
constexpr auto SRC_INI           = ISyntaxParser::TextType::Ini;
constexpr auto SRC_INNOSETUP     = ISyntaxParser::TextType::InnoSetup;
constexpr auto SRC_INSTALLSHIELD = ISyntaxParser::TextType::InstallShield;
constexpr auto SRC_JAVA          = ISyntaxParser::TextType::Java;
constexpr auto SRC_JAVASCRIPT    = ISyntaxParser::TextType::JavaScript;
constexpr auto SRC_JSON          = ISyntaxParser::TextType::Json;
constexpr auto SRC_LISP          = ISyntaxParser::TextType::Lisp;
constexpr auto SRC_LUA           = ISyntaxParser::TextType::Lua;
constexpr auto SRC_MATLAB        = ISyntaxParser::TextType::Matlab;
constexpr auto SRC_NSIS          = ISyntaxParser::TextType::Nsis;
constexpr auto SRC_PASCAL        = ISyntaxParser::TextType::Pascal;
constexpr auto SRC_PERL          = ISyntaxParser::TextType::Perl;
constexpr auto SRC_PHP           = ISyntaxParser::TextType::Php;
constexpr auto SRC_PO            = ISyntaxParser::TextType::Po;
constexpr auto SRC_POWERSHELL    = ISyntaxParser::TextType::PowerShell;
constexpr auto SRC_PYTHON        = ISyntaxParser::TextType::Python;
constexpr auto SRC_REXX          = ISyntaxParser::TextType::Rexx;
constexpr auto SRC_RSRC          = ISyntaxParser::TextType::Rsrc;
constexpr auto SRC_RUBY          = ISyntaxParser::TextType::Ruby;
constexpr auto SRC_RUST          = ISyntaxParser::TextType::Rust;
constexpr auto SRC_SGML          = ISyntaxParser::TextType::Sgml;
constexpr auto SRC_SH            = ISyntaxParser::TextType::Sh;
constexpr auto SRC_SIOD          = ISyntaxParser::TextType::Siod;
constexpr auto SRC_SMARTY        = ISyntaxParser::TextType::Smarty;
constexpr auto SRC_SQL           = ISyntaxParser::TextType::Sql;
constexpr auto SRC_TCL           = ISyntaxParser::TextType::Tcl;
constexpr auto SRC_TEX           = ISyntaxParser::TextType::Tex;
constexpr auto SRC_VERILOG       = ISyntaxParser::TextType::Verilog;
constexpr auto SRC_VHDL          = ISyntaxParser::TextType::Vhdl;
constexpr auto SRC_XML           = ISyntaxParser::TextType::Xml;
constexpr auto SRC_MAX_ENTRY     = ISyntaxParser::TextType::MaxEntry;

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

// Tabsize is commented out since we have only GUI setting for it now.
// Not removed because we may later want to have per-filetype settings again.
// See ccrystaltextview.cpp for per filetype table initialization.
struct TextDefinition
{
	TextType type;
	const tchar_t* name;
	tchar_t* exts;
	bool extsIsDynamic;
	unsigned (* ParseLineX) (unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf);
	unsigned flags;
//        unsigned tabsize;
	tchar_t opencomment[8];
	tchar_t closecomment[8];
	tchar_t commentline[8];
	unsigned encoding;
};

extern std::array<TextDefinition, static_cast<size_t>(SRC_MAX_ENTRY)> m_SourceDefs;

bool IsXKeyword(const tchar_t *pszKey, size_t nKeyLen, const tchar_t *pszKeywordList[], size_t nKeywordListCount, int(*compare)(const tchar_t *, const tchar_t *, size_t));
bool IsXNumber(const tchar_t* pszChars, int nLength);
bool IsHtmlKeyword(const tchar_t *pszChars, int nLength);
bool IsHtmlUser1Keyword(const tchar_t *pszChars, int nLength);
bool IsHtmlUser2Keyword(const tchar_t *pszChars, int nLength);

TextDefinition *GetTextType(const tchar_t *pszExt);
TextDefinition* GetTextType(int index);
void SetExtension(int index, const tchar_t *pszExts);

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
unsigned ParseLineHtmlEx(unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf, TextType nEmbeddedLanguage);
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
