///////////////////////////////////////////////////////////////////////////
//  File:       dlang.cpp
//  Version:    1.0.0.0
//  Created:    03-Nov-2021
//
//  Copyright:  devmynote
//  E-mail:     devmynote@gmail.com
//
//  D language syntax highlighing definition
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

//  D language keywords
static const tchar_t * s_apszDlangKeywordList[] =
  {
    // Keywords
    _T ("__gshared"),
    _T ("__traits"),
    _T ("__vector"),
    _T ("__parameters"),
    _T ("abstract"),
    _T ("alias"),
    _T ("align"),
    _T ("asm"),
    _T ("assert"),
    _T ("auto"),
    _T ("body"),
    _T ("bool"),
    _T ("break"),
    _T ("byte"),
    _T ("case"),
    _T ("cast"),
    _T ("catch"),
    _T ("cdouble"),
    _T ("cent"),
    _T ("cfloat"),
    _T ("char"),
    _T ("class"),
    _T ("const"),
    _T ("continue"),
    _T ("creal"),
    _T ("dchar"),
    _T ("debug"),
    _T ("default"),
    _T ("delegate"),
    _T ("delete"),
    _T ("deprecated"),
    _T ("do"),
    _T ("double"),
    _T ("else"),
    _T ("enum"),
    _T ("export"),
    _T ("extern"),
    _T ("false"),
    _T ("final"),
    _T ("finally"),
    _T ("float"),
    _T ("for"),
    _T ("foreach"),
    _T ("foreach_reverse"),
    _T ("function"),
    _T ("goto"),
    _T ("idouble"),
    _T ("if"),
    _T ("ifloat"),
    _T ("immutable"),
    _T ("import"),
    _T ("in"),
    _T ("inout"),
    _T ("int"),
    _T ("interface"),
    _T ("invariant"),
    _T ("ireal"),
    _T ("is"),
    _T ("lazy"),
    _T ("long"),
    _T ("macro"),
    _T ("mixin"),
    _T ("module"),
    _T ("new"),
    _T ("nothrow"),
    _T ("null"),
    _T ("out"),
    _T ("override"),
    _T ("package"),
    _T ("pragma"),
    _T ("private"),
    _T ("protected"),
    _T ("public"),
    _T ("pure"),
    _T ("real"),
    _T ("ref"),
    _T ("return"),
    _T ("scope"),
    _T ("shared"),
    _T ("short"),
    _T ("static"),
    _T ("struct"),
    _T ("super"),
    _T ("switch"),
    _T ("synchronized"),
    _T ("template"),
    _T ("this"),
    _T ("throw"),
    _T ("true"),
    _T ("try"),
    _T ("typeid"),
    _T ("typeof"),
    _T ("ubyte"),
    _T ("ucent"),
    _T ("uint"),
    _T ("ulong"),
    _T ("union"),
    _T ("unittest"),
    _T ("ushort"),
    _T ("version"),
    _T ("void"),
    _T ("wchar"),
    _T ("while"),
    _T ("with"),
  };

static const tchar_t * s_apszUser1KeywordList[] =
  {
    _T ("__DATE__"),
    _T ("__EOF__"),
    _T ("__FILE__"),
    _T ("__FILE_FULL_PATH__"),
    _T ("__FUNCTION__"),
    _T ("__LINE__"),
    _T ("__MODULE__"),
    _T ("__PRETTY_FUNCTION__"),
    _T ("__TIME__"),
    _T ("__TIMESTAMP__"),
    _T ("__VENDOR__"),
    _T ("__VERSION__"),
    _T ("string"),
  };

