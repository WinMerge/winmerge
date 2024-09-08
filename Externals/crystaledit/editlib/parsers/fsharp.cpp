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

  static const tchar_t* s_apszUser1KeywordList[] =
  {
    _T("__SOURCE_DIRECTORY__"),
    _T("Array"),
    _T("Async"),
    _T("Char"),
    _T("DateTime"),
    _T("Decimal"),
    _T("Guid"),
    _T("Int16"),
    _T("Int32"),
    _T("Int64"),
    _T("IntPtr"),
    _T("List"),
    _T("Map"),
    _T("None"),
    _T("Option"),
    _T("Seq"),
    _T("Some"),
    _T("String"),
    _T("Task"),
    _T("Type"),
    _T("UInt16"),
    _T("UInt32"),
    _T("UInt64"),
    _T("UIntPtr"),
    _T("Unit"),
    _T("ValueNone"),
    _T("ValueOption"),
    _T("ValueSome"),
  };

static bool
IsFsKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszFsKeywordList, pszChars, nLength);
}

static bool
IsUserKeyword(const tchar_t* pszChars, int nLength)
{
    return ISXKEYWORD(s_apszUser1KeywordList, pszChars, nLength);
}

static inline void
DefineIdentiferBlock(const tchar_t *pszChars, int nLength, CrystalLineParser::TEXTBLOCK * pBuf, int &nActualItems, int nIdentBegin, int I)
{
    if (IsFsKeyword(pszChars + nIdentBegin, I - nIdentBegin))
    {
        DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
    }
    else if (IsUserKeyword(pszChars + nIdentBegin, I - nIdentBegin))
    {
        DEFINE_BLOCK(nIdentBegin, COLORINDEX_USER1);
    }
    else if (CrystalLineParser::IsXNumber(pszChars + nIdentBegin, I - nIdentBegin))
    {
        DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
    }
    else
    {
        // Todo: F# parenthesis are often optional, so functions are not usually this easy to detect!
        bool bFunction = false;

        for (int j = I; j < nLength; j++)
        {
            if (!xisspace(pszChars[j]))
            {
                if (pszChars[j] == '(')
                {
                    bFunction = true;
                }
                break;
            }
        }
        if (bFunction)
        {
            DEFINE_BLOCK(nIdentBegin, COLORINDEX_FUNCNAME);
        }
    }
}

