/////////////////////////////////////////////////////////////////////////////
// dllver.cpp : implementation of version check for any DLL
//

#include "stdafx.h"
// Uses MFC C++ template collection classes
#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
// Need DLLGETVERSIONPROC from <shlwapi.h>
#include <shlwapi.h>
#include "dllver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CMap<CString, LPCTSTR, long, long> f_cache;


// Return major/minor version number from version resource (0 if not found)
DWORD getFixedModuleVersion(HMODULE hmod)
{
	// Do current exe if module not specified
	if (!hmod)
		hmod = ::GetModuleHandle(NULL);
	TCHAR filepath[MAX_PATH];
	if (!GetModuleFileName(hmod, filepath, sizeof(filepath)))
		return 0;
	DWORD dwDummyHandle;
	DWORD len = GetFileVersionInfoSize(filepath, &dwDummyHandle);
	CString sbuf;
	LPTSTR tbuff = sbuf.GetBuffer(len/sizeof(TCHAR));
	BYTE * buf = reinterpret_cast<BYTE *>(tbuff);
	if (!::GetFileVersionInfo(filepath, 0, len, buf))
		return 0;
	unsigned int verlen;
	LPVOID lpvi;
	if (!::VerQueryValue(buf, _T("\\"), &lpvi, &verlen))
		return 0;
	VS_FIXEDFILEINFO fileInfo;
	fileInfo = *(VS_FIXEDFILEINFO*)lpvi;
	DWORD ver = fileInfo.dwFileVersionMS;
	return ver;
}

// Load dll & return its major/minor version number
DWORD GetDllVersion_Raw(LPCTSTR lpszDllName)
{

	HINSTANCE hinstDll;
	DWORD dwVersion = 0;

	Library lib;
	if (!lib.Load(lpszDllName))
		return 0;
	hinstDll = lib.Inst();

	DLLGETVERSIONPROC pDllGetVersion;

	pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");

/*Because some DLLs might not implement this function, you
  must test for it explicitly. Depending on the particular 
  DLL, the lack of a DllGetVersion function can be a useful
  indicator of the version.
*/
	if (pDllGetVersion)
	{
		DLLVERSIONINFO dvi;
		HRESULT hr;

		ZeroMemory(&dvi, sizeof(dvi));
		dvi.cbSize = sizeof(dvi);

		hr = (*pDllGetVersion)(&dvi);

		if(SUCCEEDED(hr))
		{
			dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
			return dwVersion;

		}
	}
	// Fall back to the version resource
	dwVersion = getFixedModuleVersion(lib.Inst());

	return dwVersion;
}

// Return dll's version number
// Use cache if figured it out before
DWORD GetDllVersion(LPCTSTR lpszDllName)
{
	LONG ver=0;
	if (!f_cache.Lookup(lpszDllName, ver))
	{
		ver = GetDllVersion_Raw(lpszDllName);
		f_cache[lpszDllName] = ver;
	}
	return ver;
}

