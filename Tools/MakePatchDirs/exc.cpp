/**
 *  @file    exc.cpp
 *  @author  Perry Rapp, Creator, 2001
 *  @date    Created: 2001/10/10
 *  @date    Edited:  2002/02/16 PR
 *  @brief   Implements exception/reporting functions
 *
 * Copyright: 2001, Perry Rapp
 */

#include "stdafx.h"

#ifndef exc_h
#include "exc.h"
#endif

#ifndef __AFXDISP_H__
#include <afxdisp.h>
#endif

#ifndef _INC_TIME
#include <time.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace perry {

exc::~exc()
{
	CStdioFile & file = m_logfile;
	if (file.m_hFile!=CFile::hFileNull)
		file.Close();
}

void
exc::setLog(LPCTSTR szLogFilepath)
{
	m_sLogFilepath = szLogFilepath;
}

static void
DoThrow(CException * pExc)
{
	// this function exists as a convenient location for breakpointing
	throw pExc;
}

excException *
exc::Extract(CException * pExc)
{
	// get pointer to our type, if it is ours (else return NULL)
	excException * myExc = 0;
	if (pExc->IsKindOf(RUNTIME_CLASS(excException)))
		myExc = (excException *)pExc;
	return myExc;
}

excException *
exc::Wrap(CException * pExc)
{
	// return pointer to ours (wrap it if not ours)
	excException * myExc = Extract(pExc);
	if (!myExc)
		myExc = excException::Chained(pExc);
	return myExc;
}

void
exc::logError(LPCTSTR szText)
{
	CString str = szText;
	dolog(str, true);
}

void
exc::log(LPCTSTR szText)
{
	CString str = szText;
	dolog(str, false);
}

bool
exc::dolog(CString & str, bool error)
{
	if (str.IsEmpty()) // drop all blank messages
		return false;
	COleDateTime now = COleDateTime::GetCurrentTime();
	CString sNow = now.Format();
	for (POSITION pos = m_callbacks.GetHeadPosition(); pos; )
	{
		callback cb = m_callbacks.GetNext(pos);
		(*cb.fnc)(cb.param, sNow, str, error);
		if (str.IsEmpty()) // drop any messages cleared by a callback
			return false;
	}
	if (error)
		m_nErrors++;
	// we skip logging if no log file or not text
	if (m_sLogFilepath.IsEmpty())
		return true;
	CSingleLock lock(&m_sem);
	// we use semaphore if client has enabled multithreaded flag
	if (m_bMultithreaded || lock.Lock())
	{
		CStdioFile & file = m_logfile;
		if (file.m_hFile==CFile::hFileNull)
		{
			int mode = CFile::shareDenyWrite+CFile::modeCreate+CFile::modeWrite;
			if (m_nMessageCount || !m_bWipePrevious)
				mode+=CFile::modeNoTruncate;
			if (!file.Open(m_sLogFilepath, mode))
				return true;
		}
		file.SeekToEnd();
		CString sLeader = _T(" ");
		if (error)
			sLeader = _T("*");

		if (!m_nMessageCount)
		{
			CString sIntro = GetIntro();
			file.WriteString((CString)_T(" ") + sNow + _T(": ") + sIntro + _T("\n"));
		}
		++m_nMessageCount;
		file.WriteString(sLeader + sNow+_T(": "));
		file.WriteString(str);
		file.WriteString(_T("\n"));
		if (isTimeToFlush())
		{
			m_nLastFlushTime=time(0);
			file.Close();
		}
	}
	return true;
}

CString
exc::GetIntro()
{
	CString sMessage = _T("Initial Log Message, Module: ");
	TCHAR filename[_MAX_PATH];
	GetModuleFileName(NULL, filename, sizeof(filename)/sizeof(filename[0]));
	sMessage += filename;
	CString sDebug;
#ifdef _DEBUG
	sDebug = " (Debug)";
#endif
	return sMessage;
}

bool
exc::dolog(CException * pExc, LPCTSTR szText, bool error)
{
	// is it one of ours ?
	excException * myExc = Extract(pExc);
	if (myExc && myExc->IsLogged())
	{
		// exception has already been logged, so only log additional text
		// (dolog will ignore it if text is blank)
		CString str = szText;
		dolog(str, false);
		return true;
	}
	// forward to string logging function
	CString str, sex(getString(pExc));
	if (szText && szText[0])
		str = szText;
	if (!str.IsEmpty())
		str += _T(": ");
	str += sex;
	CString sExceptionType = getTypeName(pExc);
	if (!sExceptionType.IsEmpty())
		str += _T(" (") + sExceptionType + _T(")");
	// could add call here to report exception type & details
	bool process = dolog(str, error);
	// mark it as logged if it is one of ours
	if (myExc)
		myExc->SetLogged();
	return process;
}

