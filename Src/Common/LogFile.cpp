// LogFile.cpp: implementation of the CLogFile and CSQL LogFile classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile(LPCTSTR szLogName, LPCTSTR szLogPath /*= NULL*/, BOOL bDeleteExisting /*=FALSE*/)
	: m_nMaxSize(1024576)
	, m_bEnabled(FALSE)
// CString m_strLogPath
{

	TCHAR temp[MAX_PATH];



	// build the path to the logfile
	if (szLogPath != NULL
		&& *szLogPath != _T('\0'))
		m_strLogPath = szLogPath;
	else
	{
		GetTempPath(MAX_PATH, temp);
		m_strLogPath = temp;
	}
	if (m_strLogPath.Right(1) != _T('\\'))
		m_strLogPath += _T('\\');

	m_strLogPath += szLogName;

	if (bDeleteExisting)
		DeleteFile(m_strLogPath);

	// write start banner
	CTime t = CTime::GetCurrentTime();
	CString s = t.Format(_T("Begin Log: %A, %B %d, %Y    %H:%M:%S"));
    Write(_T("\n\n==========================================================================\n"
		"==========================================================================\n"
		"%s\n==========================================================================\n"
		"==========================================================================\n"), s);
	Write(m_strLogPath);
}

CLogFile::~CLogFile()
{
	Write(_T("##### End Log #####\n"));
}

void CLogFile::Write(LPCTSTR pszFormat, ...)
{
    TCHAR buf[2048]=_T("");
	va_list arglist;
	va_start(arglist, pszFormat);
	if (pszFormat != NULL)
		_vstprintf(buf, pszFormat, arglist);
	va_end(arglist);
    _tcscat(buf, _T("\n"));
	//TRACE(buf);

	FILE *f;
#ifndef _UNICODE
	if ((f=fopen(m_strLogPath, _T("a"))) != NULL)
#else
	if ((f=wfopen(m_strLogPath, L"a")) != NULL)
#endif
	{
		_fputts(buf, f);

		// prune the log if it gets too big
		if (ftell(f) >= (int)m_nMaxSize)
			Prune(f);
		else
			fclose(f);
	}
}

void CLogFile::Write(DWORD idFormatString, ...)
{
	if (!m_bEnabled)
		return;

	TCHAR buf[2048]=_T("");
	CString strFormat;

	if (strFormat.LoadString(idFormatString))
	{
		va_list arglist;
		va_start(arglist, idFormatString);
		vsprintf(buf, strFormat, arglist);
		va_end(arglist);
		_tcscat(buf, _T("\n"));

		FILE *f;
		if ((f=_tfopen(m_strLogPath, _T("a"))) != NULL)
		{
			_fputts(buf, f);
				
			// prune the log if it gets too big
			if (ftell(f) >= (int)m_nMaxSize)
				Prune(f);
			else
				fclose(f);
		}
	}
}

void CLogFile::WriteError(CString JobID, CString ProcessID, CString Event, long ecode, CString CIndex)
{
	if (!m_bEnabled)
		return;
	
	JobID.TrimRight();
	ProcessID.TrimRight();
	Event.TrimRight();
	CIndex.TrimRight();
	
	CString sWriteString;
	
	sWriteString.Format("%s %s %s %ld %s",JobID, ProcessID, Event, ecode, CIndex);
	Write(sWriteString);

}

void CLogFile::Prune(FILE *f)
{
	TCHAR buf[8196] = {0};
	DWORD amt;
	TCHAR tempfile[MAX_PATH];
	FILE *tf;
	GetTempFileName(_T("."),_T("LOG"),0,tempfile);
	DeleteFile(tempfile);
#ifndef _UNICODE
	if ((tf=fopen(tempfile,"w")) != NULL)
#else
	if ((tf=wfopen(tempfile,L"w")) != NULL)
#endif
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
