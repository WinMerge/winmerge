/** 
 * @file  AnsiConvert.cpp
 *
 * @brief Implementation file for Unicode to ANSI conversion routines.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <windows.h>

static LPCSTR convert(LPCTSTR str, UINT codepage);

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
	return convert(str, CP_ACP);
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
	return convert(str, CP_THREAD_ACP);
}

/**
 * @brief Convert from Unicode to Ansi using given codepage.
 * @param [in] str String to convert.
 * @param [in] codepage Codepage to use in conversion.
 * @return Ansi string.
 * @note Function must allocate a new string, as DiffFileData gets the
 * filename and expects it can be freed with free().
 */
LPCSTR convert(LPCTSTR str, UINT codepage)
{
#ifndef UNICODE
	return strdup(str);
#else
	int len = WideCharToMultiByte(codepage, 0, str, -1, 0, 0, 0, 0);
	if (len)
	{
		char * ansi = (char *)malloc(len);
		WideCharToMultiByte(codepage, 0, str, -1, ansi, len, NULL, NULL);
		return ansi;
	}
	return NULL;
#endif
}
