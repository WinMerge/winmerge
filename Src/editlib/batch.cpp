///////////////////////////////////////////////////////////////////////////
//  File:    batch.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  MS-DOS batch syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccrystaltextview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszBatKeywordList[] =
  {
    _T ("?"),
    _T ("CALL"),
    _T ("CD"),
    _T ("CDD"),
    _T ("CHCP"),
    _T ("CLS"),
    _T ("COLOR"),
    _T ("COPY"),
    _T ("CTTY"),
    _T ("DATE"),
    _T ("DEL"),
    _T ("DELAY"),
    _T ("DESCRIBE"),
    _T ("DIR"),
    _T ("DIRHISTORY"),
    _T ("DIRS"),
    _T ("DO"),
    _T ("DRAWBOX"),
    _T ("DRAWHLINE"),
    _T ("DRAWVLINE"),
    _T ("ECHO"),
    _T ("ECHOERR"),
    _T ("ECHOS"),
    _T ("ECHOSERR"),
    _T ("ENDLOCAL"),
    _T ("ERASE"),
    _T ("ESET"),
    _T ("EXCEPT"),
    _T ("EXIT"),
    _T ("FFIND"),
    _T ("FOR"),
    _T ("FREE"),
    _T ("GLOBAL"),
    _T ("GOSUB"),
    _T ("GOTO"),
    _T ("HELP"),
    _T ("HISTORY"),
    _T ("IF"),
    _T ("IFF"),
    _T ("INKEY"),
    _T ("INPUT"),
    _T ("KEYBD"),
    _T ("KEYSTACK"),
    _T ("LH"),
    _T ("LOADHIGH"),
    _T ("LIST"),
    _T ("LOADBTM"),
    _T ("LOCK"),
    _T ("LOG"),
    _T ("MD"),
    _T ("MKDIR"),
    _T ("MEMORY"),
    _T ("MOVE"),
    _T ("ON"),
    _T ("OPTION"),
    _T ("PATH"),
    _T ("PAUSE"),
    _T ("POPD"),
    _T ("PROMPT"),
    _T ("PUSHD"),
    _T ("QUIT"),
    _T ("RD"),
    _T ("REBOOT"),
    _T ("REM"),
    _T ("REN"),
    _T ("RENAME"),
    _T ("RETURN"),
    _T ("SCREEN"),
    _T ("SCRPUT"),
    _T ("SELECT"),
    _T ("SET"),
    _T ("SETDOS"),
    _T ("SETLOCAL"),
    _T ("SHIFT"),
    _T ("START"),
    _T ("SWAPPING"),
    _T ("SWITCH"),
    _T ("TEE"),
    _T ("TEXT"),
    _T ("TIME"),
    _T ("TIMER"),
    _T ("TOUCH"),
    _T ("TREE"),
    _T ("TRUENAME"),
    _T ("TYPE"),
    _T ("UNALIAS"),
    _T ("UNLOCK"),
    _T ("UNSET"),
    _T ("VER"),
    _T ("VERIFY"),
    _T ("VOL"),
    _T ("VSCRPUT"),
    _T ("Y"),
    NULL
  };

static LPTSTR s_apszUser1KeywordList[] =
  {
    _T ("APPEND"),
    _T ("ATTRIB"),
    _T ("BUSETUP"),
    _T ("CHKDSK"),
    _T ("CHOICE"),
    _T ("COMMAND"),
    _T ("DEBUG"),
    _T ("DEFRAG"),
    _T ("DELOLDOS"),
    _T ("DELTREE"),
    _T ("DISKCOMP"),
    _T ("DISKCOPY"),
    _T ("DOSKEY"),
    _T ("DRVSPACE"),
    _T ("EDIT"),
    _T ("EMM386"),
    _T ("EXPAND"),
    _T ("FASTHELP"),
    _T ("FASTOPEN"),
    _T ("FC"),
    _T ("FDISK"),
    _T ("FIND"),
    _T ("FORMAT"),
    _T ("GRAPHICS"),
    _T ("HELP"),
    _T ("INTERLNK"),
    _T ("INTERSVR"),
    _T ("KEYB"),
    _T ("LABEL"),
    _T ("LOADFIX"),
    _T ("MEM"),
    _T ("MEMMAKER"),
    _T ("MODE"),
    _T ("MORE"),
    _T ("MOUSE"),
    _T ("MOVE"),
    _T ("MSAV"),
    _T ("MSBACKUP"),
    _T ("MSCDEX"),
    _T ("MSD"),
    _T ("MWAV"),
    _T ("MWAVTSR"),
    _T ("MWBACKUP"),
    _T ("MWUNDEL"),
    _T ("NLSFUNC"),
    _T ("POWER"),
    _T ("PRINT"),
    _T ("QBASIC"),
    _T ("REPLACE"),
    _T ("RESTORE"),
    _T ("SCANDISK"),
    _T ("SETUP"),
    _T ("SETVER"),
    _T ("SHARE"),
    _T ("SIZER"),
    _T ("SMARTDRV"),
    _T ("SMARTMON"),
    _T ("SORT"),
    _T ("SUBST"),
    _T ("SYS"),
    _T ("TREE"),
    _T ("UNDELETE"),
    _T ("UNFORMAT"),
    _T ("UNINSTAL"),
    _T ("VSAFE"),
    _T ("XCOPY"),
    NULL
  };

