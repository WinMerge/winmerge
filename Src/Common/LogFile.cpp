/** 
 * @file  LogFile.h
 *
 * @brief Implementation file for CLogFile
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "LogFile.h"
#include <afxinet.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/**
 * @brief Global name for mutes protecting log file access.
 */
static const TCHAR MutexName[] = _T("WINMERGE_LOG_MUTEX");

/**
 * @brief Constant for Megabyte.
 */
static const int MEGA = 1024 * 1024;

/**
 * @brief Constructor
 * @param [in] szLogName Logfile name (not including path).
 * @param [in] szLogPath Folder for log file.
 * @param [in] bDeleteExisting Do we delete existing log file with same name?
 */
CLogFile::CLogFile(LPCTSTR szLogName /*= NULL*/,
	 LPCTSTR szLogPath /*= NULL*/, BOOL bDeleteExisting /*=FALSE*/)
	: m_nMaxSize(MEGA)
	, m_bEnabled(FALSE)
	, m_nDefaultLevel(LMSG)
	, m_nMaskLevel(LALL)
{
	SetFile(szLogName, szLogPath, bDeleteExisting);
	m_hLogMutex = CreateMutex(NULL, FALSE, MutexName);
}

/**
 * @brief Destructor.
 */
CLogFile::~CLogFile()
{
	EnableLogging(FALSE);
	CloseHandle(m_hLogMutex);
}

/**
 * @brief Set logfilename.
 * @param [in] strFile Filename of logfile.
 * @param [in] strPath Folder for logfile.
 * @param bDelExisting If TRUE, existing logfile with same name
 * is deleted.
 * @return Full path to logfile.
 * @note If strPath param is empty then system TEMP folder is used.
 */
CString CLogFile::SetFile(const CString & strFile, const CString & strPath,
	BOOL bDelExisting /*= FALSE*/)
{
	TCHAR temp[MAX_PATH] = {0};

	// build the path to the logfile
	if (!strPath.IsEmpty())
		m_strLogPath = strFile;
	else
	{
		if (GetTempPath(MAX_PATH, temp))
			m_strLogPath = temp;
	}
	
	if (m_strLogPath.Right(1) != _T('\\'))
		m_strLogPath += _T('\\');

	m_strLogPath += strFile;

	if (bDelExisting)
		DeleteFile(m_strLogPath);
	
	return m_strLogPath;
}

/**
 * @brief Enable/Disable writing log.
 * @param [in] bEnable If TRUE logging is enabled.
 */
void CLogFile::EnableLogging(BOOL bEnable)
{
	CTime t = CTime::GetCurrentTime();
	CString s = t.Format(_T(" %A, %B %d, %Y    %H:%M:%S"));

	if (bEnable)
	{
		m_bEnabled = TRUE;
		Write(_T("\n*******\nLog Started: %s"), s);
		Write(_T("Path: %s\n*******\n"), m_strLogPath);
	}
	else
	{
		m_bEnabled = FALSE;
		Write(_T("\n*******\nLog Stopped: %s\n"), s);
		Write(_T("*******\n"));
	}
}

/**
 * @brief Return default level for log messages
 * @return Default log level.
 */
UINT CLogFile::GetDefaultLevel() const
{
	return m_nDefaultLevel;
}

/**
 * @brief Set default level for log messages
 * @param [in] logLevel New default message loglevel.
 */
void CLogFile::SetDefaultLevel(UINT logLevel)
{
	m_nDefaultLevel = logLevel;
}

/**
 * @brief Get log message mask.
 *
 * Mask allows to select which level messages are written to log.
 * @return Current mask Level.
 */
UINT CLogFile::GetMaskLevel() const
{
	return m_nMaskLevel;
}

/**
 * @brief Set log message mask.
 * @param [in] maskLevel New masking level for outputted messages.
 */
void CLogFile::SetMaskLevel(UINT maskLevel)
{
	m_nMaskLevel = maskLevel;
}

/**
 * @brief Write formatted message with default log level.
 * @param [in] pszFormat Message format specifiers.
 * @return Nonzero if LOGLEVEL::LSILENTVERIFY flag is set.
 */
UINT CLogFile::Write(LPCTSTR pszFormat, ...)
{
	if (m_bEnabled && (m_nDefaultLevel & m_nMaskLevel))
	{
		va_list arglist;
		va_start(arglist, pszFormat);
		WriteV(m_nDefaultLevel, pszFormat, arglist);
		va_end(arglist);
	}
	return m_nMaskLevel & LSILENTVERIFY;
}

/**
 * @brief Write message from resource with default level.
 * This function loads messageformat specifier string from
 * resource. And applies given parameters to it.
 * @param [in] idFormatString Resource id for message.
 * @return Nonzero if LOGLEVEL::LSILENTVERIFY flag is set.
 */
