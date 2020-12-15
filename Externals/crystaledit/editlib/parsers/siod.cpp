///////////////////////////////////////////////////////////////////////////
//  File:    siod.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  SIOD syntax highlighing definition
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
static const TCHAR * s_apszSiodKeywordList[] =
  {
    _T("abs"),
    _T("alarm"),
    _T("and"),
    _T("append"),
    _T("apply"),
    _T("ass"),
    _T("assoc"),
    _T("assq"),
    _T("assv"),
    _T("base64decode"),
    _T("base64encode"),
    _T("begin"),
    _T("caaar"),
    _T("caadr"),
    _T("caar"),
    _T("cadar"),
    _T("caddr"),
    _T("cadr"),
    _T("car"),
    _T("cdaar"),
    _T("cdadr"),
    _T("cdar"),
    _T("cddar"),
    _T("cdddr"),
    _T("cddr"),
    _T("cdr"),
    _T("cond"),
    _T("cons-array"),
    _T("define"),
    _T("eq?"),
    _T("equal?"),
    _T("eqv?"),
    _T("eval"),
    _T("exec"),
    _T("exit"),
    _T("fclose"),
    _T("fopen"),
    _T("if"),
    _T("lambda"),
    _T("length"),
    _T("let"),
    _T("let*"),
    _T("letrec"),
    _T("list"),
    _T("load"),
    _T("max"),
    _T("member"),
    _T("memq"),
    _T("memv"),
    _T("min"),
    _T("nil"),
    _T("not"),
    _T("null?"),
    _T("number->string"),
    _T("number?"),
    _T("or"),
    _T("pair?"),
    _T("quit"),
    _T("quote"),
    _T("read"),
    _T("reverse"),
    _T("set!"),
    _T("set-car!"),
    _T("set-cdr!"),
    _T("string->number"),
    _T("string-append"),
    _T("string-length"),
    _T("string?"),
    _T("substring"),
    _T("symbol?"),
  };

static const TCHAR * s_apszUser1KeywordList[] =
  {
    _T("acos"),
    _T("asin"),
    _T("atan"),
    _T("cos"),
    _T("exp"),
    _T("log"),
    _T("sin"),
    _T("sqrt"),
    _T("tan"),
  };

