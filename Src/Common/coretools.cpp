/*******************************************************************************
$RCSfile$
$Author$
$Date$

*******************************************************************************/

#include "stdafx.h"
//#include "dpp32.h"
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
//#include "dpp.h"
#include "coretools.h"
#include <winsock.h>
#include <assert.h>


BOOL GetFileTimes(LPCTSTR szFilename, 
				  LPSYSTEMTIME pMod, 
				  LPSYSTEMTIME pCreate /*=NULL*/, 
				  LPSYSTEMTIME pAccess /*=NULL*/)
{
	WIN32_FIND_DATA ffi;
	ASSERT(szFilename != NULL);
	ASSERT(pMod != NULL);
	
	HANDLE hff = FindFirstFile(szFilename, &ffi);
	if (hff != INVALID_HANDLE_VALUE)
	{
		FILETIME ft;
		FileTimeToLocalFileTime(&ffi.ftLastWriteTime, &ft);
		FileTimeToSystemTime(&ft, pMod);
		if (pCreate)
		{
			FileTimeToLocalFileTime(&ffi.ftCreationTime, &ft);
			FileTimeToSystemTime(&ft, pCreate);
		}
		if (pAccess)
		{
			FileTimeToLocalFileTime(&ffi.ftLastAccessTime, &ft);
			FileTimeToSystemTime(&ft, pAccess);
		}
		FindClose(hff);
		return TRUE;
	}
	return FALSE;
}

DWORD GetFileSizeEx(LPCTSTR szFilename)
{
	WIN32_FIND_DATA ffi;
	ASSERT(szFilename != NULL);
	
	HANDLE hff = FindFirstFile(szFilename, &ffi);
	if (hff != INVALID_HANDLE_VALUE)
	{
		FindClose(hff);
		return ffi.nFileSizeLow;
	}
	return 0L;
}

#if UNUSED_CODE
// 2002-12-08, These are not used, and the UNICODE versions are wrong (wstrcpy ??)
void WriteWcsFromMbs(CFile* cf, LPCTSTR szSrc, DWORD dwCount)
{	
	static wchar_t wstr[1024];
	ASSERT(dwCount <= 1024);
	ASSERT(cf != NULL);

#ifdef UNICODE
	wstrcpy(wstr, szSrc);
#else
	mbstowcs(wstr, szSrc, min(dwCount, _tcslen(szSrc)+1));
#endif
	cf->Write(wstr,sizeof(wchar_t)*dwCount);
}

void WriteWcsFromMbs(CArchive* ar, LPCTSTR szSrc, DWORD dwCount)
{	
	static wchar_t wstr[1024];
	ASSERT(dwCount <= 1024);
	ASSERT(ar != NULL);

#ifdef UNICODE
	wstrcpy(wstr, szSrc);
#else
	mbstowcs(wstr, szSrc, min(dwCount, _tcslen(szSrc)+1));
#endif
	ar->Write(wstr,sizeof(wchar_t)*dwCount);
}

void WriteWcsFromMbs(HANDLE hf, LPCTSTR szSrc, DWORD dwCount)
{	
	static wchar_t wstr[1024];
	ASSERT(dwCount <= 1024);
	DWORD dwWrite;

#ifdef UNICODE
	wstrcpy(wstr, szSrc);
#else
	mbstowcs(wstr, szSrc, min(dwCount, _tcslen(szSrc)+1));
#endif
	WriteFile(hf, wstr,sizeof(wchar_t)*dwCount, &dwWrite, NULL);
}

void ReadWcsToMbs(CFile* cf, LPTSTR szDest, DWORD dwCount)
{
	static wchar_t wstr[1024];
	ASSERT(dwCount <= 1024);
	ASSERT(cf != NULL);

	cf->Read(wstr,sizeof(wchar_t)*dwCount);
#ifdef UNICODE
	wstrcpy(szDest, wstr);
#else
	wcstombs(szDest, wstr, wcslen(wstr)+1);
#endif
}

void ReadWcsToMbs(CArchive* ar, LPTSTR szDest, DWORD dwCount)
{
	static wchar_t wstr[1024];
	ASSERT(dwCount <= 1024);
	ASSERT(ar != NULL);

	ar->Read(wstr,sizeof(wchar_t)*dwCount);
#ifdef UNICODE
	wstrcpy(szDest, wstr);
#else
	wcstombs(szDest, wstr, wcslen(wstr)+1);
#endif
}

