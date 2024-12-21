#include <StdAfx.h>
#include "MouseHook.h"
#include <chrono>

#ifndef WM_MOUSEHWHEEL
#  define WM_MOUSEHWHEEL 0x20e
#endif

// Define state matrix
const std::function<bool(LPARAM)> CMouseHook::stateMatrix[2][8] = {
	// State::Idle
	{
		[](LPARAM) {                            // WM_RBUTTONDOWN
			Transition(State::RightButtonDown);
			return false;
		},
		[](LPARAM) {                            // WM_RBUTTONUP
			EndRightWheelScrolling();
			return false;
		},
		[](LPARAM) { return false; },           // WM_LBUTTONDOWN
		[](LPARAM) { return false; },           // WM_LBUTTONUP
		[](LPARAM) { return false; },           // WM_MBUTTONDOWN
		[](LPARAM) { return false; },           // WM_MBUTTONUP
		CMouseHook::MouseWheelAction,           // WM_MOUSEWHEEL
		CMouseHook::MouseHWheelAction           // WM_MOUSEHWHEEL
	},
	// State::RightButtonDown
	{
		[](LPARAM) { return false; },           // WM_RBUTTONDOWN
		[](LPARAM) {                            // WM_RBUTTONUP
			// Release Right button
			Transition(State::Idle);
			EndRightWheelScrolling();
			return false;
		},
		[](LPARAM) {                            // WM_LBUTTONDOWN
			return CMouseHook::RightButtonDown_ButtonClick(true);
		},
		[](LPARAM) {                            // WM_LBUTTONUP
			return m_bLeftBtnDown ? (m_bLeftBtnDown = false), true : false;
		},
		[](LPARAM) {                            // WM_MBUTTONDOWN
			return CMouseHook::RightButtonDown_ButtonClick(false);
		},
		[](LPARAM) {                            // WM_MBUTTONUP
			return m_bMidBtnDown ? (m_bMidBtnDown = false), true : false;
		},
		CMouseHook::RightButtonDown_MouseWheel, // WM_MOUSEWHEEL
		CMouseHook::RightButtonDown_MouseHWheel // WM_MOUSEHWHEEL
	}
};

LRESULT CALLBACK CMouseHook::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);

	const static std::unordered_map<UINT, int> actionIdx =
	{
		 {WM_RBUTTONDOWN, 0}
		,{WM_RBUTTONUP,   1}
		,{WM_LBUTTONDOWN, 2}
		,{WM_LBUTTONUP  , 3}
		,{WM_MBUTTONDOWN, 4}
		,{WM_MBUTTONUP  , 5}
		,{WM_MOUSEWHEEL,  6}
		,{WM_MOUSEHWHEEL, 7}
	};
	const auto it = actionIdx.find(static_cast<UINT>(wParam));
	if (it != actionIdx.end())
	{
		const auto bRet = stateMatrix[static_cast<int>(m_currentState)][it->second](lParam);
		if (bRet)
			return 1;
	}

	return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
}

bool CMouseHook::MouseWheelAction(LPARAM lParam)
{
	const MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
	const short zDelta = HIWORD(pMouseStruct->mouseData);

	if (GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		const HWND hwndTarget = GetForegroundWindow();
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
				PostMessage(hwndTarget, WM_COMMAND, ID_R2LNEXT, 0);
				return true;
			}
			else if (bShiftDown)
			{
				// Alt+Shift+ScrollUp as Alt+Left
				PostMessage(hwndTarget, WM_COMMAND, ID_R2L, 0);
				return true;
			}
			else
			{
				assert(m_currentState == State::Idle);
				// Alt+ScrollUp as Alt+Up
				PostMessage(hwndTarget, WM_COMMAND, ID_PREVDIFF, 0);
				return true;
			}
		}
		else if (zDelta < 0)
		{
			// Check Shift key hold for mice without HWheel function
			if (bShiftDown && bControlDown)
			{
				// Alt+Ctrl+Shift+ScrollDown as Alt+Ctrl+Right
				PostMessage(hwndTarget, WM_COMMAND, ID_L2RNEXT, 0);
				return true;
			}
			else if (bShiftDown)
			{
				// Alt+Shift+ScrollDown as Alt+Right
				PostMessage(hwndTarget, WM_COMMAND, ID_L2R, 0);
				return true;
			}
			else
			{
				assert(m_currentState == State::Idle);
				// Alt+ScrollDown as Alt+Down
				PostMessage(hwndTarget, WM_COMMAND, ID_NEXTDIFF, 0);
				return true;
			}
		}
	}
	return false;
}

