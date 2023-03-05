///////////////////////////////////////////////////////////////////////////
//  File:    python.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Python syntax highlighing definition
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

//  Python 2.6 keywords
static const tchar_t * s_apszPythonKeywordList[] =
  {
    _T ("and"),
    _T ("as"),
    _T ("assert"),
    _T ("break"),
    _T ("class"),
    _T ("continue"),
    _T ("def"),
    _T ("del"),
    _T ("elif"),
    _T ("else"),
    _T ("except"),
    _T ("exec"),
    _T ("finally"),
    _T ("for"),
    _T ("from"),
    _T ("global"),
    _T ("if"),
    _T ("import"),
    _T ("in"),
    _T ("is"),
    _T ("lambda"),
    _T ("not"),
    _T ("or"),
    _T ("pass"),
    _T ("print"),
    _T ("raise"),
    _T ("return"),
    _T ("try"),
    _T ("while"),
    _T ("whith"),
    _T ("yield"),
  };

static const tchar_t * s_apszUser1KeywordList[] =
  {
    _T ("AttributeError"),
    _T ("EOFError"),
    _T ("Ellipsis"),
    _T ("False"),
    _T ("IOError"),
    _T ("ImportError"),
    _T ("IndexError"),
    _T ("KeyError"),
    _T ("KeyboardInterrupt"),
    _T ("MemoryError"),
    _T ("NameError"),
    _T ("None"),
    _T ("NotImplemented"),
    _T ("OverflowError"),
    _T ("RuntimeError"),
    _T ("SyntaxError"),
    _T ("SystemError"),
    _T ("SystemExit"),
    _T ("True"),
    _T ("TypeError"),
    _T ("ValueError"),
    _T ("ZeroDivisionError"),
    _T ("__debug__"),
    _T ("argv"),
    _T ("builtin_module_names"),
    _T ("exc_traceback"),
    _T ("exc_type"),
    _T ("exc_value"),
    _T ("exit"),
    _T ("exitfunc"),
    _T ("last_traceback"),
    _T ("last_type"),
    _T ("last_value"),
    _T ("modules"),
    _T ("path"),
    _T ("ps1"),
    _T ("ps2"),
    _T ("setprofile"),
    _T ("settrace"),
    _T ("stderr"),
    _T ("stdin"),
    _T ("stdout"),
    _T ("tracebacklimit"),
  };

static const tchar_t * s_apszUser2KeywordList[] =
  {
    _T ("__abs__"),
    _T ("__add__"),
    _T ("__and__"),
    _T ("__bases__"),
    _T ("__class__"),
    _T ("__cmp__"),
    _T ("__coerce__"),
    _T ("__del__"),
    _T ("__dict__"),
    _T ("__div__"),
    _T ("__divmod__"),
    _T ("__float__"),
    _T ("__float__"),
    _T ("__getitem__"),
    _T ("__hash__"),
    _T ("__hex__"),
    _T ("__init__"),
    _T ("__int__"),
    _T ("__invert__"),
    _T ("__len__"),
    _T ("__long__"),
    _T ("__lshift__"),
    _T ("__members__"),
    _T ("__methods__"),
    _T ("__mod__"),
    _T ("__mul__"),
    _T ("__neg__"),
    _T ("__nonzero__"),
    _T ("__oct__"),
    _T ("__or__"),
    _T ("__pos__"),
    _T ("__pow__"),
    _T ("__repr__"),
    _T ("__rshift__"),
    _T ("__str__"),
    _T ("__sub__"),
    _T ("__xor__"),
    _T ("abs"),
    _T ("coerce"),
    _T ("divmod"),
    _T ("float"),
    _T ("float"),
    _T ("hex"),
    _T ("id"),
    _T ("int"),
    _T ("len"),
    _T ("long"),
    _T ("nonzero"),
    _T ("oct"),
    _T ("pow"),
    _T ("range"),
    _T ("round"),
    _T ("xrange"),
  };

static bool
IsPythonKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszPythonKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszUser1KeywordList, pszChars, nLength);
}

static bool
IsUser2Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszUser2KeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLinePython (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
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
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
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
              if (IsPythonKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
              else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
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
      if (IsPythonKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
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
