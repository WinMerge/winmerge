/*!
  \file    AppVersion.cpp
  \author  Perry Rapp, Smartronix, 2001
  \date    Created: 2001/09/15 (Perry Rapp)
  \date    Edited:  2001/12/12 PR

  \brief   Implementation of simple version functions for an MS-Win app.

  This was built by combining cfilever.*, JulianVersion.*, and VersionReport.*
  on 2001/09/15 by Perry Rapp.

  It requires a CString an a COleDateTime class.
*/
/* The MIT License
Copyright (c) 2001 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "StdAfx.h"
#include "AppVersion.h"
#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace appv {

// -- from cfilever.cpp

/*!
  Extracts the fixed-info version information from the version resource in
  the RC file for the current module. The four integers that make up the
  fixed-info version information are formatted into a string and returned
  to the caller.
*/
CString FixedModuleVersion(HMODULE hmod)
{
	if (!hmod)
		hmod = ::GetModuleHandle(NULL);
	TCHAR filepath[MAX_PATH];
	GetModuleFileName(hmod, filepath, sizeof(filepath));
	DWORD dwDummyHandle;
	DWORD len = GetFileVersionInfoSize(filepath, &dwDummyHandle);
	BYTE * buf = new BYTE[len];
	::GetFileVersionInfo(filepath, 0, len, buf);
	unsigned int verlen;
	LPVOID lpvi;
	::VerQueryValue(buf, _T("\\"), &lpvi, &verlen);
	VS_FIXEDFILEINFO fileInfo;
	fileInfo = *(VS_FIXEDFILEINFO*)lpvi;
	CString sVer;
	sVer.Format(_T("%d.%d.%d.%d")
		, HIWORD(fileInfo.dwFileVersionMS)
		, LOWORD(fileInfo.dwFileVersionMS)
		, HIWORD(fileInfo.dwFileVersionLS)
		, LOWORD(fileInfo.dwFileVersionLS));
	delete buf;
	return sVer;
}

/*!
  This routine will extract the version string from the string version
  resource in the RC file for the current module. You must add version.lib
  to your project to link to the Win32 versioning API calls. The actual call
  VerQueryValue() uses a value of 040904B0 for the language and character set.
  This value is equivalent to English language text encoded using Unicode.
 */ 
CString StringModuleVersion()
{
	TCHAR filepath[MAX_PATH];
	GetModuleFileName(::GetModuleHandle(NULL), filepath, MAX_PATH);
	DWORD dwDummyHandle;
	DWORD len = GetFileVersionInfoSize(filepath, &dwDummyHandle);
	BYTE * buf = new BYTE[len];
	::GetFileVersionInfo(filepath, 0, len, buf);
	char *version;
	unsigned int verlen;
	::VerQueryValue(buf, _T("\\StringFileInfo\\040904B0\\FileVersion"),(void **)&version, &verlen);
	CString sval = buf;
	delete buf;
	return sval;
}

// -- from JulianVersion.cpp

bool ConvertDateToWord(const COleDateTime& date, int & word)
{
	int year = date.GetYear();
	int day = date.GetDayOfYear(); // 1-based
	word = (year-1990)*1000 + day;
	return true;
}

bool ConvertDateStringToWord(const CString& sDate, int & word)
{
	COleDateTime date;
	if (!date.ParseDateTime(sDate))
		return false;
	return ConvertDateToWord(date, word);
}

bool ConvertWordToDate(int word, COleDateTime & date)
{
	int year = word/1000;
	int day = word - year*1000;
	year += 1990;
	if (year < 2000)
		return false;
	// stored using 1 based
	if (day < 1 || day > 366)
		return false;
	date.SetDate(year, 1, 1);
	date += COleDateTimeSpan(day-1, 0, 0, 0);
	return true;
}

bool ConvertVersionStringToDate(const CString& sVersion, COleDateTime & date)
{
	int n1,n2,n3,n4;
	int cvt = _stscanf(sVersion, _T("%d.%d.%d.%d"), &n1, &n2, &n3, &n4);
	if (cvt != 4)
		return false;
	return ConvertWordToDate(n3, date);
}

// -- from VersionReport.cpp

CString
GetVersionReport()
{
	CString sVersion = FixedModuleVersion();
	COleDateTime date;
	if (ConvertVersionStringToDate(sVersion, date))
		sVersion = sVersion + _T(" (") + date.Format() + _T(")");
	else
		sVersion = sVersion + _T(" (?)");
	return sVersion;
}

} // namespace
