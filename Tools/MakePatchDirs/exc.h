/**
 *  @file    exc.h
 *  @author  Perry Rapp, Creator, 2001
 *  @date    Created: 2001/10/09
 *  @date    Edited:  2001/11/28
 *  @brief   Declares exception/reporting functions
 *
 * Copyright: 2001, Perry Rapp
 */

#ifndef exc_h
#define exc_h

#ifndef __AFXMT_H__
#include <afxmt.h> // MFC multithreading classes
#endif

#ifndef __AFXTEMPL_H__
#include <afxtempl.h> // MFC C++ template collection classes
#endif

#ifndef __AFXDISP_H__
#include <afxdisp.h> // MFC Dispatch support classes (incl. COleDateTime)
#endif

namespace perry {
class excException;

// documentation below
class exc
{
public:
	// logging
		// log() & logLastSystemError() do not increment #errors)
		// others increment it unless exception is marked as having been logged already)
	void log(LPCTSTR szText);
	void log(CException * pExc, LPCTSTR szText=NULL);
	void logError(LPCTSTR szText);
	void logError(CException * pExc, LPCTSTR szText=NULL);
	void logLastSystemError();
	void logAndThrowError(CException * pExc, LPCTSTR szText=NULL);
	void logAndThrowError(LPCTSTR szText);

	// NB: callbacks can cancel messages, which also cancels throws

	// message box (also logs contents)
	void reportError(LPCTSTR szText);
	void reportError(CException * pExc, LPCTSTR szText=NULL);
	void reportAndThrowError(CException * pExc, LPCTSTR szText=NULL);
	void reportAndThrowError(LPCTSTR szText);
	// throwing exception (also logs contents, but does not do message box)
	void throwErrorString(LPCTSTR szText);
	void throwError(CException * pExc);

// settings
	void setLog(LPCTSTR szLogFilepath);

	// protect logging with semaphore
	bool isMultithreaded() const { return m_bMultithreaded; }
	void setMultithreaded(bool multithreaded=true) { m_bMultithreaded=multithreaded; }
	
	// close file after every log message
	bool isFlushEveryMessage() const { return m_bFlushEveryMessage; }
	void setFlushEveryMessage(bool bFlushEveryMessage=true) { m_bFlushEveryMessage=bFlushEveryMessage; }
	
	// close file if it has been a while since last message
	int getFlushTimeInterval() const { return m_nFlushTimeInterval; }
	void setFlushTimeInterval(int nSeconds=60) { m_nFlushTimeInterval=nSeconds; }
	
	// close file if it has been a number of messages since last message
	int getFlushMessageInterval() const { return m_nFlushMessageInterval; }
	void setFlushMessageInterval(int nMessages=10) { m_nFlushMessageInterval=nMessages; }
	
	// replace previous log file (when writing first entry)
	bool isWipePrevious() const { return m_bWipePrevious; }
	void setWipePrevious(bool bWipe=true) { m_bWipePrevious=bWipe; }
	
	// log filepath
	CString getLog() const { return m_sLogFilepath; }
	
	// error pre-callback (clear sText to suppress error)
	typedef void (*CALLBACKPTR)(void * param, LPCTSTR szTime, CString & sText, bool error);
	void addCallback(CALLBACKPTR ptr, void * param);
	void removeCallback(CALLBACKPTR ptr, void * param);

	// msgbox custom implementation
	typedef void (*MSGBOXPTR)(void * param, LPCTSTR szText);
	void setMsgbox(MSGBOXPTR fnc, void * param) { m_msgboxfnc = fnc; m_msgparam = param; }
	
	// error count
	long getErrorCount() const { return m_nErrors; }
	long getLogCount() const { return m_nMessageCount; }
	void resetCounts() { m_nErrors=m_nMessageCount=0; }
	
	// override all visible msgbox strings
	void setErrorDisplayString(LPCTSTR str) { m_sErrorDisplayOverride = str; }
	CString getErrorDisplayString() const { return m_sErrorDisplayOverride; }

	// for reportAndThrow, should msgboxes be delayed ?
	void setDelayReports(bool bDelay=true) { m_bDelayReport=bDelay; }
	bool getDelayReports() const { return m_bDelayReport; }

	// what time was the first msg box displayed ?
	COleDateTime GetMsgBoxTime() const { return m_timeMsgBox; }

// Implementation methods (publicly available)
public:
	bool isTimeToFlush();
	static CString getString(CException * pExc);
	static CString getTypeName(const CException * pExc);

// Implementation methods (not public)
protected:
		// all these return false if cancelled via callback
	bool dolog(CString & str, bool error);
	bool dolog(CException * pExc, LPCTSTR szText, bool error);
	bool doreport(LPCTSTR szText, bool error);
	bool doreport(CException * pExc, LPCTSTR szText, bool error);
	excException * Extract(CException * pExc);
	excException * Wrap(CException * pExc);
	void domsgbox(CString str);

	CString GetIntro();


// Construction
public:
	exc()
		// CString m_sLogFilepath
		: m_bMultithreaded(false)
		// CStdioFile m_logfile;
		, m_bFlushEveryMessage(true)
		, m_nFlushTimeInterval(10)
		, m_nLastFlushTime(0)
		, m_nLastFlushMessage(0)
		, m_nMessageCount(0)
		, m_nFlushMessageInterval(10)
		, m_bWipePrevious(false)
		// CSemaphore m_sem
		// CallbackList m_callbacks
		, m_nErrors(0)
		// CString m_sErrorDisplayOverride
		, m_bDelayReport(false)
		, m_msgboxfnc(0)
		, m_msgparam(0)
		{
		}
	~exc();

