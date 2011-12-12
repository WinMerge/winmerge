// stdafx.cpp : source file that includes just the standard includes
//	Sample.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


/**
 * @brief Load string resource and return as CString.
 * @param [in] id Resource string ID.
 * @return Resource string as CString.
 */
std::basic_string<TCHAR> LoadResString(UINT id)
{
	TCHAR szBuf[512];
	LoadString(NULL, id, szBuf, sizeof(szBuf));
	return std::basic_string<TCHAR>(szBuf);
}

/**
 * @brief Wrapper around CMergeApp::TranslateDialog()
 */
void NTAPI LangTranslateDialog(HWND h)
{
}
