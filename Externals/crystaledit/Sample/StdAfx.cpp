// stdafx.cpp : source file that includes just the standard includes
//	Sample.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

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
