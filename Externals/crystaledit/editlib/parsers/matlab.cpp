///////////////////////////////////////////////////////////////////////////
//  File:       matlab.cpp
//  Version:    1.0.0.0
//  Created:    18-Mar-2023
//
//  Copyright:  wiera987 @GitHub
//  E-mail:     
//
//  MATLAB language syntax highlighing definition
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

//  MATLAB keywords
static const tchar_t * s_apszMatlabKeywordList[] =
  {
    _T("arguments"),
    _T("break"),
    _T("case"),
    _T("catch"),
    _T("classdef"),
    _T("continue"),
    _T("else"),
    _T("elseif"),
    _T("end"),
    _T("enumeration"),
    _T("events"),
    _T("for"),
    _T("function"),
    _T("global"),
    _T("if"),
    _T("methods"),
    _T("otherwise"),
    _T("parfor"),
    _T("persistent"),
    _T("properties"),
    _T("return"),
    _T("spmd"),
    _T("switch"),
    _T("try"),
    _T("while"),
  };

static const tchar_t * s_apszUser1KeywordList[] =
  {
    _T("AbortSet"),
    _T("Abstract"),
    _T("Access"),
    _T("Constant"),
    _T("Dependent"),
    _T("GetAccess"),
    _T("GetObservable"),
    _T("Hidden"),
    _T("NonCopyable"),
    _T("PartialMatchPriority"),
    _T("SetAccess"),
    _T("SetObservable"),
    _T("Static"),
    _T("Transient"),
  };

static bool
IsMatlabKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszMatlabKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszUser1KeywordList, pszChars, nLength);
}

static inline void
DefineIdentiferBlock(const tchar_t *pszChars, int nLength, CrystalLineParser::TEXTBLOCK * pBuf, int &nActualItems, int nIdentBegin, int I)
{
  if (IsMatlabKeyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
    }
  if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
    }
  else if (CrystalLineParser::IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
    }
  else
    {
      bool bFunction = false;

      for (int j = I; j < nLength; j++)
        {
        if (!xisspace (pszChars[j]))
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
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
        }
    }
}

unsigned
CrystalLineParser::ParseLineMatlab (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT | 0xFF000000);

  const tchar_t *pszCommentBegin = nullptr;
  const tchar_t *pszCommentEnd = nullptr;
  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = static_cast<int>(tc::tcharnext(pszChars+I) - pszChars))
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
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*tc::tcharprev(pszChars, pszChars + nPos)) && !xisalpha (*tc::tcharnext(pszChars + nPos))))
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
                }
              else
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_OPERATOR);
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
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
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

      //  Extended comment /*....*/ (nested comments)
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          const int depth = COOKIE_GET_EXT_COMMENT_DEPTH(dwCookie);
          if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '{' && pszChars[nPrevI] == '%'))
            {
              COOKIE_SET_EXT_COMMENT_DEPTH(dwCookie, depth + 1);
              pszCommentBegin = pszChars + I + 1;
            }
          else if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '}' && pszChars[nPrevI] == '%'))
            {
              if (depth == 0)
                {
                  dwCookie &= ~COOKIE_EXT_COMMENT;
                  bRedefineBlock = true;
                }
              else
                {
                  COOKIE_SET_EXT_COMMENT_DEPTH(dwCookie, depth - 1);
                }
              pszCommentEnd = pszChars + I + 1;
            }
          continue;
        }

      // Comment other than %{
      if (pszChars[I] == '%')
        {
          if ((I == nLength - 1)
              || ((I + 1 < nLength) && (pszChars[I + 1] != '{'))
              || ((I + 2 < nLength) && (pszChars[I + 1] == '{') && !xisspace(pszChars[I + 2])))
            {
              DEFINE_BLOCK(I, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_COMMENT;
              break;
            }
        }
        // Comment after the ...
		if ((I+3 < nLength) && (pszChars[I] == '.') && (pszChars[I+1] == '.') && (pszChars[I+2] == '.'))
        {
          DEFINE_BLOCK (I+3, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Normal text
      if (pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }
      if (pszChars[I] == '\'')
      {
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum(pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }
      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '{' && pszChars[nPrevI] == '%'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          continue;
        }

      if (pBuf == nullptr)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha (pszChars[nPrevI]) && !xisalpha (pszChars[I + 1])))
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

  dwCookie &= COOKIE_EXT_COMMENT | 0xFF000000;
  return dwCookie;
}
