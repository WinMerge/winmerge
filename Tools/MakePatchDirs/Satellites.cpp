/** 
 * @file  Satellites.cpp
 *
 * @date  Created: 2004-06-06 (Perry)
 * @date  Edited:  2004-06-06 (Perry)
 * @brief Code to load list of satellite resource dlls
 */

#include "StdAfx.h"
#include "Satellites.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void
Sats_LoadList(const CString& sDir, const CString & PathSpec, LangDllArray & langdlls)
{
	langdlls.RemoveAll();

	WIN32_FIND_DATA ffi;
	HANDLE hff = FindFirstFile(PathSpec, &ffi);
	
	if (  hff != INVALID_HANDLE_VALUE )
	{
		do
		{
			if (!(ffi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CString dll = ffi.cFileName;
				HINSTANCE hinst = LoadLibrary(dll);
				if (hinst)
				{
					LangDll langd;
					langd.dllpath = sDir + dll;
					WORD langid = 0;
					if (Sats_GetSatLang(langd.dllpath, langd.m_lang))
					{
						langdlls.Add(langd);
					}
					FreeLibrary(hinst);
				}

			}
		}
		while (FindNextFile(hff, &ffi));
		FindClose(hff);
	}
}

bool
Sats_GetSatLang(const CString& dll, WORD & lang)
{
	DWORD   dwVerHnd=0;			// An 'ignored' parameter, always '0'
	DWORD dwVerInfoSize = GetFileVersionInfoSize((LPTSTR)(LPCTSTR)dll, &dwVerHnd);
	if (!dwVerInfoSize) return false;

	HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
	if (!hMem) return false;
	LPTSTR vffino = (LPTSTR)GlobalLock(hMem);
	bool result=false;
	if (GetFileVersionInfo((LPTSTR)(LPCTSTR)dll, dwVerHnd, dwVerInfoSize, vffino))
	{
		LPWORD langInfo=0;
		DWORD langLen=0;
		if (VerQueryValue((LPVOID)vffino,
			_T("\\VarFileInfo\\Translation"),
			(LPVOID *)&langInfo, (UINT *)&langLen))
		{
			lang = langInfo[0];
			result = true;
		}
	}
	GlobalUnlock(hMem);
	GlobalFree(hMem);

	return result;
}
