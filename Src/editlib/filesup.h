///////////////////////////////////////////////////////////////////////////
//  File:    filesup.h
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

#ifndef __FILESUP_H__INCLUDED__
#define __FILESUP_H__INCLUDED__

/////////////////////////////////////////////////////////////////////////////

bool EDITPADC_CLASS FileExist(LPCTSTR lpszPath);
int EDITPADC_CLASS GetExtPosition (LPCTSTR pszString);
CString EDITPADC_CLASS GetExt (CString sString);
CString EDITPADC_CLASS GetName (const CString & sString);
CString EDITPADC_CLASS GetNameExt (const CString & sString);
int EDITPADC_CLASS GetNamePosition (LPCTSTR pszString);
CString EDITPADC_CLASS GetPath (const CString & sString, BOOL bClose = FALSE);
CString EDITPADC_CLASS GetPathName (const CString & sString);

/////////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG
#include "filesup.inl"
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////

#endif // __FILESUP_H__INCLUDED__
