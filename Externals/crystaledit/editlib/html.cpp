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
#include "SyntaxColors.h"
#include "string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPCTSTR s_apszHtmlKeywordList[] =
  {
    _T ("A"),
    _T ("ABBR"),
    _T ("ACRONYM"),
    _T ("ADDRESS"),
    _T ("APPLET"),
    _T ("AREA"),
    _T ("B"),
    _T ("BASE"),
    _T ("BASEFONT"),
    _T ("BDO"),
    _T ("BIG"),
    _T ("BLOCKQUOTE"),
    _T ("BODY"),
    _T ("BR"),
    _T ("BUTTON"),
    _T ("CAPTION"),
    _T ("CENTER"),
    _T ("CITE"),
    _T ("CODE"),
    _T ("COL"),
    _T ("COLGROUP"),
    _T ("DD"),
    _T ("DEL"),
    _T ("DFN"),
    _T ("DIR"),
    _T ("DIV"),
    _T ("DL"),
    _T ("DOCTYPE"),
    _T ("DT"),
    _T ("EM"),
    _T ("FIELDSET"),
    _T ("FONT"),
    _T ("FORM"),
    _T ("FRAME"),
    _T ("FRAMESET"),
    _T ("H1"),
    _T ("H2"),
    _T ("H3"),
    _T ("H4"),
    _T ("H5"),
    _T ("H6"),
    _T ("HEAD"),
    _T ("HR"),
    _T ("HTML"),
    _T ("I"),
    _T ("IFRAME"),
    _T ("IMG"),
    _T ("INPUT"),
    _T ("INS"),
    _T ("ISINDEX"),
    _T ("KBD"),
    _T ("LABEL"),
    _T ("LEGEND"),
    _T ("LI"),
    _T ("LINK"),
    _T ("MAP"),
    _T ("MENU"),
    _T ("META"),
    _T ("NOFRAMES"),
    _T ("NOSCRIPT"),
    _T ("OBJECT"),
    _T ("OL"),
    _T ("OPTGROUP"),
    _T ("OPTION"),
    _T ("P"),
    _T ("PARAM"),
    _T ("PRE"),
    _T ("PUBLIC"),
    _T ("Q"),
    _T ("S"),
    _T ("SAMP"),
    _T ("SCRIPT"),
    _T ("SELECT"),
    _T ("SMALL"),
    _T ("SPAN"),
    _T ("STRIKE"),
    _T ("STRONG"),
    _T ("STYLE"),
    _T ("SUB"),
    _T ("SUP"),
    _T ("TABLE"),
    _T ("TBODY"),
    _T ("TD"),
    _T ("TEXTAREA"),
    _T ("TFOOT"),
    _T ("TH"),
    _T ("THEAD"),
    _T ("TITLE"),
    _T ("TR"),
    _T ("TT"),
    _T ("U"),
    _T ("UL"),
    _T ("VAR"),
  };