static const TCHAR * s_apszUser2KeywordList[] =
  {
    _T("%%%memref"),
    _T("%%closure-code"),
    _T("%%closure-env"),
    _T("%%closure"),
    _T("%%stack-limit"),
    _T("*after-gc*"),
    _T("*args*"),
    _T("*catch"),
    _T("*env*"),
    _T("*eval-history-ptr*"),
    _T("*pi*"),
    _T("*plists*"),
    _T("*throw"),
    _T("*traced*"),
    _T("access-problem?"),
    _T("allocate-heap"),
    _T("apropos"),
    _T("aref"),
    _T("array->hexstr"),
    _T("aset"),
    _T("ash"),
    _T("atan2"),
    _T("benchmark-eval"),
    _T("benchmark-funcall1"),
    _T("benchmark-funcall2"),
    _T("bit-and"),
    _T("bit-not"),
    _T("bit-or"),
    _T("bit-xor"),
    _T("butlast"),
    _T("bytes-append"),
    _T("chdir"),
    _T("chmod"),
    _T("chown"),
    _T("closedir"),
    _T("copy-list"),
    _T("cpu-usage-limits"),
    _T("crypt"),
    _T("current-resource-usage"),
    _T("datlength"),
    _T("datref"),
    _T("decode-file-mode"),
    _T("delete-file"),
    _T("delq"),
    _T("encode-file-mode"),
    _T("encode-open-flags"),
    _T("endpwent"),
    _T("env-lookup"),
    _T("eof-val"),
    _T("errobj"),
    _T("error"),
    _T("F_GETLK"),
    _T("F_SETLK"),
    _T("F_SETLKW"),
    _T("fast-load"),
    _T("fast-print"),
    _T("fast-read"),
    _T("fast-save"),
    _T("fchmod"),
    _T("fflush"),
    _T("file-times"),
    _T("first"),
    _T("fmod"),
    _T("fnmatch"),
    _T("fork"),
    _T("fread"),
    _T("fseek"),
    _T("fstat"),
    _T("ftell"),
    _T("fwrite"),
    _T("gc-info"),
    _T("gc-status"),
    _T("gc"),
    _T("get"),
    _T("getc"),
    _T("getcwd"),
    _T("getenv"),
    _T("getgid"),
    _T("getgrgid"),
    _T("getpass"),
    _T("getpgrp"),
    _T("getpid"),
    _T("getppid"),
    _T("getpwent"),
    _T("getpwnam"),
    _T("getpwuid"),
    _T("gets"),
    _T("getuid"),
    _T("gmtime"),
    _T("hexstr->bytes"),
    _T("href"),
    _T("hset"),
    _T("html-encode"),
    _T("intern"),
    _T("kill"),
    _T("larg-default"),
    _T("last-c-error"),
    _T("last"),
    _T("lchown"),
    _T("link"),
    _T("lkey-default"),
    _T("load-so"),
    _T("localtime"),
    _T("lref-default"),
    _T("lstat"),
    _T("make-list"),
    _T("mapcar"),
    _T("md5-final"),
    _T("md5-init"),
    _T("md5-update"),
    _T("mkdatref"),
    _T("mkdir"),
    _T("mktime"),
    _T("nconc"),
    _T("nice"),
    _T("nreverse"),
    _T("nth"),
    _T("opendir"),
    _T("os-classification"),
    _T("parse-number"),
    _T("pclose"),
    _T("popen"),
    _T("pow"),
    _T("prin1"),
    _T("print-to-string"),
    _T("print"),
    _T("prog1"),
    _T("putc"),
    _T("putenv"),
    _T("putprop"),
    _T("puts"),
    _T("qsort"),
    _T("rand"),
    _T("random"),
    _T("read-from-string"),
    _T("readdir"),
    _T("readline"),
    _T("readlink"),
    _T("realtime"),
    _T("rename"),
    _T("require-so"),
    _T("require"),
    _T("rest"),
    _T("rld-pathnames"),
    _T("rmdir"),
    _T("runtime"),
    _T("save-forms"),
    _T("sdatref"),
    _T("set-eval-history"),
    _T("set-symbol-value!"),
    _T("setprop"),
    _T("setpwent"),
    _T("setuid"),
    _T("siod-lib"),
    _T("sleep"),
    _T("so-ext"),
    _T("srand"),
    _T("srandom"),
    _T("stat"),
    _T("strbreakup"),
    _T("strcat"),
    _T("strcmp"),
    _T("strcpy"),
    _T("strcspn"),
    _T("strftime"),
    _T("string-dimension"),
    _T("string-downcase"),
    _T("string-lessp"),
    _T("string-search"),
    _T("string-trim-left"),
    _T("string-trim-right"),
    _T("string-trim"),
    _T("string-upcase"),
    _T("strptime"),
    _T("strspn"),
    _T("subset"),
    _T("substring-equal?"),
    _T("swrite"),
    _T("sxhash"),
    _T("symbol-bound?"),
    _T("symbol-value"),
    _T("symbolconc"),
    _T("symlink"),
    _T("system"),
    _T("t"),
    _T("the-environment"),
    _T("trace"),
    _T("trunc"),
    _T("typeof"),
    _T("unbreakupstr"),
    _T("ungetc"),
    _T("unix-ctime"),
    _T("unix-time->strtime"),
    _T("unix-time"),
    _T("unlink"),
    _T("untrace"),
    _T("url-decode"),
    _T("url-encode"),
    _T("utime"),
    _T("verbose"),
    _T("wait"),
    _T("while"),
    _T("writes"),
  };

static bool
IsSiodKeyword (const TCHAR *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszSiodKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const TCHAR *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser1KeywordList, pszChars, nLength);
}

static bool
IsUser2Keyword (const TCHAR *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszUser2KeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineSiod (unsigned dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  const TCHAR *pszCommentBegin = nullptr;
  const TCHAR *pszCommentEnd = nullptr;
  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nIdentBegin = -1;
  bool bDefun = false;

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

      //  Extended comment /*....*/
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == ';' && pszChars[nPrevI] == '|'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
              pszCommentEnd = pszChars + I + 1;
            }
          continue;
        }

      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] != '|' && pszChars[nPrevI] == ';'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
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
      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '|' && pszChars[nPrevI] == ';'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
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
              if (IsSiodKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  if (!_tcsnicmp (_T ("defun"), pszChars + nIdentBegin, 5))
                    {
                      bDefun = true;
                    }
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

                  if (!bDefun)
                    {
                      for (int j = nIdentBegin; --j >= 0;)
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
                    }
                  if (!bFunction)
                    {
                      for (int j = I; j >= 0; j--)
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
      if (IsSiodKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          if (!_tcsnicmp (_T ("defun"), pszChars + nIdentBegin, 5))
            {
              bDefun = true;
            }
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

          if (!bDefun)
            {
              for (int j = nIdentBegin; --j >= 0;)
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
            }
          if (!bFunction)
            {
              for (int j = I; j >= 0; j--)
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
            }
          if (bFunction)
            {
              DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
            }
        }
    }

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
