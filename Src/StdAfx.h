// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
/**
 * @file  Src/StdAfx.h
 *
 * @brief Project-wide includes and declarations
 */
#pragma once

// On Win XP, with VS2008, do not use default WINVER 0x0600 because of 
// some windows structure used in API (on VISTA they are longer)
#if !defined(WINVER)
#  define WINVER 0x0501
#endif /* !defined(WINVER) */

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// Common MFC headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>         // MFC support for Windows Common Controls

#include <afxtempl.h>       // MFC C++ template collection classes
#include <afxpriv.h>        // MFC private declarations (crystal text needs but doesn't include this)
#include <afxole.h>         // MFC OLE (COM) support

#include <atlimage.h>

// For CSizingControlBar
#include "sizecbar.h"
#include "scbarg.h"

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <stack>
#include <list>
#include <array>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <cassert>
#include <ctime>

/**
 * @name User-defined Windows-messages
 */
/* @{ */
/// Directory compare thread asks UI (view) update
const UINT MSG_UI_UPDATE = WM_USER + 1;
/// Request to save panesizes
const UINT MSG_STORE_PANESIZES = WM_USER + 2;
/// Request to generate file compare report
const UINT MSG_GENERATE_FLIE_COMPARE_REPORT = WM_USER + 3;
/* @} */

/// Seconds ignored in filetime differences if option enabled
static const UINT SmallTimeDiff = 2;

#include "UnicodeString.h"
#include "MergeApp.h"

	/** @brief Wrapper around CMergeApp::TranslateDialog() */
void NTAPI LangTranslateDialog(HWND);

	/** @brief Lang aware version of AfxMessageBox() */
int NTAPI LangMessageBox(UINT, UINT nType = MB_OK, UINT nIDHelp = (UINT)-1);

	/** @brief include for the custom dialog boxes, with do not ask/display again */
#include "MessageBoxDialog.h"

#ifdef _MAX_PATH
#  undef _MAX_PATH
#endif
#define _MAX_PATH (260 * sizeof(wchar_t) / sizeof(tchar_t))

#ifdef MAX_PATH
#  undef MAX_PATH
#endif
#define MAX_PATH (260 * sizeof(wchar_t) / sizeof(tchar_t))

#ifdef MAX_PATH_FULL
#  undef MAX_PATH_FULL
#endif
#define MAX_PATH_FULL (32767 * sizeof(wchar_t) / sizeof(tchar_t))

#define WMPROFILE(x) CWinMergeProfile __wmtl__(x)

class CWinMergeProfile
{
private:
	static int level;
	static CMapStringToPtr map;
	static LARGE_INTEGER origin;
	LARGE_INTEGER li[2];
	LARGE_INTEGER freq;
	tchar_t funcname[256];
public:
	explicit CWinMergeProfile(const tchar_t* pFuncName) {
		tchar_t buf[256];
		_stprintf_s(buf, _T("%-*s funcname=%s Start\n"), level, L"", pFuncName);
		OutputDebugString(buf);
		lstrcpy(funcname, pFuncName);
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&li[0]);
		if (origin.QuadPart == 0)
			origin = li[0];
		++level;
	}
	~CWinMergeProfile() {
		QueryPerformanceCounter(&li[1]);
		tchar_t buf[256];
		level--;
		int elapsed = (int)((double)(li[1].QuadPart - li[0].QuadPart) / freq.QuadPart*1000.0*1000.0);
		int tim = (int)((double)(li[1].QuadPart - origin.QuadPart) / freq.QuadPart*1000.0*1000.0);
		struct stat {
			int sum = 0;
			int count = 0;
		} *pstat;
		void *pstatv = nullptr;
		if (!map.Lookup(funcname, pstatv))
		{
			pstat = new stat();
			map[funcname] = (void *)pstat;
		}
		else
		{
			pstat = reinterpret_cast<stat *>(pstatv);
		}
		pstat->sum += elapsed;
		pstat->count++;
		_stprintf_s(buf, _T("%-*s funcname=%s t=%d[us] count=%d sum=%d[us] time=%g[ms]\n"), level, L"", funcname, elapsed, pstat->count, pstat->sum, tim/1000.0);
		OutputDebugString(buf);
	}
	static void ResetTimer()
	{
		QueryPerformanceCounter(&origin);
	}
	static void Terminiate()
	{
		map.RemoveAll();
	}
};
