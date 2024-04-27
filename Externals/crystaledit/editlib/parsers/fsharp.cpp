///////////////////////////////////////////////////////////////////////////
//  File:       fsharp.cpp
//  Version:    1.0.0
//  Created:    27-Apr-2024
//
//  Copyright:  Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalEditView class, a part of the Crystal Edit -
//  syntax coloring text editor. Based on C# version.
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

//  F# keywords 
// https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/keyword-reference
// and https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/basic-types
static const tchar_t * s_apszFsKeywordList[] =
  {
    _T ("abstract"),
    _T ("and"),
    _T ("as"),
    _T ("assert"),
    _T ("base"),
    _T ("begin"),
    _T ("class"),
    _T ("default"),
    _T ("delegate"),
    _T ("do"),
    _T ("done"),
    _T ("downcast"),
    _T ("downto"),
    _T ("elif"),
    _T ("else"),
    _T ("end"),
    _T ("exception"),
    _T ("extern"),
    _T ("false"),
    _T ("finally"),
    _T ("fixed"),
    _T ("for"),
    _T ("fun"),
    _T ("function"),
    _T ("global"),
    _T ("if"),
    _T ("in"),
    _T ("inherit"),
    _T ("interface"),
    _T ("internal"),
    _T ("lazy"),
    _T ("let"),
    _T ("let!"),
    _T ("match"),
    _T ("match!"),
    _T ("member"),
    _T ("module"),
    _T ("mutable"),
    _T ("namespace"),
    _T ("new"),
    _T ("not"),
    _T ("null"),
    _T ("of"),
    _T ("open"),
    _T ("or"),
    _T ("override"),
    _T ("private"),
    _T ("public"),
    _T ("rec"),
    _T ("return"),
    _T ("return!"),
    _T ("select"),
    _T ("static"),
    _T ("struct"),
    _T ("then"),
    _T ("to"),
    _T ("true"),
    _T ("try"),
    _T ("type"),
    _T ("upcast"),
    _T ("use"),
    _T ("use!"),
    _T ("val"),
    _T ("void"),
    _T ("when"),
    _T ("while"),
    _T ("with"),
    _T ("yield"),
    _T ("yield!"),
    _T ("const"),
    _T ("asr"),
    _T ("land"),
    _T ("lor"),
    _T ("lsl"),
    _T ("lsr"),
    _T ("mod"),
    _T ("sig"),
    _T ("break"),
    _T ("checked"),
    _T ("component"),
    _T ("const"),
    _T ("constraint"),
    _T ("continue"),
    _T ("event"),
    _T ("external"),
    _T ("include"),
    _T ("mixin"),
    _T ("parallel"),
    _T ("process"),
    _T ("protected"),
    _T ("pure"),
    _T ("sealed"),
    _T ("tailcall"),
    _T ("trait"),
    _T ("virtual"),
    _T ("bool"),
    _T ("byte"),
    _T ("sbyte"),
    _T ("int16"),
    _T ("uint16"),
    _T ("int"),
    _T ("uint"),
    _T ("int64"),
    _T ("uint64"),
    _T ("nativeint"),
    _T ("unativeint"),
    _T ("decimal"),
    _T ("float"),
    _T ("double"),
    _T ("float32"),
    _T ("single"),
    _T ("char"),
    _T ("string"),
    _T ("unit"),
    _T ("query"),
    _T ("seq"),
    _T ("task"),
    _T ("async"),
    _T ("where"),
  };


static bool
IsFsKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszFsKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineFSharp (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  return ParseLineCJava (dwCookie, pszChars, nLength, pBuf, nActualItems, IsFsKeyword, nullptr);
}
