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

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const tchar_t * s_apszCssKeywordList[] =
  {
    // CSS 1, CSS 2.1, CSS 3

    _T("accent-color"),
    _T("additive-symbols"),
    _T("align-content"),
    _T("align-items"),
    _T("align-self"),
    _T("all"),
    _T("animation"),
    _T("animation-delay"),
    _T("animation-direction"),
    _T("animation-duration"),
    _T("animation-fill-mode"),
    _T("animation-iteration-count"),
    _T("animation-name"),
    _T("animation-play-state"),
    _T("animation-timeline"),
    _T("animation-timing-function"),
    _T("appearance"),
    _T("ascent-override"),
    _T("aspect-ratio"),

    _T("backdrop-filter"),
    _T("backface-visibility"),
    _T("background"),
    _T("background-attachment"),
    _T("background-blend-mode"),
    _T("background-clip"),
    _T("background-color"),
    _T("background-image"),
    _T("background-origin"),
    _T("background-position"),
    _T("background-position-x"),
    _T("background-position-y"),
    _T("background-repeat"),
    _T("background-size"),
    _T("block-size"),
    _T("border"),
    _T("border-block"),
    _T("border-block-color"),
    _T("border-block-end"),
    _T("border-block-end-color"),
    _T("border-block-end-style"),
    _T("border-block-end-width"),
    _T("border-block-start"),
    _T("border-block-start-color"),
    _T("border-block-start-style"),
    _T("border-block-start-width"),
    _T("border-block-style"),
    _T("border-block-width"),
    _T("border-bottom"),
    _T("border-bottom-color"),
    _T("border-bottom-left-radius"),
    _T("border-bottom-right-radius"),
    _T("border-bottom-style"),
    _T("border-bottom-width"),
    _T("border-collapse"),
    _T("border-color"),
    _T("border-end-end-radius"),
    _T("border-end-start-radius"),
    _T("border-image"),
    _T("border-image-outset"),
    _T("border-image-repeat"),
    _T("border-image-slice"),
    _T("border-image-source"),
    _T("border-image-width"),
    _T("border-inline"),
    _T("border-inline-color"),
    _T("border-inline-end"),
    _T("border-inline-end-color"),
    _T("border-inline-end-style"),
    _T("border-inline-end-width"),
    _T("border-inline-start"),
    _T("border-inline-start-color"),
    _T("border-inline-start-style"),
    _T("border-inline-start-width"),
    _T("border-inline-style"),
    _T("border-inline-width"),
    _T("border-left"),
    _T("border-left-color"),
    _T("border-left-style"),
    _T("border-left-width"),
    _T("border-radius"),
    _T("border-right"),
    _T("border-right-color"),
    _T("border-right-style"),
    _T("border-right-width"),
    _T("border-spacing"),
    _T("border-start-end-radius"),
    _T("border-start-start-radius"),
    _T("border-style"),
    _T("border-top"),
    _T("border-top-color"),
    _T("border-top-left-radius"),
    _T("border-top-right-radius"),
    _T("border-top-style"),
    _T("border-top-width"),
    _T("border-width"),
    _T("bottom"),
    _T("box-decoration-break"),
    _T("box-shadow"),
    _T("box-sizing"),
    _T("break-after"),
    _T("break-before"),
    _T("break-inside"),

    _T("caption-side"),
    _T("caret-color"),
    _T("clear"),
    _T("clip"),
    _T("clip-path"),
    _T("color"),
    _T("color-scheme"),
    _T("column-count"),
    _T("column-fill"),
    _T("column-gap"),
    _T("column-rule"),
    _T("column-rule-color"),
    _T("column-rule-style"),
    _T("column-rule-width"),
    _T("column-span"),
    _T("column-width"),
    _T("columns"),
    _T("contain"),
    _T("contain-intrinsic-block-size"),
    _T("contain-intrinsic-height"),
    _T("contain-intrinsic-inline-size"),
    _T("contain-intrinsic-size"),
    _T("contain-intrinsic-width"),
    _T("content"),
    _T("content-visibility"),
    _T("counter-increment"),
    _T("counter-reset"),
    _T("counter-set"),
    _T("cursor"),

    _T("descent-override"),
    _T("direction"),
    _T("display"),

    _T("empty-cells"),

    _T("fallback"),
    _T("filter"),
    _T("flex"),
    _T("flex-basis"),
    _T("flex-direction"),
    _T("flex-flow"),
    _T("flex-grow"),
    _T("flex-shrink"),
    _T("flex-wrap"),
    _T("float"),
    _T("font"),
    _T("font-display"),
    _T("font-family"),
    _T("font-feature-settings"),
    _T("font-kerning"),
    _T("font-language-override"),
    _T("font-optical-sizing"),
    _T("font-size"),
    _T("font-size-adjust"),
    _T("font-stretch"),
    _T("font-style"),
    _T("font-synthesis"),
    _T("font-variant"),
    _T("font-variant-alternates"),
    _T("font-variant-caps"),
    _T("font-variant-east-asian"),
    _T("font-variant-ligatures"),
    _T("font-variant-numeric"),
    _T("font-variant-position"),
    _T("font-variation-settings"),
    _T("font-weight"),
    _T("forced-color-adjust"),

    _T("gap"),
    _T("grid"),
    _T("grid-area"),
    _T("grid-auto-columns"),
    _T("grid-auto-flow"),
    _T("grid-auto-rows"),
    _T("grid-column"),
    _T("grid-column-end"),
    _T("grid-column-start"),
    _T("grid-row"),
    _T("grid-row-end"),
    _T("grid-row-start"),
    _T("grid-template"),
    _T("grid-template-areasv"),
    _T("grid-template-columns"),
    _T("grid-template-rows"),

    _T("hanging-punctuation"),
    _T("height"),
    _T("hyphenate-character"),
    _T("hyphens"),

    _T("image-orientation"),
    _T("image-rendering"),
    _T("inherit"),
    _T("initial"),
    _T("initial-letter"),
    _T("inline-size"),
    _T("inset"),
    _T("inset-block"),
    _T("inset-block-end"),
    _T("inset-block-start"),
    _T("inset-inline"),
    _T("inset-inline-end"),
    _T("inset-inline-start"),
    _T("isolation"),

    _T("justify-content"),
    _T("justify-items"),
    _T("justify-self"),

    _T("left"),
    _T("letter-spacing"),
    _T("line-break"),
    _T("line-gap-override"),
    _T("line-height"),
    _T("list-style"),
    _T("list-style-image"),
    _T("list-style-position"),
    _T("list-style-type"),

    _T("margin"),
    _T("margin-block"),
    _T("margin-block-end"),
    _T("margin-block-start"),
    _T("margin-bottom"),
    _T("margin-inline"),
    _T("margin-inline-end"),
    _T("margin-inline-start"),
    _T("margin-left"),
    _T("margin-right"),
    _T("margin-top"),
    _T("margin-trim"),
    _T("mask"),
    _T("mask-border"),
    _T("mask-border-mode"),
    _T("mask-border-outset"),
    _T("mask-border-repeat"),
    _T("mask-border-slice"),
    _T("mask-border-source"),
    _T("mask-border-width"),
    _T("mask-clip"),
    _T("mask-composite"),
    _T("mask-image"),
    _T("mask-mode"),
    _T("mask-origin"),
    _T("mask-position"),
    _T("mask-repeat"),
    _T("mask-size"),
    _T("mask-type"),
    _T("max-block-size"),
    _T("max-height"),
    _T("max-inline-size"),
    _T("max-width"),
    _T("min-block-size"),
    _T("min-height"),
    _T("min-inline-size"),
    _T("min-width"),
    _T("mix-blend-mode"),

    _T("negative"),

    _T("object-fit"),
    _T("object-position"),
    _T("offset"),
    _T("offset-anchor"),
    _T("offset-distance"),
    _T("offset-path"),
    _T("offset-rotate"),
    _T("opacity"),
    _T("order"),
    _T("orphans"),
    _T("outline"),
    _T("outline-color"),
    _T("outline-offset"),
    _T("outline-style"),
    _T("outline-width"),
    _T("overflow"),
    _T("overflow-anchor"),
    _T("overflow-block"),
    _T("overflow-clip-margin"),
    _T("overflow-inline"),
    _T("overflow-wrap"),
    _T("overflow-x"),
    _T("overflow-y"),
    _T("overscroll-behavior"),
    _T("overscroll-behavior-block"),
    _T("overscroll-behavior-inline"),
    _T("overscroll-behavior-x"),
    _T("overscroll-behavior-y"),

    _T("pad"),
    _T("padding"),
    _T("padding-block"),
    _T("padding-block-end"),
    _T("padding-block-start"),
    _T("padding-bottom"),
    _T("padding-inline"),
    _T("padding-inline-end"),
    _T("padding-inline-start"),
    _T("padding-left"),
    _T("padding-right"),
    _T("padding-top"),
    _T("perspective"),
    _T("perspective-origin"),
    _T("place-content"),
    _T("place-items"),
    _T("place-self"),
    _T("pointer-events"),
    _T("position"),
    _T("prefix"),

    _T("quotes"),

    _T("range"),
    _T("resize"),
    _T("revert"),
    _T("right"),
    _T("rotate"),
    _T("row-gap"),
    _T("ruby-align"),
    _T("ruby-position"),

    _T("scale"),
    _T("scroll-behavior"),
    _T("scroll-margin"),
    _T("scroll-margin-block"),
    _T("scroll-margin-block-end"),
    _T("scroll-margin-block-start"),
    _T("scroll-margin-bottom"),
    _T("scroll-margin-inline"),
    _T("scroll-margin-inline-end"),
    _T("scroll-margin-inline-start"),
    _T("scroll-margin-left"),
    _T("scroll-margin-right"),
    _T("scroll-margin-top"),
    _T("scroll-padding"),
    _T("scroll-padding-block"),
    _T("scroll-padding-block-end"),
    _T("scroll-padding-block-start"),
    _T("scroll-padding-bottom"),
    _T("scroll-padding-inline"),
    _T("scroll-padding-inline-end"),
    _T("scroll-padding-inline-start"),
    _T("scroll-padding-left"),
    _T("scroll-padding-right"),
    _T("scroll-padding-top"),
    _T("scroll-snap-align"),
    _T("scroll-snap-stop"),
    _T("scroll-snap-type"),
    _T("scroll-timeline"),
    _T("scrollbar-color"),
    _T("scrollbar-gutter"),
    _T("scrollbar-width"),
    _T("shape-image-threshold"),
    _T("shape-margin"),
    _T("shape-outside"),
    _T("size-adjust"),
    _T("speak-as"),
    _T("src"),
    _T("suffix"),
    _T("symbols"),
    _T("system"),

    _T("tab-size"),
    _T("table-layout"),
    _T("text-align"),
    _T("text-align-last"),
    _T("text-combine-upright"),
    _T("text-decoration"),
    _T("text-decoration-color"),
    _T("text-decoration-line"),
    _T("text-decoration-skip"),
    _T("text-decoration-skip-ink"),
    _T("text-decoration-style"),
    _T("text-decoration-thickness"),
    _T("text-emphasis"),
    _T("text-emphasis-color"),
    _T("text-emphasis-position"),
    _T("text-emphasis-style"),
    _T("text-indent"),
    _T("text-justify"),
    _T("text-orientation"),
    _T("text-overflow"),
    _T("text-rendering"),
    _T("text-shadow"),
    _T("text-transform"),
    _T("text-underline-offset"),
    _T("text-underline-position"),
    _T("top"),
    _T("touch-action"),
    _T("transform"),
    _T("transform-box"),
    _T("transform-origin"),
    _T("transform-style"),
    _T("transition"),
    _T("transition-delay"),
    _T("transition-duration"),
    _T("transition-property"),
    _T("transition-timing-function"),
    _T("translate"),

    _T("unicode-bidi"),
    _T("unicode-range"),
    _T("unset"),
    _T("user-select"),

    _T("vertical-align"),
    _T("visibility"),

    _T("white-space"),
    _T("widows"),
    _T("width"),
    _T("will-change"),
    _T("word-break"),
    _T("word-spacing"),
    _T("word-wrap"),
    _T("writing-mode"),

    _T("z-index"),

    nullptr
  };

