//============================================================================================
// frhed - free hex editor

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"

HINSTANCE hMainInstance;
LRESULT CALLBACK HexWndProc(HWND, UINT, WPARAM, LPARAM);

static const TCHAR szHexClass[] = WC_HEKSEDIT;

//--------------------------------------------------------------------------------------------
// WinMain: the starting point.
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		hMainInstance = hInstance;
		WNDCLASSEX wndclass;
		ZeroMemory(&wndclass, sizeof wndclass);
		wndclass.cbSize = sizeof wndclass;
		wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
		wndclass.lpfnWndProc = HexWndProc;
		wndclass.hInstance = hInstance;
		wndclass.hCursor = 0;
		wndclass.lpszClassName = szHexClass;
		RegisterClassEx(&wndclass);
		HexEditorWindow::LoadStringTable();
		return TRUE;
	}
	if (dwReason == DLL_PROCESS_DETACH)
	{
		HexEditorWindow::FreeStringTable();
	}
	return FALSE;
}

// The hex window procedure.
LRESULT CALLBACK HexWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	if (iMsg == WM_NCCREATE)
		SetWindowLong(hwnd, GWLP_USERDATA, (LONG)new HexEditorWindow);
	HexEditorWindow *pHexWnd = (HexEditorWindow *)GetWindowLong(hwnd, GWLP_USERDATA);
	LRESULT lResult = pHexWnd->OnWndMsg(hwnd, iMsg, wParam, lParam);
	if (iMsg == WM_NCDESTROY)
		delete pHexWnd;
	return lResult;
}
//============================================================================================
