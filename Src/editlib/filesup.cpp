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

#include "stdafx.h"
#include "filesup.h"

#ifdef  _DEBUG
#include "filesup.inl"
#endif // _DEBUG

#ifdef INLINE
#undef INLINE
#endif
#define INLINE

////////////////////////////////////////////////////////////////////////////////

bool FileExist(LPCTSTR lpszPath)
{
  CFileStatus status;
  return CFile::GetStatus(lpszPath, status) != 0;
}

INLINE int
GetExtPosition (LPCTSTR pszString)
{
  if (!pszString || !*pszString)
    return 0;
  const CString sString = pszString;
  int len = sString.GetLength (), posit = len;
  TCHAR test;
  do
    if ((test = sString.GetAt (--posit)) == _T ('.'))
      return posit;
  while (posit && test != _T ('\\') && test != _T (':'));
  return len;
}

INLINE CString
GetExt (CString sString)
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

INLINE CString
GetName (const CString & sString)
{
  int nPosition = GetNamePosition (sString), nPosition2 = GetExtPosition (sString);

  return sString.IsEmpty ()? sString : (nPosition2 == sString.GetLength ()? sString.Mid (nPosition) : sString.Mid (nPosition, nPosition2 - nPosition));
}

INLINE CString
GetNameExt (const CString & sString)
{
  return sString.IsEmpty ()? sString : sString.Mid (GetNamePosition (sString));
}

INLINE int
GetNamePosition (LPCTSTR pszString)
{
  if (!pszString || !*pszString)
    return 0;
  const CString sString = pszString;
  int posit = sString.GetLength ();
  TCHAR test;
  do
    if ((test = sString.GetAt (--posit)) == _T ('\\') || test == _T (':'))
      return posit + 1;
  while (posit);
  return posit;
}

INLINE CString
GetPath (const CString & sString, BOOL bClose /*= FALSE*/ )
{
  if (sString.IsEmpty ())
    return sString;
  int posit = GetNamePosition (sString);
  if (posit == 0)
    return bClose ? _T (".\\") : _T (".");

  TCHAR test = sString.GetAt (posit - 1);

  if (test == _T (':') || test == _T ('\\') && (posit == 1 || posit != 1 && sString.GetAt (posit - 2) == _T (':')))
    return sString.Left (posit);
  return sString.Left (bClose ? posit : test == _T (':') ? posit : posit - 1);
}

INLINE CString
GetPathName (const CString & sString)
{
  int nPosition = GetExtPosition (sString);

  return sString.IsEmpty ()? sString : (nPosition == sString.GetLength ()? sString : sString.Left (nPosition));
}

////////////////////////////////////////////////////////////////////////////////
