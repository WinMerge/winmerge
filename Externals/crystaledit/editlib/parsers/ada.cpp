///////////////////////////////////////////////////////////////////////////
//  File:       ada.cpp
//  Version:    1.0
//  Created:    27-Nov-2024
//
//  Copyright:  Javier Miguel, portions by H.Saido, Tim Gerundt, Stcherbatchenko Andrei
//  E-mail:     javiermiguel4b@gmail.com
//
//  Ada syntax highlighing definition
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

//  Ada keywords
static const tchar_t * s_apszAdaKeywordList[] =
  {
    _T ("abort"),
    _T ("abs"),
    _T ("abstract"),
    _T ("accept"),
    _T ("access"),
    _T ("aliased"),
    _T ("all"),
    _T ("and"),
    _T ("array"),
    _T ("at"),
    _T ("begin"),
    _T ("body"),
    _T ("case"),
    _T ("constant"),
    _T ("declare"),
    _T ("delay"),
    _T ("delta"),
    _T ("digits"),
    _T ("do"),
    _T ("else"),
    _T ("elsif"),
    _T ("end"),
    _T ("entry"),
    _T ("exception"),
    _T ("exit"),
    _T ("for"),
    _T ("function"),
    _T ("generic"),
    _T ("goto"),
    _T ("if"),
    _T ("in"),
    _T ("interface"),
    _T ("is"),
    _T ("limited"),
    _T ("loop"),
    _T ("mod"),
    _T ("new"),
    _T ("not"),
    _T ("null"),
    _T ("of"),
    _T ("or"),
    _T ("others"),
    _T ("out"),
    _T ("overriding"),
    _T ("package"),
    _T ("parallel"),
    _T ("pragma"),
    _T ("private"),
    _T ("procedure"),
    _T ("protected"),
    _T ("raise"),
    _T ("range"),
    _T ("record"),
    _T ("rem"),
    _T ("renames"),
    _T ("requeue"),
    _T ("return"),
    _T ("reverse"),
    _T ("select"),
    _T ("separate"),
    _T ("some"),
    _T ("subtype"),
    _T ("synchronized"),
    _T ("tagged"),
    _T ("task"),
    _T ("terminate"),
    _T ("then"),
    _T ("type"),
    _T ("until"),
    _T ("use"),
    _T ("when"),
    _T ("while"),
    _T ("with"),
    _T ("xor")
};

static const tchar_t * s_apszAdaAttributeList[] =
  {
    _T ("'access"),
    _T ("'address"),
    _T ("'adjacent"),
    _T ("'aft"),
    _T ("'alignment"),
    _T ("'base"),
    _T ("'bit_Order"),
    _T ("'body_Version"),
    _T ("'callable"),
    _T ("'caller"),
    _T ("'ceiling"),
    _T ("'class"),
    _T ("'component_Size"),
    _T ("'compose"),
    _T ("'constrained"),
    _T ("'copy_Sign"),
    _T ("'count"),
    _T ("'definite"),
    _T ("'delta"),
    _T ("'denorm"),
    _T ("'digits"),
    _T ("'emax"),
    _T ("'enum_Rep"),
    _T ("'enum_Val"),
    _T ("'epsilon"),
    _T ("'exponent"),
    _T ("'external_Tag"),
    _T ("'first"),
    _T ("'first_Bit"),
    _T ("'floor"),
    _T ("'fore"),
    _T ("'fraction"),
    _T ("'has_Same_Storage"),
    _T ("'identity"),
    _T ("'image"),
    _T ("'index"),
    _T ("'input"),
    _T ("'large"),
    _T ("'last"),
    _T ("'last_Bit"),
    _T ("'leading_Part"),
    _T ("'length"),
    _T ("'machine"),
    _T ("'machine_Emax"),
    _T ("'machine_Emin"),
    _T ("'machine_Mantissa"),
    _T ("'machine_Overflows"),
    _T ("'machine_Radix"),
    _T ("'machine_Rounding"),
    _T ("'machine_Rounds"),
    _T ("'mantissa"),
    _T ("'max"),
    _T ("'max_Alignment_For_Allocation"),
    _T ("'max_Size_In_Storage_Elements"),
    _T ("'min"),
    _T ("'mod"),
    _T ("'model"),
    _T ("'model_Emin"),
    _T ("'model_Epsilon"),
    _T ("'model_Mantissa"),
    _T ("'model_Small"),
    _T ("'modulus"),
    _T ("'object_Size"),
    _T ("'old"),
    _T ("'output"),
    _T ("'overlaps_Storage"),
    _T ("'parallel_Reduce"),
    _T ("'partition_ID"),
    _T ("'pos"),
    _T ("'position"),
    _T ("'pred"),
    _T ("'preelaborate_Initialization"),
    _T ("'priority"),
    _T ("'put_Image"),
    _T ("'range"),
    _T ("'read"),
    _T ("'reduce"),
    _T ("'relative_Deadline"),
    _T ("'remainder"),
    _T ("'result"),
    _T ("'round"),
    _T ("'rounding"),
    _T ("'safe_Emax"),
    _T ("'safe_First"),
    _T ("'safe_Large"),
    _T ("'safe_Last"),
    _T ("'safe_Small"),
    _T ("'scale"),
    _T ("'scaling"),
    _T ("'signed_Zeros"),
    _T ("'size"),
    _T ("'small"),
    _T ("'storage_Pool"),
    _T ("'storage_Size"),
    _T ("'stream_Size"),
    _T ("'succ"),
    _T ("'tag"),
    _T ("'terminated"),
    _T ("'truncation"),
    _T ("'unbiased_Rounding"),
    _T ("'unchecked_Access"),
    _T ("'val"),
    _T ("'valid"),
    _T ("'value"),
    _T ("'version"),
    _T ("'wide_Image"),
    _T ("'wide_Value"),
    _T ("'wide_Wide_Image"),
    _T ("'wide_Wide_Value"),
    _T ("'wide_Wide_Width"),
    _T ("'wide_Width"),
    _T ("'width"),
    _T ("'write")
  };

