// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#include <atlimpl.cpp>

/**
 * @brief Get appropriate clipboard format for TCHAR text
 */
int GetClipTcharTextFormat()
{
#ifdef _UNICODE
	return CF_UNICODETEXT;
#else
	return CF_TEXT;
#endif // _UNICODE
}
