class CMouseHook
{
public:
	static void SetMouseHook();
	static void UnhookMouseHook();
	static bool IsRightWheelScrolling() { return m_bIgnoreRBUp; }
private:
	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void CALLBACK TimerProc(HWND unnamedParam1, UINT unnamedParam2, UINT_PTR id, DWORD unnamedParam4HWND);
	inline static HHOOK m_hMouseHook;
	inline static bool m_bIgnoreRBUp;
	inline static bool m_bRButtonDown;
};
