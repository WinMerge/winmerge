///////////////////////////////////////////////////////////////////////////
//  File:       css.cpp
//  Version:    1.0
//  Created:    22-Oct-2006
//
//  Copyright:  Stcherbatchenko Andrei, portions by Tim Gerundt
//  E-mail:     windfall@gmx.de
//
//  CSS (Cascading Stylesheets) syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"
#include "SyntaxColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPTSTR s_apszCss1KeywordList[] =
  {
    // CSS 1
    _T ("background"),
    _T ("background-attachment"),
    _T ("background-color"),
    _T ("background-image"),
    _T ("background-position"),
    _T ("background-repeat"),
    _T ("border"),
    _T ("border-bottom"),
    _T ("border-bottom-width"),
    _T ("border-color"),
    _T ("border-left"),
    _T ("border-left-width"),
    _T ("border-right"),
    _T ("border-right-width"),
    _T ("border-style"),
    _T ("border-top"),
    _T ("border-top-width"),
    _T ("border-width"),
    _T ("clear"),
    _T ("color"),
    _T ("display"),
    _T ("float"),
    _T ("font"),
    _T ("font-family"),
    _T ("font-size"),
    _T ("font-style"),
    _T ("font-variant"),
    _T ("font-weight"),
    _T ("height"),
    _T ("letter-spacing"),
    _T ("line-height"),
    _T ("list-style"),
    _T ("list-style-image"),
    _T ("list-style-position"),
    _T ("list-style-type"),
    _T ("margin"),
    _T ("margin-bottom"),
    _T ("margin-left"),
    _T ("margin-right"),
    _T ("margin-top"),
    _T ("padding"),
    _T ("padding-bottom"),
    _T ("padding-left"),
    _T ("padding-right"),
    _T ("padding-top"),
    _T ("text-align"),
    _T ("text-decoration"),
    _T ("text-indent"),
    _T ("text-transform"),
    _T ("vertical-align"),
    _T ("white-space"),
    _T ("width"),
    _T ("word-spacing"),
    NULL
  };

static LPTSTR s_apszCss2KeywordList[] =
  {
    // CSS 2
    _T ("ascent"),
    _T ("azimuth"),
    _T ("baseline"),
    _T ("bbox"),
    _T ("border-bottom-color"),
    _T ("border-bottom-style"),
    _T ("border-collapse"),
    _T ("border-color"),
    _T ("border-left-color"),
    _T ("border-left-style"),
    _T ("border-right-color"),
    _T ("border-right-style"),
    _T ("border-spacing"),
    _T ("border-style"),
    _T ("border-top-color"),
    _T ("border-top-style"),
    _T ("bottom"),
    _T ("cap-height"),
    _T ("caption-side"),
    _T ("centerline"),
    _T ("clip"),
    _T ("content"),
    _T ("counter-increment"),
    _T ("counter-reset"),
    _T ("cue"),
    _T ("cue-after"),
    _T ("cue-before"),
    _T ("cursor"),
    _T ("definition-src"),
    _T ("descent"),
    _T ("direction"),
    _T ("elevation"),
    _T ("empty-cells"),
    _T ("font-size-adjust"),
    _T ("font-stretch"),
    _T ("left"),
    _T ("marker-offset"),
    _T ("marks"),
    _T ("mathline"),
    _T ("max-height"),
    _T ("max-width"),
    _T ("min-height"),
    _T ("min-width"),
    _T ("orphans"),
    _T ("outline"),
    _T ("outline-color"),
    _T ("outline-style"),
    _T ("outline-width"),
    _T ("overflow"),
    _T ("page"),
    _T ("page-break-after"),
    _T ("page-break-before"),
    _T ("page-break-inside"),
    _T ("panose-1"),
    _T ("pause"),
    _T ("pause-after"),
    _T ("pause-before"),
    _T ("pitch"),
    _T ("pitch-range"),
    _T ("play-during"),
    _T ("position"),
    _T ("quotes"),
    _T ("richness"),
    _T ("right"),
    _T ("size"),
    _T ("slope"),
    _T ("speak"),
    _T ("speak-header"),
    _T ("speak-numeral"),
    _T ("speak-punctuation"),
    _T ("speech-rate"),
    _T ("src"),
    _T ("stemh"),
    _T ("stemv"),
    _T ("stress"),
    _T ("table-layout"),
    _T ("text-shadow"),
    _T ("top"),
    _T ("topline"),
    _T ("unicode-bidi"),
    _T ("unicode-range"),
    _T ("units-per-em"),
    _T ("visibility"),
    _T ("voice-family"),
    _T ("volume"),
    _T ("widows"),
    _T ("widths"),
    _T ("x-height"),
    _T ("z-index"),
    NULL
  };

