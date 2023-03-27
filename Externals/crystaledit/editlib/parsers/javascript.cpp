///////////////////////////////////////////////////////////////////////////
//  File:    javascript.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  JavaScript syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  JavaScript keywords
static const tchar_t * s_apszJavaScriptKeywordList[] =
  {
    _T ("async"),
    _T ("await"),
    _T ("break"),
    _T ("case"),
    _T ("catch"),
    _T ("class"),
    _T ("const"),
    _T ("constructor"),
    _T ("continue"),
    _T ("debugger"),
    _T ("default"),
    _T ("delete"),
    _T ("do"),
    _T ("each"),
    _T ("enum"),
    _T ("else"),
    _T ("export"),
    _T ("extends"),
    _T ("false"),
    _T ("finally"),
    _T ("for"),
    _T ("function"),
    _T ("if"),
    _T ("implements"),
    _T ("import"),
    _T ("in"),
    _T ("instanceof"),
    _T ("interface"),
    _T ("let"),
    _T ("new"),
    _T ("null"),
    _T ("of"),
    _T ("package"),
    _T ("protected"),
    _T ("public"),
    _T ("resolve"),
    _T ("return"),
    _T ("static"),
    _T ("super"),
    _T ("switch"),
    _T ("target"),
    _T ("this"),
    _T ("throw"),
    _T ("true"),
    _T ("try"),
    _T ("typeof"),
    _T ("undefined"),
    _T ("var"),
    _T ("void"),
    _T ("while"),
    _T ("with"),
    _T ("yield")
  };

static bool
IsJavaScriptKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszJavaScriptKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineJavaScript (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  return ParseLineCJava (dwCookie, pszChars, nLength, pBuf, nActualItems, IsJavaScriptKeyword, nullptr);
}
