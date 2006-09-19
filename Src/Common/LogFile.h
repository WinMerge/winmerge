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
 * @brief Class for writing log files.
 *
 * Allows setting masks and levels for messages. They are used for
 * filtering messages written to log. For example usually its not
 * needed to see all informal messages, but errors are always good
 * to log. For simpler usage, default is that all messages are written
 * and functions with take just message in are provided.
 *
 * Outputting messages can be limited by giving a max level for outputted
 * messages. Only lower level messages are then outputted. For example if
 * level is set to LERROR only error messages are outputted.
 *
 * Written messages are given using printf() -style messageformat specifiers.
 * See MSDN documentation about printf() function for more information.
 *
 * @note User can easily define more levels, just add new constant to
 * struct LOGLEVEL above, and possibly prefix to GetPrefix(UINT level).
 */
class CLogFile  
{
public:
	/** @brief Messagelevels for log writing. */
	enum 
	{
		LALL = -1, /**< All messages written */
		LERROR = 0x1, /**< Error messages */
		LWARNING = 0x2, /**< Warning messages */ 
		LNOTICE = 0x4, /**< Important messages */
		LMSG = 0x8, /**< Normal messages */
		LCODEFLOW = 0x10, /**< Code flow messages */
		LCOMPAREDATA = 0x20, /**< Compare data (dump) */

		/* These flags are not loglevels, but have special meaning */
		LOSERROR = 0x1000, /**< Append description of last error */
		LSILENTVERIFY = 0x2000, /**< No VERIFY popup, please */
		LDEBUG = 0x4000,  /**< Append message to debug window as well */
	};

	CLogFile(LPCTSTR szLogName = NULL, LPCTSTR szLogPath = NULL,
		BOOL bDeleteExisting = FALSE);
	virtual ~CLogFile();

	CString SetFile(const CString & strFile, const CString & strPath = _T(""),
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

	void SetMaxLogSize(DWORD dwMax) { m_nMaxSize = dwMax; }
	CString GetPath() const { return m_strLogPath; }


protected:
	void Prune(FILE *f);
	LPCTSTR GetPrefix(UINT level) const;
	void WriteV(UINT level, LPCTSTR pszFormat, va_list);
	void WriteRaw(LPCTSTR msg);

private:
	HANDLE    m_hLogMutex; /**< Mutex protecting log writing */
	DWORD     m_nMaxSize; /**< Max size of the log file */
	BOOL      m_bEnabled; /**< Is logging enabled? */
	CString   m_strLogPath; /**< Full path to log file */
	UINT      m_nDefaultLevel; /**< Default level for log messages */
	UINT      m_nMaskLevel; /**< Level to mask messages written to log */
};


#endif // !defined(AFX_LOGFILE_H__803A3641_FE03_11D0_95CD_444553540000__INCLUDED_)
