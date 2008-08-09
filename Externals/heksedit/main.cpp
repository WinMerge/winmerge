//============================================================================================
// frhed - free hex editor

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "toolbar.h"

static const char szMainClass[] = "frhed wndclass";
static const char szHexClass[] = "frhed hexclass";

HINSTANCE hMainInstance;
LRESULT CALLBACK MainWndProc (HWND, UINT, WPARAM, LPARAM);

static BOOL CALLBACK WndEnumProcCountInstances(HWND hwnd, LPARAM lParam)
{
	char buf[64];
	if (GetClassName(hwnd, buf, 64) != 0)
		if (strcmp(buf, szMainClass) == 0)
			++*(int *)lParam;
	return TRUE;
}

//--------------------------------------------------------------------------------------------
// WinMain: the starting point.

static HWND hwndMain = 0;
static HWND hwndHex = 0;
static HWND hwndToolBar = 0;
static HWND hwndStatusBar = 0;
static HexEditorWindow *pHexWnd = 0;

int WINAPI WinMain(HINSTANCE hIconInstance, HINSTANCE, char *szCmdLine, int)
{
	InitCommonControls();

	hMainInstance = LoadLibrary("heksedit.dll");

	// Register window class and open window.

	MSG msg;

	WNDCLASS wndclass;
	Zero(wndclass);

	//Register the main window class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = MainWndProc;
	wndclass.hIcon = LoadIcon(hIconInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance = hMainInstance;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
	wndclass.lpszClassName = szMainClass;

	RegisterClass(&wndclass);

	OleInitialize(NULL);

	int iInstCount = 0;
	EnumWindows(WndEnumProcCountInstances, (LPARAM)&iInstCount);

	CreateWindow(szMainClass, 0, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hMainInstance, NULL);

	// Read in the last saved preferences.
	pHexWnd->iInstCount = iInstCount;
	pHexWnd->read_ini_data();

	/*The if prevents the window from being resized to 0x0
	 it becomes just a title bar*/
	if (pHexWnd->iWindowX != CW_USEDEFAULT)
	{
		//Prevent window creep when Taskbar is at top or left of screen
		WINDOWPLACEMENT wp;
		wp.length = sizeof wp;
		GetWindowPlacement(hwndMain, &wp);
		wp.showCmd = pHexWnd->iWindowShowCmd;
		wp.rcNormalPosition.left = pHexWnd->iWindowX;
		wp.rcNormalPosition.top = pHexWnd->iWindowY;
		wp.rcNormalPosition.right = pHexWnd->iWindowWidth + pHexWnd->iWindowX;
		wp.rcNormalPosition.bottom = pHexWnd->iWindowHeight + pHexWnd->iWindowY;
		SetWindowPlacement(hwndMain, &wp);
	}
	ShowWindow(hwndMain, pHexWnd->iWindowShowCmd);
	UpdateWindow(hwndMain);

	if (szCmdLine != NULL && strlen(szCmdLine) != 0)
	{
		// Command line not empty: open a file on startup.
		char *p = szCmdLine;
		char *q = szCmdLine;
		DWORD dwStart = 0, dwLength = 0, dwEnd = 0; // MF cmd line parms
		while ((*p = *q) != '\0')
		{
			switch (*q++)
			{
			case '/': // switch coming up
				switch (*q)
				{
				case 'S': // Start offset
				case 's':
					dwStart = strtoul(++q, &q, 0);
					break;
				case 'L': // Length of selection
				case 'l':
					dwLength = strtoul(++q, &q, 0);
					break;
				case 'E': // End of selection
				case 'e':
					dwEnd = strtoul(++q, &q, 0);
					break;
				}
				// fall through
			case '"':
				break;
			default:
				++p;
				break;
			}
		}
		if (dwLength)
			dwEnd = dwStart + dwLength - 1;
		char lpszPath[MAX_PATH];
		HRESULT hres = pHexWnd->ResolveIt(szCmdLine, lpszPath);
		if (SUCCEEDED(hres))
		{
			// Trying to open a link file: decision by user required.
			int ret = MessageBox( hwndMain,
				"You are trying to open a link file.\n"
				"Click on Yes if you want to open the file linked to,\n"
				"or click on No if you want to open the link file itself.\n"
				"Choose Cancel if you want to abort opening.",
				"frhed", MB_YESNOCANCEL | MB_ICONQUESTION );
			switch( ret )
			{
			case IDYES:
				pHexWnd->load_file(lpszPath);
				break;
			case IDNO:
				pHexWnd->load_file(szCmdLine);
				break;
			case IDCANCEL:
				break;
			}
		}
		else
		{
			pHexWnd->load_file(szCmdLine);
		}
		if (dwEnd) pHexWnd->CMD_setselection(dwStart, dwEnd);
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!pHexWnd->translate_accelerator(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	OleUninitialize();

	return msg.wParam;
}

//--------------------------------------------------------------------------------------------
// The main window procedure.
LRESULT CALLBACK MainWndProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	switch (iMsg) {
		case WM_CREATE:
			hwndMain = hwnd;
			DragAcceptFiles(hwnd, TRUE); // Accept files dragged into main window.
			hwndToolBar = CreateTBar(hwnd, hMainInstance);
			hwndHex = CreateWindowEx(WS_EX_CLIENTEDGE, szHexClass, 0,
				WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
				10, 10, 100, 100, hwnd, 0, hMainInstance, 0);
			pHexWnd = (HexEditorWindow *)GetWindowLong(hwndHex, GWL_USERDATA);
			hwndStatusBar = CreateStatusWindow(
				CCS_BOTTOM | WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
				"Ready", hwnd, 2);
			pHexWnd->hwndMain = hwnd;
			pHexWnd->hwndToolBar = hwndToolBar;
			pHexWnd->hwndStatusBar = hwndStatusBar;
			pHexWnd->bSaveIni = TRUE;
			pHexWnd->bCenterCaret = TRUE;
			pHexWnd->set_wnd_title();
			return 0;
		case WM_COMMAND: return SendMessage(hwndHex, iMsg, wParam, lParam);
		case WM_SETFOCUS: SetFocus(hwndHex); break;
		case WM_CLOSE: 
			if (!pHexWnd->close())
				return 0;
			break;
		//return SendMessage(hwndHex, iMsg, wParam, lParam);
		case WM_INITMENUPOPUP: pHexWnd->initmenupopup( wParam, lParam ); return 0;
		case WM_DROPFILES: return SendMessage(hwndHex, iMsg, wParam, lParam);

		case WM_SIZE:{
			SendMessage(hwndStatusBar, WM_SIZE, 0 , 0); //Moves status bar back to the bottom
			SendMessage(hwndToolBar, WM_SIZE, 0 , 0); //Moves tool bar back to the top

			//--------------------------------------------
			// Set statusbar divisions.
			int statbarw = LOWORD(lParam);
			// Calculate the right edge coordinate for each part
			int parts[] = { statbarw * 4 / 6, statbarw * 5 / 6, statbarw };

			SendMessage(hwndStatusBar, SB_SETPARTS, (WPARAM) 3, (LPARAM)parts);

			RECT rect;
			GetClientRect(hwndToolBar, &rect);
			int iToolbarHeight = rect.bottom - rect.top;
			GetClientRect(hwndStatusBar, &rect);
			MoveWindow(hwndHex, 0, iToolbarHeight, LOWORD(lParam), HIWORD(lParam)-rect.bottom-iToolbarHeight, TRUE);
			break;
		}

		case WM_NOTIFY:{
			//See if someone sent us invalid data
			HWND h;
			UINT code;
			try{
				//Attempt to dereference
				NMHDR& pn = *(NMHDR*)lParam;
				h = pn.hwndFrom;
				code = pn.code;
			}
			catch(...){ return 0; }

			if (h == hwndStatusBar)
			{
				if(code == NM_CLICK || code == NM_RCLICK)
					pHexWnd->status_bar_click(code == NM_CLICK);
			}
			else if (h == hwndToolBar)
			{
				if (code == TBN_GETINFOTIPA || code == TBN_GETINFOTIPW)
				{
					try{
						if(code == TBN_GETINFOTIPA){
							NMTBGETINFOTIPA& pi = *(NMTBGETINFOTIPA*) lParam;
							LoadStringA(hMainInstance,pi.iItem,pi.pszText,pi.cchTextMax);
						} else {
							NMTBGETINFOTIPW& pi = *(NMTBGETINFOTIPW*) lParam;
							LoadStringW(hMainInstance,pi.iItem,pi.pszText,pi.cchTextMax);
						}
					}
					catch(...){}
				}
			}
		}
		return 0;

		case WM_DESTROY: {
			// Store window position for next startup.
			WINDOWPLACEMENT wndpl;
			wndpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement(hwnd, &wndpl);
			pHexWnd->iWindowShowCmd = wndpl.showCmd;
			pHexWnd->iWindowX = wndpl.rcNormalPosition.left;
			pHexWnd->iWindowY = wndpl.rcNormalPosition.top;
			pHexWnd->iWindowWidth = wndpl.rcNormalPosition.right - pHexWnd->iWindowX;
			pHexWnd->iWindowHeight = wndpl.rcNormalPosition.bottom - pHexWnd->iWindowY;
			pHexWnd->save_ini_data();
			DragAcceptFiles(hwndMain, FALSE);
			PostQuitMessage(0);
		}
		return 0;

	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam );
}

// The hex window procedure.
/*LRESULT CALLBACK HexWndProc( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	if (iMsg == WM_NCCREATE)
		SetWindowLong(hwnd, GWL_USERDATA, (LONG)new HexEditorWindow);
	HexEditorWindow *pHexWnd = (HexEditorWindow *)GetWindowLong(hwnd, GWL_USERDATA);
	LRESULT lResult = pHexWnd->OnWndMsg( hwnd, iMsg, wParam, lParam );
	if (iMsg == WM_NCDESTROY)
		delete pHexWnd;
	return lResult;
}*/
//============================================================================================
