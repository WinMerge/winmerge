#ifndef WaitStatusCursor_included_h
#define WaitStatusCursor_included_h

class IStatusDisplay;

/**
 * @brief CustomStatusCursor enhanced cursor with GUI status feedback.
 * 
 * A CustomStatusCursor does what a CWaitCursor does, plus the caller selects
 * the wait cursor and sets the status message. 
 * Clients create WaitStatusCursors instead of CWaitCursors (don't mix them), and are isolated from
 * status message implementation. Whoever wants to implement the status messages
 * should call SetStatusDisplay to begin (or end) doing so.
 *
 * Example: CustomStatusCursor(0, IDC_APPSTARTING, _T("Rescanning"));
 *
 * @note This class must be called in the main thread
 * It supports multithread (just uncomment the critical section code)
 * But StatusDisplay does not... 
 */
class CustomStatusCursor
{
// public interface
public:
	CustomStatusCursor(HINSTANCE hinst, LPCTSTR lpCursorName, LPCTSTR fmt, ...);
	void ChangeMsg(LPCTSTR fmt, ...);
	void End();
	~CustomStatusCursor();

// implementation data
protected:
	// default constructor for derivation, in two steps 'cause of variable argument list
	CustomStatusCursor();
	void Create(HINSTANCE hinst, LPCTSTR lpCursorName, LPCTSTR fmt, ...);

private:
	int posInStack;
	bool m_ended;

	/**
	 * @brief Number of active wait cursors + 1
	 *
	 * @note valid values : 0 (no wait cursor), 
	 * >= 2 (stack[0] is the default cursor, and stack[1..n] are wait cursors)
	 */
	static int stackSize;
	/**
	 * @brief Stack of messages, the currently displayed is the one on top of stack
	 *
	 * @note Allow overlapping creation/destruction of messages
	 */
	static CStringArray msgStack;
	/**
	 * @brief Stack of cursors, the currently displayed is the one on top of stack
	 *
	 * @note Allow overlapping creation/destruction of cursors
	 */
	static CPtrArray cursorStack;
	static CDWordArray validStack;

// critical section 
/*
private:
	static CRITICAL_SECTION CriticalSectionStack;
	static BOOL bCriticalSectionExist;
	static BOOL bCriticalSectionEntered;
	static void EnterStackCriticalSection();
	static void LeaveStackCriticalSection();
	static BOOL TryEnterStackCriticalSection();
*/

// factor/class interface
public:
	static void SetStatusDisplay(IStatusDisplay * piStatusDisplay);
private:
	static IStatusDisplay * c_piStatusDisplay;

	// for OnSetCursor
public:
	static BOOL HasWaitCursor();
	static void RestoreWaitCursor();
};

/**
 * @brief WaitStatusCursor enhanced with GUI status feedback.
 * 
 * A WaitStatusCursor is a limited CustomStatusCursor.
 * It is easier to create and display the standard wait cursor.
 */
class WaitStatusCursor : public CustomStatusCursor
{
// public interface
public:
	WaitStatusCursor(LPCTSTR fmt, ...);
	WaitStatusCursor(UINT fmtid, ...);
};



// Something that implements status messages for WaitStatusCursors
class IStatusDisplay
{
public:
	virtual CString BeginStatus(LPCTSTR str) = 0;
	virtual void ChangeStatus(LPCTSTR str) = 0;
	virtual void EndStatus(LPCTSTR str, LPCTSTR oldstr) = 0;
};

#endif // WaitStatusCursor_included_h
