///////////////////////////////////////////////////////////////////////////
//  File:    html.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  HTML syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned
CrystalLineParser::ParseLineHtmlEx (unsigned dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems, int nEmbeddedLanguage)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT|COOKIE_EXT_USER1|COOKIE_ELEMENT|COOKIE_BLOCK_SCRIPT|COOKIE_BLOCK_STYLE|COOKIE_EXT_DEFINITION|COOKIE_EXT_VALUE);

  bool bRedefineBlock = true;
  if (!(dwCookie & COOKIE_ELEMENT))
    bRedefineBlock = !(dwCookie & (COOKIE_EXT_USER1|COOKIE_BLOCK_SCRIPT|COOKIE_BLOCK_STYLE));
  bool bDecIndex = false;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = static_cast<int>(::CharNext(pszChars+I) - pszChars))
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
          if (dwCookie & COOKIE_EXT_COMMENT)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else if (dwCookie & COOKIE_ELEMENT)
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

      if (!(dwCookie & COOKIE_ELEMENT) && (dwCookie & (COOKIE_EXT_USER1|COOKIE_BLOCK_SCRIPT|COOKIE_BLOCK_STYLE)))
        {
          if (dwCookie & COOKIE_BLOCK_SCRIPT)
            {
              const TCHAR *pszEnd = _tcsstr(pszChars + I, _T("</script>"));
              int nextI = pszEnd ? static_cast<int>(pszEnd - pszChars) : nLength;
              dwCookie = ParseLineJava(dwCookie & ~COOKIE_BLOCK_SCRIPT, pszChars + I, nextI - I, pBuf, nActualItems);
              if (!pszEnd)
                dwCookie |= COOKIE_BLOCK_SCRIPT;
              else
                {
                  dwCookie = 0;
                  bRedefineBlock = true;
                }
              I = nextI - 1;
            }
          else if (dwCookie & COOKIE_BLOCK_STYLE)
            {
              const TCHAR *pszEnd = _tcsstr(pszChars + I, _T("</style>"));
              int nextI = pszEnd ? static_cast<int>(pszEnd - pszChars) : nLength;
              dwCookie = ParseLineCss(dwCookie & ~COOKIE_BLOCK_STYLE, pszChars + I, nextI - I, pBuf, nActualItems);
              if (!pszEnd)
                dwCookie |= COOKIE_BLOCK_STYLE;
              else
                {
                  dwCookie = 0;
                  bRedefineBlock = true;
                }
              I = nextI - 1;
            }
          else if ((dwCookie & COOKIE_EXT_USER1))
            {
              const TCHAR *pszEnd = _tcsstr(pszChars + I, _T("?>"));
              if (!pszEnd)
                pszEnd = _tcsstr(pszChars + I, _T("%>"));
              int nextI = pszEnd ? static_cast<int>(pszEnd - pszChars) : nLength;
              unsigned (*pParseLineFunc)(unsigned, const TCHAR *, int, TEXTBLOCK *, int &);
              switch (nEmbeddedLanguage)
              {
              case SRC_BASIC: pParseLineFunc = ParseLineBasic; break;
              case SRC_PHP: pParseLineFunc = ParseLinePhpLanguage; break;
              default: pParseLineFunc = ParseLineJava; break;
              }
              dwCookie = pParseLineFunc(dwCookie & ~COOKIE_EXT_USER1, pszChars + I, nextI - I, pBuf, nActualItems);
              if (!pszEnd)
                dwCookie |= COOKIE_EXT_USER1;
              else
                {
                  dwCookie = 0;
                  bRedefineBlock = true;
                }
              I = nextI - 1;
            }
          continue;
        }

      //  String constant "...."
      if (dwCookie & COOKIE_STRING)
        {
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Extended comment <!--....-->
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if (I > 1 && pszChars[I] == '>' && pszChars[nPrevI] == '-' && *::CharPrev(pszChars, pszChars + nPrevI) == '-')
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Normal text
      if ((dwCookie & COOKIE_ELEMENT) && pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }

      if ((dwCookie & COOKIE_ELEMENT) && pszChars[I] == '\'')
        {
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

      if (I < nLength - 3 && pszChars[I] == '<' && pszChars[I + 1] == '!' && pszChars[I + 2] == '-' && pszChars[I + 3] == '-')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          I += 3;
          dwCookie |= COOKIE_EXT_COMMENT;
          dwCookie &= ~COOKIE_ELEMENT;
          continue;
        }

      //  User1 start: <?
      if (I < nLength && pszChars[I] == '<' && I < nLength - 1 && (pszChars[I + 1] == '?' || pszChars[I + 1] == '%'))
        {
          DEFINE_BLOCK (I, COLORINDEX_NORMALTEXT);
          dwCookie |= COOKIE_EXT_USER1;
          nIdentBegin = -1;
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
              if (dwCookie & COOKIE_ELEMENT)
                {
                  if (IsHtmlKeyword (pszChars + nIdentBegin, I - nIdentBegin) && (pszChars[nIdentBegin - 1] == _T ('<') || pszChars[nIdentBegin - 1] == _T ('/')))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                      if (nIdentBegin > 0 && _tcsnicmp(pszChars + nIdentBegin - 1, _T("<script"), sizeof("<script") - 1) == 0)
                        dwCookie |= COOKIE_BLOCK_SCRIPT;
                      else if (nIdentBegin > 0 && _tcsnicmp(pszChars + nIdentBegin - 1, _T("<style"), sizeof("<style") - 1) == 0)
                        dwCookie |= COOKIE_BLOCK_STYLE;
                    }
                  else if (IsHtmlUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                    }
                  else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                    }
                  else
                    {
                      goto next;
                    }
                }
              else if (dwCookie & COOKIE_USER1)
                {
                  if (IsHtmlUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
                    }
                  else
                    {
                      goto next;
                    }
                }
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
next:
              ;
            }

          //  Preprocessor start: < or bracket
          if (I < nLength && pszChars[I] == '<' && !(I < nLength - 3 && pszChars[I + 1] == '!' && pszChars[I + 2] == '-' && pszChars[I + 3] == '-'))
            {
              DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
              DEFINE_BLOCK (I + 1, COLORINDEX_PREPROCESSOR);
              dwCookie |= COOKIE_ELEMENT;
              nIdentBegin = -1;
              continue;
            }

          //  Preprocessor end: > or bracket
          if (dwCookie & COOKIE_ELEMENT)
            {
              if (pszChars[I] == '>')
                {
                  dwCookie &= ~COOKIE_ELEMENT;
                  nIdentBegin = -1;
                  bRedefineBlock = true;
                  bDecIndex = true;
                  continue;
                }
            }

          //  Preprocessor start: &
          if (pszChars[I] == '&')
            {
              dwCookie |= COOKIE_USER1;
              nIdentBegin = -1;
              continue;
            }

          //  Preprocessor end: ;
          if (dwCookie & COOKIE_USER1)
            {
              if (pszChars[I] == ';')
                {
                  dwCookie &= ~COOKIE_USER1;
                  nIdentBegin = -1;
                  continue;
                }
            }
        }
    }

  if (nIdentBegin >= 0 && (dwCookie & COOKIE_ELEMENT))
    {
      if (IsHtmlKeyword (pszChars + nIdentBegin, I - nIdentBegin) && (pszChars[nIdentBegin - 1] == _T ('<') || pszChars[nIdentBegin - 1] == _T ('/')))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
          if (nIdentBegin > 0 && _tcsnicmp(pszChars + nIdentBegin - 1, _T("<script"), sizeof("<script") - 1) == 0)
            dwCookie |= COOKIE_BLOCK_SCRIPT;
          else if (nIdentBegin > 0 && _tcsnicmp(pszChars + nIdentBegin - 1, _T("<style"), sizeof("<style") - 1) == 0)
            dwCookie |= COOKIE_BLOCK_STYLE;
        }
      else if (IsHtmlUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsHtmlUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
        }
      else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
    }

  dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_ELEMENT | COOKIE_EXT_USER1 | COOKIE_BLOCK_SCRIPT | COOKIE_BLOCK_STYLE | COOKIE_EXT_DEFINITION | COOKIE_EXT_VALUE);
  return dwCookie;
}

unsigned
CrystalLineParser::ParseLineHtml (unsigned dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  return ParseLineHtmlEx(dwCookie, pszChars, nLength, pBuf, nActualItems, SRC_JAVA);
}
