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

#include "StdAfx.h"
#include "filesup.h"

////////////////////////////////////////////////////////////////////////////////

bool FileExist(const tchar_t* lpszPath)
{
  CFileStatus status;
  return CFile::GetStatus(lpszPath, status) != 0;
}

int GetExtPosition (const tchar_t* pszString)
{
  if (pszString == nullptr || !*pszString)
    return 0;
  const CString sString = pszString;
  int len = sString.GetLength (), posit = len;
  tchar_t test;
  do
    if ((test = sString.GetAt (--posit)) == _T ('.'))
      return posit;
#ifdef _UNICODE
  while (posit && test != _T ('\\') && test != _T (':'));
#else
  while (posit && (test != _T ('\\') || _ismbstrail((unsigned char *)pszString, (unsigned char *)pszString + posit)) && test != _T (':'));
#endif
  return len;
}

CString GetExt (CString sString)
{
  if (!sString.IsEmpty ())
    {
      sString = sString.Mid (GetExtPosition (sString));
      if (!sString.IsEmpty () && sString[0] == _T ('.'))
        {
          sString = sString.Mid (1);
        }
    }
  return sString;
}

CString GetName (const CString & sString)
{
  int nPosition = GetNamePosition (sString), nPosition2 = GetExtPosition (sString);

  return sString.IsEmpty ()? sString : (nPosition2 == sString.GetLength ()? sString.Mid (nPosition) : sString.Mid (nPosition, nPosition2 - nPosition));
}

CString GetNameExt (const CString & sString)
{
  return sString.IsEmpty ()? sString : sString.Mid (GetNamePosition (sString));
}

int GetNamePosition (const tchar_t* pszString)
{
  if (pszString == nullptr || !*pszString)
    return 0;
  const CString sString = pszString;
  int posit = sString.GetLength ();
  do
  {
    tchar_t test;
#ifdef _UNICODE
    if ((test = sString.GetAt (--posit)) == _T ('\\') || test == _T (':'))
#else
    if (((test = sString.GetAt (--posit)) == _T ('\\') && !_ismbstrail((unsigned char *)pszString, (unsigned char *)pszString + posit)) || test == _T (':'))
#endif
      return posit + 1;
  }
  while (posit);
  return posit;
}

CString GetPath (const CString & sString, bool bClose /*= false*/ )
{
  if (sString.IsEmpty ())
    return sString;
  int posit = GetNamePosition (sString);
  if (posit == 0)
    return bClose ? _T (".\\") : _T (".");

  tchar_t test = sString.GetAt (posit - 1);

#ifdef _UNICODE
  if (test == _T (':') || test == _T ('\\') && (posit == 1 || sString.GetAt (posit - 2) == _T (':')))
#else
  if (test == _T (':') || (test == _T ('\\') && !_ismbstrail((unsigned char *)(const tchar_t*)sString, (unsigned char *)(const tchar_t*)sString + posit)) && (posit == 1 || sString.GetAt (posit - 2) == _T (':')))
#endif
    return sString.Left (posit);
  return sString.Left (bClose ? posit : test == _T (':') ? posit : posit - 1);
}

CString GetPathName (const CString & sString)
{
  int nPosition = GetExtPosition (sString);

  return sString.IsEmpty ()? sString : (nPosition == sString.GetLength ()? sString : sString.Left (nPosition));
}

////////////////////////////////////////////////////////////////////////////////