//  Ada Types
static const tchar_t * s_apszAdaTypeList[] =
  {
    _T ("boolean"),
    _T ("duration")
    _T ("float"),
    _T ("integer"),
    _T ("long_float"),
    _T ("long_integer"),
    _T ("long_long_float"),
    _T ("long_long_integer"),
    _T ("natural"),
    _T ("positive"),
    _T ("short_float"),
    _T ("short_integer"),
    _T ("short_short_integer"),
    _T ("string"),
    _T ("wide_string"),
  };

static bool
IsAdaKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI(s_apszAdaKeywordList, pszChars, nLength);
}

static bool
IsAdaAttribute (const tchar_t *pszChars, int nLength, int *nAttributeBegin)
{
  for (int I = 0; I < nLength; I++)
    {
      if (pszChars[I] == '\'')
        {
          *nAttributeBegin = I;
          return ISXKEYWORDI(s_apszAdaAttributeList, &pszChars[I], nLength-I);
        }
   }
  return false;
}

static bool
IsAdaType (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI(s_apszAdaTypeList, pszChars, nLength);
}

static bool
IsDigitBase (tchar_t cChar, int nBase) {
  if (nBase <= 10)
    {
      return cChar >= '0' && cChar < ('0' + nBase);
    }
  else
    {
      return (cChar >= '0' && cChar <= '9') ||
        (tc::totupper(cChar) >= 'A' && tc::totupper(cChar) < ('A' + nBase - 10));
    }
}

