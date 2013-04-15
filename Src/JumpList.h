/** 
 * @file  JumpList.h
 *
 * @brief Declaration file for JumpList helper functions.
 *
 */
#ifndef _JUMPLIST_H_
#define _JUMPLIST_H_

#include "UnicodeString.h"

namespace JumpList
{
	bool SetCurrentProcessExplicitAppUserModelID(const std::wstring& appid);
	bool AddToRecentDocs(const String& app_path = _T(""), const String& params = _T(""), const String& title = _T(""), const String& desc = _T(""), int icon_index = 0);
}

#endif
