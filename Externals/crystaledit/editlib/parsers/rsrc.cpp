///////////////////////////////////////////////////////////////////////////
//  File:    rsrc.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Windows resources syntax highlighing definition
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

//  C++ keywords (MSVC5.0 + POET5.0)
static const TCHAR * s_apszRsrcKeywordList[] =
  {
    _T ("ACCELERATORS"),
    _T ("ALT"),
    _T ("ASCII"),
    _T ("AUTO3STATE"),
    _T ("AUTOCHECKBOX"),
    _T ("AUTORADIOBUTTON"),
    _T ("BEDIT"),
    _T ("BEGIN"),
    _T ("BITMAP"),
    _T ("BLOCK"),
    _T ("BUTTON"),
    _T ("CAPTION"),
    _T ("CHARACTERISTICS"),
    _T ("CHECKBOX"),
    _T ("CHECKED"),
    _T ("CLASS"),
    _T ("COMBOBOX"),
    _T ("CONTROL"),
    _T ("CTEXT"),
    _T ("CURSOR"),
    _T ("DEFPUSHBUTTON"),
    _T ("DIALOG"),
    _T ("DIALOGEX"),
    _T ("DISCARDABLE"),
    _T ("EDIT"),
    _T ("EDITTEXT"),
    _T ("END"),
    _T ("EXSTYLE"),
    _T ("FILEFLAGS"),
    _T ("FILEFLAGSMASK"),
    _T ("FILEOS"),
    _T ("FILESUBTYPE"),
    _T ("FILETYPE"),
    _T ("FILEVERSION"),
    _T ("FIXED"),
    _T ("FONT"),
    _T ("GRAYED"),
    _T ("GROUPBOX"),
    _T ("HEDIT"),
    _T ("HELP"),
    _T ("ICON"),
    _T ("IEDIT"),
    _T ("IMPURE"),
    _T ("INACTIVE"),
    _T ("LANGUAGE"),
    _T ("LISTBOX"),
    _T ("LOADONCALL"),
    _T ("LTEXT"),
    _T ("MENU"),
    _T ("MENUBARBREAK"),
    _T ("MENUBREAK"),
    _T ("MENUEX"),
    _T ("MENUITEM"),
    _T ("MESSAGETABLE"),
    _T ("MOVEABLE"),
    _T ("NOINVERT"),
    _T ("NONSHARED"),
    _T ("POPUP"),
    _T ("PRELOAD"),
    _T ("PRODUCTVERSION"),
    _T ("PURE"),
    _T ("PUSHBOX"),
    _T ("PUSHBUTTON"),
    _T ("RADIOBUTTON"),
    _T ("RCDATA"),
    _T ("RTEXT"),
    _T ("SCROLLBAR"),
    _T ("SEPARATOR"),
    _T ("SHARED"),
    _T ("SHIFT"),
    _T ("STATE3"),
    _T ("STATIC"),
    _T ("STRINGTABLE"),
    _T ("STYLE"),
    _T ("TEXTINCLUDE"),
    _T ("USERBUTTON"),
    _T ("VALUE"),
    _T ("VERSION"),
    _T ("VERSIONINFO"),
    _T ("VIRTKEY"),
  };

static const TCHAR * s_apszUser1KeywordList[] =
  {
    _T ("VK_BACK"),
    _T ("VK_CANCEL"),
    _T ("VK_CAPITAL"),
    _T ("VK_CLEAR"),
    _T ("VK_CONTROL"),
    _T ("VK_DELETE"),
    _T ("VK_DOWN"),
    _T ("VK_END"),
    _T ("VK_ESCAPE"),
    _T ("VK_F1"),
    _T ("VK_F10"),
    _T ("VK_F11"),
    _T ("VK_F12"),
    _T ("VK_F13"),
    _T ("VK_F14"),
    _T ("VK_F15"),
    _T ("VK_F16"),
    _T ("VK_F17"),
    _T ("VK_F18"),
    _T ("VK_F19"),
    _T ("VK_F2"),
    _T ("VK_F20"),
    _T ("VK_F21"),
    _T ("VK_F22"),
    _T ("VK_F23"),
    _T ("VK_F24"),
    _T ("VK_F3"),
    _T ("VK_F4"),
    _T ("VK_F5"),
    _T ("VK_F6"),
    _T ("VK_F7"),
    _T ("VK_F8"),
    _T ("VK_F9"),
    _T ("VK_HELP"),
    _T ("VK_HOME"),
    _T ("VK_INSERT"),
    _T ("VK_LBUTTON"),
    _T ("VK_LEFT"),
    _T ("VK_MBUTTON"),
    _T ("VK_MENU"),
    _T ("VK_NEXT"),
    _T ("VK_PAUSE"),
    _T ("VK_PRIOR"),
    _T ("VK_RBUTTON"),
    _T ("VK_RETURN"),
    _T ("VK_RIGHT"),
    _T ("VK_SELECT"),
    _T ("VK_SHIFT"),
    _T ("VK_SPACE"),
    _T ("VK_TAB"),
    _T ("VK_UP"),
  };

static bool
IsRsrcKeyword (const TCHAR *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszRsrcKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const TCHAR *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser1KeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineRsrc (unsigned dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  bool bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  const TCHAR *pszCommentBegin = nullptr;
  const TCHAR *pszCommentEnd = nullptr;
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
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*::CharPrev(pszChars, pszChars + nPos)) && !xisalpha (*::CharNext(pszChars + nPos))))
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
          if (pszChars[I] == '\'' && (I == 0 || pszChars[nPrevI] != '\\'))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = true;
            }
          continue;
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

      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '/'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Preprocessor directive #....
      if (dwCookie & COOKIE_PREPROCESSOR)
        {
          if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/'))
            {
              DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_EXT_COMMENT;
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
      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          continue;
        }

      if (bFirstChar)
        {
          if (pszChars[I] == '#')
            {
              DEFINE_BLOCK (I, COLORINDEX_PREPROCESSOR);
              dwCookie |= COOKIE_PREPROCESSOR;
              continue;
            }
          if (!xisspace (pszChars[I]))
            bFirstChar = false;
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
              if (IsRsrcKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
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
      if (IsRsrcKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
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