void ReadWcsToMbs(HANDLE hf, LPTSTR szDest, DWORD dwCount)
{
	static wchar_t wstr[1024];
	ASSERT(dwCount <= 1024);
	DWORD dwRead;

	ReadFile(hf, wstr,sizeof(wchar_t)*dwCount, &dwRead, NULL);
#ifdef UNICODE
	wstrcpy(szDest, wstr);
#else
	wcstombs(szDest, wstr, wcslen(wstr)+1);
#endif
}
#endif // UNUSED_CODE


int tcssubptr(LPCTSTR start, LPCTSTR end)
{
	LPCTSTR p = start;
	register int cnt=0;
	while (p <= end)
	{
		cnt++;
		p = _tcsinc(p);
	}
	return cnt;
}

BOOL DoModalProcess(CWnd *pWndParent, LPCTSTR szExeFile, 
					LPCTSTR szCmdLine, LPCTSTR szWindowCaption)
{
	BOOL result = FALSE;
	TCHAR temp[MAX_PATH],path[MAX_PATH];
	if (GetModuleFileName(NULL, temp, MAX_PATH))
	{
		split_filename(temp,path,NULL,NULL);
		_stprintf(temp,_T("%s\\%s"),path, szExeFile);
		if ((int)ShellExecute(pWndParent->GetSafeHwnd(), _T("open"), temp, 
						 szCmdLine, path, SW_SHOWNORMAL) > 32)
		{
			result=TRUE;
			if (szWindowCaption != NULL)
			{
				pWndParent->EnableWindow(FALSE);
				
				CTime start = CTime::GetCurrentTime();
				CTimeSpan span;
				BOOL found=FALSE;

				// wait for window to appear
				while (!found)
				{
					span = CTime::GetCurrentTime() - start;
					if (span.GetTotalSeconds() > 10)
						return FALSE;
					if (CWnd::FindWindow(NULL,szWindowCaption))
						break;
					Sleep(100);
				}

				// wait for window to close
				while (CWnd::FindWindow(NULL,szWindowCaption))
					Sleep(200);
				pWndParent->SetForegroundWindow();
				pWndParent->EnableWindow(TRUE);
			}
		}
	}
	return result;
}


void GetLocalDrives(LPTSTR letters)
{
	TCHAR temp[100];

	_tccpy(letters,_T("\0"));
	if (GetLogicalDriveStrings(100,temp))
	{
		TCHAR *p,*pout=letters;
		for (p=temp; *p != _T('\0'); )
		{
			if (GetDriveType(p)!=DRIVE_REMOTE)
			{
				_tccpy(pout,p);
				pout = _tcsinc(pout);
			}
			p = _tcsninc(p,_tcslen(p)+1);
		}
		_tccpy(pout,_T("\0"));
	}
}

CString GetCDPath()
{
	TCHAR drives[100];
	CString s;
	GetLocalDrives(drives);
	for (TCHAR *d = drives;  *d != _T('\0'); d = _tcsinc(d))
	{
		s.Format(_T("%c:\\"), *d);
		if (GetDriveType(s) == DRIVE_CDROM)
			return s;
	}
	return CString("");
}



BOOL GetIP(LPTSTR straddr) 
{
	char     szHostname[100];
	HOSTENT *pHent;
	SOCKADDR_IN sinRemote;         
	
	gethostname( szHostname, sizeof( szHostname ));
	pHent = gethostbyname( szHostname );
	
	if (pHent != NULL)
	{
		sinRemote.sin_addr.s_addr = *(u_long *)pHent->h_addr;
		_stprintf(straddr,_T("%d.%d.%d.%d"),
			sinRemote.sin_addr.S_un.S_un_b.s_b1,
			sinRemote.sin_addr.S_un.S_un_b.s_b2,
			sinRemote.sin_addr.S_un.S_un_b.s_b3,
			sinRemote.sin_addr.S_un.S_un_b.s_b4);
		return TRUE;
	}
	
	*straddr=_T('\0');
	return FALSE;
}


