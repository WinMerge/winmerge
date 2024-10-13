#pragma once

class CMouseHook
{
public:
	static BOOL PreTranslateMessage(MSG* pMsg);
	static bool CallWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	inline static bool m_bIgnoreRBUp;
	inline static bool m_bRButtonDown;
};
