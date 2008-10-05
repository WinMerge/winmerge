//============================================================================================
// frhed - free hex editor

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"

HINSTANCE hMainInstance;
LRESULT CALLBACK HexWndProc(HWND, UINT, WPARAM, LPARAM);

static const char szHexClassA[] = "hekseditA_"
	SHARPEN(A,FRHED_MAJOR_VERSION) "."
	SHARPEN(A,FRHED_MINOR_VERSION) "."
	SHARPEN(A,FRHED_SUB_RELEASE_NO) "."
	SHARPEN(A,FRHED_BUILD_NO);

static const WCHAR szHexClassW[] = L"hekseditW_"
	SHARPEN(W,FRHED_MAJOR_VERSION) L"."
	SHARPEN(W,FRHED_MINOR_VERSION) L"."
	SHARPEN(W,FRHED_SUB_RELEASE_NO) L"."
	SHARPEN(W,FRHED_BUILD_NO);

//--------------------------------------------------------------------------------------------
// WinMain: the starting point.
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		hMainInstance = hInstance;
		union
		{
			WNDCLASSEXA a;
			WNDCLASSEXW w;
		} wndclass;
		ZeroMemory(&wndclass, sizeof wndclass);
		wndclass.a.cbSize = sizeof wndclass;
		wndclass.a.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
		wndclass.a.lpfnWndProc = HexWndProc;
		wndclass.a.hInstance = hInstance;
		wndclass.a.hCursor = 0;
		wndclass.a.lpszClassName = szHexClassA;
		RegisterClassExA(&wndclass.a);
		wndclass.w.lpszClassName = szHexClassW;
		RegisterClassExW(&wndclass.w);
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
