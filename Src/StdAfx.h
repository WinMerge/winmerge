// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
/**
 * @file  Src/StdAfx.h
 *
 * @brief Project-wide includes and declarations
 */
// ID line follows -- this is updated by SVN
// $Id: StdAfx.h 6457 2009-02-15 14:08:50Z kimmov $

#if !defined(AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1500
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

// Set WINVER for VC6, VS2005 and VS2008
//On VS2003 default is set to 0x0501 (Windows XP and Windows .NET Server)
//On VS2005 default is set to 0x0502 (Windows Server 2003)
//On VS2008 default is set to 0x0600 (Windows VISTA)
//
// Set WINVER below or in project properties (for preprocessor)

#if _MSC_VER < 1300
#	define WINVER 0x0400
#elif _MSC_VER == 1400
	// VS2005 is too noisy when _MSC_VER is not set
#	if !defined(WINVER)
#		define WINVER 0x0501
#	endif
#elif _MSC_VER >= 1500
	// On Win XP, with VS2008, do not use default WINVER 0x0600 because of 
	// some windows structure used in API (on VISTA they are longer)
#	if !defined(WINVER)
#		define WINVER 0x0501
#	endif /* !defined(WINVER) */
#endif /* _MSC_VER = 1500 */

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// Disable VC6's "identifier was truncated..." warning. It is meaningless.
#if _MSC_VER == 1200
	#pragma warning(disable: 4786)
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

#include "coretypes.h"

#define IF_IS_TRUE_ALL(expression, index, count) \
	for (index = 0; index < count; index++) { if ((expression)) ; else break; } \
	if (index == count)


/**
 * @name User-defined Windows-messages
 */
/* @{ */
/// Directory compare thread asks UI (view) update
const UINT MSG_UI_UPDATE = WM_USER + 1;
/// Request to save panesizes
const UINT MSG_STORE_PANESIZES = WM_USER + 2;
/* @} */

/// Seconds ignored in filetime differences if option enabled
static const UINT SmallTimeDiff = 2;

#include "UnicodeString.h"
#include "MergeApp.h"

	/** @brief Wrapper around CMergeApp::TranslateDialog() */
void NTAPI LangTranslateDialog(HWND);

	/** @brief Lang aware version of AfxMessageBox() */
int NTAPI LangMessageBox(UINT, UINT nType = MB_OK, UINT nIDHelp = (UINT)-1);

	/** @brief Format single-argument resource string and display via AfxMessageBox */
int ResMsgBox1(UINT msgid, LPCTSTR arg, UINT nType = MB_OK, UINT nIDHelp = 0);

	/** @brief Get appropriate clipboard format for TCHAR text, ie, CF_TEXT or CF_UNICODETEXT */
int GetClipTcharTextFormat();

	/** @brief include for the custom dialog boxes, with do not ask/display again */
#include "MessageBoxDialog.h"

#ifdef _MAX_PATH
#  undef _MAX_PATH
#endif
#define _MAX_PATH (260 * sizeof(wchar_t) / sizeof(TCHAR))

#ifdef MAX_PATH
#  undef MAX_PATH
#endif
#define MAX_PATH (260 * sizeof(wchar_t) / sizeof(TCHAR))

#define WMPROFILE(x) CWinMergeProfile __wmtl__(x)

class CWinMergeProfile
{
private:
	LARGE_INTEGER li[2];
	LARGE_INTEGER freq;
	TCHAR funcname[256];
public:
	CWinMergeProfile(LPCTSTR pFuncName) {
		lstrcpy(funcname, pFuncName);
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&li[0]);
	}
	~CWinMergeProfile() {
		QueryPerformanceCounter(&li[1]);
		TCHAR buf[256];
		wsprintf(buf, _T("funcname=%s t=%d[us]\n"), funcname, (int)((double)(li[1].QuadPart-li[0].QuadPart)/freq.QuadPart*1000.0*1000.0));
		OutputDebugString(buf);
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BBCD4F8A_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
