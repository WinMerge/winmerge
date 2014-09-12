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
#include <string>
#include <algorithm>
#include "resource.h"
#include "WinIMergeLib.h"

HINSTANCE m_hInstance;
HINSTANCE hInstDLL;
HWND m_hWnd;
wchar_t m_szTitle[256] = L"WinIMerge";
wchar_t m_szWindowClass[256] = L"WinIMergeClass";
IImgMergeWindow *m_pImgMergeWindow = NULL;

struct CmdLineInfo
{
	CmdLineInfo(const wchar_t *cmdline) : nImages(0)
	{
		if (cmdline[0] == 0)
			return;

		int argc;
		wchar_t **argv = CommandLineToArgvW(cmdline, &argc);
		for (int i = 0; i < argc; ++i)
		{
			if (argv[i][0] != '-' && argv[i][0] != '/' && nImages < 3)
			{
				sFileName[nImages] = argv[i];
				++nImages;
			}
		}
	}

	std::wstring sFileName[3];
	int nImages;
};

bool OpenImages(HWND hWnd, int nImages, const std::wstring filename[3]);
void UpdateMenuState(HWND hWnd);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	MyRegisterClass(hInstance);
	hInstDLL = GetModuleHandleW(L"WinIMergeLib.dll");

	if (!InitInstance (hInstance, nCmdShow)) 
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WINIMERGE);

	CmdLineInfo cmdline(lpCmdLine);
	if (cmdline.nImages > 0)
		OpenImages(m_hWnd, cmdline.nImages, cmdline.sFileName);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {0};
	wcex.cbSize         = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = (WNDPROC)WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, (LPCTSTR)IDI_WINIMERGE);
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = (LPCTSTR)IDC_WINIMERGE;
	wcex.lpszClassName  = m_szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINIMERGE);
	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   m_hInstance = hInstance;
   m_hWnd = CreateWindow(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!m_hWnd)
      return FALSE;
   ShowWindow(m_hWnd, nCmdShow);
   UpdateWindow(m_hWnd);
   return TRUE;
}

bool OpenImages(HWND hWnd, int nImages, const std::wstring filename[3])
{
	bool bSucceeded;
	wchar_t title[256];
	if (nImages <= 2)
	{
		bSucceeded = m_pImgMergeWindow->OpenImages(filename[0].c_str(), filename[1].c_str());
		if (bSucceeded)
			wsprintfW(title, L"WinIMerge(%s - %s)", filename[0].c_str(), filename[1].c_str());
	}
	else
	{
		bSucceeded = m_pImgMergeWindow->OpenImages(filename[0].c_str(), filename[1].c_str(), filename[2].c_str());
		if (bSucceeded)
			wsprintfW(title, L"WinIMerge(%s - %s - %s)", filename[0].c_str(), filename[1].c_str(), filename[2].c_str());
	}
	if (bSucceeded)
		SetWindowTextW(hWnd, title);
	InvalidateRect(hWnd, NULL, TRUE);
	return bSucceeded;
}

void UpdateMenuState(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu, ID_VIEW_VIEWDIFFERENCES,    m_pImgMergeWindow->GetShowDifferences() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_VIEW_SPLITHORIZONTALLY,  m_pImgMergeWindow->GetHorizontalSplit() ? MF_CHECKED : MF_UNCHECKED);
	CheckMenuRadioItem(hMenu, ID_VIEW_OVERLAY_NONE, ID_VIEW_OVERLAY_ALPHABLEND,
		m_pImgMergeWindow->GetOverlayMode() + ID_VIEW_OVERLAY_NONE, MF_BYCOMMAND);
	int blockSize = m_pImgMergeWindow->GetDiffBlockSize();
	int id = ID_VIEW_DIFFBLOCKSIZE_1;
	while (blockSize > 1)
	{
		++id;
		blockSize >>= 1;
	}
	CheckMenuRadioItem(hMenu, ID_VIEW_DIFFBLOCKSIZE_1, ID_VIEW_DIFFBLOCKSIZE_32, id, MF_BYCOMMAND);
	CheckMenuItem(hMenu, ID_VIEW_USEBACKCOLOR, m_pImgMergeWindow->GetUseBackColor() ? MF_CHECKED : MF_UNCHECKED);
}