unsigned
CrystalLineParser::ParseLineFSharp (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
    if (nLength == 0)
        return dwCookie & (COOKIE_EXT_COMMENT | COOKIE_RAWSTRING);

    bool bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT & ~COOKIE_RAWSTRING) == 0;
    const tchar_t* pszCommentBegin = nullptr;
    const tchar_t* pszCommentEnd = nullptr;
    const tchar_t* pszTextBegin = nullptr;
    const tchar_t* pszTextEnd = nullptr;
    bool bRedefineBlock = true;
    bool bDecIndex = false;
    int nIdentBegin = -1;
    int nPrevI = -1;
    int nPrevII = -2;
    int I = 0;
    for (I = 0;; nPrevII=nPrevI, nPrevI = I, I = static_cast<int>(tc::tcharnext(pszChars + I) - pszChars))
    {
        if (I == nPrevI)
        {
            // CharNext did not advance, so we're at the end of the string
            // and we already handled this character, so stop
            break;
        }

        if (bRedefineBlock)
        {
            int nPos = I;
            if (bDecIndex)
                nPos = nPrevI;
            if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
                DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
            }
            else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING | COOKIE_RAWSTRING))
            {
                DEFINE_BLOCK(nPos, COLORINDEX_STRING);
            }
            else if (dwCookie & COOKIE_PREPROCESSOR)
            {
                DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
            }
            else
            {
                if (xisalnum(pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha(*tc::tcharprev(pszChars, pszChars + nPos)) && !xisalpha(*tc::tcharnext(pszChars + nPos))))
                {
                    DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
                }
                else
                {
                    DEFINE_BLOCK(nPos, COLORINDEX_OPERATOR);
                    bRedefineBlock = true;
                    bDecIndex = true;
                    goto out;
                }
            }
            bRedefineBlock = false;
            bDecIndex = false;
        }
    out:

        // Can be bigger than length if there is binary data
        // See bug #1474782 Crash when comparing SQL with with binary data
        if (I >= nLength || pszChars[I] == 0)
            break;

        if (dwCookie & COOKIE_COMMENT)
        {
            DEFINE_BLOCK(I, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_COMMENT;
            break;
        }

        //  String constant "...."
        if (dwCookie & COOKIE_STRING)
        {
            if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '\\')))
            {
                dwCookie &= ~COOKIE_STRING;
                bRedefineBlock = true;
            }
            continue;
        }

        //  Char constant '..'
        if (dwCookie & COOKIE_CHAR)
        {
            if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '\\')))
            {
                dwCookie &= ~COOKIE_CHAR;
                bRedefineBlock = true;
            }
            continue;
        }

        //  Extended comment /*....*/
        if (dwCookie & COOKIE_EXT_COMMENT)
        {
            if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == ')' && pszChars[nPrevI] == '*'))
            {
                dwCookie &= ~COOKIE_EXT_COMMENT;
                bRedefineBlock = true;
                pszCommentEnd = pszChars + I + 1;
            }
            continue;
        }

        if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '/'))
        {
            DEFINE_BLOCK(nPrevI, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_COMMENT;
            break;
        }

        //  Multi-line string constant """...."""
        if (dwCookie & COOKIE_RAWSTRING)
        {
            if ((pszTextBegin < pszChars + I) && (I > 1 && pszChars[I] == '"' && pszChars[nPrevI] == '"' && pszChars[nPrevII] == '"'))
            {
                dwCookie &= ~COOKIE_RAWSTRING;
                bRedefineBlock = true;
                pszTextEnd = pszChars + I + 2;
            }
            continue;
        }

        if ((pszTextEnd < pszChars + I) && (I > 1 && pszChars[I] == '"' && pszChars[nPrevI] == '"' && pszChars[nPrevII] == '"'))
        {
            DEFINE_BLOCK(nPrevII, COLORINDEX_STRING);
            DEFINE_BLOCK(I, COLORINDEX_STRING);
            dwCookie |= COOKIE_RAWSTRING;
            continue;
        }

        //  Preprocessor directive #....
        if (dwCookie & COOKIE_PREPROCESSOR)
        {
            if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == ')'))
            {
                DEFINE_BLOCK(nPrevI, COLORINDEX_COMMENT);
                dwCookie |= COOKIE_EXT_COMMENT;
            }
            if ((pszTextEnd < pszChars + I) && (I > 1 && pszChars[I] == '"' && pszChars[nPrevI] == '"' && pszChars[nPrevII] == '"'))
            {
                DEFINE_BLOCK(nPrevII, COLORINDEX_STRING);
                dwCookie |= COOKIE_RAWSTRING;
                break;
            }
            continue;
        }

        //  Normal text
        if (pszChars[I] == '"' && (I < 2 || pszChars[nPrevI] != '"' || pszChars[nPrevII] != '"'))
        {
            DEFINE_BLOCK(I, COLORINDEX_STRING);
            dwCookie |= COOKIE_STRING;
            continue;
        }
        if (pszChars[I] == '\'')
        {
            // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
            if (!I || !xisalnum(pszChars[nPrevI]))
            {
                DEFINE_BLOCK(I, COLORINDEX_STRING);
                dwCookie |= COOKIE_CHAR;
                continue;
            }
        }
        if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '('))
        {
            DEFINE_BLOCK(nPrevI, COLORINDEX_COMMENT);
            dwCookie |= COOKIE_EXT_COMMENT;
            pszCommentBegin = pszChars + I + 1;
            continue;
        }
        if ((pszTextEnd < pszChars + I) && (I > 1 && pszChars[I] == '"' && pszChars[nPrevI] == '"' && pszChars[nPrevII] == '"'))
        {
            DEFINE_BLOCK(nPrevII, COLORINDEX_STRING);
            dwCookie |= COOKIE_RAWSTRING;
            pszTextBegin = pszChars + I + 1;
            continue;
        }

        if (bFirstChar)
        {
            if (pszChars[I] == '#')
            {
                DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
                dwCookie |= COOKIE_PREPROCESSOR;
                continue;
            }
            if (!xisspace(pszChars[I]))
                bFirstChar = false;
        }

        if (pBuf == nullptr)
            continue;               //  We don't need to extract keywords,
        //  for faster parsing skip the rest of loop

        if (xisalnum(pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha(pszChars[nPrevI]) && !xisalpha(pszChars[I + 1])))
        {
            if (nIdentBegin == -1)
                nIdentBegin = I;
        }
        else
        {
            if (nIdentBegin >= 0)
            {
                DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);
                bRedefineBlock = true;
                bDecIndex = true;
                nIdentBegin = -1;
            }
        }
    }

    if (nIdentBegin >= 0)
    {
        DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);
    }

    if (pszChars[nLength - 1] != '\\' || IsMBSTrail(pszChars, nLength - 1))
        dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_RAWSTRING);

    return dwCookie;
}
