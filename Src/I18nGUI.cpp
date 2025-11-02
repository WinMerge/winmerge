/**
 * @file I18nGUI.cpp
 * @brief Implementation of GUI-related internationalization functions
 */
#include "StdAfx.h"
#include "Merge.h"
#include "I18nGUI.h"
#include "LanguageSelect.h"
#include "paths.h"

namespace I18n
{
/**
 * @brief Return windows language ID of current WinMerge GUI language
 */
short GetLangId()
{
	return theApp.m_pLangDlg->GetLangId();
}

String GetLangName()
{
	String name, ext;
	paths::SplitFilename(theApp.m_pLangDlg->GetFileName(GetLangId()), nullptr, &name, &ext);
	return name;
}

/**
 * @brief Lang aware version of CStatusBar::SetIndicators()
 */
void SetIndicators(CStatusBar& sb, const UINT* rgid, int n)
{
	theApp.m_pLangDlg->SetIndicators(sb, rgid, n);
}

/**
 * @brief Translate menu to current WinMerge GUI language
 */
void TranslateMenu(HMENU h)
{
	theApp.m_pLangDlg->TranslateMenu(h);
}

/**
 * @brief Translate dialog to current WinMerge GUI language
 */
void TranslateDialog(HWND h)
{
	CWnd* pWnd = CWnd::FromHandle(h);
	pWnd->SetFont(const_cast<CFont*>(&theApp.m_fontGUI));
	pWnd->SendMessageToDescendants(WM_SETFONT, (WPARAM)theApp.m_fontGUI.m_hObject, MAKELPARAM(FALSE, 0), TRUE);

	theApp.m_pLangDlg->TranslateDialog(h);
}

/**
 * @brief Load string and translate to current WinMerge GUI language
 */
String LoadString(UINT id)
{
	return theApp.m_pLangDlg->LoadString(id);
}

bool TranslateString(const std::string& str, String& translated_str)
{
	return theApp.m_pLangDlg->TranslateString(str, translated_str);
}

bool TranslateString(const std::wstring& str, String& translated_str)
{
	return theApp.m_pLangDlg->TranslateString(str, translated_str);
}

/**
 * @brief Load dialog caption and translate to current WinMerge GUI language
 */
std::wstring LoadDialogCaption(const tchar_t* lpDialogTemplateID)
{
	return theApp.m_pLangDlg->LoadDialogCaption(lpDialogTemplateID);
}

/**
 * @brief Lang aware version of AfxMessageBox()
 */
int MessageBox(UINT nIDPrompt, UINT nType, UINT nIDHelp)
{
	String string = I18n::LoadString(nIDPrompt);
	if (nIDHelp == (UINT)-1)
		nIDHelp = nIDPrompt;
	return AfxMessageBox(string.c_str(), nType, nIDHelp);
}

String tr(const std::string& str)
{
	String translated_str;
	TranslateString(str, translated_str);
	return translated_str;
}

String tr(const std::wstring& str)
{
	String translated_str;
	TranslateString(str, translated_str);
	return translated_str;
}

String tr(const char* msgctxt, const std::string& str)
{
	String translated_str;
	if (msgctxt)
		TranslateString("\x01\"" + std::string(msgctxt) + "\"" + str, translated_str);
	else
		TranslateString(str, translated_str);
	return translated_str;
}

}

