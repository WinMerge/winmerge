///////////////////////////////////////////////////////////////////////////
//  File:       powershell.cpp
//  Version:    1.0
//  Created:    23-Dec-2007
//
//  Copyright:  Stcherbatchenko Andrei, portions by Tim Gerundt
//  E-mail:     windfall@gmx.de
//
//  PowerShell syntax highlighing definition
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

//  PowerShell keywords
static LPTSTR s_apszPowerShellKeywordList[] =
  {
    _T ("break"),
    _T ("continue"),
    _T ("do"),
    _T ("else"),
    _T ("elseif"),
    _T ("filter"),
    _T ("for"),
    _T ("foreach"),
    _T ("function"),
    _T ("if"),
    _T ("in"),
    _T ("return"),
    _T ("switch"),
    _T ("until"),
    _T ("where"),
    _T ("while"),
    // Operators...
    _T ("-and"),
    _T ("-as"),
    _T ("-band"),
    _T ("-bor"),
    _T ("-ccontains"),
    _T ("-ceq"),
    _T ("-cge"),
    _T ("-cgt"),
    _T ("-cle"),
    _T ("-clike"),
    _T ("-clt"),
    _T ("-cmatch"),
    _T ("-cne"),
    _T ("-cnotcontains"),
    _T ("-cnotlike"),
    _T ("-cnotmatch"),
    _T ("-contains"),
    _T ("-comp"),
    _T ("-creplace"),
    _T ("-eq"),
    _T ("-ge"),
    _T ("-gt"),
    _T ("-icontains"),
    _T ("-ieq"),
    _T ("-ige"),
    _T ("-igt"),
    _T ("-ile"),
    _T ("-ilike"),
    _T ("-ilt"),
    _T ("-imatch"),
    _T ("-ine"),
    _T ("-inot"),
    _T ("-inotcontains"),
    _T ("-inotlike"),
    _T ("-inotmatch"),
    _T ("-ireplace"),
    _T ("-is"),
    _T ("-isnot"),
    _T ("-le"),
    _T ("-like"),
    _T ("-lt"),
    _T ("-match"),
    _T ("-ne"),
    _T ("-not"),
    _T ("-notcontains"),
    _T ("-notlike"),
    _T ("-notmatch"),
    _T ("-or"),
    _T ("-replace"),
    _T ("-sl"),
    _T ("-sr"),
    _T ("-xor"),
    NULL
  };

