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
    _T ("asr"),
    _T ("assert"),
    _T ("async"),
    _T ("base"),
    _T ("begin"),
    _T ("bool"),
    _T ("break"),
    _T ("byte"),
    _T ("char"),
    _T ("checked"),
    _T ("class"),
    _T ("component"),
    _T ("const"),
    _T ("const"),
    _T ("constraint"),
    _T ("continue"),
    _T ("decimal"),
    _T ("default"),
    _T ("delegate"),
    _T ("do"),
    _T ("done"),
    _T ("double"),
    _T ("downcast"),
    _T ("downto"),
    _T ("elif"),
    _T ("else"),
    _T ("end"),
    _T ("event"),
    _T ("exception"),
    _T ("extern"),
    _T ("external"),
    _T ("false"),
    _T ("finally"),
    _T ("fixed"),
    _T ("float"),
    _T ("float32"),
    _T ("for"),
    _T ("fun"),
    _T ("function"),
    _T ("global"),
    _T ("if"),
    _T ("in"),
    _T ("include"),
    _T ("inherit"),
    _T ("int"),
    _T ("int16"),
    _T ("int64"),
    _T ("interface"),
    _T ("internal"),
    _T ("land"),
    _T ("lazy"),
    _T ("let"),
    _T ("let!"),
    _T ("lor"),
    _T ("lsl"),
    _T ("lsr"),
    _T ("match"),
    _T ("match!"),
    _T ("member"),
    _T ("mixin"),
    _T ("mod"),
    _T ("module"),
    _T ("mutable"),
    _T ("namespace"),
    _T ("nativeint"),
    _T ("new"),
    _T ("not"),
    _T ("null"),
    _T ("of"),
    _T ("open"),
    _T ("or"),
    _T ("override"),
    _T ("parallel"),
    _T ("private"),
    _T ("process"),
    _T ("protected"),
    _T ("public"),
    _T ("pure"),
    _T ("query"),
    _T ("rec"),
    _T ("return"),
    _T ("return!"),
    _T ("sbyte"),
    _T ("sealed"),
    _T ("select"),
    _T ("seq"),
    _T ("sig"),
    _T ("single"),
    _T ("static"),
    _T ("string"),
    _T ("struct"),
    _T ("tailcall"),
    _T ("task"),
    _T ("then"),
    _T ("to"),
    _T ("trait"),
    _T ("true"),
    _T ("try"),
    _T ("type"),
    _T ("uint"),
    _T ("uint16"),
    _T ("uint64"),
    _T ("unativeint"),
    _T ("unit"),
    _T ("upcast"),
    _T ("use"),
    _T ("use!"),
    _T ("val"),
    _T ("virtual"),
    _T ("void"),
    _T ("when"),
    _T ("where"),
    _T ("while"),
    _T ("with"),
    _T ("yield"),
    _T ("yield!"),
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