static bool
IsAdaNumber (const tchar_t* pszChars, int nLength) {
  int I = 0;
  bool bHasDigits = false;
  bool bHasDot = false;
  bool bHasExponent = false;
  bool bInExponent = false;
  bool bExponentHasSign = false;
  bool bInBaseMode = false;
  int nBase = 10;  // Default to base 10

  // Handle Base-N format (e.g., "2#101#", "16#B#E2")
  if (nLength > 1 && pszChars[0] >= '2' && pszChars[0] <= '9' && pszChars[1] == '#')
    {
      nBase = pszChars[I] - '0';
      I++;
      if (pszChars[I] == '#') {
          bInBaseMode = true;
          I++;
      } else {
          // Invalid format (e.g., "2abc" without a valid base enclosure)
          return false;
      }
    }
  else if (pszChars[0] == '1' && nLength > 2 && pszChars[2] == '#')
    {
      // Base could be between 10 and 16
      I++;
      if (pszChars[I] >= '0' && pszChars[I] <= '6')
        {
          nBase = 10 + (pszChars[I] - '0');
          I++;
          if (pszChars[I] == '#') {
              bInBaseMode = true;
              I++;
          } else {
              return false;
          }
        }
      else if (pszChars[I] == '#')
        {
          bInBaseMode = true;
          I++;
        }
    }

  while (I < nLength)
    {
      if (pszChars[I] == '_')
        {
          // Ignore underscores as they are just for legibility
          I++;
          continue;
        }

      if (bInBaseMode)
        {
          // We're in base-N mode
          if (pszChars[I] == '#')
            {
              // End of base-N number, switch to exponent check
              bInBaseMode = false;
              I++;
              break;
            }
          if (!IsDigitBase(pszChars[I], nBase))
            {
              return false;  // Invalid character for the current base
            }
          bHasDigits = true;
        }
      else
        {
          // Base 10 or exponent handling
          if (tc::istdigit(pszChars[I]))
            {
              bHasDigits = true;
            }
          else if (pszChars[I] == '.')
            {
              if (bHasDot || bHasExponent)
                {
                  return false;  // Only one decimal allowed, no dot after exponent
                }
              bHasDot = true;
            }
          else if (pszChars[I] == 'e' || pszChars[I] == 'E')
            {
              if (bHasExponent || !bHasDigits)
                {
                  return false;  // Only one exponent allowed, must come after digits
                }
              bHasExponent = true;
              bInExponent = true;
              bHasDigits = false;  // Reset digits check for exponent part
            }
          else if (pszChars[I] == '+' || pszChars[I] == '-')
            {
              if (!bInExponent || bExponentHasSign || bHasDigits)
                {
                  return false;  // Sign must be right after 'e' and before digits in exponent
                }
              if (!bHasDot && pszChars[I] == '-')
                {
                  return false; // Not allowed by standard
                }
              bExponentHasSign = true;
            }
          else
            {
              return false;  // Invalid character for a base-10 number
            }
        }
      I++;
    }

  // Closing '#' not reached
  if (bInBaseMode)
    {
      return false;
    }
  return bHasDigits;
}

static bool
IsAdaChar (const tchar_t *pszChars, int nLength)
{
  return (nLength == 3 && pszChars[0] == '\'' && pszChars[2] == '\'');
}

static inline void
DefineIdentiferBlock(const tchar_t *pszChars, int nLength, CrystalLineParser::TEXTBLOCK * pBuf, int &nActualItems, int nIdentBegin, int I, unsigned &dwCookie, int &nAttributeBegin)
{
  if (IsAdaNumber (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
    }
  else if (pszChars[nIdentBegin] == '"')
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_STRING);
      dwCookie |= COOKIE_STRING;
    }
  else if (IsAdaKeyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
    }
  else if (IsAdaAttribute (pszChars + nIdentBegin, I - nIdentBegin, &nAttributeBegin))
    {
      DEFINE_BLOCK (nIdentBegin + nAttributeBegin, COLORINDEX_FUNCNAME);
    }
  else if (IsAdaType (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_PREPROCESSOR);
    }
  else if (IsAdaChar (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_STRING);
    }
}

unsigned
CrystalLineParser::ParseLineAda (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nIdentBegin = -1;
  int nAttributeBegin = 0;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = static_cast<int>(tc::tcharnext(pszChars+I) - pszChars))
    {
      if (I == nPrevI)
        {
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
          else if (dwCookie & (COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else if (dwCookie & COOKIE_PREPROCESSOR)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_PREPROCESSOR);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*tc::tcharprev(pszChars, pszChars + nPos)) && !xisalpha (*tc::tcharnext(pszChars + nPos))))
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
                }
              else if (pszChars[nPos] == '"' || pszChars[nPos] == '\'')
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
      if (I >= nLength)
        break;

      if (dwCookie & COOKIE_COMMENT)
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      // Line comment --...
      if (I > 0 && pszChars[I] == '-' && pszChars[nPrevI] == '-')
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
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

      if (pBuf == nullptr)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha (pszChars[nPrevI]) && !xisalpha (pszChars[I + 1])))
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else if (pszChars[I] == '"' || pszChars[I] == '\'')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I, dwCookie, nAttributeBegin);
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I, dwCookie, nAttributeBegin);
    }

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
}