static LPTSTR s_apszCmdletKeywordList[] =
  {
    // Commands...
    _T ("add-content"),
    _T ("add-history"),
    _T ("add-member"),
    _T ("add-pssnapin"),
    _T ("clear-content"),
    _T ("clear-item"),
    _T ("clear-itemproperty"),
    _T ("clear-variable"),
    _T ("compare-object"),
    _T ("convertfrom-securestring"),
    _T ("convert-path"),
    _T ("convertto-html"),
    _T ("convertto-securestring"),
    _T ("copy-item"),
    _T ("copy-itemproperty"),
    _T ("export-alias"),
    _T ("export-clixml"),
    _T ("export-console"),
    _T ("export-csv"),
    _T ("foreach-object"),
    _T ("format-custom"),
    _T ("format-list"),
    _T ("format-table"),
    _T ("format-wide"),
    _T ("get-acl"),
    _T ("get-alias"),
    _T ("get-authenticodesignature"),
    _T ("get-childitem"),
    _T ("get-command"),
    _T ("get-content"),
    _T ("get-credential"),
    _T ("get-culture"),
    _T ("get-date"),
    _T ("get-eventlog"),
    _T ("get-executionpolicy"),
    _T ("get-help"),
    _T ("get-history"),
    _T ("get-host"),
    _T ("get-item"),
    _T ("get-itemproperty"),
    _T ("get-location"),
    _T ("get-member"),
    _T ("get-pfxcertificate"),
    _T ("get-process"),
    _T ("get-psdrive"),
    _T ("get-psprovider"),
    _T ("get-pssnapin"),
    _T ("get-service"),
    _T ("get-tracesource"),
    _T ("get-uiculture"),
    _T ("get-unique"),
    _T ("get-variable"),
    _T ("get-wmiobject"),
    _T ("group-object"),
    _T ("import-alias"),
    _T ("import-clixml"),
    _T ("import-csv"),
    _T ("invoke-expression"),
    _T ("invoke-history"),
    _T ("invoke-item"),
    _T ("join-path"),
    _T ("measure-command"),
    _T ("measure-object"),
    _T ("move-item"),
    _T ("move-itemproperty"),
    _T ("new-alias"),
    _T ("new-item"),
    _T ("new-itemproperty"),
    _T ("new-object"),
    _T ("new-psdrive"),
    _T ("new-service"),
    _T ("new-timespan"),
    _T ("new-variable"),
    _T ("out-default"),
    _T ("out-file"),
    _T ("out-host"),
    _T ("out-null"),
    _T ("out-printer"),
    _T ("out-string"),
    _T ("pop-location"),
    _T ("push-location"),
    _T ("read-host"),
    _T ("remove-item"),
    _T ("remove-itemproperty"),
    _T ("remove-psdrive"),
    _T ("remove-pssnapin"),
    _T ("remove-variable"),
    _T ("rename-item"),
    _T ("rename-itemproperty"),
    _T ("resolve-path"),
    _T ("restart-service"),
    _T ("resume-service"),
    _T ("select-object"),
    _T ("select-string"),
    _T ("set-acl"),
    _T ("set-alias"),
    _T ("set-authenticodesignature"),
    _T ("set-content"),
    _T ("set-date"),
    _T ("set-executionpolicy"),
    _T ("set-item"),
    _T ("set-itemproperty"),
    _T ("set-location"),
    _T ("set-psdebug"),
    _T ("set-service"),
    _T ("set-tracesource"),
    _T ("set-variable"),
    _T ("sort-object"),
    _T ("split-path"),
    _T ("start-service"),
    _T ("start-sleep"),
    _T ("start-transcript"),
    _T ("stop-process"),
    _T ("stop-service"),
    _T ("stop-transcript"),
    _T ("suspend-service"),
    _T ("tee-object"),
    _T ("test-path"),
    _T ("trace-command"),
    _T ("update-formatdata"),
    _T ("update-typedata"),
    _T ("where-object"),
    _T ("write-debug"),
    _T ("write-error"),
    _T ("write-host"),
    _T ("write-output"),
    _T ("write-progress"),
    _T ("write-verbose"),
    _T ("write-warning"),
    // Aliases...
    _T ("ac"),
    _T ("asnp"),
    _T ("clc"),
    _T ("cli"),
    _T ("clp"),
    _T ("clv"),
    _T ("cpi"),
    _T ("cpp"),
    _T ("cvpa"),
    _T ("diff"),
    _T ("epal"),
    _T ("epcsv"),
    _T ("fc"),
    _T ("fl"),
    _T ("foreach"),
    _T ("ft"),
    _T ("fw"),
    _T ("gal"),
    _T ("gc"),
    _T ("gci"),
    _T ("gcm"),
    _T ("gdr"),
    _T ("ghy"),
    _T ("gi"),
    _T ("gl"),
    _T ("gm"),
    _T ("gp"),
    _T ("gps"),
    _T ("group"),
    _T ("gsv"),
    _T ("gsnp"),
    _T ("gu"),
    _T ("gv"),
    _T ("gwmi"),
    _T ("iex"),
    _T ("ihy"),
    _T ("ii"),
    _T ("ipal"),
    _T ("ipcsv"),
    _T ("mi"),
    _T ("mp"),
    _T ("nal"),
    _T ("ndr"),
    _T ("ni"),
    _T ("nv"),
    _T ("oh"),
    _T ("rdr"),
    _T ("ri"),
    _T ("rni"),
    _T ("rnp"),
    _T ("rp"),
    _T ("rsnp"),
    _T ("rv"),
    _T ("rvpa"),
    _T ("sal"),
    _T ("sasv"),
    _T ("sc"),
    _T ("select"),
    _T ("si"),
    _T ("sl"),
    _T ("sleep"),
    _T ("sort"),
    _T ("sp"),
    _T ("spps"),
    _T ("spsv"),
    _T ("sv"),
    _T ("tee"),
    _T ("where"),
    _T ("write"),
    _T ("cat"),
    _T ("cd"),
    _T ("clear"),
    _T ("cp"),
    _T ("h"),
    _T ("history"),
    _T ("kill"),
    _T ("lp"),
    _T ("ls"),
    _T ("mount"),
    _T ("mv"),
    _T ("popd"),
    _T ("ps"),
    _T ("pushd"),
    _T ("pwd"),
    _T ("r"),
    _T ("rm"),
    _T ("rmdir"),
    _T ("echo"),
    _T ("cls"),
    _T ("chdir"),
    _T ("copy"),
    _T ("del"),
    _T ("dir"),
    _T ("erase"),
    _T ("move"),
    _T ("rd"),
    _T ("ren"),
    _T ("set"),
    _T ("type"),
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
IsPowerShellKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszPowerShellKeywordList, pszChars, nLength);
}

static BOOL
IsCmdletKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszCmdletKeywordList, pszChars, nLength);
}

static BOOL
IsPowerShellNumber (LPCTSTR pszChars, int nLength)
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
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010
#define COOKIE_VARIABLE         0x0020

DWORD CCrystalTextView::
ParseLinePowerShell (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  int nLength = GetLineLength (nLineIndex);
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  LPCTSTR pszChars = GetLineChars (nLineIndex);
  BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  BOOL bRedefineBlock = TRUE;
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
              //if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' || pszChars[nPos] == '-')
              if (xisalnum (pszChars[nPos]) || (pszChars[nPos] == '-' && nPos > 0 && (xisalpha (*::CharNext(pszChars + nPos)))))
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

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength)
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
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || pszChars[nPrevI] == '\\' && *::CharPrev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      //  Variable
      if (dwCookie & COOKIE_VARIABLE)
        {
          if (!xisalnum (pszChars[I]))
            {
              dwCookie &= ~COOKIE_VARIABLE;
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
            }
          continue;
        }

      // Comment #
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

      // Variable
      if (pszChars[I] == '$')
        {
          DEFINE_BLOCK (I, COLORINDEX_USER1);
          dwCookie |= COOKIE_VARIABLE;
          continue;
        }

      if (bFirstChar)
        {
          if (!xisspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '-')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (IsPowerShellKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsCmdletKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                }
              else if (IsPowerShellNumber (pszChars + nIdentBegin, I - nIdentBegin))
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
      if (IsPowerShellKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsCmdletKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
        }
      else if (IsPowerShellNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
    }

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
