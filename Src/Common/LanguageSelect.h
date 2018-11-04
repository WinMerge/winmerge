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
	void InitializeLanguage(WORD langID);

	bool TranslateString(unsigned uid, std::string &) const;
	bool TranslateString(unsigned uid, std::wstring &) const;
	bool TranslateString(const std::string&, String &) const;
	void SetIndicators(CStatusBar &, const UINT *, int) const;
	void TranslateMenu(HMENU) const;
	void TranslateDialog(HWND) const;
	void RetranslateDialog(HWND, const TCHAR *name) const;
	String LoadString(UINT) const;
	std::wstring LoadDialogCaption(LPCTSTR lpDialogTemplateID) const;
	std::vector<std::pair<LANGID, String> > GetAvailableLanguages() const;
	bool SetLanguage(LANGID, bool bShowError = false);

// Implementation data
private:
	HINSTANCE m_hCurrentDll;
	LANGID m_wCurLanguage;
	std::map<unsigned, std::string> m_map_uid_to_msgid;
	typedef std::map<std::string, unsigned> EngMsgIDToUIDMap;
	EngMsgIDToUIDMap m_map_msgid_to_uid;
	unsigned m_codepage;
// Implementation methods
private:
	String GetFileName(LANGID) const;
	bool LoadLanguageFile(LANGID, bool bShowError = false);
};
