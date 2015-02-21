/** 
 * @file  LanguageSelect.h
 *
 * @brief Declaration file for CLanguageSelect dialog.
 */
#pragma once

#include <vector>
#include <string>
#include <map>
#include "CMoveConstraint.h"

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

	bool TranslateString(size_t line, std::string &) const;
	bool TranslateString(size_t line, std::wstring &) const;
	bool TranslateString(const std::string&, String &) const;
	void SetIndicators(CStatusBar &, const UINT *, int) const;
	void TranslateMenu(HMENU) const;
	void TranslateDialog(HWND) const;
	String LoadString(UINT) const;
	std::wstring LoadDialogCaption(LPCTSTR lpDialogTemplateID) const;
	std::vector<std::pair<LANGID, String> > GetAvailableLanguages() const;
	BOOL SetLanguage(LANGID, BOOL bShowError = FALSE);

// Implementation data
private:
	HINSTANCE m_hCurrentDll;
	LANGID m_wCurLanguage;
	std::vector<std::string> m_strarray;
	typedef std::map<std::string, int> EngLinenoMap;
	EngLinenoMap m_map_lineno;
	unsigned m_codepage;
// Implementation methods
private:
	String GetFileName(LANGID) const;
	BOOL LoadLanguageFile(LANGID, BOOL bShowError = FALSE);
};