static LPCTSTR s_apszUser1KeywordList[] =
  {
    _T ("ABBR"),
    _T ("ACCEPT"),
    _T ("ACCEPT-CHARSET"),
    _T ("ACCESSKEY"),
    _T ("ACTION"),
    _T ("ALIGN"),
    _T ("ALINK"),
    _T ("ALT"),
    _T ("ARCHIVE"),
    _T ("AXIS"),
    _T ("BACKGROUND"),
    _T ("BGCOLOR"),
    _T ("BORDER"),
    _T ("CELLPADDING"),
    _T ("CELLSPACING"),
    _T ("CHAR"),
    _T ("CHAROFF"),
    _T ("CHARSET"),
    _T ("CHECKED"),
    _T ("CITE"),
    _T ("CLASS"),
    _T ("CLASSID"),
    _T ("CLEAR"),
    _T ("CODE"),
    _T ("CODEBASE"),
    _T ("CODETYPE"),
    _T ("COLOR"),
    _T ("COLS"),
    _T ("COLSPAN"),
    _T ("COMPACT"),
    _T ("CONTENT"),
    _T ("COORDS"),
    _T ("DATA"),
    _T ("DATAFLD"),
    _T ("DATAFORMATAS"),
    _T ("DATASRC"),
    _T ("DATETIME"),
    _T ("DECLARE"),
    _T ("DEFER"),
    _T ("DIR"),
    _T ("DISABLED"),
    _T ("ENCTYPE"),
    _T ("EVENT"),
    _T ("FACE"),
    _T ("FOR"),
    _T ("FRAME"),
    _T ("FRAMEBORDER"),
    _T ("HEADERS"),
    _T ("HEIGHT"),
    _T ("HREF"),
    _T ("HREFLANG"),
    _T ("HSPACE"),
    _T ("HTTP-EQUIV"),
    _T ("ID"),
    _T ("ISMAP"),
    _T ("LABEL"),
    _T ("LANG"),
    _T ("LANGUAGE"),
    _T ("LINK"),
    _T ("LONGDESC"),
    _T ("MARGINHEIGHT"),
    _T ("MARGINWIDTH"),
    _T ("MAXLENGTH"),
    _T ("MEDIA"),
    _T ("METHOD"),
    _T ("MULTIPLE"),
    _T ("NAME"),
    _T ("NOHREF"),
    _T ("NORESIZE"),
    _T ("NOSHADE"),
    _T ("NOWRAP"),
    _T ("ONBLUR"),
    _T ("ONCHANGE"),
    _T ("ONCLICK"),
    _T ("ONDBLCLICK"),
    _T ("ONFOCUS"),
    _T ("ONKEYDOWN"),
    _T ("ONKEYPRESS"),
    _T ("ONKEYUP"),
    _T ("ONLOAD"),
    _T ("ONMOUSEDOWN"),
    _T ("ONMOUSEMOVE"),
    _T ("ONMOUSEOUT"),
    _T ("ONMOUSEOVER"),
    _T ("ONMOUSEUP"),
    _T ("ONRESET"),
    _T ("ONSELECT"),
    _T ("ONSUBMIT"),
    _T ("ONUNLOAD"),
    _T ("PROFILE"),
    _T ("PROMPT"),
    _T ("READONLY"),
    _T ("REL"),
    _T ("REV"),
    _T ("ROWS"),
    _T ("ROWSPAN"),
    _T ("RULES"),
    _T ("SCHEME"),
    _T ("SCOPE"),
    _T ("SCROLLING"),
    _T ("SELECTED"),
    _T ("SHAPE"),
    _T ("SIZE"),
    _T ("SPAN"),
    _T ("SRC"),
    _T ("STANDBY"),
    _T ("START"),
    _T ("STYLE"),
    _T ("SUMMARY"),
    _T ("TABINDEX"),
    _T ("TARGET"),
    _T ("TEXT"),
    _T ("TITLE"),
    _T ("TYPE"),
    _T ("USEMAP"),
    _T ("VALIGN"),
    _T ("VALUE"),
    _T ("VALUETYPE"),
    _T ("VERSION"),
    _T ("VLINK"),
    _T ("VSPACE"),
    _T ("WIDTH"),
  };

