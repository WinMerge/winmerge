/* dllpstub.cpp: Help implement DLL proxies
 * Copyright (c) 2005 Jochen Tucht
 *
 * License:	This program is free software; you can redistribute it and/or modify
 *			it under the terms of the GNU General Public License as published by
 *			the Free Software Foundation; either version 2 of the License, or
 *			(at your option) any later version.
 *
 *			This program is distributed in the hope that it will be useful,
 *			but WITHOUT ANY WARRANTY; without even the implied warranty of
 *			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *			GNU General Public License for more details.
 *
 *			You should have received a copy of the GNU General Public License
 *			along with this program; if not, write to the Free Software
 *			Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Please mind 2. a) of the GNU General Public License, and log your changes below.

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2005/02/26	Jochen Tucht		Created
2006/09/10	Kimmo Varis			Don't use 'export' as variable name
								Visual Studio 2005 warns about it.
*/

#include "stdafx.h"
#include <shlwapi.h>
#include <afxdisp.h>
#include <afxinet.h>
#include "dllpstub.h"

/**
 * @brief Throw DLLPSTUB related exception.
 */
void DLLPSTUB::Throw(LPCSTR name, HMODULE handle, DWORD dwError, BOOL bFreeLibrary)
{
	CString strError = name;
	if (handle)
	{
		TCHAR module[4096];
		module[0] = '@';
		if (::GetModuleFileName(handle, module + 1, 4095) == 0)
		{
			wsprintf(module + 1, _T("%08lX"), handle);
		}
		strError += strError.IsEmpty() ? module + 1 : module;
	}
	TCHAR szError[512];
	if (CInternetException(dwError).GetErrorMessage(szError + 2, 510))
	{
		szError[0] = ':';
		szError[1] = '\n';
		strError += szError;
	}
	if (bFreeLibrary)
	{
		FreeLibrary(handle);
	}
	AfxThrowOleDispatchException(0, strError);
}

/**
 * @brief Load a dll and import a number of functions.
 */
HMODULE DLLPSTUB::Load()
{
	// DLLPSTUB assumes that it is embedded in its owner
	// followed by a char array of the DLL name to load
	// so it access the char array via *(this + 1)
	LPCSTR *proxy = (LPCSTR *) (this + 1);
	HMODULE handle = NULL;
	if (LPCSTR name = *proxy)
	{
		if (proxy[1] && proxy[1] != name)
		{
			handle = LoadLibraryA(name);
			if (handle)
			{
				// If any of the version members are non-zero
				// then we require that DLL export "DllGetVersion"
				// and report a version as least as high as our
				// version number members
				if (dwMajorVersion || dwMinorVersion || dwBuildNumber)
				{
					// Is the DLL up to date?
					DLLVERSIONINFO dvi;
					dvi.cbSize = sizeof dvi;
					dvi.dwMajorVersion = 0;
					dvi.dwMinorVersion = 0;
					dvi.dwBuildNumber = 0;
					DLLGETVERSIONPROC DllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(handle, "DllGetVersion");
					if
					(
						DllGetVersion == NULL
					||	FAILED(DllGetVersion(&dvi))
					||	(
							dwMajorVersion && dvi.dwMajorVersion != dwMajorVersion
							? dvi.dwMajorVersion < dwMajorVersion :
							dwMinorVersion && dvi.dwMinorVersion != dwMinorVersion
							? dvi.dwMinorVersion < dwMinorVersion :
							dvi.dwBuildNumber < dwBuildNumber
						)
					)
					{
						// Well, that's the most appropriate canned system
						// message I came across: If DLL is outdated, it may
						// actually lack some interface we need...
						Throw(0, handle, CO_S_NOTALLINTERFACES, TRUE);
					}
				}
				LPCSTR *pszExport = proxy;
				*proxy = NULL;
				while ((name = *++pszExport) != NULL)
				{
					*pszExport = (LPCSTR)GetProcAddress(handle, name);
					if (*pszExport == NULL)
					{
						*proxy = proxy[1] = name;
						pszExport = proxy + 2;
						break;
					}
				}
				*pszExport = (LPCSTR)handle;
			}
		}
		if ((name = *proxy) != NULL)
		{
			DWORD dwError = ERROR_MOD_NOT_FOUND;
			HMODULE handle = 0;
			if (proxy[1] == name)
			{
				dwError = ERROR_PROC_NOT_FOUND;
				handle = (HMODULE)proxy[2];
			}
			Throw(name, handle, dwError, FALSE);
		}
	}
	return handle;
}

