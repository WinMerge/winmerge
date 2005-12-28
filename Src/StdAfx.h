// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
/**
 * @file  Src/StdAfx.h
 *
 * @brief Project-wide includes and declarations
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// Common MFC headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>         // MFC support for Windows Common Controls

#include <afxtempl.h>       // MFC C++ template collection classes
#include <afxpriv.h>        // MFC private declarations (crystal text needs but doesn't include this)
#include <afxole.h>         // MFC OLE (COM) support


// Miscellaneous macros

#ifndef countof
#define countof(array)  (sizeof(array)/sizeof((array)[0]))
#endif /* countof */

/**
 * @name User-defined Windows-messages
 */
/* @{ */
/// Directory compare thread asks UI (view) update
const UINT MSG_UI_UPDATE = WM_USER + 1;
/// New item compared, update statepane
const UINT MSG_STAT_UPDATE = WM_USER + 2;
/* @} */

/// Seconds ignored in filetime differences if option enabled
static const UINT SmallTimeDiff = 2;

// Miscellaneous functions defined in StdAfx.cpp

int xisspecial (int c);
int xisalpha (int c);
int xisalnum (int c);
int xisspace (int c);

	/** @brief Load string from string resources; shortcut for CString::LoadString */
CString LoadResString(int id);

	/** @brief Format single-argument resource string and display via AfxMessageBox */
int ResMsgBox1(int msgid, LPCTSTR arg, UINT nType = MB_OK, UINT nIDHelp = 0);

	/** @brief Retrieve error description from Windows; uses FormatMessage */
CString GetSysError(int nerr);

	/** @brief Send message to log file (in theory; actually doesn't yet) */
void LogErrorString(LPCTSTR sz);

	/** @brief Inline sprintf-style format; shortcut for CString::Format */
CString Fmt(LPCTSTR fmt, ...);

	/** @brief Get appropriate clipboard format for TCHAR text, ie, CF_TEXT or CF_UNICODETEXT */
int GetClipTcharTextFormat();

class CLogFile;
extern CLogFile gLog;

	/** @brief Shortcuts for common gLog messages */
struct gLog
{
	static UINT DeleteFileFailed(LPCTSTR path);
};

bool IsUnicodeBuild();

	/** @brief include for the custom dialog boxes, with do not ask/display again */
#include "MessageBoxDialog.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
