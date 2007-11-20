/** 
 * @file  locality.h
 *
 * @brief Implementation of helper functions involving locale
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h"
#include "locality.h"
#include "Merge.h"

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
 *
 * @param [in] n Number to convert.
 * @return Converted string.
 */
String NumToLocaleStr(int n)
{
	TCHAR numbuff[34];
	_ltot(n, numbuff, 10);
	return GetLocaleStr(numbuff);
}

/**
 * @brief Print an integer into a CString, in appropriate fashion for locale & user preferences
 *
 * NB: We are not converting digits from ASCII via LOCALE_SNATIVEDIGITS
 *   So we always use ASCII digits, instead of, eg, the Chinese digits
 *
 * @param [in] n Number to convert.
 * @return Converted string.
 */
String NumToLocaleStr(__int64 n)
{
	TCHAR numbuff[34];
	_i64tot(n, numbuff, 10);
	return GetLocaleStr(numbuff);
}

/**
 * @brief Insert commas (or periods) into string, as appropriate for locale & preferences
 *
 * NB: We are not converting digits from ASCII via LOCALE_SNATIVEDIGITS
 *   So we always use ASCII digits, instead of, eg, the Chinese digits
 */
String GetLocaleStr(LPCTSTR str, int decimalDigits)
{
	// Fill in currency format with locale info
	// except we hardcode for no decimal
	TCHAR DecimalSep[8];
	TCHAR ThousandSep[8];
	NUMBERFMT NumFormat;
	memset(&NumFormat, 0, sizeof(NumFormat));
	NumFormat.NumDigits = decimalDigits; // LOCALE_IDIGITS
	NumFormat.LeadingZero = getLocaleUint(LOCALE_ILZERO, 0);
	NumFormat.Grouping = GetLocaleGrouping(3);
	NumFormat.lpDecimalSep = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, DecimalSep, 8) ? DecimalSep : _T(".");
	NumFormat.lpThousandSep = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, ThousandSep, 8) ? ThousandSep : _T(",");
	NumFormat.NegativeOrder = getLocaleUint(LOCALE_INEGNUMBER , 0);
	String out;
	out.resize(48);
	LPTSTR outbuff = &*out.begin(); //GetBuffer(48);
	int rt = GetNumberFormat(LOCALE_USER_DEFAULT // a predefined value for user locale
		, 0                // operation option (allow overrides)
		, str              // input number (see MSDN for legal chars)
		, &NumFormat           // formatting specifications
		, outbuff             // output buffer
		, 48
		);             // size of output buffer
	if (rt)
	{
		// rt includes terminating zero
		out.resize(rt - 1);
	}
	else
	{
		int nerr = GetLastError();
		TRACE(_T("Error %d in NumToStr(): %s\n"), nerr, GetSysError(nerr));
		out = str;
	}
	return out;
}

/**
 * @brief Return time displayed appropriately, as string
 */
String TimeString(const __int64 * tim)
{
	USES_CONVERSION;
	if (!tim) return _T("---");
	// _tcsftime does not respect user date customizations from
	// Regional Options/Configuration Regional; COleDateTime::Format does so.
#if _MSC_VER < 1300
		// MSVC6
	COleDateTime odt = (time_t)*tim;
#else
		// MSVC7 (VC.NET)
	COleDateTime odt = *tim;
#endif
	// If invalid, return DateTime resource string
	if (odt.GetStatus() == COleDateTime::null)
		return String();
	if (odt.GetStatus() == COleDateTime::invalid)
		return theApp.LoadString(AFX_IDS_INVALID_DATETIME);
	COleVariant var;
	// Don't need to trap error. Should not fail due to type mismatch
	AfxCheckError(VarBstrFromDate(odt.m_dt, LANG_USER_DEFAULT, 0, &V_BSTR(&var)));
	var.vt = VT_BSTR;
	return OLE2CT(V_BSTR(&var));
}

}; // namespace locality
