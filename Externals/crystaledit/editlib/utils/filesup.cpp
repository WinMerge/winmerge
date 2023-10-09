///////////////////////////////////////////////////////////////////////////
//  File:    filesup.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Some handy stuff to deal with files and their names
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "filesup.h"

////////////////////////////////////////////////////////////////////////////////

int GetExtPosition (const tchar_t* pszString)
{
  if (pszString == nullptr || !*pszString)
    return 0;
  const std::basic_string<tchar_t> sString = pszString;
  int len = static_cast<int>(sString.length ()), posit = len;
  tchar_t test;
  do
    if ((test = sString.at (--posit)) == _T ('.'))
      return posit;
#ifdef _UNICODE
  while (posit && test != _T ('\\') && test != _T (':'));
#else
  while (posit && (test != _T ('\\') || _ismbstrail((unsigned char *)pszString, (unsigned char *)pszString + posit)) && test != _T (':'));
#endif
  return len;
}

std::basic_string<tchar_t> GetExt (const std::basic_string<tchar_t>& sString)
{
  std::basic_string<tchar_t> sString2 = sString;
  if (!sString2.empty ())
    {
      sString2 = sString2.substr (GetExtPosition (sString2.c_str ()));
      if (!sString2.empty () && sString2[0] == _T ('.'))
        {
          sString2 = sString2.substr (1);
        }
    }
  return sString2;
}

////////////////////////////////////////////////////////////////////////////////