void OnChildPaneEvent(const IImgMergeWindow::Event& evt)
{
	if (evt.eventType == IImgMergeWindow::CONTEXTMENU)
	{
		HMENU hPopup = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_POPUPMENU));
		HMENU hSubMenu = GetSubMenu(hPopup, 0);
		TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, evt.x, evt.y, 0, m_hWnd, NULL); 
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_CREATE:
		m_pImgMergeWindow = WinIMerge_CreateWindow(hInstDLL, hWnd);
		m_pImgMergeWindow->AddEventListener(OnChildPaneEvent, NULL);
		UpdateMenuState(hWnd);
		break;
	case WM_SIZE:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		m_pImgMergeWindow->SetWindowRect(rc);
		break;
	}
	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam); 
		int wmEvent = HIWORD(wParam); 
		switch (wmId)
		{
		case ID_FILE_OPEN:
		case ID_FILE_OPEN3:
		{
			wchar_t szFileName[MAX_PATH] = {0}, szFile[MAX_PATH] = {0};
			OPENFILENAMEW ofn = {0};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = L"Images (*.jpg;*.png;*.bmp;*.gif;*.tga;*.psd;*.ico;*.cur)\0*.jpg;*.png;*.bmp;*.gif;*.tga;*.psd;*.ico;*.cur\0\0";
			ofn.lpstrFile = szFileName;
			ofn.lpstrFileTitle = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.nMaxFileTitle = sizeof(szFile);
			ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrTitle = L"Open File";
			std::wstring filename[3];
			wchar_t title[256];
			int nImages = (wmId - ID_FILE_OPEN) + 2;
			int i;
			for (i = 0; i < nImages; ++i)
			{
				wsprintfW(title, L"Open #%d File", i + 1);
				ofn.lpstrTitle = title;
				if (GetOpenFileNameW(&ofn) != 0)
					filename[i] = ofn.lpstrFile;
				else
					break;
			}
			if (nImages == i)
				OpenImages(hWnd, nImages, filename);
			break;
		}
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_VIEW_ZOOM_400:
		case ID_VIEW_ZOOM_200:
		case ID_VIEW_ZOOM_100:
		case ID_VIEW_ZOOM_50:
		case ID_VIEW_ZOOM_25:
			m_pImgMergeWindow->SetZoom(pow(2.0, (wmId - ID_VIEW_ZOOM_100)));
			break;
		case ID_VIEW_OVERLAY_NONE:
			m_pImgMergeWindow->SetOverlayMode(IImgMergeWindow::OVERLAY_NONE);
			UpdateMenuState(hWnd);
			break;
		case ID_VIEW_OVERLAY_ALPHABLEND:
			m_pImgMergeWindow->SetOverlayMode(IImgMergeWindow::OVERLAY_ALPHABLEND);
			UpdateMenuState(hWnd);
			break;
		case ID_VIEW_OVERLAY_XOR:
			m_pImgMergeWindow->SetOverlayMode(IImgMergeWindow::OVERLAY_XOR);
			UpdateMenuState(hWnd);
			break;
		case ID_VIEW_VIEWDIFFERENCES:
			m_pImgMergeWindow->SetShowDifferences(!m_pImgMergeWindow->GetShowDifferences());
			UpdateMenuState(hWnd);
			break;
		case ID_VIEW_DIFFBLOCKSIZE_1:
		case ID_VIEW_DIFFBLOCKSIZE_2:
		case ID_VIEW_DIFFBLOCKSIZE_4:
		case ID_VIEW_DIFFBLOCKSIZE_8:
		case ID_VIEW_DIFFBLOCKSIZE_16:
		case ID_VIEW_DIFFBLOCKSIZE_32:
			m_pImgMergeWindow->SetDiffBlockSize(1 << (wmId - ID_VIEW_DIFFBLOCKSIZE_1));
			UpdateMenuState(hWnd);
			break;
		case ID_VIEW_SPLITHORIZONTALLY:
			m_pImgMergeWindow->SetHorizontalSplit(!m_pImgMergeWindow->GetHorizontalSplit());
			UpdateMenuState(hWnd);
			break;
		case ID_VIEW_PAGE_NEXTPAGE:
		{
			int page = m_pImgMergeWindow->GetCurrentMaxPage();
			m_pImgMergeWindow->SetCurrentPageAll(page + 1);
			break;
		}
		case ID_VIEW_PAGE_PREVPAGE:
		{
			int page = m_pImgMergeWindow->GetCurrentMaxPage();
			m_pImgMergeWindow->SetCurrentPageAll(page - 1);
			break;
		}
		case ID_VIEW_USEBACKCOLOR:
		{
			bool useBackColor = !m_pImgMergeWindow->GetUseBackColor();
			if (useBackColor)
			{
				RGBQUAD backColor = m_pImgMergeWindow->GetBackColor();
				CHOOSECOLORW cc = {0};
				static DWORD dwCustColors[16];
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hWnd;
				cc.lpCustColors = dwCustColors;
				cc.rgbResult = RGB(backColor.rgbRed, backColor.rgbGreen, backColor.rgbBlue);
				cc.Flags = CC_RGBINIT;
				if (ChooseColorW(&cc) != 0)
				{
					RGBQUAD backColor = {GetBValue(cc.rgbResult), GetGValue(cc.rgbResult), GetRValue(cc.rgbResult)};
					m_pImgMergeWindow->SetBackColor(backColor);
					m_pImgMergeWindow->SetUseBackColor(useBackColor);
				}
			}
			else
			{
				m_pImgMergeWindow->SetUseBackColor(useBackColor);
			}
			UpdateMenuState(hWnd);
			break;
		}
		case ID_MERGE_NEXTDIFFERENCE:
			m_pImgMergeWindow->NextDiff();
			break;
		case ID_MERGE_PREVIOUSDIFFERENCE:
			m_pImgMergeWindow->PrevDiff();
			break;
		case ID_MERGE_FIRSTDIFFERENCE:
			m_pImgMergeWindow->FirstDiff();
			break;
		case ID_MERGE_LASTDIFFERENCE:
			m_pImgMergeWindow->LastDiff();
			break;
		case ID_MERGE_NEXTCONFLICT:
			m_pImgMergeWindow->NextConflict();
			break;
		case ID_MERGE_PREVIOUSCONFLICT:
			m_pImgMergeWindow->PrevConflict();
			break;
		case ID_HELP_ABOUT:
			MessageBoxW(hWnd, 
				L"WinIMerge\n\n"
				L"(c) 2014 sdottaka@sourceforge.net All rights reserved.\n\n"
				L"This software uses the FreeImage open source image library. \n"
				L"See http://freeimage.sourceforge.net for details.\n"
				L"FreeImage is used under the GNU GPL version.\n", L"WinIMerge", MB_OK | MB_ICONINFORMATION);
			break;
		case ID_POPUP_CURPANE_PREVIOUSPAGE:
		{
			int nActivePane = m_pImgMergeWindow->GetActivePane();
			m_pImgMergeWindow->SetCurrentPage(nActivePane, m_pImgMergeWindow->GetCurrentPage(nActivePane) - 1);
			break;
		}
		case ID_POPUP_CURPANE_NEXTPAGE:
		{
			int nActivePane = m_pImgMergeWindow->GetActivePane();
			m_pImgMergeWindow->SetCurrentPage(nActivePane, m_pImgMergeWindow->GetCurrentPage(nActivePane) + 1);
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_MOUSEWHEEL:
		PostMessage(m_pImgMergeWindow->GetHWND(), message, wParam, lParam);
		break;
	case WM_DESTROY:
		WinIMerge_DestroyWindow(m_pImgMergeWindow);	
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
