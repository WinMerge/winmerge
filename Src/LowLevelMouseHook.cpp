#include <StdAfx.h>
#include "LowLevelMouseHook.h"

void CALLBACK CMouseHook::TimerProc(HWND unnamedParam1, UINT unnamedParam2, UINT_PTR id, DWORD unnamedParam4HWND)
{
	KillTimer(nullptr, id);
	EndMenu();
	m_bIgnoreRBUp = false;
}

LRESULT CALLBACK CMouseHook::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);

	if (wParam == WM_RBUTTONDOWN)
	{
		m_bRButtonDown = true;
	}
	else if (wParam == WM_RBUTTONUP)
	{
		m_bRButtonDown = false;
		if (m_bIgnoreRBUp)
		{
			LRESULT result = CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
			SetTimer(nullptr, 0, USER_TIMER_MINIMUM, TimerProc);
			return result;
		}
	}
	else if (wParam == WM_MOUSEWHEEL)
	{
		MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
		short zDelta = HIWORD(pMouseStruct->mouseData);

		if (GetKeyState(VK_MENU) & 0x8000)
		{
			HWND hwndTarget = GetForegroundWindow();
			// When hold Alt key, use nFlags to check MK_CONTROL MK_SHIFT holding got problem, Use GetKeyState() instead.
			const auto bShiftDown = GetKeyState(VK_SHIFT) & 0x8000;
			const auto bControlDown = GetKeyState(VK_CONTROL) & 0x8000;
			// zDelta > 0 scrool up, < 0 scrool down
			if (zDelta > 0)
			{
				// Check Shift key hold for mice without HWheel function
				if (bShiftDown && bControlDown)
				{
					// Alt+Ctrl+Shift+ScrollUp as Alt+Ctrl+Left
					PostMessage(hwndTarget, WM_COMMAND, ID_R2LNEXT, 0);
					return 1;
				}
				else if (bShiftDown)
				{
					// Alt+Shift+ScrollUp as Alt+Left
					PostMessage(hwndTarget, WM_COMMAND, ID_R2L, 0);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+ScrollUp as Alt+Up
					PostMessage(hwndTarget, WM_COMMAND, ID_PREVDIFF, 0);
					return 1;
				}
			}
			else if (zDelta < 0)
			{
				// Check Shift key hold for mice without HWheel function
				if (bShiftDown && bControlDown)
				{
					// Alt+Ctrl+Shift+ScrollDown as Alt+Ctrl+Right
					PostMessage(hwndTarget, WM_COMMAND, ID_L2RNEXT, 0);
					return 1;
				}
				else if (bShiftDown)
				{
					// Alt+Shift+ScrollDown as Alt+Right
					PostMessage(hwndTarget, WM_COMMAND, ID_L2R, 0);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+ScrollDown as Alt+Down
					PostMessage(hwndTarget, WM_COMMAND, ID_NEXTDIFF, 0);
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
				m_bIgnoreRBUp = true;
				PostMessage(hwndTarget, WM_COMMAND, ID_PREVDIFF, 0);
				return 1;
			}
			else if (zDelta < 0)
			{
				// RButton+ScrollDown as Alt+Down
				m_bIgnoreRBUp = true;
				PostMessage(hwndTarget, WM_COMMAND, ID_NEXTDIFF, 0);
				return 1;
			}
		}
	}
	else if (wParam == WM_MOUSEHWHEEL)
	{
		MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
		short zDelta = HIWORD(pMouseStruct->mouseData);

		if (GetKeyState(VK_MENU) & 0x8000)
		{
			HWND hwndTarget = GetForegroundWindow();
			const auto bControlDown = GetKeyState(VK_CONTROL) & 0x8000;
			// zDelta > 0 scrool right, < 0 scrool left
			if (zDelta > 0)
			{
				if (bControlDown)
				{
					// Alt+Ctrl+HScrollRight as Alt+Ctrl+Right
					PostMessage(hwndTarget, WM_COMMAND, ID_L2RNEXT, 0);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+HScrollRight as Alt+Right
					PostMessage(hwndTarget, WM_COMMAND, ID_L2R, 0);
					return 1;
				}
			}
			else if (zDelta < 0)
			{
				if (bControlDown)
				{
					// Alt+Ctrl+HScrollLeft as Alt+Ctrl+Left
					PostMessage(hwndTarget, WM_COMMAND, ID_R2LNEXT, 0);
					return 1;
				}
				else if (!m_bRButtonDown)
				{
					// Alt+HScrollLeft as Alt+Left
					PostMessage(hwndTarget, WM_COMMAND, ID_R2L, 0);
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
				m_bIgnoreRBUp = true;
				PostMessage(hwndTarget, WM_COMMAND, ID_L2R, 0);
				return 1;
			}
			else if (zDelta < 0)
			{
				// RButton+ScrollLeft as Alt+Left
				m_bIgnoreRBUp = true;
				PostMessage(hwndTarget, WM_COMMAND, ID_R2L, 0);
				return 1;
			}
		}
	}
	return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
}

void CMouseHook::SetMouseHook()
{
	m_hMouseHook = SetWindowsHookEx(WH_MOUSE, LowLevelMouseProc, GetModuleHandle(nullptr), GetCurrentThreadId());
}

void CMouseHook::UnhookMouseHook()
{
	if (m_hMouseHook)
	{
		UnhookWindowsHookEx(m_hMouseHook);
		m_hMouseHook = nullptr;
	}
}

