///////////////////////////////////////////////////////////////////////////
//  File:    autoit.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  AutoIt syntax highlighing definition
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

//  (Visual) AutoIt keywords
static const TCHAR * s_apszAutoItKeywordList[] =
  {
    _T ("And"),
    _T ("ByRef"),
    _T ("Case"),
    _T ("Const"),
    _T ("ContinueCase"),
    _T ("ContinueLoop"),
    _T ("Default"),
    _T ("Dim"),
    _T ("Do"),
    _T ("Else"),
    _T ("ElseIf"),
    _T ("EndFunc"),
    _T ("EndIf"),
    _T ("EndSelect"),
    _T ("EndSwitch"),
    _T ("EndWith"),
    _T ("Enum"),
    _T ("Exit"),
    _T ("ExitLoop"),
    _T ("False"),
    _T ("For"),
    _T ("Func"),
    _T ("Global"),
    _T ("If"),
    _T ("In"),
    _T ("Local"),
    _T ("Next"),
    _T ("Not"),
    _T ("Null"),
    _T ("Or"),
    _T ("ReDim"),
    _T ("Return"),
    _T ("Select"),
    _T ("Static"),
    _T ("Step"),
    _T ("Switch"),
    _T ("Then"),
    _T ("To"),
    _T ("True"),
    _T ("Until"),
    _T ("Volatile"),
    _T ("WEnd"),
    _T ("While"),
    _T ("With"),
  };

static bool
IsAutoItKeyword (const TCHAR *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszAutoItKeywordList, pszChars, nLength);
}

inline void
DefineIdentiferBlock(const TCHAR *pszChars, int nLength, CrystalLineParser::TEXTBLOCK * pBuf, int &nActualItems, int nIdentBegin, int I)
{
  if (IsAutoItKeyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
    }
  else if (CrystalLineParser::IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
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

unsigned
CrystalLineParser::ParseLineAutoIt (unsigned dwCookie, const TCHAR *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  bool bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
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
          if (pszChars[I] == '"')
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Extended comment #cs .... #ce
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if (bFirstChar && pszChars[I] == '#' &&
              ((I +  3 <= nLength && memcmp(&pszChars[I], _T("#ce"),            3 * sizeof(TCHAR)) == 0) ||
               (I + 13 <= nLength && memcmp(&pszChars[I], _T("#comments-end"), 13 * sizeof(TCHAR)) == 0)))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
              bFirstChar = false;
              I += pszChars[I + 2] == 'e' ? 2 : 12;
            }
          if (!xisspace (pszChars[I]))
            bFirstChar = false;
          continue;
        }

      if (pszChars[I] == ';')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Preprocessor directive #....
      if (dwCookie & COOKIE_PREPROCESSOR)
        {
          continue;
        }

      //  Normal text
      if (pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }

      if (bFirstChar)
        {
          if (pszChars[I] == '#')
            {
              if ((I +  3 <= nLength && memcmp(&pszChars[I], _T("#cs"),              3 * sizeof(TCHAR)) == 0) ||
                  (I + 15 <= nLength && memcmp(&pszChars[I], _T("#comments-start"), 15 * sizeof(TCHAR)) == 0))
                {
                  DEFINE_BLOCK (I, COLORINDEX_COMMENT);
                  dwCookie |= COOKIE_EXT_COMMENT;
                }
              else
                {
                  DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
                  dwCookie |= COOKIE_PREPROCESSOR;
                }
              bFirstChar = false;
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
              DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