	struct callback { CALLBACKPTR fnc; void * param; callback(CALLBACKPTR f=0, void * p=0) : fnc(f), param(p) { } };
	typedef CList<callback, callback> CallbackList;
// Implementation data
private:
	CString m_sLogFilepath;
	bool m_bMultithreaded;
	CStdioFile m_logfile;
	bool m_bFlushEveryMessage; // write every message out separately
	int m_nFlushTimeInterval; // flush if over this much time since last flush
	int m_nLastFlushTime;
	int m_nLastFlushMessage;
	long m_nMessageCount;
	int m_nFlushMessageInterval; // flush after this many messages
	bool m_bWipePrevious;
	CSemaphore m_sem; // for multithreaded use
	CallbackList m_callbacks;
	long m_nErrors;
	CString m_sErrorDisplayOverride;
	bool m_bDelayReport;
	MSGBOXPTR m_msgboxfnc;
	void * m_msgparam;
	COleDateTime m_timeMsgBox;
};

// utility functions
bool RunNotepad(LPCTSTR szFilepath);
bool IsPathSep(TCHAR ch);
CString PathConcat(CString sPath, CString sFile);
CString GetModuleFilepath(HINSTANCE hinst);
CString GetPathFromFilepath(CString sFile);

// exception type used by exc
// exposed here in header in case client wants to catch or use them
class excException : public CException
{
public:
	DECLARE_DYNAMIC(excException)
	static excException * New(LPCTSTR sz) { return new excException(sz); }
	static excException * Chained(CException * pExc) { return new excException(pExc); }
	virtual BOOL GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT /*pnHelpContext*/ = NULL)
	{
		_tcsncpy(lpszError, m_str, nMaxError);
		return TRUE;
	}
	virtual int ReportError(UINT nType = MB_OK, UINT nMessageID = 0)
	{
		int rtn=IDOK;
		if (!IsReported())
		{
			rtn=CException::ReportError(nType, nMessageID);
			SetReported(true);
		}
		return rtn;
	}
	bool IsLogged() const { return m_bLogged; }
	bool IsReported() const { return m_bReported; }
	void SetLogged(bool logged=true) { m_bLogged=logged; }
	void SetReported(bool reported=true) { m_bReported=reported; }
	virtual CString GetString() const { return m_str; }
	const CException * getChained() const { return m_pExc; }
protected:
	excException(LPCTSTR sz) : m_str(sz),m_bLogged(false),m_bReported(false),m_pExc(0) { }
	excException(CException * pExc) : m_str(exc::getString(pExc)), m_bLogged(false),m_bReported(false), m_pExc(pExc) { }
	~excException() { if (m_pExc) { m_pExc->Delete(); m_pExc=0; } }
private:
	CString m_str;
	bool m_bLogged;
	bool m_bReported;
	CException * m_pExc; // throwing someone else's exception
};

/*!
	@class   exc

	Simplest way to use:

	** #1) Add these two lines to stdafx.h:

#include "exc.h"
extern perry::exc appexc;
 
	** #2) Add this line to stdafx.cpp:

perry::exc appexc;

	** #3a) Add these lines to App::InitInstance:

	appexc.setLog(_T("Wherever you want the log file.log")); 
 
	** #3b) or, to put it in the directory with the exe:

	CString sPath = perry::GetPathFromFilepath(perry::GetModuleFilepath(0));
	CString sAppName = m_pszAppName;
	CString sLog = perry::PathConcat(sPath, sAppName+_T(".log"));
	appexc.setLog(sLog);

	** #4) Write simple exception handlers like so (for message boxes):

		} catch(CException * pExc) {
			appexc.report(pExc);
			pExc->Delete();
		} catch(...) {
			appexc.logLastSystemError(); // Logs GetLastError if nonzero
			appexc.report(_T("Unknown exception in foo"));
		}
    
	** #5) Write simple exception handlers like so (for silent handling):

		} catch(CException * pExc) {
			appexc.log(pExc);
			pExc->Delete();
		} catch(...) {
			appexc.logLastSystemError(); // Logs GetLastError if nonzero
			appexc.log(_T("Unknown exception in foo"));
		}

	** #6) Translate exceptions like so (silently, but rethrowing):

		} catch(dbException & exc) {
			appexc.logLastSystemError();
			appexc.log(_T("fastdb exception"));
			appexc.throwString(exc.getMsg() ? exc.getMsg() : _T("Unspecified fastdb exception"));
			// this throws an excException, which is a type of CException
		}

	** #7) Write a message to the log at any time like so:
		appexc.log(_T("The user just put the mouse into the CD drive!"));

	** #8) Menu handlers for viewing log file from menu choice:

void CMsiBrowserApp::OnViewlog() 
{
	perry::RunNotepad(appexc.getLog());	
}
void CMsiBrowserApp::OnUpdateViewlog(CCmdUI* pCmdUI) 
{
	bool empty=true;
	CFileStatus fs;
	if (CFile::GetStatus(appexc.getLog(), fs))
	{
		if (fs.m_size>0)
			empty=false;
	}
	pCmdUI->Enable(!empty);
}


  Notes:
   An excException is logged when thrown, not when caught.
	An excException does not present a message box again if report is called 
	 for it a second time.
*/

} // namespace

#endif // exc_h
