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

#pragma once

#include "ctchar.h"

/////////////////////////////////////////////////////////////////////////////

bool EDITPADC_CLASS FileExist(const tchar_t* lpszPath);
int EDITPADC_CLASS GetExtPosition (const tchar_t* pszString);
CString EDITPADC_CLASS GetExt (CString sString);
CString EDITPADC_CLASS GetName (const CString & sString);
CString EDITPADC_CLASS GetNameExt (const CString & sString);
int EDITPADC_CLASS GetNamePosition (const tchar_t* pszString);
CString EDITPADC_CLASS GetPath (const CString & sString, bool bClose = false);
CString EDITPADC_CLASS GetPathName (const CString & sString);
