// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__90247E26_68CE_4BA4_A7E2_B13CE471C2A0__INCLUDED_)
#define AFX_STDAFX_H__90247E26_68CE_4BA4_A7E2_B13CE471C2A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

CString MakeStr(LPCTSTR fmt, ...);

CString LoadResString(int id);

#ifndef countof
#define countof(array)  (sizeof(array)/sizeof((array)[0]))
#endif /* countof */


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__90247E26_68CE_4BA4_A7E2_B13CE471C2A0__INCLUDED_)
