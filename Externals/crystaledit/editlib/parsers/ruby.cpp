///////////////////////////////////////////////////////////////////////////
//  File:    ruby.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  RUBY syntax highlighing definition
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

// Ruby reserved words.
static const tchar_t * s_apszRubyKeywordList[] =
  {
    _T ("BEGIN"),
    _T ("END"),
    _T ("alias"),
    _T ("alias_method"),
    _T ("and"),
    _T ("begin"),
    _T ("break"),
    _T ("case"),
    _T ("catch"),
    _T ("class"),
    _T ("def"),
    _T ("do"),
    _T ("else"),
    _T ("elsif"),
    _T ("end"),
    _T ("ensure"),
    _T ("false"),
    _T ("for"),
    _T ("if"),
    _T ("in"),
    _T ("lambda"),
    _T ("load"),
    _T ("module"),
    _T ("next"),
    _T ("nil"),
    _T ("not"),
    _T ("or"),
    _T ("proc"),
    _T ("raise"),
    _T ("redo"),
    _T ("remove_method"),
    _T ("require"),
    _T ("require_gem"),
    _T ("rescue"),
    _T ("retry"),
    _T ("return"),
    _T ("self"),
    _T ("super"),
    _T ("then"),
    _T ("throw"),
    _T ("true"),
    _T ("undef"),
    _T ("undef_method"),
    _T ("unless"),
    _T ("until"),
    _T ("when"),
    _T ("while"),
    _T ("yield"),
  };

// Ruby constants (preprocessor color).
static const tchar_t * s_apszRubyConstantsList[] =
  {
    _T ("$defout"),
    _T ("$deferr"),
    _T ("$stderr"),
    _T ("$stdin"),
    _T ("$stdout"),
    _T ("$DEBUG"),
    _T ("$FILENAME"),
    _T ("$LOAD_PATH"),
    _T ("$SAFE"),
    _T ("$VERBOSE"),
    _T ("__FILE__"),
    _T ("__LINE__"),
    _T ("ARGF"),
    _T ("ARGV"),
    _T ("ENV"),
    _T ("DATA"),
    _T ("FALSE"),
    _T ("NIL"),
    _T ("RUBY_PLATFORM"),
    _T ("RUBY_RELEASE_DATE"),
    _T ("RUBY_VERSION"),
    _T ("STDERR"),
    _T ("STDIN"),
    _T ("STDOUT"),
    _T ("SCRIPT_LINES__"),
    _T ("TOPLEVEL_BINDING"),
    _T ("TRUE"),
  };

static bool
IsRubyKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszRubyKeywordList, pszChars, nLength);
}

static bool
IsRubyConstant (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszRubyConstantsList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineRuby(unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

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
          else if (dwCookie & COOKIE_VARIABLE)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_USER1);
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

      if (pszChars[I] == '#')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
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
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

	  // Variable begins
      if (pszChars[I] == '$' || pszChars[I] == '@')
        {
		  DEFINE_BLOCK (I, COLORINDEX_USER1);
          dwCookie |= COOKIE_VARIABLE;
          continue;
        }

      // Variable ends
      if (dwCookie & COOKIE_VARIABLE)
        {
          if (!xisalnum (pszChars[I]))
            {
              dwCookie &= ~COOKIE_VARIABLE;
              bRedefineBlock = true;
              bDecIndex = true;
            }
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
              if (IsRubyKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsRubyConstant (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_PREPROCESSOR);
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
      if (IsRubyKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsRubyConstant (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_PREPROCESSOR);
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

  if (pszChars[nLength - 1] != '\\' || IsMBSTrail(pszChars, nLength - 1))
    dwCookie &= COOKIE_EXT_COMMENT;
  
  return dwCookie;
}
