/*/DllProxies.cpp

[The MIT license]

Copyright (c) Jochen Neubeck

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

[ChangeLog]

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2007-02-03	Jochen Neubeck		Created
2008-08-24	Jochen Neubeck		FormatMessage
2008-10-15	Kimmo Varis			Rename 'export' variable to 'strExport' to avoid
								compiler warnings.
*/
#include "precomp.h"
#include "DllProxies.h"

// If MFC is unavailable, resort to FatalAppExit for exceptions.
#ifndef _AFX
#define AfxThrowOleDispatchException FatalAppExit
#endif

// TCHAR-aware function name decoration
#ifdef _UNICODE
#define AW "W"
#else
#define AW "A"
#endif

/**
 * @brief Load a dll and import a number of functions.
 */
LPVOID DllProxy::Load()
{
	if (Names[0])
	{
		if (Names[1] == 0 || Names[1] == Names[0])
			return 0;
		HMODULE handle = LoadLibraryA(Names[0]);
		if (handle == 0)
		{
			Names[1] = 0;
			return 0;
		}
		LPCSTR *strExport = Names;
		*Names = 0;
		while (LPCSTR name = *++strExport)
		{
			*strExport = (LPCSTR)GetProcAddress(handle, name);
			if (*strExport == 0)
			{
				Names[0] = Names[1] = name;
				Names[2] = (LPCSTR)handle;
				return 0;
			}
		}
		*strExport = (LPCSTR)handle;
	}
	return this + 1;
}

/**
 * @brief Load a dll and import a number of functions, or throw an exception.
 */
LPVOID DllProxy::EnsureLoad()
{
	if (!Load())
	{
		TCHAR buf[1024];
		FormatMessage(buf);
		AfxThrowOleDispatchException(0, buf);
	}
	return this + 1;
}

/**
 * @brief Format an appropriate error message.
 */
void DllProxy::FormatMessage(LPTSTR buf)
{
	int cch = wsprintf(buf, _T("%hs"), Names[0]);
	DWORD error = ERROR_MOD_NOT_FOUND;
	if (Names[1])
	{
		buf[cch++] = '@';
		cch += ::GetModuleFileName((HMODULE)Names[2], buf + cch, MAX_PATH);
		error = ERROR_PROC_NOT_FOUND;
	}
	buf[cch++] = ':';
	buf[cch++] = '\n';
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, error, 0, buf + cch, MAX_PATH, 0);
}

/**
 * @brief RAWIO32 dll proxy
 */
DllProxy::Instance<struct RAWIO32> RAWIO32 =
{
	"RAWIO32.DLL",
	"ResetDisk",
	"ReadPhysicalSector",
	"WritePhysicalSector",
	"ReadDiskGeometry",
	"EI13GetDriveParameters",
	"EI13ReadSector",
	"EI13WriteSector",
	(HMODULE)0
};

/**
 * @brief IMAGEHLP dll proxy
 */
DllProxy::Instance<struct IMAGEHLP> IMAGEHLP =
{
	"IMAGEHLP.DLL",
	"UnMapAndLoad",
	"ImageRvaToVa",
	"MapAndLoad",
	(HMODULE)0
};