void
exc::logError(CException * pExc, LPCTSTR szText)
{
	dolog(pExc, szText, true);
}

void
exc::log(CException * pExc, LPCTSTR szText)
{
	dolog(pExc, szText, false);
}

void
exc::logAndThrowError(CException * pExc, LPCTSTR szText)
{
	// wrap it with ours, if it isn't already one of ours
	excException * myExc = Wrap(pExc);
	// log it, and that will update it, as it is one of ours now
	logError(myExc, szText);
	DoThrow(myExc);
}

void
exc::logAndThrowError(LPCTSTR szText)
{
	// create an exception for it
	excException * myExc = excException::New(szText);
	// log it, and that will update it, as it is one of ours now
	logError(myExc, szText);
	DoThrow(myExc);
}

bool
exc::isTimeToFlush()
{
	if (m_bFlushEveryMessage)
		return true;
	if (m_nFlushMessageInterval 
		&& (m_nMessageCount%m_nFlushMessageInterval==0))
	{
		return true;
	}
	if (m_nFlushTimeInterval
		&& (time(0)-m_nLastFlushTime >= m_nFlushTimeInterval))
	{
		return true;
	}
	return false;
}

CString
exc::getString(CException * pExc)
{
	TCHAR buffer[1024]=_T("");
	if (!pExc->GetErrorMessage(buffer, sizeof(buffer)/sizeof(buffer[0])))
		_tcscpy(buffer, _T("Exception with unobtainable content"));
	return buffer;
}

CString
exc::getTypeName(const CException * pExc)
{
	CString sNull;
	if (pExc->IsKindOf(RUNTIME_CLASS(excException)))
	{
		excException * myExc = (excException *)pExc;
		if (myExc->getChained())
			return getTypeName(myExc->getChained());
		return sNull;
	}
	if (pExc->IsKindOf(RUNTIME_CLASS(CFileException)))
		return _T("FileException");
	if (pExc->IsKindOf(RUNTIME_CLASS(CMemoryException)))
		return _T("MemoryException");
	if (pExc->IsKindOf(RUNTIME_CLASS(CResourceException)))
		return _T("ResourceException");
#ifdef __AFXDAO_H
	if (pExc->IsKindOf(RUNTIME_CLASS(CDaoException)))
		return _T("DaoException");
#endif
	if (pExc->IsKindOf(RUNTIME_CLASS(CNotSupportedException)))
		return _T("NotSupportedException");
#ifndef __AFXOLE_H__
	if (pExc->IsKindOf(RUNTIME_CLASS(COleException)))
		return _T("OleException");
#endif
#ifndef __AFXDISP_H__
	if (pExc->IsKindOf(RUNTIME_CLASS(COleDispatchException)))
		return _T("OleDispatchException");
#endif
	return sNull;
}

void
exc::reportError(LPCTSTR szText)
{
	doreport(szText, true);
}

bool
exc::doreport(LPCTSTR szText, bool error)
{
	CString str = szText;
	if (!dolog(str, error))
		return false;
	domsgbox(str);
	return true;
}

void
exc::domsgbox(CString str)
{
	if (!m_sErrorDisplayOverride.IsEmpty())
		str = m_sErrorDisplayOverride;
	if (m_timeMsgBox.GetStatus() != COleDateTime::valid)
		m_timeMsgBox = COleDateTime::GetCurrentTime();
	if (m_msgboxfnc)
		(*m_msgboxfnc)(m_msgparam, str);
	else
		AfxMessageBox(str);
}

void
exc::reportError(CException * pExc, LPCTSTR szText)
{
	doreport(pExc, szText, true);
}

bool
exc::doreport(CException * pExc, LPCTSTR szText, bool error)
{
	excException * myExc = Extract(pExc);
	if (myExc && myExc->IsReported())
	{
		// if already reported, just log it (dolog will skip it if already logged as well)
		return dolog(myExc, szText, true);
	}
	// assemble msg string
	CString str, sex(getString(pExc));
	if (szText && szText[0])
		str=szText;
	if (!str.IsEmpty())
		str += _T(": ");
	str += sex;
	// log it first
	bool result = dolog(pExc, str, error);
	// if not cancelled by callback, display it
	if (result)
		domsgbox(str);
	// mark it as logged & displayed, if one of ours
	if (myExc)
	{
		myExc->SetLogged();
		myExc->SetReported();
	}
	return result;
}

