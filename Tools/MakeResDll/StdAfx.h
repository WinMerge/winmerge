// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A0529471_F288_11D2_826F_00A024706EDC__INCLUDED_)
#define AFX_STDAFX_H__A0529471_F288_11D2_826F_00A024706EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <iostream>
#include <afxsock.h>

// TODO: reference additional headers your program requires here

CString LoadResString(int id);
class CRegKeyEx;
bool RegOpenMachine(CRegKeyEx & reg, LPCTSTR key);
bool RegOpenUser(CRegKeyEx & reg, LPCTSTR key);

enum VS_VERSION { VS_NONE=-1, VS_6, VS_2002, VS_2003, VS_2005, VS_COUNT };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

	/** @brief Get appropriate clipboard format for TCHAR text, ie, CF_TEXT or CF_UNICODETEXT */
int GetClipTcharTextFormat();

#endif // !defined(AFX_STDAFX_H__A0529471_F288_11D2_826F_00A024706EDC__INCLUDED_)