static const tchar_t * s_apszCssExKeywordList[] =
  {
    // experimental or deprecated keywords

    _T("align-tracks"),
    _T("animation-composition"),
    _T("azimuth"),
    _T("clip"),
    _T("cue"),
    _T("cue-after"),
    _T("cue-before"),
    _T("elevation"),
    _T("image-resolution"),
    _T("ime-mode"),
    _T("inherits"),
    _T("initial-letter-align"),
    _T("initial-value"),
    _T("justify-tracks"),
    _T("line-height-step"),
    _T("marker-offset"),
    _T("marks"),
    _T("masonry-auto-flow"),
    _T("math-depth"),
    _T("math-shift"),
    _T("math-style"),
    _T("offset-position"),
    _T("page-break-after"),
    _T("page-break-before"),
    _T("page-break-inside"),
    _T("paint-order"),
    _T("pause"),
    _T("pause-after"),
    _T("pause-before"),
    _T("pitch"),
    _T("pitch-range"),
    _T("play-during"),
    _T("print-color-adjust"),
    _T("richness"),
    _T("scroll-timeline-axis"),
    _T("scroll-timeline-name"),
    _T("size"),
    _T("speak"),
    _T("speak-header"),
    _T("speak-numeral"),
    _T("speak-punctuation"),
    _T("speech-rate"),
    _T("stress"),
    _T("syntax"),
    _T("text-size-adjust"),
    _T("voice-family"),
    _T("volume"),

    nullptr
  };

