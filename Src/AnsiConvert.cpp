/** 
 * @file  AnsiConvert.cpp
 *
 * @brief Implementation file for Unicode to ANSI conversion routines.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"

/**
 * @brief Convert from Unicode to Ansi using system codepage.
 * This function converts Unicode string to ansi string using system codepage.
 * This conversion function should be used when converting strings containing
 * paths. As paths are handled by the system and are not file content.
 * @param [in] str String to convert.
 * @return Ansi string.
 */
LPCSTR ansiconvert_SystemCP(LPCTSTR str)
{
	// These lines replace USES_CONVERSION macro
	int _convert = 0;
	UINT _acp = GetACP();
	LPCWSTR _lpw = 0;
	LPCSTR  _lpa = 0;

	return T2CA(str);
}

/**
 * @brief Convert from Unicode to Ansi using thread codepage.
 * This function converts Unicode string to ansi string using thread codepage.
 * Thread codepage is practically the codepage WinMerge is using internally.
 * @param [in] str String to convert.
 * @return Ansi string.
 */
LPCSTR ansiconvert_ThreadCP(LPCTSTR str)
{
	USES_CONVERSION;
	return T2CA(str);
}


