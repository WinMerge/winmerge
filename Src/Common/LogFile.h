/** 
 * @file  LogFile.h
 *
 * @brief Declaration file for CLogFile
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_LOGFILE_H__803A3641_FE03_11D0_95CD_444553540000__INCLUDED_)
#define AFX_LOGFILE_H__803A3641_FE03_11D0_95CD_444553540000__INCLUDED_

/**
 * @brief Messagelevels for log writing.
 */
namespace LOGLEVEL
{
	enum 
	{
		LALL = -1, /**< All messages written */
		LERROR = 0x1, /**< Error messages */
		LWARNING = 0x2, /**< Warning messages */ 
		LNOTICE = 0x4, /**< Important messages */
		LMSG = 0x8, /**< Normal messages */
		LCODEFLOW = 0x10, /**< Code flow messages */
		LCOMPAREDATA = 0x20,
		LOSERROR = 0x1000, /**< Append description of last error */
		LSILENTVERIFY = 0x2000, /**< No VERIFY popup, please */
		LDEBUG = 0x4000,  /**< Append message to debug window as well */
	};
};

/**
 * @brief Class for writing log files.
 *
 * Allows setting masks and levels for messages. They are used for
 * filtering messages written to log. For example usually its not
 * needed to see all informal messages, but errors are always good
 * to log. For simpler usage, default is that all messages are written
 * and functions with take just message in are provided.
 * @note User can easily define more levels, just add new constant to
 * namespace LOGLEVEL above, and possibly prefix to GetPrefix(UINT level).
 */
class CLogFile  
{
public:
	CLogFile(LPCTSTR szLogName = NULL, LPCTSTR szLogPath = NULL,
		BOOL bDeleteExisting = FALSE);
	virtual ~CLogFile();

	CString SetFile(CString strFile, CString strPath = _T(""),
		BOOL bDelExisting = FALSE);
	void EnableLogging(BOOL bEnable);
	UINT GetDefaultLevel() const;
	void SetDefaultLevel(UINT logLevel);
	UINT GetMaskLevel() const;
	void SetMaskLevel(UINT maskLevel);

	UINT Write(LPCTSTR pszFormat, ...);
	UINT Write(DWORD idFormatString, ...);
	UINT Write(UINT level, LPCTSTR pszFormat, ...);
	UINT Write(UINT level, DWORD idFormatString, ...);

	// overloaded Write Function to map to Write to Error Set code //
	void WriteError(CString JobID, CString ProcessID, CString Event, long ecode, CString CIndex);

	void SetMaxLogSize(DWORD dwMax) { m_nMaxSize = dwMax; }
	CString GetPath() const { return m_strLogPath; }


protected:
	void Prune(FILE *f);
	LPCTSTR GetPrefix(UINT level) const;
	void WriteV(UINT level, LPCTSTR pszFormat, va_list);
	void WriteRaw(LPCTSTR msg);

private:
	HANDLE    m_hLogMutex;
	DWORD     m_nMaxSize;
	BOOL      m_bEnabled;
	CString   m_strLogPath;
	UINT      m_nDefaultLevel;
	UINT      m_nMaskLevel;
};


#endif // !defined(AFX_LOGFILE_H__803A3641_FE03_11D0_95CD_444553540000__INCLUDED_)