void
exc::reportAndThrowError(CException * pExc, LPCTSTR szText)
{
	// wrap it with ours, if it isn't already one of ours
	excException * myExc = Wrap(pExc);
	// report it or just log it
	if (m_bDelayReport)
	{
		if (!dolog(myExc, szText, true))
			return;
	}
	else
	{
		if (!doreport(myExc, szText, true))
			return;
	}
	DoThrow(myExc);
}

void
exc::reportAndThrowError(LPCTSTR szText)
{
	// create an exception for it
	excException * myExc = excException::New(szText);
	reportAndThrowError(myExc, NULL);
}

void
exc::throwErrorString(LPCTSTR szText)
{
	if (!szText)
		szText = _T("");
	excException * pExc = excException::New(szText);
	if (!dolog(pExc, NULL, true))
	{
		pExc->Delete();
		return;
	}
	DoThrow(pExc);
}

void
exc::throwError(CException * pExc)
{
	throwErrorString(getString(pExc));
}

static CString
GetSystemError(int n)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		 FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		 FORMAT_MESSAGE_FROM_SYSTEM | 
		 FORMAT_MESSAGE_IGNORE_INSERTS,
		 NULL,
		 n,
		 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		 (LPTSTR) &lpMsgBuf,
		 0,
		 NULL 
	);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	CString str = (LPCTSTR)lpMsgBuf;
	// Free the buffer.
	LocalFree( lpMsgBuf );
	return str;
}

void
exc::logLastSystemError()
{
	int err = GetLastError();
	if (err)
	{
		CString sTemp;
		sTemp.Format(_T("Last System Error (%d): "), err);
		sTemp += GetSystemError(err);
		dolog(sTemp, false);
	}
}

void
exc::addCallback(CALLBACKPTR ptr, void * param)
{
	m_callbacks.AddTail(callback(ptr, param));
}

void
exc::removeCallback(CALLBACKPTR ptr, void * param)
{
	for (POSITION pos = m_callbacks.GetHeadPosition(); pos; )
	{
		POSITION prev = pos;
		callback cb = m_callbacks.GetNext(pos);
		if (cb.fnc == ptr && cb.param == param)
		{
			m_callbacks.RemoveAt(prev);
			return;
		}
	}
}

IMPLEMENT_DYNAMIC(excException, CException)

struct StartupInfo : STARTUPINFO
{
	StartupInfo()
	{
		memset(this, 0, sizeof(*this));
		cb = sizeof(this);
	}
};
bool
RunNotepad(LPCTSTR szFilepath)
{
	CString sCommand;
	sCommand.Format(_T("Notepad %s"), szFilepath);
	StartupInfo sinfo;
	sinfo.lpTitle=_T("test");
	PROCESS_INFORMATION pinfo;
	LPTSTR cmdline = sCommand.GetBuffer(sCommand.GetLength()+2);
	int rtn = CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &sinfo, &pinfo);
	sCommand.ReleaseBuffer();
	return rtn!=0;
}

bool
IsPathSep(TCHAR ch)
{
	return ch=='\\' || ch=='/';
}


CString
PathConcat(CString sPath, CString sFile)
{
	if (!sPath.IsEmpty() && IsPathSep(sPath[sPath.GetLength()-1]))
	{
		if (!sFile.IsEmpty() && IsPathSep(sFile[0]))
			return sPath + ((LPCTSTR)sFile+1);
		else
			return sPath + sFile;
	}
	else
	{
		if (!sFile.IsEmpty() && IsPathSep(sFile[0]))
			return sPath + sFile;
		else
			return sPath + '\\' + sFile;
	}
}

CString
GetModuleFilepath(HINSTANCE hinst)
{
	TCHAR filepath[MAX_PATH];
	if (!hinst)
		hinst=::GetModuleHandle(NULL);
	GetModuleFileName(hinst, filepath, MAX_PATH);
	return filepath;
}

CString
GetPathFromFilepath(CString sFile)
{
	if (sFile.IsEmpty())
		return _T("");
	int count=0;
	for (LPCTSTR ptr = (LPCTSTR)sFile+sFile.GetLength()-1; *ptr && *ptr!=':' && !IsPathSep(*ptr); --ptr)
	{
		++count;
	}
	return sFile.Left(sFile.GetLength()-count);
}


} // namespace
