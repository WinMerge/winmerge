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
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "resource.h"
#include "WinIMergeLib.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

HINSTANCE m_hInstance;
HINSTANCE hInstDLL;
HWND m_hWnd;
HWND m_hwndStatusBar;
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

	InitCommonControls();
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

void UpdateWindowTitle(HWND hWnd)
{
	wchar_t title[1024];
	wchar_t fnames[3][260];
	for (int i = 0; i < m_pImgMergeWindow->GetPaneCount(); ++i)
		wsprintfW(fnames[i], L"%s%s", m_pImgMergeWindow->GetFileName(i), m_pImgMergeWindow->IsModified(i) ? "*" : "");
	int npanes = m_pImgMergeWindow->GetPaneCount();
	if (npanes == 2)
		wsprintfW(title, L"WinIMerge(%s - %s)", fnames[0], fnames[1]);
	else if (npanes == 3)
		wsprintfW(title, L"WinIMerge(%s - %s - %s)", fnames[0], fnames[1], fnames[2]);
	if (npanes > 0)
		SetWindowTextW(hWnd, title);
}

bool OpenImages(HWND hWnd, int nImages, const std::wstring filename[3])
{
	bool bSucceeded;
	if (nImages <= 2)
		bSucceeded = m_pImgMergeWindow->OpenImages(filename[0].c_str(), filename[1].c_str());
	else
		bSucceeded = m_pImgMergeWindow->OpenImages(filename[0].c_str(), filename[1].c_str(), filename[2].c_str());
	if (bSucceeded)
		UpdateWindowTitle(hWnd);
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
	int threshold = static_cast<int>(m_pImgMergeWindow->GetColorDistanceThreshold());
	if (threshold == 0)
		id = ID_VIEW_THRESHOLD_0;
	else
	{
		id = ID_VIEW_THRESHOLD_2;
		while (threshold > 2)
		{
			++id;
			threshold >>= 1;
		}
	}
	CheckMenuRadioItem(hMenu, ID_VIEW_THRESHOLD_0, ID_VIEW_THRESHOLD_64, id, MF_BYCOMMAND);
}