DWORD FPRINTF(HANDLE hf, LPCTSTR fmt, ... )
{
	static TCHAR fprintf_buffer[8192];
    va_list vl;
    va_start( vl, fmt );

	_vstprintf(fprintf_buffer, fmt, vl);
	DWORD dwWritten;
	WriteFile(hf, fprintf_buffer, _tcslen(fprintf_buffer)*sizeof(TCHAR), &dwWritten, NULL);

    va_end( vl );
	return dwWritten;
}

DWORD FPUTS(LPCTSTR s, HANDLE hf)
{
	static DWORD fputs_written;
	WriteFile(hf, s, _tcslen(s)*sizeof(TCHAR), &fputs_written, NULL);
	return fputs_written;
}


HANDLE FOPEN(LPCTSTR path, DWORD mode /*= GENERIC_READ*/, DWORD access /*= OPEN_EXISTING*/)
{
	HANDLE hf = CreateFile(path,	
					mode, 	
					0,	
					NULL,	
					access,	
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	if (hf == INVALID_HANDLE_VALUE) // give it another shot
		return CreateFile(path,	
					mode, 	
					0,	
					NULL,	
					access,	
					FILE_ATTRIBUTE_NORMAL,
					NULL);
	return hf;
}

void
replace_char(LPTSTR s, int target, int repl)
{
	TCHAR *p;
	for (p=s; *p != _T('\0'); p = _tcsinc(p))
		if (*p == target)
			*p = (TCHAR)repl;
}


CString ConvertPath2PS(LPCTSTR szPath)
{
	CString result(_T(""));
	TCHAR path[MAX_PATH];
	_tcscpy(path,szPath);

	replace_char(path, _T('\\'), _T('//'));
	if (_tcslen(path)>2
		&& path[1] == _T(':')
		&& path[2] == _T('/'))
	{
#ifdef _UNICODE
		result.Format(_T("%%%c%%%s"), towupper(path[0]), _tcsninc(path, 2));
#else
		result.Format(_T("%%%c%%%s"), toupper(path[0]), _tcsninc(path, 2));
#endif
	}
	else
		result = path;

	return result;
}


BOOL 
FileExtMatches(LPCTSTR filename, LPCTSTR ext)
{
  LPCTSTR p; 

  /* if ext is empty, it is considered a no-match */
  if (*ext == _T('\0'))
    return FALSE;

  p = filename;
  p = _tcsninc(p, _tcslen(filename) - _tcslen(ext));
  if (p >= filename)
    return (!_tcsicmp(p,ext));

  return FALSE;
}


void
split_filename(LPCTSTR s, LPTSTR path, LPTSTR name, LPTSTR ext)
{
	TCHAR *p;
	if ((p=_tcsrchr(s,_T('\\'))) != NULL)
	{
		if (path != NULL)
		{
			_tcsncpy(path, s, tcssubptr(s, p));
			path[p-s]=NULL;
		}
		if (name != NULL) 
		{
			_tcscpy(name,_tcsinc(p));
		}
	}
	else
	{
		if (path != NULL)
			*path = _T('\0');
		if (name != NULL)
			_tcscpy(name,s);
	}       
	
	if (ext != NULL)
	{
		if ((p=_tcsrchr(s,_T('.'))) != NULL)
		{          
			_tcscpy(ext,_tcsinc(p));
			if (name!=NULL) 
				_tccpy(_tcsninc(name, _tcslen(name)-_tcslen(ext)-1), _T("\0"));
		}                   
		else
			ext[0] = NULL; 
	}
}  


void
AddExtension(LPTSTR name, LPCTSTR ext)
{
	TCHAR *p;

	assert(ext[0] != _T('.'));
	if (!((p=_tcsrchr(name,_T('.'))) != NULL
		  && !_tcscmp(p+1,ext)))
	{
		_tcscat(name,_T("."));
		_tcscat(name,ext);
	}
}
   
   

BOOL
is_all_whitespace(LPCTSTR s)
{
  LPCTSTR p;
  for (p=s; *p != _T('\0'); p = _tcsinc(p))
  {
    if (!_istspace(*p))
      return FALSE;
  }
  return TRUE;
}


BOOL
GetFreeSpaceString(LPCTSTR drivespec, ULONG mode, LPTSTR s)
{
  DWORD sectorsPerCluster,
	  bytesPerSector,
	  numberOfFreeClusters,
	  totalNumberOfClusters, total;
  
  if (!GetDiskFreeSpace(drivespec, 
						&sectorsPerCluster,
						&bytesPerSector,
						&numberOfFreeClusters,
						&totalNumberOfClusters))
    return FALSE;
  
  total = numberOfFreeClusters*bytesPerSector*sectorsPerCluster;
  if (mode==BYTES)
    _stprintf(s, _T("%lu bytes available"), total);
  else if (mode==KBYTES)
    _stprintf(s, _T("%1.1fK available"), (float)(total/(float)mode));
  else
    _stprintf(s, _T("%1.1f MB available"), (float)(total/(float)mode));
  return TRUE;
}




BOOL 
is_regfile(LPCTSTR buf,LPCTSTR path)
{
  static struct _stat s;
  CString strPath;

  if (!_tcschr(buf,_T('\\')))
    strPath.Format(_T("%s\\%s"),path,buf);
  else
    strPath = buf;
  if (_tstat(strPath, &s) == -1)
    return FALSE;
  else if (s.st_mode & S_IFDIR)
    return FALSE;

  return TRUE;
}


int 
is_regfile2(LPCTSTR path)
{
  static struct _stat s;

  if (_tstat(path, &s) == -1)
    return FALSE;
  else if (s.st_mode & S_IFDIR)
    return FALSE;

  return TRUE;
}


BOOL 
is_dirfile(LPCTSTR buf,LPCTSTR path)
{
  static struct _stat s;
  CString strPath;
 
  strPath.Format(_T("%s\\%s"),path,buf);
  if (_tstat(strPath, &s) == -1)
    return FALSE;
  else if (s.st_mode & S_IFDIR)
    return TRUE;

  return FALSE;
}



BOOL 
is_dirfile2(LPCTSTR path)
{
  static struct _stat s;
 
  if (_tstat(path, &s) == -1)
    return FALSE;
  else if (s.st_mode & S_IFDIR)
    return TRUE;

  return FALSE;
}


int 
fcmp(float a,float b)
/* return -1 if a<b, 0 if a=b, or 1 if a>b */
{
  long la,lb;

  la = (long)(a * 10000.0);
  lb = (long)(b * 10000.0);

  if (la < lb)
    return -1;
  
  return (la > lb);
}


void 
aswap(LPTSTR a,LPTSTR b)
{
 TCHAR t[200];
 _tcscpy(t,a);
 _tcscpy(a,b);
 _tcscpy(b,t);
}


BOOL FindAnyFile(LPTSTR filespec, LPTSTR name)
{
#ifndef _UNICODE
   struct _finddata_t c_file;
#else
   struct _wfinddata_t c_file;
#endif
   long hFile;

   if( (hFile = _tfindfirst( filespec, &c_file )) == -1L )
       return FALSE;

    _tcscpy(name, c_file.name);
	_findclose( hFile );
	return TRUE;
}


long 
SwapEndian(long val)
{
#ifndef _WINDOWS
  long t = 0x00000000;
  t = ((val >> 24) & 0x000000FF);
  t |= ((val >> 8) & 0x0000FF00);
  t |= ((val << 8) & 0x00FF0000);
  t |= ((val << 24) & 0xFF000000);
  return (long)t; 
#else
  return val;
#endif
}



short int 
SwapEndian(short int val)
{
#ifndef _WINDOWS
  short int t = 0x0000;
  t = (val & 0xFF00) >> 8;
  t |= (val & 0x00FF) << 8;
  return (short int)t;
#else
  return val;
#endif
}


BOOL MkDirEx(LPCTSTR filename)
{
	TCHAR tempPath[MAX_PATH];
	LPTSTR p; 

	_tcscpy(tempPath, filename);
	if (*_tcsinc(filename)==_T(':'))
		p=_tcschr(_tcsninc(tempPath,3),_T('\\'));
	else if (*filename==_T('\\'))
		p=_tcschr(_tcsinc(tempPath),_T('\\'));
	else
		p=tempPath;
	if (p!=NULL)
		for (; *p != _T('\0'); p = _tcsinc(p))
		{
			if (*p == _T('\\'))
			{
				_tccpy(p, _T("\0"));
				if (!CreateDirectory(tempPath, NULL)
					&& !CreateDirectory(tempPath, NULL))
					TRACE(_T("Failed to create folder %s (%ld)\n"),tempPath, GetLastError());
				_tccpy(p, _T("\\"));
			}
			
		}

		if (!CreateDirectory(filename, NULL)
			&& !CreateDirectory(filename, NULL))
			TRACE(_T("Failed to create folder %s (%ld)\n"),filename, GetLastError());

	CFileStatus status;
	return (CFile::GetStatus(filename, status));
}


float
RoundMeasure(float measure, float units)
{
	float res1,res2,divisor;
	if (units == 25.4f)
		divisor = 0.03937f;
	else if (units == 6.f || units == 72.f)
		divisor = 0.0139f;
	else // 128ths
		divisor = 0.0078125f;

	res1 = (float)fmod(measure,divisor);
	res2 = divisor - res1;
	if (res1 > res2)
		return (measure-res1);

	return (measure-res1+divisor);
}


BOOL HaveAdminAccess()
{
	// make sure this is NT first
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	// if this fails, we want to default to being enabled
	if (!(GetVersionEx(&ver) && ver.dwPlatformId==VER_PLATFORM_WIN32_NT))
	{
		return TRUE;
	}

	// now check with the security manager
	HANDLE hHandleToken;
	
	if(!::OpenProcessToken(::GetCurrentProcess(), TOKEN_READ, &hHandleToken))
		return(FALSE);
	
	UCHAR TokenInformation[1024];
	DWORD dwTokenInformationSize;
	
	BOOL bSuccess = ::GetTokenInformation(hHandleToken, TokenGroups, 
		TokenInformation, sizeof(TokenInformation), &dwTokenInformationSize);
	
	::CloseHandle(hHandleToken);
	
	if(!bSuccess)
		return FALSE;
	
	SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
	PSID psidAdministrators;
	
	if(!::AllocateAndInitializeSid(&siaNtAuthority, 2, 
		SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, 
		&psidAdministrators))
		return FALSE;
	
	// assume that we don't find the admin SID.
	bSuccess = FALSE;
	
	PTOKEN_GROUPS ptgGroups = (PTOKEN_GROUPS)TokenInformation;
	
	for(UINT x=0; x < ptgGroups->GroupCount; x++)
	{
		if(::EqualSid(psidAdministrators, ptgGroups->Groups[x].Sid))
		{
			bSuccess = TRUE;
			break;
		}
	}
	
	::FreeSid(psidAdministrators);
	
	return bSuccess;
}



CString LegalizeFileName(LPCTSTR szFileName)
{
	TCHAR tempname[MAX_PATH];
	LPTSTR p;

	_tcscpy(tempname, szFileName);
	while ((p=_tcspbrk(tempname, _T("\\'/:?\"<>|*\t\r\n"))) != NULL)
	{
		*p = _T('_');
	}
	
	return CString(tempname);
}

void DDX_Float( CDataExchange* pDX, int nIDC, float& value )
{
	pDX->PrepareEditCtrl(nIDC);
	CEdit *pEdit = (CEdit *)pDX->m_pDlgWnd->GetDlgItem(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		//DDX_Text(pDX, nIDC, value);
		static TCHAR dec[4]=_T("");
		static TCHAR thous[4]=_T("");
		static TCHAR negsign[4]=_T("");
		static int negstyle=1;
		
		if (*dec == _T('\0'))
		{
			LCID lcid = GetThreadLocale();
			GetLocaleInfo(lcid, LOCALE_INEGNUMBER, dec, 4);
			negstyle = _ttoi(dec);
			GetLocaleInfo(lcid, LOCALE_SDECIMAL, dec, 4);
			GetLocaleInfo(lcid, LOCALE_STHOUSAND, thous, 4);
			GetLocaleInfo(lcid, LOCALE_SNEGATIVESIGN, negsign, 4);
		}
		
		value = 0.f;
		
		// are we negative?
		TCHAR *p,text[80];
		pEdit->GetWindowText(text, 80);
		BOOL bIsNeg = (_tcschr(text, negsign[0])!=NULL);
		
		// separate at the decimal point
		TCHAR istr[80]=_T("");
		TCHAR fstr[80]=_T("");
		p = _tcstok(text, dec);
		if (p != NULL)
		{
			_tcscpy(istr, p);
			p = _tcstok(NULL, dec);
			if (p != NULL)
				_tcscpy(fstr, p);
		}
		
		// get the int part
		if (*istr != _T('\0'))
		{
			CString strResult(_T(""));
			CString sep(negsign);
			sep += thous;
			p = _tcstok(istr, sep);
			while (p != NULL)
			{
				strResult += p;
				p = _tcstok(NULL, sep);
			}
			value = (float)_ttoi(strResult);
		}
		
		// get the fract part
		if (*fstr != _T('\0'))
		{
			p = _tcstok(fstr, negsign);
			if (p != NULL)
			{
				double shift = (pow(10, _tcslen(p)));
				if (shift)
					value += (float)(_tcstod(p,NULL)/shift);
				else
					pDX->Fail();
			}
		}
		
		// convert to neg
		if (bIsNeg)
			value *= -1.f;
	}
	else
	{
		if (pEdit != NULL)
		{
			CString s = GetLocalizedNumberString(value, 4);
			pEdit->SetWindowText(s);
		}
	}
}

void DDX_Double( CDataExchange* pDX, int nIDC, double& value )
{
	pDX->PrepareEditCtrl(nIDC);
	CEdit *pEdit = (CEdit *)pDX->m_pDlgWnd->GetDlgItem(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		//DDX_Text(pDX, nIDC, value);
		static TCHAR dec[4]=_T("");
		static TCHAR thous[4]=_T("");
		static TCHAR negsign[4]=_T("");
		static int negstyle=1;
		
		if (*dec == _T('\0'))
		{
			LCID lcid = GetThreadLocale();
			GetLocaleInfo(lcid, LOCALE_INEGNUMBER, dec, 4);
			negstyle = _ttoi(dec);
			GetLocaleInfo(lcid, LOCALE_SDECIMAL, dec, 4);
			GetLocaleInfo(lcid, LOCALE_STHOUSAND, thous, 4);
			GetLocaleInfo(lcid, LOCALE_SNEGATIVESIGN, negsign, 4);
		}
		
		value = 0.f;
		
		// are we negative?
		TCHAR *p,text[80];
		pEdit->GetWindowText(text, 80);
		BOOL bIsNeg = (_tcschr(text, negsign[0])!=NULL);
		
		// separate at the decimal point
		TCHAR istr[80]=_T("");
		TCHAR fstr[80]=_T("");
		p = _tcstok(text, dec);
		if (p != NULL)
		{
			_tcscpy(istr, p);
			p = _tcstok(NULL, dec);
			if (p != NULL)
				_tcscpy(fstr, p);
		}
		
		// get the int part
		if (*istr != _T('\0'))
		{
			CString strResult(_T(""));
			CString sep(negsign);
			sep += thous;
			p = _tcstok(istr, sep);
			while (p != NULL)
			{
				strResult += p;
				p = _tcstok(NULL, sep);
			}
			value = /*(float)*/_ttoi(strResult);
		}
		
		// get the fract part
		if (*fstr != _T('\0'))
		{
			p = _tcstok(fstr, negsign);
			if (p != NULL)
			{
				double shift = (pow(10, _tcslen(p)));
				if (shift)
					value += /*(float)*/(_tcstod(p,NULL)/shift);
				else
					pDX->Fail();
			}
		}
		
		// convert to neg
		if (bIsNeg)
			value *= -1.f;
	}
	else
	{
		if (pEdit != NULL)
		{
			CString s = GetLocalizedNumberString(value, 4);
			pEdit->SetWindowText(s);
		}
	}
}


CString GetLocalizedNumberString(double dVal, int nPlaces /*=-1*/, BOOL bSeparate /*=FALSE*/, BOOL bTrailZeros /*=FALSE*/, LCID lcidNew /*=LOCALE_USER_DEFAULT*/)
// this function is duplicated in CFloatEdit
{
	static LCID lcid = 0;
	static TCHAR dec[4]=_T(".");
	static TCHAR thous[4]=_T(",");
	static TCHAR buf[80];
	static int nGrpSize=3;
	static int nDecimals=2;
	static BOOL bLeadZero=TRUE;
	static int negstyle=1;
	CString s, s2, strResult(_T(""));
	DWORD intpart=0;
	BOOL bIsNeg=(dVal < 0.0);

	ASSERT(nPlaces!=0);

	if (lcid != lcidNew)
	{
		lcid = lcidNew;
		GetLocaleInfo(lcid, LOCALE_SDECIMAL, dec, 4);
		GetLocaleInfo(lcid, LOCALE_STHOUSAND, thous, 4);
		GetLocaleInfo(lcid, LOCALE_SGROUPING, buf, 80);
		s = buf[0];
		nGrpSize = _ttoi(s);
		GetLocaleInfo(lcid, LOCALE_IDIGITS, buf, 4);
		nDecimals = _ttoi(buf);
		GetLocaleInfo(lcid, LOCALE_ILZERO, buf, 4);
		bLeadZero = !_tcsncmp(buf,_T("1"), 1);
		GetLocaleInfo(lcid, LOCALE_INEGNUMBER, buf, 4);
		negstyle = _ttoi(buf);
	}

	TCHAR szFract[80]; 
	if (1)
	{
		// split into integer & fraction
		char tszInt[80],tszFract[80]; 
		int places = (nPlaces==-1? nDecimals : nPlaces);
		int  decimal, sign;   
		char *buffer;

		buffer = _fcvt( dVal+1e-10, places, &decimal, &sign );
		if (decimal > 0)
		{
			strncpy(tszInt, buffer, decimal);
			tszInt[decimal] = NULL;
		}
		strncpy(tszFract, &buffer[decimal], strlen(buffer)-decimal);
		tszFract[strlen(buffer)-decimal] = NULL;
		intpart = (DWORD)atoi(tszInt);
#ifdef _UNICODE
		mbstowcs(szFract, tszFract, strlen(tszFract));
#else
		_tcscpy(szFract, tszFract);
#endif
	}


	// take care of leading negative sign
	if (bIsNeg)
	{
		if (negstyle==0)
			strResult = _T("(");
		else if (negstyle==1)
			strResult = _T("-");
		else if (negstyle==2)
			strResult = _T("- ");
	}
	
	// format integer part
	if (intpart >= 1)
	{
		s.Format(_T("%lu"), intpart);

		// do thousands separation
		if (bSeparate)
		{
			int len = s.GetLength();
			int leftover = (len % nGrpSize);

			// format the leading group
			if (leftover)
			{
				s2 = s.Left(leftover);
				len -= leftover;
				s = s.Right(len);
				strResult += s2;
			}
			else if (len)
			{
				s2 = s.Left(nGrpSize);
				len -= nGrpSize;
				s = s.Right(len);
				strResult += s2;
			}

			// format the remaining groups
			while (len)
			{
				s2 = s.Left(nGrpSize);
				len -= nGrpSize;
				s = s.Right(len);
				strResult += thous + s2;
			}
		}
		// want integer value without thousands separation
		else
		{
			strResult += s;
		}
	}
	else
	{
		// add leading zero
		if (bLeadZero)
			strResult += _T("0");
	}

	// add decimal separator & fractional part
	if (szFract != NULL)
	{
		// add the decimal separator
		strResult += dec;
		strResult += szFract;
		
		// get rid of trailing zeros
		if (!bTrailZeros)
		{
			LPTSTR p, start = strResult.GetBuffer(1);
			p = _tcsdec(start, start + _tcslen(start));
			if (p != NULL)
			{
				for (; p > start && *p==_T('0'); 
				p = _tcsdec(start, p));
				if (p != NULL)
				{
					if (*p==_T('.')) // we don't want numbers like 4.
						*p=_T('\0');
					else
						*(p+1)=_T('\0');
				}
			}
			strResult.ReleaseBuffer();
		}
	}

	// take care of trailing negative sign
	if (bIsNeg)
	{
		if (negstyle==0)
			strResult += _T(")");
		else if (negstyle==3)
			strResult += _T("-");
		else if (negstyle==4)
			strResult += _T(" -");
	}

	return strResult;
}


HANDLE RunIt(LPCTSTR szExeFile, LPCTSTR szArgs, BOOL bMinimized /*= TRUE*/, BOOL bNewConsole /*= FALSE*/)
{
    STARTUPINFO si;
	PROCESS_INFORMATION procInfo;

    si.cb = sizeof(STARTUPINFO); 
    si.lpReserved=NULL; 
    si.lpDesktop = _T(""); 
    si.lpTitle = NULL; 
    si.dwFlags = STARTF_USESHOWWINDOW; 
    si.wShowWindow = (WORD)(bMinimized? SW_MINIMIZE : SW_NORMAL); 
    si.cbReserved2 = 0; 
    si.lpReserved2 = NULL; 

	TCHAR args[4096];
	_stprintf(args,_T("\"%s\" %s"), szExeFile, szArgs);
    if (CreateProcess(szExeFile, args, NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS|(bNewConsole? CREATE_NEW_CONSOLE:0), 
                         NULL, _T(".\\"), &si, &procInfo))
	{
		CloseHandle(procInfo.hThread);
		return procInfo.hProcess;
	}

	return INVALID_HANDLE_VALUE;
}

BOOL HasExited(HANDLE hProcess, DWORD *pCode)
{
     DWORD code;
     if (GetExitCodeProcess(hProcess, &code)
         && code != STILL_ACTIVE)
     {
		 *pCode = code;
         return TRUE;
     }
     return FALSE;
}


BOOL IsLocalPath(LPCTSTR path) 
{

  _TCHAR finalpath[_MAX_PATH];
  _TCHAR temppath[_MAX_PATH];
  _TCHAR uncname[_MAX_PATH];
  _TCHAR computername[_MAX_PATH];

  BOOL bUNC=FALSE;
  BOOL bLocal=FALSE;

  _TCHAR* pLoc;
  
  CString szInfo;


  //We need to get the root directory into an sz
  if((pLoc=_tcsstr(path, _T("\\\\")))!=NULL)
  {
    bUNC=TRUE;

    _tcscpy(temppath, pLoc+_tcslen(_T("\\\\")));
    _tcscpy(finalpath, _T("\\\\"));

    if((pLoc=_tcsstr(temppath, _T("\\")))!=NULL)
    {
      _tcsncpy(uncname, temppath, pLoc-temppath);
      _tcscat(finalpath, uncname);

    }
    else
    {
      _tcscpy(uncname, temppath);
      _tcscat(finalpath, uncname);
    }
    
  }
  else //standard path
  {
    _tcscpy(temppath, path);
    if((pLoc=_tcsstr(temppath, _T("\\")))!=NULL)
    {
      *pLoc = _T('\0');
    }
    _tcscpy(finalpath, temppath);    
  }


  if(bUNC)
  {
    // get the machine name and compare it to the local machine name
    //if cluster, GetComputerName returns the name of the cluster
    DWORD len=_MAX_PATH-1;
    if(GetComputerName(computername, &len) && _tcsicmp(computername, uncname)==0
      )
    {
      //szInfo =  _T("This is a UNC path to the local machine or cluster");
      bLocal=TRUE;
    }
    else
    {
      //szInfo =  _T("This is a UNC path to a remote machine");
    }
  }
  else
  {
    //test the standard path
    UINT uType;
    uType = GetDriveType(finalpath);
    
    switch(uType)
    {
    case DRIVE_UNKNOWN: 
      //szInfo = _T("The drive type cannot be determined.");
      break;
    case DRIVE_NO_ROOT_DIR:
      //szInfo = _T("The root directory does not exist.");
      break;
    case DRIVE_REMOVABLE:
      //szInfo = _T("The disk can be removed from the drive.");
      bLocal=TRUE;
      break;
    case DRIVE_FIXED: 
      //szInfo = _T("The disk cannot be removed from the drive.");
      bLocal=TRUE;
      break;
    case DRIVE_REMOTE:
      //szInfo = _T("The drive is a remote (network) drive.");
      break;
    case DRIVE_CDROM:
      //szInfo = _T("The drive is a CD-ROM drive.");
      bLocal=TRUE;
      break;
    case DRIVE_RAMDISK:
      //szInfo = _T("The drive is a RAM disk.");
      bLocal=TRUE;
      break;
    //default:
      //szInfo = _T("GetDriveType returned an unknown type");
      
    }
  }  

  return bLocal;
}

CString GetModulePath(HMODULE hModule /* = NULL*/)
{
	TCHAR path[MAX_PATH], temp[MAX_PATH];
	GetModuleFileName(hModule, temp, MAX_PATH); 
	split_filename(temp, path, NULL, NULL);
	return CString(path);
}



/*******************************************************************************
       END OF FILE
*******************************************************************************/

