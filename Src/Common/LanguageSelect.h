/** 
 * @file  LanguageSelect.h
 *
 * @brief Declaration file for CLanguageSelect dialog.
 */
#pragma once

#include <vector>
#include <string>
#include <map>

/////////////////////////////////////////////////////////////////////////////
// CLanguageSelect class

/**
 * @brief Class for selecting GUI language.
 *
 * Language select dialog shows list of installed GUI languages and
 * allows user to select one for use.
 */
class CLanguageSelect
{
// Construction
public:
	CLanguageSelect();   // standard constructor
	WORD GetLangId() const { return m_wCurLanguage; }
	String GetFileName(LANGID) const;
	void InitializeLanguage(WORD langID);

	bool TranslateString(const std::wstring&, std::wstring&) const;
	bool TranslateString(const std::string&, String&) const;
	void SetIndicators(CStatusBar &, const UINT *, int) const;
	void TranslateMenu(HMENU) const;
	void TranslateDialog(HWND) const;
	void RetranslateDialog(HWND, const tchar_t *name) const;
	String LoadString(UINT) const;
	std::wstring LoadDialogCaption(const tchar_t* lpDialogTemplateID) const;
	std::vector<std::pair<LANGID, String> > GetAvailableLanguages() const;
	bool SetLanguage(LANGID, bool bShowError = false);

// Implementation data
private:
	LANGID m_wCurLanguage;
	std::map<std::wstring, std::wstring> m_map_msgid_to_msgstr;
// Implementation methods
private:
	bool LoadLanguageFile(LANGID, bool bShowError = false);
};
