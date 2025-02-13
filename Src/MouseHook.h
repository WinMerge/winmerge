#include <chrono>

class CMouseHook
{
public:
	static void SetMouseHook();
	static void UnhookMouseHook();
	static bool IsRightWheelScrolling() { return m_bIgnoreRBUp || (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_endTimeRightWheelScrolling).count() < 100); }
private:
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	inline static void StartRightWheelScrolling() { m_bIgnoreRBUp = true; }
	inline static void EndRightWheelScrolling() { if (!m_bIgnoreRBUp) return; m_endTimeRightWheelScrolling = std::chrono::system_clock::now(); m_bIgnoreRBUp = false; }
	inline static HHOOK m_hMouseHook;
	inline static bool m_bIgnoreRBUp;
	inline static bool m_bRButtonDown;
	inline static std::chrono::system_clock::time_point m_endTimeRightWheelScrolling;
};