static LPCTSTR s_apszUser2KeywordList[] =
  {
    _T ("Aacute"),
    _T ("aacute"),
    _T ("acirc"),
    _T ("Acirc"),
    _T ("acute"),
    _T ("aelig"),
    _T ("AElig"),
    _T ("agrave"),
    _T ("Agrave"),
    _T ("amp"),
    _T ("aring"),
    _T ("Aring"),
    _T ("Atilde"),
    _T ("atilde"),
    _T ("Auml"),
    _T ("auml"),
    _T ("brvbar"),
    _T ("ccedil"),
    _T ("Ccedil"),
    _T ("cedil"),
    _T ("cent"),
    _T ("copy"),
    _T ("curren"),
    _T ("deg"),
    _T ("divide"),
    _T ("Eacute"),
    _T ("eacute"),
    _T ("Ecirc"),
    _T ("ecirc"),
    _T ("Egrave"),
    _T ("egrave"),
    _T ("eth"),
    _T ("ETH"),
    _T ("Euml"),
    _T ("euml"),
    _T ("frac12"),
    _T ("frac14"),
    _T ("frac34"),
    _T ("gt"),
    _T ("iacute"),
    _T ("Iacute"),
    _T ("icirc"),
    _T ("Icirc"),
    _T ("iexcl"),
    _T ("Igrave"),
    _T ("igrave"),
    _T ("iquest"),
    _T ("Iuml"),
    _T ("iuml"),
    _T ("laquo"),
    _T ("lt"),
    _T ("macr"),
    _T ("micro"),
    _T ("middot"),
    _T ("nbsp"),
    _T ("not"),
    _T ("Ntilde"),
    _T ("ntilde"),
    _T ("Oacute"),
    _T ("oacute"),
    _T ("ocirc"),
    _T ("Ocirc"),
    _T ("ograve"),
    _T ("Ograve"),
    _T ("ordf"),
    _T ("ordm"),
    _T ("oslash"),
    _T ("Oslash"),
    _T ("otilde"),
    _T ("Otilde"),
    _T ("ouml"),
    _T ("Ouml"),
    _T ("para"),
    _T ("plusmn"),
    _T ("pound"),
    _T ("quot"),
    _T ("raquo"),
    _T ("reg"),
    _T ("sect"),
    _T ("shy"),
    _T ("sup1"),
    _T ("sup2"),
    _T ("sup3"),
    _T ("szlig"),
    _T ("thorn"),
    _T ("THORN"),
    _T ("times"),
    _T ("uacute"),
    _T ("Uacute"),
    _T ("Ucirc"),
    _T ("ucirc"),
    _T ("ugrave"),
    _T ("Ugrave"),
    _T ("uml"),
    _T ("uuml"),
    _T ("Uuml"),
    _T ("Yacute"),
    _T ("yacute"),
    _T ("yen"),
    _T ("yuml"),
  };

static bool
IsHtmlKeyword (LPCTSTR pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszHtmlKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (LPCTSTR pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser1KeywordList, pszChars, nLength);
}

static bool
IsUser2Keyword (LPCTSTR pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser2KeywordList, pszChars, nLength);
}