static bool
IsXKeyword (const tchar_t *apszKeywords[], const tchar_t *pszChars, int nLength)
{
  for (int L = 0; apszKeywords[L] != nullptr; L++)
    {
      if (tc::tcsnicmp (apszKeywords[L], pszChars, nLength) == 0
            && apszKeywords[L][nLength] == 0)
        return true;
    }
  return false;
}

static bool
IsCssKeyword(const tchar_t *pszChars, int nLength)
{
  return IsXKeyword (s_apszCssKeywordList, pszChars, nLength);
}

static bool
IsCssExKeyword(const tchar_t *pszChars, int nLength)
{
  return IsXKeyword (s_apszCssExKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineCss (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT|COOKIE_EXT_DEFINITION|COOKIE_EXT_VALUE);

  bool bFirstChar = (dwCookie & ~(COOKIE_EXT_COMMENT|COOKIE_EXT_DEFINITION|COOKIE_EXT_VALUE)) == 0;
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
          if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '*'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
              pszCommentEnd = pszChars + I + 1;
            }
          continue;
        }

      //  Normal text
      if (pszChars[I] == '{')
        {
          dwCookie |= COOKIE_EXT_DEFINITION;
        }
      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          continue;
        }

      if (bFirstChar)
        {
          if (!xisspace (pszChars[I]))
            bFirstChar = false;
        }

      if (pBuf == nullptr)
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
                  if (IsCssKeyword(pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                    }
                  else if (IsCssExKeyword(pszChars + nIdentBegin, I - nIdentBegin))
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
        }
    }

  if ((nIdentBegin >= 0) && (dwCookie & COOKIE_EXT_VALUE))
    {
      if (IsCssKeyword(pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsCssExKeyword(pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
        }
    }

  dwCookie &= (COOKIE_EXT_COMMENT|COOKIE_EXT_DEFINITION|COOKIE_EXT_VALUE);
  return dwCookie;
}