static bool
IsDlangKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszDlangKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszUser1KeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineDlang (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT | COOKIE_RAWSTRING | COOKIE_STRING | 0xFF000000);

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
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING | COOKIE_RAWSTRING))
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
          if (pszChars[I] == '"')
            {
              if (dwCookie & COOKIE_RAWSTRING)
                {
                  tchar_t tc = COOKIE_GET_RAWSTRING_DELIMITER(dwCookie);
                  if (tc == '\0' || tc == pszChars[nPrevI])
                    {
                      dwCookie &= ~(COOKIE_STRING | COOKIE_RAWSTRING);
                      bRedefineBlock = true;
                    }
                }
              else
                {
                  bool bStringEnd = true;
                  const tchar_t *pszString = pszChars + I;
                  for (int nSize= 0; nSize < I; nSize += 2)
                    {
                      pszString = tc::tcharprev(pszChars, pszString);
                      if (*pszString != '\\')
                        {
                          break;
                        }
                      if (I == nSize + 1)
                        {
                          bStringEnd = false;
                          break;
                        }
                      pszString = tc::tcharprev(pszChars, pszString);
                      if (*pszString != '\\')
                        {
                          bStringEnd = false;
                          break;
                        }
                    }
                  if (bStringEnd)
                    {
                      dwCookie &= ~COOKIE_STRING;
                      bRedefineBlock = true;
                    }
                }
            }
          continue;
        }

      //  Raw string constant `....`
      //  Token string q{....} (nested string)
      if (dwCookie & COOKIE_RAWSTRING)
        {
          unsigned depth = COOKIE_GET_RAWSTRING_NUMBER_COUNT(dwCookie);
          if (depth == 0)
            {
              if (pszChars[I] == '`')
                {
                  dwCookie &= ~COOKIE_RAWSTRING;
                  bRedefineBlock = true;
                }
            }
          else
            {
              if (pszChars[I] == '{')
                {
                  COOKIE_SET_RAWSTRING_NUMBER_COUNT(dwCookie, depth + 1);
                }
              else if (pszChars[I] == '}')
                {
                  COOKIE_SET_RAWSTRING_NUMBER_COUNT(dwCookie, depth - 1);
                  if (depth <= 1)
                    {
                      dwCookie &= ~COOKIE_RAWSTRING;
                      bRedefineBlock = true;
                    }
                }
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

      //  Extended comment /*....*/ or /+....+/(nested comments)
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          unsigned depth = COOKIE_GET_EXT_COMMENT_DEPTH(dwCookie);
          if (depth == 0)
            {
              if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '*'))
                {
                  dwCookie &= ~COOKIE_EXT_COMMENT;
                  bRedefineBlock = true;
                  pszCommentEnd = pszChars + I + 1;
                }
            }
          else
            {
              if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '+' && pszChars[nPrevI] == '/'))
                {
                  COOKIE_SET_EXT_COMMENT_DEPTH(dwCookie, depth + 1);
                  pszCommentBegin = pszChars + I + 1;
                }
              else if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '+'))
                {
                  COOKIE_SET_EXT_COMMENT_DEPTH(dwCookie, depth - 1);
                  if (depth <= 1)
                    {
                      dwCookie &= ~COOKIE_EXT_COMMENT;
                      bRedefineBlock = true;
                    }
                  pszCommentEnd = pszChars + I + 1;
                }
            }
          continue;
        }

      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '/'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Normal text
      if (pszChars[I] == '"')
        {
          if (I > 0 && (pszChars[nPrevI] == 'r' || pszChars[nPrevI] == 'q'))
            { // String constant r"...." or q"...."
              tchar_t tc = '\0';
              if (pszChars[nPrevI] == 'q')
                {
                  tc = *tc::tcharnext(pszChars + I);
                  if ( tc == '(' )
                    {
                      tc = ')';
                    }
                  else if ( tc == '[' )
                    {
                      tc = ']';
                    }
                  else if ( tc == '{' )
                    {
                      tc = '}';
                    }
                  else if ( tc == '<' )
                    {
                      tc = '>';
                    }
                }
              COOKIE_SET_RAWSTRING_DELIMITER(dwCookie, tc);
              DEFINE_BLOCK (nPrevI, COLORINDEX_STRING);
              dwCookie |= COOKIE_RAWSTRING;
            }
          else
            { // String constant "...."
              DEFINE_BLOCK (I, COLORINDEX_STRING);
            }
          dwCookie |= COOKIE_STRING;
          continue;
        }
      if (pszChars[I] == 'q' && *tc::tcharnext(pszChars + I) == '{')
        { //  Token string q{....}
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_RAWSTRING;
          unsigned depth = 0;
          COOKIE_SET_RAWSTRING_NUMBER_COUNT(dwCookie, depth + 1);
          I++;
          continue;
        }
      if ((pszChars[I] == 'c' || pszChars[I] == 'w' || pszChars[I] == 'd') && I > 0 && (pszChars[nPrevI] == '"' || pszChars[nPrevI] == '`'))
        { //  String postfix
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie &= ~COOKIE_STRING;
          bRedefineBlock = true;
          continue;
        }
      if (pszChars[I] == '`')
        { //  Raw string constant `....`
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_RAWSTRING;
          continue;
        }
      if (pszChars[I] == '\'')
        { //  Char constant '..'
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }
      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/'))
        { //  Extended comment /*....*/
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          continue;
        }
      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '+' && pszChars[nPrevI] == '/'))
        { //  Extended comment /+....+/(nested comments)
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          unsigned depth = 0;
          COOKIE_SET_EXT_COMMENT_DEPTH(dwCookie, depth + 1);
          continue;
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
              if (IsDlangKeyword (pszChars + nIdentBegin, I - nIdentBegin))
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
      if (IsDlangKeyword (pszChars + nIdentBegin, I - nIdentBegin))
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

  dwCookie &= COOKIE_EXT_COMMENT | COOKIE_RAWSTRING | COOKIE_STRING | 0xFF000000;
  return dwCookie;
}
