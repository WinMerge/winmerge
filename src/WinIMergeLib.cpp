/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ImgWindow.hpp"
#include "ImgMergeWindow.hpp"

extern "C" IImgMergeWindow *
WinIMerge_CreateWindow(HINSTANCE hInstance, HWND hWndParent)
{
	RECT rc = {0};
	CImgMergeWindow *pImgMergeWindow = new CImgMergeWindow();
	pImgMergeWindow->Create(hInstance, hWndParent, rc);
	return static_cast<IImgMergeWindow *>(pImgMergeWindow);
}

extern "C" bool
WinIMerge_DestroyWindow(IImgMergeWindow *pImgMergeWindow)
{
	CImgMergeWindow *pImgMergeWindow2 = static_cast<CImgMergeWindow *>(pImgMergeWindow);
	pImgMergeWindow2->Destroy();
	delete pImgMergeWindow2;
	return true;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		FreeImage_Initialise();
		return TRUE;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		FreeImage_DeInitialise();
	}
	return FALSE;
}
