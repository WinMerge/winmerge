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

#define WINVER 0x400
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// Disable VC6's "identifier was truncated..." warning. It is meaningless.
#if _MSC_VER == 1200
	#pragma warning(disable: 4786)
#endif
 
// Declare this function for VC6 and VS2002
// We dont' know if the function exists in VS2002 so we'd better be safe here!
#if _MSC_VER <= 1300
	__int64 _abs64_patch(__int64 n);
#endif

// A patch to solve a VC 7.1 bug. It seems that it doesn't export _abs64
// function.
#if _MSC_VER <= 1310
#define _abs64	_abs64_patch
#endif

//Make VS2005 build less noisy by disabling warnings for deprecated string functions.
#if _MSC_VER >= 1400
	#define _CRT_SECURE_NO_DEPRECATE
#endif

// Common MFC headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>         // MFC support for Windows Common Controls

#include <afxtempl.h>       // MFC C++ template collection classes
#include <afxpriv.h>        // MFC private declarations (crystal text needs but doesn't include this)
#include <afxole.h>         // MFC OLE (COM) support

// For CSizingControlBar
#include "sizecbar.h"
#include "scbarg.h"

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
/// Request to save panesizes
const UINT MSG_STORE_PANESIZES = WM_USER + 2;
/* @} */

/**
 * @brief Indexes to Merge views (file compare).
 * We currently have two views, left and right, but in future there
 * can be more views.
 */
enum MERGE_VIEW_ID
{
	MERGE_VIEW_LEFT, /**< Left view */
	MERGE_VIEW_RIGHT, /**< Right view */
	MERGE_VIEW_COUNT, /**< Count of views */
};

/// Seconds ignored in filetime differences if option enabled
static const UINT SmallTimeDiff = 2;

// Miscellaneous functions defined in StdAfx.cpp

int xisspecial (wint_t c);
int xisalpha (wint_t c);
int xisalnum (wint_t c);
int xisspace (wint_t c);

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

bool IsUnicodeBuild();

	/** @brief include for the custom dialog boxes, with do not ask/display again */
#include "MessageBoxDialog.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
