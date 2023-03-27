/** 
 * @file  locality.cpp
 *
 * @brief Implementation of helper functions involving locale
 */

#include "pch.h"
#include "locality.h"
#include <windows.h>
#include <Poco/Format.h>
#include <Poco/Debugger.h>
#include <Poco/Timestamp.h>

using Poco::format;
using Poco::Debugger;
using Poco::Timestamp;

namespace locality {

/**
 * @brief Get numeric value from an LC_ entry in windows locale (NLS) database
 */
static unsigned getLocaleUint(int lctype, int defval)
{
	tchar_t buff[64];
	if (!GetLocaleInfo(LOCALE_USER_DEFAULT, lctype, buff, sizeof(buff)/sizeof(buff[0])))
		return defval;
	return tc::ttol(buff);
}

/**
 * @brief Get numeric value for LOCALE_SGROUPING
 */
static unsigned GetLocaleGrouping(int defval)
{
	tchar_t buff[64];
	if (!GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, buff, sizeof(buff)/sizeof(buff[0])))
		return defval;
	// handling for Indic 3;2
	if (!tc::tcscmp(buff, _T("3;2;0")))
		return 32;
	return tc::ttol(buff);
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
	return GetLocaleStr(strutils::to_str(n).c_str());
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
String NumToLocaleStr(int64_t n)
{
	return GetLocaleStr(strutils::to_str(n).c_str());
}

/**
 * @brief Insert commas (or periods) into string, as appropriate for locale & preferences
 *
 * NB: We are not converting digits from ASCII via LOCALE_SNATIVEDIGITS
 *   So we always use ASCII digits, instead of, eg, the Chinese digits
 */
String GetLocaleStr(const tchar_t *str, int decimalDigits)
{
	// Fill in currency format with locale info
	// except we hardcode for no decimal
	tchar_t DecimalSep[8];
	tchar_t SepDefault[] = _T(".");
	tchar_t ThousandSep[8];
	NUMBERFMT NumFormat = { 0 };
	NumFormat.NumDigits = decimalDigits; // LOCALE_IDIGITS
	NumFormat.LeadingZero = getLocaleUint(LOCALE_ILZERO, 0);
	NumFormat.Grouping = GetLocaleGrouping(3);
	NumFormat.lpDecimalSep = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, DecimalSep, 8) ? DecimalSep : SepDefault;
	NumFormat.lpThousandSep = GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, ThousandSep, 8) ? ThousandSep : SepDefault;
	NumFormat.NegativeOrder = getLocaleUint(LOCALE_INEGNUMBER , 0);
	String out;
	out.resize(48);
	tchar_t* outbuff = &*out.begin(); //GetBuffer(48);
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
		Debugger::message(format("Error %d in NumToStr()\n", GetLastError()));
		out = str;
	}
	return out;
}

/**
 * @brief Convert unix time to string to show in the GUI.
 * @param [in] tim Time in seconds since 1.1.1970.
 * @return Time as a string, proper to show in the GUI.
 */
String TimeString(const int64_t * tim)
{
	if (tim == nullptr) return _T("---");
	
	SYSTEMTIME sysTimeGlobal, sysTime;
	FILETIME ft;
	Timestamp t(*tim * Timestamp::resolution());

	if (t == 0)
		return String();
	t.toFileTimeNP((unsigned int&)ft.dwLowDateTime, (unsigned int&)ft.dwHighDateTime);
	if (!FileTimeToSystemTime(&ft, &sysTimeGlobal) ||
	    !SystemTimeToTzSpecificLocalTime(nullptr, &sysTimeGlobal, &sysTime))
		return _T("---");

	tchar_t buff[128];
	int len = GetDateFormat(LOCALE_USER_DEFAULT, 0, &sysTime, nullptr, buff, sizeof(buff)/sizeof(buff[0]));
	buff[len - 1] = ' ';
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &sysTime, nullptr, buff + len, sizeof(buff)/sizeof(buff[0]) - len - 1);
	return buff;
}

}; // namespace locality