UINT CLogFile::Write(DWORD idFormatString, ...)
{
	CString strFormat;
	if (m_bEnabled && (m_nDefaultLevel & m_nMaskLevel)
		&& strFormat.LoadString(idFormatString))
	{
		va_list arglist;
		va_start(arglist, idFormatString);
		WriteV(m_nDefaultLevel, strFormat, arglist);
		va_end(arglist);
	}
	return m_nMaskLevel & LSILENTVERIFY;
}

/**
 * @brief Write formatted message to log with given level.
 * @param [in] level Level for message.
 * @param [in] pszFormat Messageformat specifier.
 * @return Nonzero if LOGLEVEL::LSILENTVERIFY flag is set.
 */
UINT CLogFile::Write(UINT level, LPCTSTR pszFormat, ...)
{
	if (m_bEnabled && (level & m_nMaskLevel))
	{
		va_list arglist;
		va_start(arglist, pszFormat);
		WriteV(level, pszFormat, arglist);
		va_end(arglist);
	}
	return m_nMaskLevel & LSILENTVERIFY;
}

/**
 * @brief Write message from resource to log with given level.
 * @param [in] level Messagelevel for this message.
 * @param [in] idFormatString ResourceID for message.
 * @return Nonzero if LOGLEVEL::LSILENTVERIFY flag is set.
 */
UINT CLogFile::Write(UINT level, DWORD idFormatString, ...)
{
	CString strFormat;
	if (m_bEnabled && (level & m_nMaskLevel)
		&& strFormat.LoadString(idFormatString))
	{
		va_list arglist;
		va_start(arglist, idFormatString);
		WriteV(level, strFormat, arglist);
		va_end(arglist);
	}
	return m_nMaskLevel & LSILENTVERIFY;
}

/**
 * @brief Internal function for writing the messages.
 * @param [in] level Message's level.
 * @param [in] pszFormat Message's format specifier.
 * @param [in] arglist Message argumets to format string.
 */
void CLogFile::WriteV(UINT level, LPCTSTR pszFormat, va_list arglist)
{
	CString msg;
	msg.FormatV(pszFormat, arglist);
	msg.Insert(0, GetPrefix(level));
	if (level & LOSERROR)
	{
		TCHAR cause[5120];
		CInternetException(GetLastError()).GetErrorMessage(cause, countof(cause));
		msg += cause;
	}
	msg.TrimRight(_T("\r\n"));
	msg += _T("\n");
	WriteRaw(msg);
	if (level & LDEBUG)
	{
		OutputDebugString(msg);
	}
}

/**
 * @brief Internal function to write new line to log-file.
 * @param [in] msg Message to add to log-file.
 */
void CLogFile::WriteRaw(LPCTSTR msg)
{
	DWORD dwWaitRes = WaitForSingleObject(m_hLogMutex, 10000);

	if (dwWaitRes == WAIT_OBJECT_0)
	{
		FILE *f;
		if ((f=_tfopen(m_strLogPath, _T("a"))) != NULL)
		{
			_fputts(msg, f);

			// prune the log if it gets too big
			if (ftell(f) >= (int)m_nMaxSize)
				Prune(f);
			else
				fclose(f);
		}
		ReleaseMutex(m_hLogMutex);
	}
}

/**
 * @brief Prune log file if it exceeds max given size.
 * @param [in] f Pointer to FILE structure.
 * @todo This is not safe function at all. We should check return values!
 */
void CLogFile::Prune(FILE *f)
{
	TCHAR buf[8196] = {0};
	DWORD amt;
	TCHAR tempfile[MAX_PATH];
	FILE *tf;
	GetTempFileName(_T("."),_T("LOG"),0,tempfile);
	DeleteFile(tempfile);
	if ((tf=_tfopen(tempfile,_T("w"))) != NULL)
	{
		fseek(f, ftell(f)/4, SEEK_SET);
		_fputts(_T("#### The log has been truncated due to size limits ####\n"), tf);

		while ((amt=fread(buf, 1, 8196, f)) > 0)
			fwrite(buf, amt, 1, tf);
		fclose(tf);
		fclose(f);
		DeleteFile(m_strLogPath);
		MoveFile(tempfile,m_strLogPath);
	}
}

/**
 * @brief Return message prefix string for given loglevel.
 * @param [in] level Level to query prefix string.
 * @return Pointer to string containing prefix.
 */
LPCTSTR CLogFile::GetPrefix(UINT level) const
{
	LPCTSTR str = _T("");
	switch (level & 0x0FFF)
	{
		case LERROR:
			str = _T("ERROR: ");
			break;
		case LWARNING:
			str = _T("WARNING: ");
			break;
		case LNOTICE:
			str = _T("NOTICE: ");
			break;
		case LMSG:
			break;
		case LCODEFLOW:
			str = _T("FLOW: ");
			break;
		case LCOMPAREDATA:
			str = _T("COMPARE: ");
			break;
		default:
			break;
	}
	return str;
}
