// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__803A3641_FE03_11D0_95CD_444553540000__INCLUDED_)
#define AFX_LOGFILE_H__803A3641_FE03_11D0_95CD_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CLogFile  
{
public:

	CLogFile(LPCTSTR szLogName, LPCTSTR szLogPath = NULL, BOOL bDeleteExisting = FALSE);

	void Prune(FILE *f);
	void Write(LPCTSTR pszFormat, ...);
	void Write(DWORD idFormatString, ...);

	// overloaded Write Function to map to Write to Error Set code //
	void WriteError(CString JobID, CString ProcessID, CString Event, long ecode, CString CIndex);

	virtual ~CLogFile();
	void SetMaxLogSize(DWORD dwMax) { m_nMaxSize = dwMax; }
	CString GetPath() { return m_strLogPath; }

protected:
	DWORD		m_nMaxSize;
	CString	m_strLogPath;

};


#endif // !defined(AFX_LOGFILE_H__803A3641_FE03_11D0_95CD_444553540000__INCLUDED_)
