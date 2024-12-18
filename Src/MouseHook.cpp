#include <StdAfx.h>
#include "MouseHook.h"
#include <chrono>

#ifndef WM_MOUSEHWHEEL
#  define WM_MOUSEHWHEEL 0x20e
#endif

// Define state matrix
const std::function<bool(LPARAM)> CMouseHook::stateMatrix[3][6] = {
	// State::Idle
	{
		[](LPARAM) { Transition(State::RightButtonDown); return false; }, // WM_RBUTTONDOWN
		[](LPARAM) { EndRightWheelScrolling(); return false; }, // WM_RBUTTONUP
		[](LPARAM) { return false; }, // WM_LBUTTONDOWN
		[](LPARAM) { return false; }, // WM_MBUTTONUP
		[](LPARAM lParam) { return MouseWheelAction(lParam); }, // WM_MOUSEWHEEL
		[](LPARAM lParam) { return MouseHWheelAction(lParam); } // WM_MOUSEHWHEEL
	},
	// State::RightButtonDown
	{
		[](LPARAM) { return false; }, // WM_RBUTTONDOWN
		[](LPARAM) { Transition(State::Idle); EndRightWheelScrolling(); return false; }, // WM_RBUTTONUP
		[](LPARAM) { Transition(State::Idle); EndRightWheelScrolling(); return false; }, // WM_LBUTTONDOWN
		[](LPARAM) { Transition(State::HorizontalScrollSimulated); return false; }, // WM_MBUTTONUP
		[](LPARAM lParam) { return RightButtonDown_MouseWheel(lParam); }, // WM_MOUSEWHEEL
		[](LPARAM lParam) { return RightButtonDown_MouseHWheel(lParam); } // WM_MOUSEHWHEEL
	},
	// State::HorizontalScrollSimulated
	{
		[](LPARAM) { return false; }, // WM_RBUTTONDOWN
		[](LPARAM) { Transition(State::Idle); EndRightWheelScrolling(); return false; }, // WM_RBUTTONUP
		[](LPARAM) { Transition(State::Idle); EndRightWheelScrolling(); return false; }, // WM_LBUTTONDOWN
		[](LPARAM) { Transition(State::RightButtonDown); return false; }, // WM_MBUTTONUP
		[](LPARAM lParam) { // WM_MOUSEWHEEL
			const auto b = RightButtonDown_MouseWheel(lParam);
			Transition(State::RightButtonDown);
			return b;
		},
		[](LPARAM lParam) { // WM_MOUSEHWHEEL
			const auto b = RightButtonDown_MouseHWheel(lParam);
			Transition(State::RightButtonDown);
			return b;
		}
	}
};

void CMouseHook::Transition(State nextState) {
    m_currentState = nextState;
}

LRESULT CALLBACK CMouseHook::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);

	const static std::unordered_map<UINT, int> actionIdx =
	{
		 {WM_RBUTTONDOWN, 0}
		,{WM_RBUTTONUP,   1}
		,{WM_LBUTTONDOWN, 2}
		,{WM_MBUTTONUP,   3}
		,{WM_MOUSEWHEEL,  4}
		,{WM_MOUSEHWHEEL, 5}
	};
	auto it = actionIdx.find(static_cast<UINT>(wParam));
	if (it != actionIdx.end())
	{
		auto bRet = stateMatrix[static_cast<int>(m_currentState)][it->second](lParam);
		if (bRet)
			return 1;
	}

	return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
}

bool CMouseHook::MouseWheelAction(LPARAM lParam)
{
	MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
	short zDelta = HIWORD(pMouseStruct->mouseData);

	if (GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		HWND hwndTarget = GetForegroundWindow();
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
			else if (m_currentState == State::Idle)
			{
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
			else if (m_currentState == State::Idle)
			{
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
				PostMessage(hwndTarget, WM_COMMAND, ID_L2RNEXT, 0);
				return true;
			}
			else if (m_currentState == State::Idle)
			{
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
			else if (m_currentState == State::Idle)
			{
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
	MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
	short zDelta = HIWORD(pMouseStruct->mouseData);
	HWND hwndTarget = GetForegroundWindow();
	if (zDelta > 0)
	{
		// RButton(hold)+ScrollUp as Alt+Up, RButton(hold) MButtonClk WheelScrollUp as Alt+Left
		StartRightWheelScrolling();
		PostMessage(hwndTarget, WM_COMMAND, (State::HorizontalScrollSimulated == m_currentState) ?
			ID_R2L : ID_PREVDIFF, 0);
		return true;
	}
	else if (zDelta < 0)
	{
		// RButton(hold)+ScrollDown as Alt+Down, RButton(hold) MButtonClk WheelScrollUp as Alt+Right
		StartRightWheelScrolling();
		PostMessage(hwndTarget, WM_COMMAND, (State::HorizontalScrollSimulated == m_currentState) ?
			ID_L2R : ID_NEXTDIFF, 0);
		return true;
	}
	return false;
}

bool CMouseHook::RightButtonDown_MouseHWheel(LPARAM lParam)
{
	MOUSEHOOKSTRUCTEX* pMouseStruct = (MOUSEHOOKSTRUCTEX*)lParam;
	short zDelta = HIWORD(pMouseStruct->mouseData);
	HWND hwndTarget = GetForegroundWindow();
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