bool CMouseHook::MouseHWheelAction(LPARAM lParam)
{
	const MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
	const short zDelta = HIWORD(pMouseStruct->mouseData);

	if (GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		const HWND hwndTarget = GetForegroundWindow();
		const auto bControlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000;
		// zDelta > 0 scrool right, < 0 scrool left
		if (zDelta > 0)
		{
			if (bControlDown)
			{
				// Alt+Ctrl+HScrollRight as Alt+Ctrl+Right
				PostMessage(hwndTarget, WM_COMMAND, ID_L2RNEXT, 0);
				return true;
			}
			else
			{
				assert(m_currentState == State::Idle);
				// Alt+HScrollRight as Alt+Right
				PostMessage(hwndTarget, WM_COMMAND, ID_L2R, 0);
				return true;
			}
		}
		else if (zDelta < 0)
		{
			if (bControlDown)
			{
				// Alt+Ctrl+HScrollLeft as Alt+Ctrl+Left
				PostMessage(hwndTarget, WM_COMMAND, ID_R2LNEXT, 0);
				return true;
			}
			else
			{
				assert(m_currentState == State::Idle);
				// Alt+HScrollLeft as Alt+Left
				PostMessage(hwndTarget, WM_COMMAND, ID_R2L, 0);
				return true;
			}
		}
	}
	return false;
}

bool CMouseHook::RightButtonDown_MouseWheel(LPARAM lParam)
{
	const MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
	const short zDelta = HIWORD(pMouseStruct->mouseData);
	const HWND hwndTarget = GetForegroundWindow();
	if (zDelta > 0)
	{
		// RButton(hold)+ScrollUp as Alt+Up, RButton(hold) MButtonClk WheelScrollUp as Alt+Left
		StartRightWheelScrolling();
		PostMessage(hwndTarget, WM_COMMAND, ID_PREVDIFF, 0);
		return true;
	}
	else if (zDelta < 0)
	{
		// RButton(hold)+ScrollDown as Alt+Down, RButton(hold) MButtonClk WheelScrollUp as Alt+Right
		StartRightWheelScrolling();
		PostMessage(hwndTarget, WM_COMMAND, ID_NEXTDIFF, 0);
		return true;
	}
	return false;
}

bool CMouseHook::RightButtonDown_MouseHWheel(LPARAM lParam)
{
	const MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
	const short zDelta = HIWORD(pMouseStruct->mouseData);
	const HWND hwndTarget = GetForegroundWindow();
	if (zDelta > 0)
	{
		// RButton+ScrollRight as Alt+Right
		StartRightWheelScrolling();
		PostMessage(hwndTarget, WM_COMMAND, ID_L2R, 0);
		return true;
	}
	else if (zDelta < 0)
	{
		// RButton+ScrollLeft as Alt+Left
		StartRightWheelScrolling();
		PostMessage(hwndTarget, WM_COMMAND, ID_R2L, 0);
		return true;
	}
	return false;
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

bool CMouseHook::RightButtonDown_ButtonClick(bool isLeftButton)
{
	const auto bControlDown = GetAsyncKeyState(VK_CONTROL) & 0x8000;
	if(isLeftButton)
	{
		// Hold Right button + Left button click as Alt+Left, plus hold Ctrl as Ctrl+Alt+Left
		m_bLeftBtnDown = true;
		StartRightWheelScrolling();
		PostMessage(GetForegroundWindow(), WM_COMMAND, bControlDown ? ID_R2LNEXT : ID_R2L, 0);
	}
	else
	{
		// Hold Right button + Middle button click as Alt+Right, plus hold Ctrl as Ctrl+Alt+Right
		m_bMidBtnDown = true;
		StartRightWheelScrolling();
		PostMessage(GetForegroundWindow(), WM_COMMAND, bControlDown ? ID_L2RNEXT : ID_L2R, 0);
	}
	return true;
}
