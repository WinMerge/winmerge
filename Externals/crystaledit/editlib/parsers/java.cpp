///////////////////////////////////////////////////////////////////////////
//  File:    java.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Java syntax highlighing definition
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

//  Java keywords
static const tchar_t * s_apszJavaKeywordList[] =
  {
    _T ("abstract"),
    _T ("boolean"),
    _T ("break"),
    _T ("byte"),
    _T ("byvalue"),
    _T ("case"),
    _T ("catch"),
    _T ("char"),
    _T ("class"),
    _T ("const"),
    _T ("continue"),
    _T ("default"),
    _T ("do"),
    _T ("double"),
    _T ("else"),
    _T ("extends"),
    _T ("false"),
    _T ("final"),
    _T ("finally"),
    _T ("float"),
    _T ("for"),
    _T ("goto"),
    _T ("if"),
    _T ("implements"),
    _T ("import"),
    _T ("instanceof"),
    _T ("int"),
    _T ("interface"),
    _T ("long"),
    _T ("native"),
    _T ("new"),
    _T ("null"),
    _T ("package"),
    _T ("private"),
    _T ("protected"),
    _T ("public"),
    _T ("return"),
    _T ("short"),
    _T ("static"),
    _T ("super"),
    _T ("switch"),
    _T ("synchronized"),
    _T ("this"),
    _T ("threadsafe"),
    _T ("throw"),
    _T ("transient"),
    _T ("true"),
    _T ("try"),
    _T ("void"),
    _T ("while"),
  };

static bool
IsJavaKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszJavaKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineJava (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  return ParseLineCJava (dwCookie, pszChars, nLength, pBuf, nActualItems, IsJavaKeyword, nullptr);
}
