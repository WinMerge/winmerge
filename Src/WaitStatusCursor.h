#ifndef WaitStatusCursor_included_h
#define WaitStatusCursor_included_h

class IStatusDisplay;

// A WaitStatusCursor does what a CWaitCursor does, and also sets a status message.
// Clients create WaitStatusCursors instead of CWaitCursors, and are isolated from
// status message implementation. Whoever wants to implement the status messages
// should call SetStatusDisplay to begin (or end) doing so.
class WaitStatusCursor
{

// factor/class interface
public:
	static void SetStatusDisplay(IStatusDisplay * piStatusDisplay);

// public interface
public:
	WaitStatusCursor(LPCTSTR fmt, ...);
	void ChangeMsg(LPCTSTR fmt, ...);
	void End();
	~WaitStatusCursor();


// implementation methods
protected:

// implementation data
private:
	CString m_msg;
	CString m_oldmsg;
	bool m_ended;
	CWaitCursor m_wait;

// factory/class data
private:
	static IStatusDisplay * c_piStatusDisplay;
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
