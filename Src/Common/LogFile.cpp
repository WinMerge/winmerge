/** 
 * @file  LogFile.h
 *
 * @brief Implementation file for CLogFile
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "LogFile.h"
#include <cstdio>
#include <Poco/ScopedLock.h>
#include <Poco/Debugger.h>
#include <ctime>
#include "UnicodeString.h"
#include "paths.h"
#include "Environment.h"
#include "unicoder.h"
#include "MergeApp.h"
#include "TFile.h"

using Poco::NamedMutex;
using Poco::ScopedLock;
using Poco::Debugger;

/** @brief Global name for mutes protecting log file access. */
static const char MutexName[] = "WINMERGE_LOG_MUTEX";

/** @brief Constant for Megabyte. */
static const int MEGA = 1024 * 1024;

/**
 * @brief Constructor
 * @param [in] szLogName Logfile name (including path).
 * @param [in] bDeleteExisting Do we delete existing log file with same name?
 */
CLogFile::CLogFile(const TCHAR * szLogName /*= NULL*/,
	 bool bDeleteExisting /*=false*/)
	: m_nMaxSize(MEGA)
	, m_bEnabled(false)
	, m_nDefaultLevel(LMSG)
	, m_nMaskLevel(LALL)
	, m_hLogMutex(MutexName)
{
	String path;
	if (szLogName != NULL)
		path = szLogName;
	SetFile(path, bDeleteExisting);
}

/**
 * @brief Destructor.
 */
CLogFile::~CLogFile()
{
	EnableLogging(false);
}

/**
 * @brief Set logfilename.
 * @param [in] strFile Filename and path of logfile.
 * @param bDelExisting If true, existing logfile with same name
 * is deleted.
 * @note If strPath param is empty then system TEMP folder is used.
 */
void CLogFile::SetFile(const String & strFile, bool bDelExisting /*= false*/)
{
	m_strLogPath = strFile;

	if (bDelExisting)
	{
		try { TFile(m_strLogPath).remove(); } catch (...) {}
	}
}

/**
 * @brief Enable/Disable writing log.
 * @param [in] bEnable If true logging is enabled.
 */
void CLogFile::EnableLogging(bool bEnable)
{
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	TCHAR s[80];
	_tcsftime(s, sizeof(s)/sizeof(s[0]), _T(" %A, %B %d, %Y    %H:%M:%S"), tm);

	if (bEnable)
	{
		m_bEnabled = true;
		Write(_T("\n*******\nLog Started: %s"), s);
		Write(_T("Path: %s\n*******\n"), m_strLogPath.c_str());
	}
	else
	{
		m_bEnabled = false;
		Write(_T("\n*******\nLog Stopped: %s\n"), s);
		Write(_T("*******\n"));
	}
}

/**
 * @brief Return default level for log messages
 * @return Default log level.
 */
unsigned CLogFile::GetDefaultLevel() const
{
	return m_nDefaultLevel;
}

/**
 * @brief Set default level for log messages
 * @param [in] logLevel New default message loglevel.
 */
void CLogFile::SetDefaultLevel(unsigned logLevel)
{
	m_nDefaultLevel = logLevel;
}

/**
 * @brief Get log message mask.
 *
 * Mask allows to select which level messages are written to log.
 * @return Current mask Level.
 */
unsigned CLogFile::GetMaskLevel() const
{
	return m_nMaskLevel;
}

/**
 * @brief Set log message mask.
 * @param [in] maskLevel New masking level for outputted messages.
 */
void CLogFile::SetMaskLevel(unsigned maskLevel)
{
	m_nMaskLevel = maskLevel;
}

/**
 * @brief Write formatted message with default log level.
 * @param [in] pszFormat Message format specifiers.
 * @return Nonzero if LOGLEVEL::LSILENTVERIFY flag is set.
 */
unsigned CLogFile::Write(const TCHAR * pszFormat, ...)
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
 * @brief Write formatted message to log with given level.
 * @param [in] level Level for message.
 * @param [in] pszFormat Messageformat specifier.
 * @return Nonzero if LOGLEVEL::LSILENTVERIFY flag is set.
 */
unsigned CLogFile::Write(unsigned level, const TCHAR * pszFormat, ...)
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
 * @brief Internal function for writing the messages.
 * @param [in] level Message's level.
 * @param [in] pszFormat Message's format specifier.
 * @param [in] arglist Message argumets to format string.
 */
void CLogFile::WriteV(unsigned level, const TCHAR * pszFormat, va_list arglist)
{
	String msg = string_format_arg_list(pszFormat, arglist);
	msg.insert(0, GetPrefix(level));
	if (level & LOSERROR)
	{
		msg += GetSysError();
	}
	msg.erase(msg.find_last_not_of(_T("\r\n")) + 1);
	msg += _T("\n");
	WriteRaw(msg.c_str());
	if (level & LDEBUG)
	{
		Debugger::message(ucr::toUTF8(msg));
	}
}

/**
 * @brief Internal function to write new line to log-file.
 * @param [in] msg Message to add to log-file.
 */
void CLogFile::WriteRaw(const TCHAR * msg)
{
	ScopedLock<NamedMutex> lock(m_hLogMutex);

	FILE *f;
	if ((f=_tfopen(m_strLogPath.c_str(), _T("a"))) != NULL)
	{
		_fputts(msg, f);

		// prune the log if it gets too big
		if (ftell(f) >= (int)m_nMaxSize)
			Prune(f);
		else
			fclose(f);
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
	size_t amt;
	FILE *tf;
	String tempfile = env_GetTempFileName(_T("."), _T("LOG"));
	TFile tfile(tempfile);
	try
	{
		tfile.remove();
		if ((tf = _tfopen(tempfile.c_str(), _T("w"))) != NULL)
		{
			fseek(f, ftell(f) / 4, SEEK_SET);
			_fputts(_T("#### The log has been truncated due to size limits ####\n"), tf);

			while ((amt = fread(buf, 1, 8196, f)) > 0)
				fwrite(buf, amt, 1, tf);
			fclose(tf);
			fclose(f);
			TFile(m_strLogPath).remove();
			tfile.renameTo(m_strLogPath);
		}
	}
	catch (...)
	{
	}
}

/**
 * @brief Return message prefix string for given loglevel.
 * @param [in] level Level to query prefix string.
 * @return Pointer to string containing prefix.
 */
const TCHAR * CLogFile::GetPrefix(unsigned level) const
{
	const TCHAR * str = _T("");
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

/** @brief Report DeleteFile() failure to Log */
unsigned CLogFile::DeleteFileFailed(const TCHAR * path)
{
	return Write(CLogFile::LERROR|CLogFile::LOSERROR|CLogFile::LDEBUG, _T("DeleteFile(%s) failed: "), path);
}
