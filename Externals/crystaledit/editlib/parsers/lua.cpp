///////////////////////////////////////////////////////////////////////////
//  File:       lua.cpp
//  Version:    1.0.0.0
//  Created:    17-May-2019
//
//  Copyright:  Stcherbatchenko Andrei, portions by Takashi Sawanaka
//  E-mail:     sdottaka@users.sourceforge.net
//
//  Lua syntax highlighing definition
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
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  Lua keywords
static const tchar_t * s_apszLuaKeywordList[] =
  {
_T ("and"),
_T ("break"),
_T ("do"),
_T ("else"),
_T ("elseif"),
_T ("end"),
_T ("false"),
_T ("for"),
_T ("function"),
_T ("if"),
_T ("in"),
_T ("local"),
_T ("nil"),
_T ("not"),
_T ("or"),
_T ("repeat"),
_T ("return"),
_T ("then"),
_T ("true"),
_T ("until"),
_T ("while"),
  };

static bool
IsLuaKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszLuaKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineLua (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT | COOKIE_RAWSTRING | 0xFF000000);

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
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING | COOKIE_RAWSTRING))
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

      //  Raw string constant [[ ... ]], [=[ ... ]=], [==[ ... ]==], ...
      if (dwCookie & COOKIE_RAWSTRING)
        {
          const int nEqualsSignCount = COOKIE_GET_LUA_EQUALS_SIGN_COUNT(dwCookie);
          if (I >= nEqualsSignCount + 1 && pszChars[I] == ']' && pszChars[I - nEqualsSignCount - 1] == ']' && 
              std::all_of(pszChars + I - nEqualsSignCount, pszChars + I, [](const auto c) { return c == '='; }))
            {
              dwCookie &= ~COOKIE_RAWSTRING;
              COOKIE_SET_LUA_EQUALS_SIGN_COUNT(dwCookie, 0);
              bRedefineBlock = true;
            }
          continue;
        }

      //  Extended comment --[[ ... ]] , --[=[ ... ]=], --[==[ ... ]==], ...
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          const int nEqualsSignCount = COOKIE_GET_LUA_EQUALS_SIGN_COUNT(dwCookie);
          if (I >= nEqualsSignCount + 1 && pszChars[I] == ']' && pszChars[I - nEqualsSignCount - 1] == ']' && 
              std::all_of(pszChars + I - nEqualsSignCount, pszChars + I, [](const auto c) { return c == '='; }))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              COOKIE_SET_LUA_EQUALS_SIGN_COUNT(dwCookie, 0);
              bRedefineBlock = true;
            }
          continue;
        }

      if (I > 0 && pszChars[I] == '-' && pszChars[nPrevI] == '-')
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
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
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }
      // Raw string [[ ... ]], [=[ ... ]=], [==[ ... ]==], ...
      if (pszChars[I] == '[' && I + 1 < nLength && (pszChars[I + 1] == '[' || pszChars[I + 1] == '='))
        {
          int nEqualsSignCount = 0;
          while (I + 1 + nEqualsSignCount < nLength && pszChars[I + 1 + nEqualsSignCount] == '=')
            ++nEqualsSignCount;
          if (I + 1 + nEqualsSignCount < nLength && pszChars[I + 1 + nEqualsSignCount] == '[')
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_RAWSTRING;
              COOKIE_SET_LUA_EQUALS_SIGN_COUNT(dwCookie, nEqualsSignCount);
              continue;
            }
        }
      // Extended comment [[ ... ]], [=[ ... ]=], [==[ ... ]==], ...
      if (pszChars[I] == '-' && I + 3 < nLength && pszChars[I + 1] == '-' && pszChars[I + 2] == '[' && (pszChars[I + 3] == '[' || pszChars[I + 3] == '='))
        {
          int nEqualsSignCount = 0;
          while (I + 3 + nEqualsSignCount < nLength && pszChars[I + 3 + nEqualsSignCount] == '=')
            ++nEqualsSignCount;
          if (I + 3 + nEqualsSignCount < nLength && pszChars[I + 3 + nEqualsSignCount] == '[')
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_EXT_COMMENT;
              COOKIE_SET_LUA_EQUALS_SIGN_COUNT(dwCookie, nEqualsSignCount);
              continue;
            }
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
              if (IsLuaKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
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
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsLuaKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
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

  dwCookie &= COOKIE_EXT_COMMENT | COOKIE_RAWSTRING | 0xFF000000;
  return dwCookie;
}
