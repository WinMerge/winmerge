///////////////////////////////////////////////////////////////////////////
//  File:    perl.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  PERL syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszPerlKeywordList[] =
  {
    _T ("q"),
    _T ("qq"),
    _T ("qw"),
    _T ("qx"),
    _T ("m"),
    _T ("s"),
    _T ("y"),
    _T ("tr"),
    _T ("if"),
    _T ("sub"),
    _T ("return"),
    _T ("while"),
    _T ("for"),
    _T ("elsif"),
    _T ("foreach"),
    _T ("else"),
    _T ("unless"),
    _T ("eq"),
    _T ("not"),
    _T ("and"),
    _T ("or"),
    _T ("ne"),
    _T ("until"),
    _T ("abs"),
    _T ("accept"),
    _T ("alarm"),
    _T ("atan2"),
    _T ("bind"),
    _T ("binmode"),
    _T ("bless"),
    _T ("caller"),
    _T ("chdir"),
    _T ("chmod"),
    _T ("chomp"),
    _T ("chop"),
    _T ("chown"),
    _T ("chr"),
    _T ("chroot"),
    _T ("close"),
    _T ("closedir"),
    _T ("connect"),
    _T ("cos"),
    _T ("crypt"),
    _T ("dbmclose"),
    _T ("dbmopen"),
    _T ("defined"),
    _T ("delete"),
    _T ("die"),
    _T ("do"),
    _T ("dump"),
    _T ("each"),
    _T ("eof"),
    _T ("eval"),
    _T ("exec"),
    _T ("exists"),
    _T ("exit"),
    _T ("exp"),
    _T ("fcntl"),
    _T ("fileno"),
    _T ("flock"),
    _T ("fork"),
    _T ("formline"),
    _T ("getc"),
    _T ("getlogin"),
    _T ("getpeername"),
    _T ("getpgrp"),
    _T ("getppid"),
    _T ("getpriority"),
    _T ("getpwnam"),
    _T ("getgrname"),
    _T ("gethostbyname"),
    _T ("getnetbyname"),
    _T ("getprotobyname"),
    _T ("getpwuid"),
    _T ("getgrgid"),
    _T ("getservbyname"),
    _T ("gethostbyaddr"),
    _T ("getnetbyaddr"),
    _T ("getprotobynumber"),
    _T ("getservbyport"),
    _T ("getpwent"),
    _T ("getgrent"),
    _T ("gethostent"),
    _T ("getnetent"),
    _T ("getprotoent"),
    _T ("getservent"),
    _T ("setpwent"),
    _T ("setgrent"),
    _T ("sethostent"),
    _T ("setnetent"),
    _T ("setprotoent"),
    _T ("setservent"),
    _T ("endpwent"),
    _T ("endgrent"),
    _T ("endhostent"),
    _T ("endnetent"),
    _T ("endprotoent"),
    _T ("endservent"),
    _T ("getsockname"),
    _T ("getsockopt"),
    _T ("glob"),
    _T ("gmtime"),
    _T ("goto"),
    _T ("grep"),
    _T ("hex"),
    _T ("import"),
    _T ("index"),
    _T ("int"),
    _T ("ioctl"),
    _T ("join"),
    _T ("keys"),
    _T ("kill"),
    _T ("last"),
    _T ("lc"),
    _T ("lcfirst"),
    _T ("length"),
    _T ("link"),
    _T ("listen"),
    _T ("local"),
    _T ("localtime"),
    _T ("log"),
    _T ("lstat"),
    _T ("map"),
    _T ("mkdir"),
    _T ("msgctl"),
    _T ("msgget"),
    _T ("msgsnd"),
    _T ("msgrcv"),
    _T ("my"),
    _T ("next"),
    _T ("no"),
    _T ("oct"),
    _T ("open"),
    _T ("opendir"),
    _T ("ord"),
    _T ("pack"),
    _T ("pipe"),
    _T ("pop"),
    _T ("pos"),
    _T ("print"),
    _T ("printf"),
    _T ("push"),
    _T ("quotemeta"),
    _T ("rand"),
    _T ("read"),
    _T ("readdir"),
    _T ("readlink"),
    _T ("recv"),
    _T ("redo"),
    _T ("ref"),
    _T ("rename"),
    _T ("require"),
    _T ("reset"),
    _T ("return"),
    _T ("reverse"),
    _T ("rewinddir"),
    _T ("rindex"),
    _T ("rmdir"),
    _T ("scalar"),
    _T ("seek"),
    _T ("seekdir"),
    _T ("select"),
    _T ("semctl"),
    _T ("semget"),
    _T ("semop"),
    _T ("send"),
    _T ("setpgrp"),
    _T ("setpriority"),
    _T ("setsockopt"),
    _T ("shift"),
    _T ("shmctl"),
    _T ("sgmget"),
    _T ("shmread"),
    _T ("shmwrite"),
    _T ("shutdown"),
    _T ("sin"),
    _T ("sleep"),
    _T ("socket"),
    _T ("socketpair"),
    _T ("sort"),
    _T ("splice"),
    _T ("split"),
    _T ("sprintf"),
    _T ("sqrt"),
    _T ("srand"),
    _T ("stat"),
    _T ("study"),
    _T ("substr"),
    _T ("symlink"),
    _T ("syscall"),
    _T ("sysread"),
    _T ("system"),
    _T ("syswrite"),
    _T ("tell"),
    _T ("telldir"),
    _T ("tie"),
    _T ("time"),
    _T ("times"),
    _T ("truncate"),
    _T ("uc"),
    _T ("ucfirst"),
    _T ("umask"),
    _T ("undef"),
    _T ("unlink"),
    _T ("unpack"),
    _T ("untie"),
    _T ("unshift"),
    _T ("use"),
    _T ("utime"),
    _T ("values"),
    _T ("vec"),
    _T ("wait"),
    _T ("waitpid"),
    _T ("wantarray"),
    _T ("warn"),
    _T ("write"),
    _T ("x"),
    _T ("continue"),
    _T ("package"),
    _T ("bootstrap"),
    _T ("getgrnam"),
    _T ("tan"),
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
IsPerlKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszPerlKeywordList, pszChars, nLength);
}

static BOOL
IsPerlNumber (LPCTSTR pszChars, int nLength)
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

DWORD CCrystalTextView::
ParseLinePerl (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
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
  for (int I = 0;; nPrevI = I, I = CharNext(pszChars+I) - pszChars)
    {
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
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*::CharPrev(pszChars, pszChars + nPos)) && !xisalpha (*::CharNext(pszChars + nPos))))
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

      if (bFirstChar)
        {
          if (!xisspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
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
              if (IsPerlKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsPerlNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = FALSE;

                  for (int j = I; j < nLength; j++)
                    {
                      if (!xisspace (pszChars[j]))
                        {
                          if (pszChars[j] == '(')
                            {
                              bFunction = TRUE;
                            }
                          break;
                        }
                    }
                  if (bFunction)
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                    }
                }
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsPerlKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsPerlNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = FALSE;

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '(')
                    {
                      bFunction = TRUE;
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

  if (pszChars[nLength - 1] != '\\' || m_pTextBuffer->IsMBSTrail(nLineIndex, nLength - 1))
    dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
