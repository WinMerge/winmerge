class CMouseHook
{
public:
	static void SetMouseHook();
	static void UnhookMouseHook();
	static bool IsRightWheelScrolling() { return m_bIgnoreRBUp; }
private:
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	inline static HHOOK m_hMouseHook;
	inline static bool m_bIgnoreRBUp;
	inline static bool m_bRButtonDown;
};
