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

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  PERL keywords
static const tchar_t * s_apszPerlKeywordList[] =
  {
    _T ("abs"),
    _T ("accept"),
    _T ("alarm"),
    _T ("and"),
    _T ("atan2"),
    _T ("bind"),
    _T ("binmode"),
    _T ("bless"),
    _T ("bootstrap"),
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
    _T ("continue"),
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
    _T ("else"),
    _T ("elsif"),
    _T ("endgrent"),
    _T ("endhostent"),
    _T ("endnetent"),
    _T ("endprotoent"),
    _T ("endpwent"),
    _T ("endservent"),
    _T ("eof"),
    _T ("eq"),
    _T ("eval"),
    _T ("exec"),
    _T ("exists"),
    _T ("exit"),
    _T ("exp"),
    _T ("fcntl"),
    _T ("fileno"),
    _T ("flock"),
    _T ("for"),
    _T ("foreach"),
    _T ("fork"),
    _T ("formline"),
    _T ("getc"),
    _T ("getgrent"),
    _T ("getgrgid"),
    _T ("getgrnam"),
    _T ("getgrname"),
    _T ("gethostbyaddr"),
    _T ("gethostbyname"),
    _T ("gethostent"),
    _T ("getlogin"),
    _T ("getnetbyaddr"),
    _T ("getnetbyname"),
    _T ("getnetent"),
    _T ("getpeername"),
    _T ("getpgrp"),
    _T ("getppid"),
    _T ("getpriority"),
    _T ("getprotobyname"),
    _T ("getprotobynumber"),
    _T ("getprotoent"),
    _T ("getpwent"),
    _T ("getpwnam"),
    _T ("getpwuid"),
    _T ("getservbyname"),
    _T ("getservbyport"),
    _T ("getservent"),
    _T ("getsockname"),
    _T ("getsockopt"),
    _T ("glob"),
    _T ("gmtime"),
    _T ("goto"),
    _T ("grep"),
    _T ("hex"),
    _T ("if"),
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
    _T ("m"),
    _T ("map"),
    _T ("mkdir"),
    _T ("msgctl"),
    _T ("msgget"),
    _T ("msgrcv"),
    _T ("msgsnd"),
    _T ("my"),
    _T ("ne"),
    _T ("next"),
    _T ("no"),
    _T ("not"),
    _T ("oct"),
    _T ("open"),
    _T ("opendir"),
    _T ("or"),
    _T ("ord"),
    _T ("pack"),
    _T ("package"),
    _T ("pipe"),
    _T ("pop"),
    _T ("pos"),
    _T ("print"),
    _T ("printf"),
    _T ("push"),
    _T ("q"),
    _T ("qq"),
    _T ("quotemeta"),
    _T ("qw"),
    _T ("qx"),
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
    _T ("return"),
    _T ("reverse"),
    _T ("rewinddir"),
    _T ("rindex"),
    _T ("rmdir"),
    _T ("s"),
    _T ("scalar"),
    _T ("seek"),
    _T ("seekdir"),
    _T ("select"),
    _T ("semctl"),
    _T ("semget"),
    _T ("semop"),
    _T ("send"),
    _T ("setgrent"),
    _T ("sethostent"),
    _T ("setnetent"),
    _T ("setpgrp"),
    _T ("setpriority"),
    _T ("setprotoent"),
    _T ("setpwent"),
    _T ("setservent"),
    _T ("setsockopt"),
    _T ("sgmget"),
    _T ("shift"),
    _T ("shmctl"),
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
    _T ("sub"),
    _T ("substr"),
    _T ("symlink"),
    _T ("syscall"),
    _T ("sysread"),
    _T ("system"),
    _T ("syswrite"),
    _T ("tan"),
    _T ("tell"),
    _T ("telldir"),
    _T ("tie"),
    _T ("time"),
    _T ("times"),
    _T ("tr"),
    _T ("truncate"),
    _T ("uc"),
    _T ("ucfirst"),
    _T ("umask"),
    _T ("undef"),
    _T ("unless"),
    _T ("unlink"),
    _T ("unpack"),
    _T ("unshift"),
    _T ("untie"),
    _T ("until"),
    _T ("use"),
    _T ("utime"),
    _T ("values"),
    _T ("vec"),
    _T ("wait"),
    _T ("waitpid"),
    _T ("wantarray"),
    _T ("warn"),
    _T ("while"),
    _T ("write"),
    _T ("x"),
    _T ("y"),
  };

static bool
IsPerlKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszPerlKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLinePerl (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
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
              if (IsPerlKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
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
      if (IsPerlKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
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
