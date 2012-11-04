/** 
 * @file  LogFile.h
 *
 * @brief Declaration file for CLogFile
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include "UnicodeString.h"
#define POCO_NO_UNWINDOWS 1
#include <Poco/NamedMutex.h>

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
 * struct LOGLEVEL above, and possibly prefix to GetPrefix(unsigned level).
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

	CLogFile(const TCHAR * szLogName = NULL, bool bDeleteExisting = false);
	virtual ~CLogFile();

	void SetFile(const String & strFile, bool bDelExisting = false);
	void EnableLogging(bool bEnable);
	unsigned GetDefaultLevel() const;
	void SetDefaultLevel(unsigned logLevel);
	unsigned GetMaskLevel() const;
	void SetMaskLevel(unsigned maskLevel);

	unsigned Write(const TCHAR * pszFormat, ...);
	unsigned Write(unsigned level, const TCHAR * pszFormat, ...);

	unsigned DeleteFileFailed(const TCHAR * path);

	void SetMaxLogSize(size_t dwMax) { m_nMaxSize = dwMax; }
	String GetPath() const { return m_strLogPath; }


protected:
	void Prune(FILE *f);
	const TCHAR * GetPrefix(unsigned level) const;
	void WriteV(unsigned level, const TCHAR * pszFormat, va_list);
	void WriteRaw(const TCHAR * msg);

private:
	Poco::NamedMutex m_hLogMutex; /**< Mutex protecting log writing */
	size_t    m_nMaxSize; /**< Max size of the log file */
	bool      m_bEnabled; /**< Is logging enabled? */
	String    m_strLogPath; /**< Full path to log file */
	unsigned      m_nDefaultLevel; /**< Default level for log messages */
	unsigned      m_nMaskLevel; /**< Level to mask messages written to log */
};


#endif // _LOGFILE_H_
