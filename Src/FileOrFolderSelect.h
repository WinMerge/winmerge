/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2006  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  FileOrFolderSelect.h
 *
 * @brief Declaration of the file and folder selection routines.
 */
#pragma once

#include "UnicodeString.h"

bool SelectFile(HWND parent, String& path, bool is_open = true,
			const tchar_t* initialPath = nullptr, const String& stitle = _T(""),
			const String& sfilter = _T(""), const tchar_t* defaultExtension = nullptr);

bool SelectFolder(String& path, const tchar_t* root_path = nullptr, 
			 const String& title = _T(""),
			 HWND hwndOwner = nullptr);

bool SelectFileOrFolder(HWND parent, String& path, const tchar_t* root_path = nullptr);