static BOOL
IsXKeyword (LPTSTR apszKeywords[], LPCTSTR pszChars, int nLength)
{
  for (int L = 0; apszKeywords[L] != NULL; L++)
    {
      if (_tcsnicmp (apszKeywords[L], pszChars, nLength) == 0
            && apszKeywords[L][nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsCss1Keyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszCss1KeywordList, pszChars, nLength);
}

static BOOL
IsCss2Keyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszCss2KeywordList, pszChars, nLength);
}

#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != NULL)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010
#define COOKIE_EXT_DEFINITION   0x0020
#define COOKIE_EXT_VALUE        0x0040

DWORD CCrystalTextView::
ParseLineCss (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  int nLength = GetLineLength (nLineIndex);
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT|COOKIE_EXT_DEFINITION|COOKIE_EXT_VALUE);

  LPCTSTR pszChars = GetLineChars (nLineIndex);
  BOOL bFirstChar = (dwCookie & ~(COOKIE_EXT_COMMENT|COOKIE_EXT_DEFINITION|COOKIE_EXT_VALUE)) == 0;
  BOOL bRedefineBlock = TRUE;
  BOOL bWasCommentStart = FALSE;
  BOOL bDecIndex = FALSE;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = CharNext(pszChars+I) - pszChars)
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
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' || pszChars[nPos] == '-' || pszChars[nPos] == '%')
                {
                  if (dwCookie & COOKIE_EXT_VALUE)
                    {
                      DEFINE_BLOCK (nPos, COLORINDEX_STRING);
                    }
                  else
                    {
                      DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
                    }
                }
              else
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_OPERATOR);
                  bRedefineBlock = TRUE;
                  bDecIndex = TRUE;
                  goto out;
                }
            }
          bRedefineBlock = FALSE;
          bDecIndex = FALSE;
        }
out:

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength)
        break;

      //  Extended definition {....}
      if (dwCookie & COOKIE_EXT_DEFINITION)
        {
          if (pszChars[I] == ':') //Value start...
            {
              dwCookie |= COOKIE_EXT_VALUE;
            }
          else if (pszChars[I] == ';') //Value end...
            {
              dwCookie &= ~COOKIE_EXT_VALUE;
            }
          else if (pszChars[I] == '}') //Definition end...
            {
              dwCookie &= ~COOKIE_EXT_DEFINITION;
              dwCookie &= ~COOKIE_EXT_VALUE;
            }
        }

      //  Extended comment /*....*/
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if ((I > 1 && pszChars[I] == '/' && pszChars[nPrevI] == '*' && !bWasCommentStart) || (I == 1 && pszChars[I] == '/' && pszChars[nPrevI] == '*'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = TRUE;
            }
          bWasCommentStart = FALSE;
          continue;
        }

      //  Normal text
      if (pszChars[I] == '{')
        {
          dwCookie |= COOKIE_EXT_DEFINITION;
        }
      if (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/')
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          bWasCommentStart = TRUE;
          continue;
        }

      bWasCommentStart = FALSE;

      if (bFirstChar)
        {
          if (!xisspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.' || pszChars[I] == '-' || pszChars[I] == '%')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (dwCookie & COOKIE_EXT_VALUE)
                {
                  if (IsCss1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                    }
                  else if (IsCss2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
                    }
                  else
                    {
                      goto next;
                    }
                }
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
              nIdentBegin = -1;
next:
              ;
            }
        }
    }

  if ((nIdentBegin >= 0) && (dwCookie & COOKIE_EXT_VALUE))
    {
      if (IsCss1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsCss2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
        }
    }

  dwCookie &= (COOKIE_EXT_COMMENT|COOKIE_EXT_DEFINITION|COOKIE_EXT_VALUE);
  return dwCookie;
}
