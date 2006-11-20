// stdafx.cpp : source file that includes just the standard includes
//	ProjectFile.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

/**
 * @brief Load string resource and return as CString.
 * @param [in] id Resource string ID.
 * @return Resource string as CString.
 */
CString LoadResString(int id)
{
	CString s;
	VERIFY(s.LoadString(id));
	return s;
}