static bool
IsHtmlNumber (LPCTSTR pszChars, int nLength)
{
  if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
    {
      for (int I = 2; I < nLength; I++)
        {
          if (_istdigit (pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
                (pszChars[I] >= 'a' && pszChars[I] <= 'f'))
            continue;
          return false;
        }
      return true;
    }
  if (!_istdigit (pszChars[0]))
    return false;
  for (int I = 1; I < nLength; I++)
    {
      if (!_istdigit (pszChars[I]) && pszChars[I] != '+' &&
            pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
            pszChars[I] != 'E')
        return false;
    }
  return true;
}

DWORD
CrystalLineParser::ParseLineHtml (DWORD dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT|COOKIE_EXT_USER1);

  bool bFirstChar = (dwCookie & ~(COOKIE_EXT_COMMENT|COOKIE_EXT_USER1)) == 0;
  bool bRedefineBlock = true;
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
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else if (dwCookie & COOKIE_PREPROCESSOR)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_PREPROCESSOR);
            }
          else if (dwCookie & COOKIE_EXT_USER1)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_FUNCNAME);
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

      //  Extended comment <?....?>
      if (dwCookie & COOKIE_EXT_USER1)
        {
          if (I > 0 && pszChars[I] == '>' && (pszChars[nPrevI] == '?' || pszChars[nPrevI] == '%'))
            {
              dwCookie &= ~COOKIE_EXT_USER1;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Normal text
      if ((dwCookie & COOKIE_PREPROCESSOR) && pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }

      if ((dwCookie & COOKIE_PREPROCESSOR) && pszChars[I] == '\'')
        {
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

      if (!(dwCookie & COOKIE_EXT_USER1) && I < nLength - 3 && pszChars[I] == '<' && pszChars[I + 1] == '!' && pszChars[I + 2] == '-' && pszChars[I + 3] == '-')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          I += 3;
          dwCookie |= COOKIE_EXT_COMMENT;
          dwCookie &= ~COOKIE_PREPROCESSOR;
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

      if (xisalnum (pszChars[I]) || pszChars[I] == '.')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (dwCookie & COOKIE_PREPROCESSOR)
                {
                  if (IsHtmlKeyword (pszChars + nIdentBegin, I - nIdentBegin) && (pszChars[nIdentBegin - 1] == _T ('<') || pszChars[nIdentBegin - 1] == _T ('/')))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                    }
                  else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                    }
                  else if (IsHtmlNumber (pszChars + nIdentBegin, I - nIdentBegin))
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
                  if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
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

          //  User1 start: <?
          if (I < nLength && pszChars[I] == '<' && I < nLength - 1 && (pszChars[I + 1] == '?' || pszChars[I + 1] == '%'))
            {
              DEFINE_BLOCK (I, COLORINDEX_FUNCNAME);
              dwCookie |= COOKIE_EXT_USER1;
              nIdentBegin = -1;
              continue;
            }

          //  Preprocessor start: < or bracket
          if (!(dwCookie & COOKIE_EXT_USER1) && I < nLength && (pszChars[I] == '<' && !(I < nLength - 3 && pszChars[I + 1] == '!' && pszChars[I + 2] == '-' && pszChars[I + 3] == '-')/* || pszChars[I] == '{'*/))
            {
              DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
              DEFINE_BLOCK (I + 1, COLORINDEX_PREPROCESSOR);
              dwCookie |= COOKIE_PREPROCESSOR;
              nIdentBegin = -1;
              continue;
            }

          //  User1 end: ?>
          if (dwCookie & COOKIE_EXT_USER1)
            {
              if (I > 0 && pszChars[I] == '>' && (pszChars[nPrevI] == '?' || pszChars[nPrevI] == '%'))
                {
                  dwCookie &= ~COOKIE_EXT_USER1;
                  nIdentBegin = -1;
                  bRedefineBlock = true;
                  bDecIndex = true;
                  continue;
                }
            }

          //  Preprocessor end: > or bracket
          if (dwCookie & COOKIE_PREPROCESSOR)
            {
              if (pszChars[I] == '>'/* || pszChars[I] == '}'*/)
                {
                  dwCookie &= ~COOKIE_PREPROCESSOR;
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

  if (nIdentBegin >= 0 && (dwCookie & COOKIE_PREPROCESSOR))
    {
      if (IsHtmlKeyword (pszChars + nIdentBegin, I - nIdentBegin) && (pszChars[nIdentBegin - 1] == _T ('<') || pszChars[nIdentBegin - 1] == _T ('/')))
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
      else if (IsHtmlNumber (pszChars + nIdentBegin, I - nIdentBegin))
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

  //  User1 start: <?
  if (I < nLength && pszChars[I] == '<' && I < nLength - 1 && (pszChars[I + 1] == '?' || pszChars[I + 1] == '%'))
    {
      DEFINE_BLOCK (I, COLORINDEX_FUNCNAME);
      dwCookie |= COOKIE_EXT_USER1;
      nIdentBegin = -1;
      goto end;
    }

  //  Preprocessor start: < or {
  if (!(dwCookie & COOKIE_EXT_USER1) && I < nLength && (pszChars[I] == '<' && !(I < nLength - 3 && pszChars[I + 1] == '!' && pszChars[I + 2] == '-' && pszChars[I + 3] == '-')/* || pszChars[I] == '{'*/))
    {
      DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
      DEFINE_BLOCK (I + 1, COLORINDEX_PREPROCESSOR);
      dwCookie |= COOKIE_PREPROCESSOR;
      nIdentBegin = -1;
      goto end;
    }

  //  User1 end: ?>
  if (dwCookie & COOKIE_EXT_USER1)
    {
      if (I > 0 && pszChars[I] == '>' && (pszChars[nPrevI] == '?' || pszChars[nPrevI] == '%'))
        {
          dwCookie &= ~COOKIE_EXT_USER1;
          nIdentBegin = -1;
        }
    }

  //  Preprocessor end: > or }
  if (dwCookie & COOKIE_PREPROCESSOR)
    {
      if (pszChars[I] == '>'/* || pszChars[I] == '}'*/)
        {
          dwCookie &= ~COOKIE_PREPROCESSOR;
          nIdentBegin = -1;
        }
    }

end:
  dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_PREPROCESSOR | COOKIE_EXT_USER1);
  return dwCookie;
}
