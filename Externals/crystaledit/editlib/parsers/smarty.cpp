///////////////////////////////////////////////////////////////////////////
//  File:    smarty.cpp
//  Version: 1.0.0.0
//  Updated: 11-Jun-2021
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Smarty syntax highlighing definition
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

// See https://www.smarty.net/docs/en/

// Chapter7
// Built-in Functions
static const tchar_t * s_apszBuiltInFunctionList[] =
  {
    _T ("append"),
    _T ("as"),            // This is used with "foreach".
    _T ("assign"),
    _T ("block"),
    _T ("break"),
    _T ("call"),
    _T ("capture"),
    _T ("config_load"),
    _T ("continue"),
    _T ("debug"),
    _T ("else"),
    _T ("elseif"),
    _T ("extends"),
    _T ("for"),
    _T ("foreach"),
    _T ("foreachelse"),
    _T ("forelse"),
    _T ("function"),
    _T ("if"),
    _T ("include"),
    _T ("include_php"),
    _T ("insert"),
    _T ("ldelim"),
    _T ("literal"),
    _T ("nocache"),
    _T ("php"),
    _T ("rdelim"),
    _T ("section"),
    _T ("sectionelse"),
    _T ("setfilter"),
    _T ("step"),      // This is used with "for".
    _T ("strip"),
    _T ("to"),        // This is used with "for".
    _T ("while"),
  };

// Operators
static const tchar_t * s_apszOperatorList[] =
  {
    _T ("by"),
    _T ("div"),
    _T ("eq"),
    _T ("even"),
    _T ("ge"),
    _T ("gt"),
    _T ("gte"),
    _T ("is"),
    _T ("le"),
    _T ("lt"),
    _T ("lte"),
    _T ("mod"),
    _T ("ne"),
    _T ("neq"),
    _T ("not"),
    _T ("odd"),
  };

// Chapter8
// Custom Functions
static const tchar_t * s_apszCustomFunctionList[] =
  {
    _T ("counter"),
    _T ("cycle"),
    _T ("eval"),
    _T ("fetch"),
    _T ("html_checkboxes"),
    _T ("html_image"),
    _T ("html_options"),
    _T ("html_radios"),
    _T ("html_select_date"),
    _T ("html_select_time"),
    _T ("html_table"),
    _T ("mailto"),
    _T ("math"),
    _T ("textformat"),
  };

// Chapter5
// Variable Modifiers
static const tchar_t * s_apszVariableModifierList[] =
  {
    _T ("capitalize"),
    _T ("cat"),
    _T ("count_characters"),
    _T ("count_paragraphs"),
    _T ("count_sentences"),
    _T ("count_words"),
    _T ("date_format"),
    _T ("default"),
    _T ("escape"),
    _T ("from_charset"),
    _T ("indent"),
    _T ("lower"),
    _T ("nl2br"),
    _T ("regex_replace"),
    _T ("replace"),
    _T ("spacify"),
    _T ("string_format"),
//  _T ("strip"),             // This is also defined as a build-in function, so comment it out.
    _T ("strip_tags"),
    _T ("to_charset"),
    _T ("truncate"),
    _T ("unescape"),
    _T ("upper"),
    _T ("wordwrap"),
  };


static bool
IsSmartyKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszBuiltInFunctionList, pszChars, nLength);
}

static bool
IsOperatorKeyword(const tchar_t* pszChars, int nLength)
{
    return ISXKEYWORDI(s_apszOperatorList, pszChars, nLength);
}

static bool
IsUser1Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszCustomFunctionList, pszChars, nLength);
}

static bool
IsUser2Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszVariableModifierList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineSmarty(unsigned dwCookie, const tchar_t* pszChars, int nLength, TEXTBLOCK* pBuf, int& nActualItems)
{
  return ParseLineHtmlEx(dwCookie, pszChars, nLength, pBuf, nActualItems, SRC_SMARTY);
}

unsigned
CrystalLineParser::ParseLineSmartyLanguage (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_CHAR);

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
          else if (dwCookie & COOKIE_USER1)         // Config???
            {
              DEFINE_BLOCK(nPos, COLORINDEX_USER1);
            }
          else if (dwCookie & COOKIE_PREPROCESSOR)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_PREPROCESSOR);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.')
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

      //  Variables loaded from config files #....#
      if (dwCookie & COOKIE_USER1)
        {
          if (pszChars[I] == '#' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_USER1;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Comment
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '}' && pszChars[nPrevI] == '*'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
              pszCommentEnd = pszChars + I + 1;
            }
          continue;
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

      if (pszChars[I] == '#')
        {
          DEFINE_BLOCK(I, COLORINDEX_USER1);
          dwCookie |= COOKIE_USER1;
          continue;
        }

      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '{'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          continue;
        }

      if (pBuf == nullptr)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (dwCookie & COOKIE_USER2)
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
              if (IsSmartyKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                }
              else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
                }
              else if (IsOperatorKeyword(pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK(nIdentBegin, COLORINDEX_OPERATOR);
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

          //  Preprocessor start: $
          if (pszChars[I] == '$')
            {
              dwCookie |= COOKIE_USER2;
              nIdentBegin = -1;
              continue;
            }

          //  Preprocessor end: ...
          if (dwCookie & COOKIE_USER2)
            {
              if (!xisalnum (pszChars[I]))
                {
                  dwCookie &= ~COOKIE_USER2;
                  nIdentBegin = -1;
                  continue;
                }
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsSmartyKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
        }
      else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
        }
      else if (IsOperatorKeyword(pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK(nIdentBegin, COLORINDEX_OPERATOR);
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

  dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_CHAR);
  return dwCookie;
}