void UpdateStatusBar()
{
	POINT pt = {-1, -1}, ptCursor;
	GetCursorPos(&ptCursor);
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
	{
		RECT rc;
		GetWindowRect(m_pImgMergeWindow->GetPaneHWND(pane), &rc);
		if (PtInRect(&rc, ptCursor))
			pt = m_pImgMergeWindow->GetCursorPos(pane);
	}
	
	RGBQUAD color[3];
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		color[pane] = m_pImgMergeWindow->GetPixelColor(pane, pt.x, pt.y);
	double colorDistance01, colorDistance12;
	colorDistance01 = m_pImgMergeWindow->GetColorDistance(0, 1, pt.x, pt.y);
	if (m_pImgMergeWindow->GetPaneCount() == 3)
		colorDistance12 = m_pImgMergeWindow->GetColorDistance(1, 2, pt.x, pt.y);
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
	{
		wchar_t buf[256], *p = buf;
		if (pt.x >= 0 && pt.y >= 0 &&
		    pt.x < m_pImgMergeWindow->GetImageWidth(pane) &&
		    pt.y < m_pImgMergeWindow->GetImageHeight(pane))
		{
			p += wsprintfW(p, L"Pt:(%d,%d) ", pt.x, pt.y);
			p += wsprintfW(p, L"RGBA:(%d,%d,%d,%d) ",
				color[pane].rgbRed, color[pane].rgbGreen, color[pane].rgbBlue, color[pane].rgbReserved);
			if (pane == 1 && m_pImgMergeWindow->GetPaneCount() == 3)
				p += swprintf_s(p, buf + sizeof(buf)/sizeof(wchar_t) - p, L"Dist:%g,%g ", colorDistance01, colorDistance12);
			else
				p += swprintf_s(p, buf + sizeof(buf)/sizeof(wchar_t) - p, L"Dist:%g ", colorDistance01);
		}
		p += wsprintfW(p, L"Page:%d/%d Zoom:%d%% Diff:%d/%d %dx%dpx %dbpp", 
			m_pImgMergeWindow->GetCurrentPage(pane) + 1,
			m_pImgMergeWindow->GetPageCount(pane),
			static_cast<int>(m_pImgMergeWindow->GetZoom() * 100),
			m_pImgMergeWindow->GetCurrentDiffIndex() + 1,
			m_pImgMergeWindow->GetDiffCount(),
			m_pImgMergeWindow->GetImageWidth(pane),
			m_pImgMergeWindow->GetImageHeight(pane),
			m_pImgMergeWindow->GetImageBitsPerPixel(pane)
			);
		SendMessage(m_hwndStatusBar, SB_SETTEXT, (WPARAM)pane | 0, (LPARAM)buf);
	}
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

bool GenerateHTMLReport(const wchar_t *filename)
{
	wchar_t imgdir[MAX_PATH], imgdir_full[MAX_PATH], imgfilepath[3][MAX_PATH], difffilename[3][MAX_PATH];
	char imgfilepath_utf8[3][MAX_PATH], difffilename_utf8[3][MAX_PATH];
	wcscpy_s(imgdir_full, filename);
	PathRemoveExtensionW(imgdir_full);
	PathAddExtensionW(imgdir_full, L".files");
	wcscpy_s(imgdir, PathFindFileName(imgdir_full));
	CreateDirectoryW(imgdir_full, NULL);
	for (int i = 0; i < m_pImgMergeWindow->GetPaneCount(); ++i)
	{
		wcscpy_s(imgfilepath[i], m_pImgMergeWindow->GetFileName(i));
		WideCharToMultiByte(CP_UTF8, 0, imgfilepath[i], -1, imgfilepath_utf8[i], sizeof(imgfilepath_utf8[i]), NULL, NULL);
		wsprintfW(difffilename[i], L"%s/%d.png", imgdir, i + 1);
		WideCharToMultiByte(CP_UTF8, 0, difffilename[i], -1, difffilename_utf8[i], sizeof(difffilename_utf8[i]), NULL, NULL);
		wchar_t tmp[MAX_PATH];
		wsprintfW(tmp, L"%s\\%d.png", imgdir_full, i + 1);
		m_pImgMergeWindow->SaveDiffImageAs(i, tmp);
	}
	std::ofstream fout;
	try
	{
		fout.open(filename, std::ios::out | std::ios::trunc);
		fout << 
			"<!DOCTYPE html>" << std::endl <<
			"<html>" << std::endl <<
			"<head>" << std::endl <<
			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">" << std::endl <<
			"<title>WinMerge Image Compare Report</title>" << std::endl <<
			"<style type=\"text/css\">" << std::endl <<
			"table { table-layout: fixed; width: 100%; height: 100%; border-collapse: collapse; }" << std::endl <<
			"th,td { border: solid 1px black; }" << std::endl <<
			".title { color: white; background-color: blue; vertical-align: top; }" << std::endl <<
			".img   { height: 100%; overflow: scroll; text-align: center; }" << std::endl <<
			"</style>" << std::endl <<
			"</head>" << std::endl <<
			"<body>" << std::endl <<
			"<table>" << std::endl <<
			"<tr>" << std::endl;
		for (int i = 0; i < m_pImgMergeWindow->GetPaneCount(); ++i)
			fout << "<th class=\"title\">" << imgfilepath_utf8[i] << "</th>" << std::endl;
		fout << 
			"</tr>" << std::endl <<
			"<tr>" << std::endl;
		for (int i = 0; i < m_pImgMergeWindow->GetPaneCount(); ++i)
			fout << "<td><div class=\"img\"><img src=\"" << difffilename_utf8[i] << 
			"\" alt=\"" << difffilename_utf8[i] << "\"></div></td>" << std::endl;
		fout <<
			"</tr>" << std::endl <<
			"</table>" << std::endl <<
			"</body>" << std::endl <<
			"</html>" << std::endl;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_CREATE:
		m_hwndStatusBar = CreateWindowEx(0, 
			STATUSCLASSNAME, NULL, 
			WS_CHILD | SBARS_SIZEGRIP | CCS_BOTTOM | WS_VISIBLE, 
			0, 0, 0, 0, hWnd, (HMENU)1000, m_hInstance, NULL);
		m_pImgMergeWindow = WinIMerge_CreateWindow(hInstDLL, hWnd);
		m_pImgMergeWindow->AddEventListener(OnChildPaneEvent, NULL);
		SetTimer(hWnd, 1, 250, NULL);
		UpdateMenuState(hWnd);
		break;
	case WM_TIMER:
	{
		int widths[3] = {0};
		for (int i = 0; i < m_pImgMergeWindow->GetPaneCount(); ++i)
		{
			RECT rc = m_pImgMergeWindow->GetPaneWindowRect(i);
			widths[i] = rc.right - rc.left + ((i > 0) ? widths[i - 1] : 0);
		}
		SendMessage(m_hwndStatusBar, SB_SETPARTS, (WPARAM)m_pImgMergeWindow->GetPaneCount(), (LPARAM)widths);
		UpdateWindowTitle(hWnd);
		UpdateStatusBar();
		break;
	}
	case WM_SIZE:
	{
		RECT rc, rcStatusBar;
		GetClientRect(hWnd, &rc);
		GetClientRect(m_hwndStatusBar, &rcStatusBar);
		rc.bottom -= rcStatusBar.bottom;
		m_pImgMergeWindow->SetWindowRect(rc);
		MoveWindow(m_hwndStatusBar, 0, rc.bottom, rc.right, rc.bottom + rcStatusBar.bottom, TRUE);
		break;
	}
	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam); 
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
		case ID_FILE_SAVE:
			m_pImgMergeWindow->SaveImages();
			break;
		case ID_FILE_RELOAD:
			m_pImgMergeWindow->ReloadImages();
			break;
		case ID_FILE_GENERATE_REPORT:
		{
			wchar_t szFileName[MAX_PATH] = {0}, szFile[MAX_PATH] = {0};
			OPENFILENAMEW ofn = {0};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = L"HTML file(*.html)\0*.html\0\0";
			ofn.lpstrFile = szFileName;
			ofn.lpstrFileTitle = szFile;
			ofn.nMaxFile = MAX_PATH;
			ofn.nMaxFileTitle = sizeof(szFile);
			ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
			ofn.lpstrTitle = L"Generate HTML Report File";
			ofn.lpstrDefExt = L"html";
			if (GetSaveFileNameW(&ofn) != 0)
			{
				GenerateHTMLReport(ofn.lpstrFile);
			}
			break;
		}
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_EDIT_UNDO:
			m_pImgMergeWindow->Undo();
			break;
		case ID_EDIT_REDO:
			m_pImgMergeWindow->Redo();
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
		case ID_VIEW_THRESHOLD_0:
			m_pImgMergeWindow->SetColorDistanceThreshold(0);
			UpdateMenuState(hWnd);
			break;
		case ID_VIEW_THRESHOLD_2:
		case ID_VIEW_THRESHOLD_4:
		case ID_VIEW_THRESHOLD_8:
		case ID_VIEW_THRESHOLD_16:
		case ID_VIEW_THRESHOLD_32:
		case ID_VIEW_THRESHOLD_64:
			m_pImgMergeWindow->SetColorDistanceThreshold((1 << (wmId - ID_VIEW_THRESHOLD_2)) * 2.0);
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
		case ID_MERGE_COPYTORIGHT:
		{
			int srcPane = m_pImgMergeWindow->GetActivePane();
			if (srcPane < 0)
				srcPane = 0;
			if (srcPane >= m_pImgMergeWindow->GetPaneCount() - 1)
				srcPane = m_pImgMergeWindow->GetPaneCount() - 2;
			int dstPane = srcPane + 1;
			m_pImgMergeWindow->CopyDiff(m_pImgMergeWindow->GetCurrentDiffIndex(), srcPane, dstPane);
			break;
		}
		case ID_MERGE_COPYTOLEFT:
		{
			int srcPane = m_pImgMergeWindow->GetActivePane();
			if (srcPane < 1)
				srcPane = 1;
			int dstPane = srcPane - 1;
			m_pImgMergeWindow->CopyDiff(m_pImgMergeWindow->GetCurrentDiffIndex(), srcPane, dstPane);
			break;
		}
		case ID_MERGE_COPYFROMLEFT:
		{
			int srcPane = m_pImgMergeWindow->GetActivePane() - 1;
			if (srcPane < 0)
				srcPane = 0;
			int dstPane = srcPane + 1;
			m_pImgMergeWindow->CopyDiff(m_pImgMergeWindow->GetCurrentDiffIndex(), srcPane, dstPane);
			break;
		}
		case ID_MERGE_COPYFROMRIGHT:
		{
			int srcPane = m_pImgMergeWindow->GetActivePane() + 1;
			if (srcPane > m_pImgMergeWindow->GetPaneCount() - 1)
				srcPane = m_pImgMergeWindow->GetPaneCount() - 1;
			int dstPane = srcPane - 1;
			m_pImgMergeWindow->CopyDiff(m_pImgMergeWindow->GetCurrentDiffIndex(), srcPane, dstPane);
			break;
		}
		case ID_MERGE_COPYALLTORIGHT:
		{
			int srcPane = m_pImgMergeWindow->GetActivePane();
			if (srcPane < 0)
				srcPane = 0;
			if (srcPane >= m_pImgMergeWindow->GetPaneCount() - 1)
				srcPane = m_pImgMergeWindow->GetPaneCount() - 2;
			int dstPane = srcPane + 1;
			m_pImgMergeWindow->CopyDiffAll(srcPane, dstPane);
			break;
		}
		case ID_MERGE_COPYALLTOLEFT:
		{
			int srcPane = m_pImgMergeWindow->GetActivePane();
			if (srcPane < 1)
				srcPane = 1;
			int dstPane = srcPane - 1;
			m_pImgMergeWindow->CopyDiffAll(srcPane, dstPane);
			break;
		}
		case ID_MERGE_AUTOMERGE:
		{
			int dstPane = m_pImgMergeWindow->GetActivePane();
			if (dstPane < 0)
				break;
			int nMerged = m_pImgMergeWindow->CopyDiff3Way(dstPane);
			wchar_t msg[256];
			wsprintfW(msg, L"The number of automatically merged changes: %d\n"
			               L"The number of unresolved conflicts: %d\n"
						   , nMerged, m_pImgMergeWindow->GetConflictCount());
			MessageBoxW(hWnd, msg, L"WinIMerge", MB_OK | MB_ICONINFORMATION);

			break;
		}
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
