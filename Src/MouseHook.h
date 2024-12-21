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
	inline static std::chrono::system_clock::time_point m_endTimeRightWheelScrolling;

	enum class State {
		Idle,
		RightButtonDown
	};
	inline static State m_currentState = State::Idle;
	inline static void Transition(State nextState) { m_currentState = nextState; };
	// State matrix definition
	static const std::function<bool(LPARAM)> stateMatrix[2][8];

	static bool MouseWheelAction(LPARAM lParam);
	static bool MouseHWheelAction(LPARAM lParam);
	static bool RightButtonDown_MouseWheel(LPARAM lParam);
	static bool RightButtonDown_MouseHWheel(LPARAM lParam);
	static bool RightButtonDown_ButtonClick(bool isLeftButton);
	inline static bool m_bLeftBtnDown = false;
	inline static bool m_bMidBtnDown = false;
};
