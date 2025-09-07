/**
 * @file I18n.h
 * @brief Declaration of GUI-related internationalization functions
 */
#pragma once

#include "UnicodeString.h"

class CStatusBar;

namespace I18n
{
	short GetLangId();
	String GetLangName();
	void TranslateMenu(HMENU);
	void TranslateDialog(HWND);
	String LoadString(UINT);
	bool TranslateString(const std::string&, String&);
	bool TranslateString(const std::wstring&, String&);
	std::wstring LoadDialogCaption(const tchar_t*);
	void SetIndicators(CStatusBar& sb, const UINT* rgid, int n);
	int MessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp = (UINT)-1);
}

