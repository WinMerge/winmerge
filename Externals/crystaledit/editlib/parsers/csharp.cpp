///////////////////////////////////////////////////////////////////////////
//  File:       cplusplus.cpp
//  Version:    1.2.0.5
//  Created:    29-Dec-1998
//
//  Copyright:  Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalEditView class, a part of the Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  16-Aug-99
//      Ferdinand Prantl:
//  +   FEATURE: corrected bug in syntax highlighting C comments
//  +   FEATURE: extended levels 1- 4 of keywords in some languages
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  C# keywords
static const tchar_t * s_apszCppKeywordList[] =
  {
    _T ("abstract"),
    _T ("base"),
    _T ("bool"),
    _T ("break"),
    _T ("byte"),
    _T ("case"),
    _T ("catch"),
    _T ("char"),
    _T ("checked"),
    _T ("class"),
    _T ("const"),
    _T ("continue"),
    _T ("decimal"),
    _T ("default"),
    _T ("delegate"),
    _T ("do"),
    _T ("double"),
    _T ("else"),
    _T ("enum"),
    _T ("event"),
    _T ("exdouble"),
    _T ("exfloat"),
    _T ("explicit"),
    _T ("extern"),
    _T ("false"),
    _T ("finally"),
    _T ("fixed"),
    _T ("float"),
    _T ("for"),
    _T ("foreach"),
    _T ("get"),
    _T ("goto"),
    _T ("if"),
    _T ("implicit"),
    _T ("in"),
    _T ("int"),
    _T ("interface"),
    _T ("internal"),
    _T ("is"),
    _T ("lock"),
    _T ("long"),
    _T ("nameof"),
    _T ("namespace"),
    _T ("new"),
    _T ("null"),
    _T ("object"),
    _T ("operator"),
    _T ("out"),
    _T ("override"),
    _T ("private"),
    _T ("protected"),
    _T ("public"),
    _T ("readonly"),
    _T ("ref"),
    _T ("return"),
    _T ("sbyte"),
    _T ("sealed"),
    _T ("set"),
    _T ("short"),
    _T ("sizeof"),
    _T ("static"),
    _T ("string"),
    _T ("struct"),
    _T ("switch"),
    _T ("this"),
    _T ("throw"),
    _T ("true"),
    _T ("try"),
    _T ("typeof"),
    _T ("uint"),
    _T ("ulong"),
    _T ("unchecked"),
    _T ("unsafe"),
    _T ("ushort"),
    _T ("using"),
    _T ("virtual"),
    _T ("void"),
    _T ("when"),
    _T ("while"),
  };


static bool
IsCppKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszCppKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineCSharp (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  return ParseLineCJava (dwCookie, pszChars, nLength, pBuf, nActualItems, IsCppKeyword, nullptr);
}