static BOOL
IsBatKeyword (LPCTSTR pszChars, int nLength)
{
  for (int L = 0; s_apszBatKeywordList[L] != NULL; L++)
    {
      if (_tcsnicmp (s_apszBatKeywordList[L], pszChars, nLength) == 0
            && s_apszBatKeywordList[L][nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsUser1Keyword (LPCTSTR pszChars, int nLength)
{
  TCHAR buffer[13];

  for (int L = 0; s_apszUser1KeywordList[L] != NULL; L++)
    {
      if (_tcsnicmp (s_apszUser1KeywordList[L], pszChars, nLength) == 0
            && s_apszUser1KeywordList[L][nLength] == 0)
        return TRUE;
      _tcscpy (buffer, s_apszUser1KeywordList[L]);
      _tcscat (buffer, _T (".COM"));
      if (_tcsnicmp (buffer, pszChars, nLength) == 0
            && buffer[nLength] == 0)
        return TRUE;
      _tcscpy (buffer, s_apszUser1KeywordList[L]);
      _tcscat (buffer, _T (".EXE"));
      if (_tcsnicmp (buffer, pszChars, nLength) == 0
            && buffer[nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsBatNumber (LPCTSTR pszChars, int nLength)
{
  if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
    {
      for (int I = 2; I < nLength; I++)
        {
          if (_istdigit (pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
                (pszChars[I] >= 'a' && pszChars[I] <= 'f'))
            continue;
          return FALSE;
        }
      return TRUE;
    }
  if (!_istdigit (pszChars[0]))
    return FALSE;
  for (int I = 1; I < nLength; I++)
    {
      if (!_istdigit (pszChars[I]) && pszChars[I] != '+' &&
            pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
            pszChars[I] != 'E')
        return FALSE;
    }
  return TRUE;
}

#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != NULL)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010

DWORD CCrystalTextView::
ParseLineBatch (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  int nLength = GetLineLength (nLineIndex);
  if (nLength <= 1)
    return dwCookie & COOKIE_EXT_COMMENT;

  LPCTSTR pszChars = GetLineChars (nLineIndex);
  BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  BOOL bRedefineBlock = TRUE;
  BOOL bDecIndex = FALSE;
  int nIdentBegin = -1;
  for (int I = 0;; I++)
    {
      if (bRedefineBlock)
        {
          int nPos = I;
          if (bDecIndex)
            nPos--;
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
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.')
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
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

      if (I == nLength)
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
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[I - 1] != '\\' || I >= 2 && (pszChars[I - 1] != '\\' || pszChars[I - 1] == '\\' && pszChars[I - 2] == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[I - 1] != '\\' || I >= 2 && (pszChars[I - 1] != '\\' || pszChars[I - 1] == '\\' && pszChars[I - 2] == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = TRUE;
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
          if (!I || !xisalnum (pszChars[I - 1]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

      if (bFirstChar)
        {
          if (pszChars[I] == ':')
            {
              DEFINE_BLOCK (I, COLORINDEX_PREPROCESSOR);
              dwCookie |= COOKIE_PREPROCESSOR;
              continue;
            }
          if (nLength >= I + 3 && !_tcsnicmp (pszChars + I, _T ("REM"), 3) && (isspace (pszChars[I + 3]) || nLength == I + 3))
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              dwCookie |= COOKIE_COMMENT;
              break;
            }

          if (!isspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
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
              if (IsBatKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
              else if (IsBatNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsBatKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
        }
      else if (IsBatNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
    }

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
