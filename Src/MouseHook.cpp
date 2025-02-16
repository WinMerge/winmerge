#include <StdAfx.h>
#include "MouseHook.h"

#ifndef WM_MOUSEHWHEEL
#  define WM_MOUSEHWHEEL 0x20e
#endif

static bool IsWindowDialog(HWND hwndTarget)
{
	tchar_t className[256];
	if (GetClassName(hwndTarget, className, sizeof(className) / sizeof(className[0])) == 0)
		return false;
	return tc::tcscmp(className, _T("#32770")) == 0;
}

static void PostCommandMessage(WPARAM wParam)
{
	HWND hwndTarget = GetForegroundWindow();
	if (IsWindowDialog(hwndTarget))
		return;
	PostMessage(hwndTarget, WM_COMMAND, wParam, 0);
}

LRESULT CALLBACK CMouseHook::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);

	if (wParam == WM_LBUTTONDOWN)
	{
		EndRightWheelScrolling();
	}
	else if (wParam == WM_RBUTTONDOWN)
	{
		m_bRButtonDown = true;
	}
	else if (wParam == WM_RBUTTONUP)
	{
		m_bRButtonDown = false;
		EndRightWheelScrolling();
	}
	else if (wParam == WM_MOUSEWHEEL)
	{
		MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
		short zDelta = HIWORD(pMouseStruct->mouseData);

		if (GetAsyncKeyState(VK_MENU) & 0x8000)
		{
			// When hold Alt key, use nFlags to check MK_CONTROL MK_SHIFT holding got problem, Use GetAsyncKeyState() instead.
			const auto bShiftDown = GetAsyncKeyState(VK_SHIFT) & 0x8000;
			const auto bControlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000;
			// zDelta > 0 scrool up, < 0 scrool down
			if (zDelta > 0)
			{
				// Check Shift key hold for mice without HWheel function
				if (bShiftDown && bControlDown)
				{
					// Alt+Ctrl+Shift+ScrollUp as Alt+Ctrl+Left
					PostCommandMessage(ID_R2LNEXT);
					return 1;
				}
				else if (bShiftDown)
				{
					// Alt+Shift+ScrollUp as Alt+Left
					PostCommandMessage(ID_R2L);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+ScrollUp as Alt+Up
					PostCommandMessage(ID_PREVDIFF);
					return 1;
				}
			}
			else if (zDelta < 0)
			{
				// Check Shift key hold for mice without HWheel function
				if (bShiftDown && bControlDown)
				{
					// Alt+Ctrl+Shift+ScrollDown as Alt+Ctrl+Right
					PostCommandMessage(ID_L2RNEXT);
					return 1;
				}
				else if (bShiftDown)
				{
					// Alt+Shift+ScrollDown as Alt+Right
					PostCommandMessage(ID_L2R);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+ScrollDown as Alt+Down
					PostCommandMessage(ID_NEXTDIFF);
					return 1;
				}
			}
		}

		// Hold mice right button for One-handed operation
		if (m_bRButtonDown)
		{
			HWND hwndTarget = GetForegroundWindow();
			if (zDelta > 0)
			{
				// RButton+ScrollUp as Alt+Up
				StartRightWheelScrolling();
				PostCommandMessage(ID_PREVDIFF);
				return 1;
			}
			else if (zDelta < 0)
			{
				// RButton+ScrollDown as Alt+Down
				StartRightWheelScrolling();
				PostCommandMessage(ID_NEXTDIFF);
				return 1;
			}
		}
	}
	else if (wParam == WM_MOUSEHWHEEL)
	{
		MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
		short zDelta = HIWORD(pMouseStruct->mouseData);

		if (GetAsyncKeyState(VK_MENU) & 0x8000)
		{
			HWND hwndTarget = GetForegroundWindow();
			const auto bControlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000;
			// zDelta > 0 scrool right, < 0 scrool left
			if (zDelta > 0)
			{
				if (bControlDown)
				{
					// Alt+Ctrl+HScrollRight as Alt+Ctrl+Right
					PostCommandMessage(ID_L2RNEXT);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+HScrollRight as Alt+Right
					PostCommandMessage(ID_L2R);
					return 1;
				}
			}
			else if (zDelta < 0)
			{
				if (bControlDown)
				{
					// Alt+Ctrl+HScrollLeft as Alt+Ctrl+Left
					PostCommandMessage(ID_R2LNEXT);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+HScrollLeft as Alt+Left
					PostCommandMessage(ID_R2L);
					return 1;
				}
			}
		}

		// Hold mice right button for One-handed operation
		if (m_bRButtonDown)
		{
			HWND hwndTarget = GetForegroundWindow();
			if (zDelta > 0)
			{
				// RButton+ScrollRight as Alt+Right
				StartRightWheelScrolling();
				PostCommandMessage(ID_L2R);
				return 1;
			}
			else if (zDelta < 0)
			{
				// RButton+ScrollLeft as Alt+Left
				StartRightWheelScrolling();
				PostCommandMessage(ID_R2L);
				return 1;
			}
		}
	}
	return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
}

void CMouseHook::SetMouseHook()
{
	m_hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc, AfxGetInstanceHandle(), GetCurrentThreadId());
}

void CMouseHook::UnhookMouseHook()
{
	if (m_hMouseHook)
	{
		UnhookWindowsHookEx(m_hMouseHook);
		m_hMouseHook = nullptr;
	}
}

