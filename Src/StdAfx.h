// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <afxsock.h>
#include <afxole.h>
#include <afxtempl.h>
#include <afxpriv.h>
#include <afxole.h>
#include <afxmt.h>
#include <afxrich.h>

#ifndef countof
#define countof(array)  (sizeof(array)/sizeof((array)[0]))
#endif /* countof */

int xisspecial (int c);
int xisalpha (int c);
int xisalnum (int c);

CString LoadResString(int id);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
