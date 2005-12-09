/** 
 * @file  locality.h
 *
 * @brief Implementation of helper functions involving locale
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include "locality.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace locality {

/**
 * @brief Get numeric value from an LC_ entry in windows locale (NLS) database
 */
static UINT getLocaleUint(int lctype, int defval)
{
	TCHAR buff[64];
	if (!GetLocaleInfo(LOCALE_USER_DEFAULT, lctype, buff, sizeof(buff)/sizeof(buff[0])))
		return defval;
	return _ttol(buff);
	
}

/**
 * @brief Get string value from LC_ entry in windows locale (NLS) database
 */
static CString getLocaleStr(int lctype, LPCTSTR defval)
{
	CString out;
	LPTSTR outbuff = out.GetBuffer(64);
	int rt = GetLocaleInfo(LOCALE_USER_DEFAULT, lctype, outbuff, 64);
	out.ReleaseBuffer();
	if (!rt)
		out = defval;
	return out;

}

/**
 * @brief Get numeric value for LOCALE_SGROUPING
 */
static UINT GetLocaleGrouping(int defval)
{
	TCHAR buff[64];
	if (!GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, buff, sizeof(buff)/sizeof(buff[0])))
		return defval;
	// handling for Indic 3;2
	if (!_tcscmp(buff, _T("3;2;0")))
		return 32;
	return _ttol(buff);
}

/**
 * @brief Print an integer into a CString, in appropriate fashion for locale & user preferences
 *
 * NB: We are not converting digits from ASCII via LOCALE_SNATIVEDIGITS
 *   So we always use ASCII digits, instead of, eg, the Chinese digits
 */
CString NumToLocaleStr(UINT n)
{
	TCHAR numbuff[34];
	_ltot(n, numbuff, 10);
	return GetLocaleStr(numbuff);
}

/**
 * @brief Insert commas (or periods) into string, as appropriate for locale & preferences
 *
 * NB: We are not converting digits from ASCII via LOCALE_SNATIVEDIGITS
 *   So we always use ASCII digits, instead of, eg, the Chinese digits
 */
CString GetLocaleStr(const CString & str, int decimalDigits)
{
	// Fill in currency format with locale info
	// except we hardcode for no decimal
	NUMBERFMT NumFormat;
	memset(&NumFormat, 0, sizeof(NumFormat));
	NumFormat.NumDigits = decimalDigits; // LOCALE_IDIGITS
	NumFormat.LeadingZero = getLocaleUint(LOCALE_ILZERO, 0);
	NumFormat.Grouping = GetLocaleGrouping(3);
	NumFormat.lpDecimalSep = _T("."); // should not be used
	CString sep = getLocaleStr(LOCALE_STHOUSAND, _T(","));
	NumFormat.lpThousandSep = (LPTSTR)(LPCTSTR)sep;
	NumFormat.NegativeOrder = getLocaleUint(LOCALE_INEGNUMBER , 0);

	CString out;
	LPTSTR outbuff = out.GetBuffer(48);
	int rt = GetNumberFormat(LOCALE_USER_DEFAULT // a predefined value for user locale
		, 0                // operation option (allow overrides)
		, str              // input number (see MSDN for legal chars)
		, &NumFormat           // formatting specifications
		, outbuff             // output buffer
		, 48
		);             // size of output buffer
	out.ReleaseBuffer();
	if (!rt) {
		int nerr = GetLastError();
		CString msg;
		msg.Format(_T("Error %d in NumToStr(): %s"), nerr, GetSysError(nerr));
		TRACE(_T("%s\n"), msg);
		out = str;
	}
	return out;
}


}; // namespace locality
