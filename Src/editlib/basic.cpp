///////////////////////////////////////////////////////////////////////////
//  File:    basic.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Visual Basic syntax highlighing definition
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
static LPTSTR s_apszBasicKeywordList[] =
  {
    _T ("If"),
    _T ("Function"),
    _T ("Else"),
    _T ("End"),
    _T ("For"),
    _T ("Next"),
    _T ("While"),
    _T ("Wend"),
    _T ("SMDoMenu"),
    _T ("GetAttrType"),
    _T ("GetAttrName"),
    _T ("GetAttrValString"),
    _T ("GetAttrValFloat"),
    _T ("GetAttrValInt"),
    _T ("GetAttrValBool"),
    _T ("GetAttrValEnumInt"),
    _T ("GetAttrValEnumString"),
    _T ("GetClassId"),
    _T ("GetGeoType"),
    _T ("SetAttrValString"),
    _T ("SetAttrValInt"),
    _T ("SetAttrValFloat"),
    _T ("SetAttrValBool"),
    _T ("SetAttrValEnumString"),
    _T ("SetAttrValEnumInt"),
    _T ("CreateVerifyItem"),
    _T ("VerifyCardinalities"),
    _T ("As"),
    _T ("Abs"),
    _T ("AppActivate"),
    _T ("Asc"),
    _T ("Atn"),
    _T ("Beep"),
    _T ("Call"),
    _T ("CDbl"),
    _T ("ChDir"),
    _T ("ChDrive"),
    _T ("CheckBox"),
    _T ("Chr"),
    _T ("CInt"),
    _T ("CLng"),
    _T ("Close"),
    _T ("Const"),
    _T ("Cos"),
    _T ("CreateObject"),
    _T ("CSng"),
    _T ("CStr"),
    _T ("CVar"),
    _T ("CurDir"),
    _T ("Date"),
    _T ("Declare"),
    _T ("Dialog"),
    _T ("Dim"),
    _T ("Dir"),
    _T ("DlgEnable"),
    _T ("DlgText"),
    _T ("DlgVisible"),
    _T ("Do"),
    _T ("Double"),
    _T ("Loop"),
    _T ("End"),
    _T ("EOF"),
    _T ("Erase"),
    _T ("Exit"),
    _T ("Exp"),
    _T ("FileCopy"),
    _T ("FileLen"),
    _T ("Fix"),
    _T ("For"),
    _T ("To"),
    _T ("Step"),
    _T ("Next"),
    _T ("Format"),
    _T ("Function"),
    _T ("GetObject"),
    _T ("Global"),
    _T ("GoSub"),
    _T ("Return"),
    _T ("GoTo"),
    _T ("Hex"),
    _T ("Hour"),
    _T ("If"),
    _T ("Then"),
    _T ("Else"),
    _T ("Input"),
    _T ("InputBox"),
    _T ("InStr"),
    _T ("Int"),
    _T ("IsDate"),
    _T ("IsEmpty"),
    _T ("IsNull"),
    _T ("IsNumeric"),
    _T ("Kill"),
    _T ("LBound"),
    _T ("LCase"),
    _T ("LCase$"),
    _T ("Left"),
    _T ("Left$"),
    _T ("Len"),
    _T ("Let"),
    _T ("Line"),
    _T ("Input#"),
    _T ("Log"),
    _T ("Long"),
    _T ("Mid"),
    _T ("Minute"),
    _T ("MkDir"),
    _T ("Month"),
    _T ("MsgBox"),
    _T ("Name"),
    _T ("Now"),
    _T ("Oct"),
    _T ("On"),
    _T ("Error"),
    _T ("Open"),
    _T ("Option"),
    _T ("Base"),
    _T ("Print"),
    _T ("Print"),
    _T ("Rem"),
    _T ("Right"),
    _T ("RmDir"),
    _T ("Rnd"),
    _T ("Second"),
    _T ("Seek"),
    _T ("Seek"),
    _T ("Select Case"),
    _T ("SendKeys"),
    _T ("Set"),
    _T ("Shell"),
    _T ("Sin"),
    _T ("Space"),
    _T ("Sqr"),
    _T ("Static"),
    _T ("Stop"),
    _T ("Str"),
    _T ("StrComp"),
    _T ("String"),
    _T ("StringFunction"),
    _T ("Sub"),
    _T ("Tan"),
    _T ("Text"),
    _T ("TextBox"),
    _T ("Time"),
    _T ("TimeSerial"),
    _T ("TimeValue"),
    _T ("Trim"),
    _T ("LTrim"),
    _T ("RTrim"),
    _T ("Type"),
    _T ("UBound"),
    _T ("UCase"),
    _T ("Val"),
    _T ("VarType"),
    _T ("While"),
    _T ("Wend"),
    _T ("With"),
    _T ("Write"),
    _T ("Year"),
    NULL
  };

static BOOL
IsXKeyword (LPTSTR apszKeywords[], LPCTSTR pszChars, int nLength)
{
  for (int L = 0; apszKeywords[L] != NULL; L++)
    {
      if (_tcsncmp (apszKeywords[L], pszChars, nLength) == 0
            && apszKeywords[L][nLength] == 0)
        return TRUE;
    }
  return FALSE;
}

static BOOL
IsBasicKeyword (LPCTSTR pszChars, int nLength)
{
  return IsXKeyword (s_apszBasicKeywordList, pszChars, nLength);
}

static BOOL
IsBasicNumber (LPCTSTR pszChars, int nLength)
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

#define DEFINE_BLOCK(pos, colorindex) \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != NULL)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT      0x0001
#define COOKIE_PREPROCESSOR   0x0002
#define COOKIE_EXT_COMMENT    0x0004
#define COOKIE_STRING     0x0008
#define COOKIE_CHAR       0x0010

DWORD CCrystalTextView::
ParseLineBasic (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
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
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (pszChars[nPos - 1]) && !xisalpha (pszChars[nPos + 1])))
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
      if (I > 2 && !xisalnum (pszChars[I]) && (pszChars[I - 1] == 'M' || pszChars[I - 1] == 'm') && (pszChars[I - 2] == 'E' || pszChars[I - 2] == 'e') && (pszChars[I - 3] == 'R' || pszChars[I - 3] == 'r'))
        {
          DEFINE_BLOCK (I - 3, COLORINDEX_COMMENT);
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

      if (bFirstChar)
        {
          if (!isspace (pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha (pszChars[I - 1]) && !xisalpha (pszChars[I + 1])))
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (IsBasicKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsBasicNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = FALSE;

                  for (int j = I; j < nLength; j++)
                    {
                      if (!isspace (pszChars[j]))
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
      if (IsBasicKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsBasicNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = FALSE;

          for (int j = I; j < nLength; j++)
            {
              if (!isspace (pszChars[j]))
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

  if (pszChars[nLength - 1] != '\\')
    dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
