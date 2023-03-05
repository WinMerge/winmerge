///////////////////////////////////////////////////////////////////////////
//  File:    fortran.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  FORTRAN syntax highlighing definition
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

//  FORTRAN keywords
static const tchar_t * s_apszFortranKeywordList[] =
  {
    _T (".and."),
    _T (".not."),
    _T (".or."),
    _T ("abs"),
    _T ("achar"),
    _T ("acos"),
    _T ("adjustl"),
    _T ("adjustr"),
    _T ("aimag"),
    _T ("aint"),
    _T ("all"),
    _T ("allocatable"),
    _T ("allocate"),
    _T ("allocated"),
    _T ("anint"),
    _T ("any"),
    _T ("asin"),
    _T ("assignment"),
    _T ("associated"),
    _T ("atan"),
    _T ("atan2"),
    _T ("backspace"),
    _T ("bit_size"),
    _T ("block"),
    _T ("blockdata"),
    _T ("btest"),
    _T ("call"),
    _T ("call"),
    _T ("case"),
    _T ("ceiling"),
    _T ("char"),
    _T ("character"),
    _T ("close"),
    _T ("cmplx"),
    _T ("common"),
    _T ("complex"),
    _T ("conjg"),
    _T ("contains"),
    _T ("continue"),
    _T ("cos"),
    _T ("cosh"),
    _T ("count"),
    _T ("cshift"),
    _T ("cycle"),
    _T ("data"),
    _T ("date_and_time"),
    _T ("dble"),
    _T ("deallocate"),
    _T ("default"),
    _T ("digits"),
    _T ("dim"),
    _T ("dimension"),
    _T ("do"),
    _T ("dot_product"),
    _T ("double"),
    _T ("doubleprecision"),
    _T ("dprod"),
    _T ("else"),
    _T ("elseif"),
    _T ("elsewhere"),
    _T ("end"),
    _T ("endblock"),
    _T ("endblockdata"),
    _T ("enddo"),
    _T ("endfile"),
    _T ("endif"),
    _T ("endinterface"),
    _T ("endselect"),
    _T ("endtype"),
    _T ("endwhere"),
    _T ("entry"),
    _T ("eoshift"),
    _T ("epsilon"),
    _T ("equivalence"),
    _T ("exit"),
    _T ("exp"),
    _T ("exponent"),
    _T ("external"),
    _T ("file"),
    _T ("floor"),
    _T ("format."),
    _T ("fraction"),
    _T ("function"),
    _T ("go"),
    _T ("goto"),
    _T ("huge"),
    _T ("iachar"),
    _T ("iand"),
    _T ("ibclr"),
    _T ("ibits"),
    _T ("ibset"),
    _T ("ichar"),
    _T ("ieor"),
    _T ("if"),
    _T ("implicit"),
    _T ("in"),
    _T ("index"),
    _T ("inout"),
    _T ("inquire"),
    _T ("int"),
    _T ("integer"),
    _T ("intent"),
    _T ("interface"),
    _T ("intrinsic"),
    _T ("iolength"),
    _T ("ior"),
    _T ("ishftc"),
    _T ("ishift"),
    _T ("kind"),
    _T ("lbound"),
    _T ("len"),
    _T ("len_trim"),
    _T ("lge"),
    _T ("lgt"),
    _T ("lle"),
    _T ("llt"),
    _T ("log"),
    _T ("log10"),
    _T ("logical"),
    _T ("matmul"),
    _T ("max"),
    _T ("maxexponent"),
    _T ("maxloc"),
    _T ("maxval"),
    _T ("merge"),
    _T ("min"),
    _T ("minexponent"),
    _T ("minloc"),
    _T ("minval"),
    _T ("mod"),
    _T ("module"),
    _T ("modulo"),
    _T ("mvbits"),
    _T ("namelist"),
    _T ("nearest"),
    _T ("nint"),
    _T ("none"),
    _T ("nullify"),
    _T ("only"),
    _T ("open"),
    _T ("operator"),
    _T ("optional"),
    _T ("out"),
    _T ("pack"),
    _T ("parameter"),
    _T ("pointer"),
    _T ("precision"),
    _T ("present"),
    _T ("print"),
    _T ("private"),
    _T ("procedure"),
    _T ("product"),
    _T ("program"),
    _T ("public"),
    _T ("radix"),
    _T ("random_number"),
    _T ("random_seed"),
    _T ("rangereal"),
    _T ("read"),
    _T ("real"),
    _T ("recursive"),
    _T ("repeat"),
    _T ("reshape"),
    _T ("result"),
    _T ("return"),
    _T ("rewrind"),
    _T ("rrspacing"),
    _T ("save"),
    _T ("scale"),
    _T ("scan"),
    _T ("select"),
    _T ("selectcase"),
    _T ("selected_int_kind"),
    _T ("selected_real_kind"),
    _T ("sequence"),
    _T ("set_exponent"),
    _T ("shape"),
    _T ("sign"),
    _T ("sin"),
    _T ("sinh"),
    _T ("size"),
    _T ("spacing"),
    _T ("spread"),
    _T ("sqrt"),
    _T ("stop"),
    _T ("subroutine"),
    _T ("sum"),
    _T ("system_clock"),
    _T ("tan"),
    _T ("tanh"),
    _T ("target"),
    _T ("then"),
    _T ("tiny"),
    _T ("to"),
    _T ("transfer"),
    _T ("transpose"),
    _T ("trim"),
    _T ("type"),
    _T ("unbound"),
    _T ("unpack"),
    _T ("use"),
    _T ("verify"),
    _T ("where"),
    _T ("while"),
    _T ("write"),
  };

static bool
IsFortranKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszFortranKeywordList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLineFortran (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
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

      if (pszChars[I] == '!' || !I && (pszChars[I] == 'C' || pszChars[I] == 'c'))
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
              if (IsFortranKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsFortranKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
    }

  if (pszChars[nLength - 1] != '\\' || IsMBSTrail(pszChars, nLength - 1))
    dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
