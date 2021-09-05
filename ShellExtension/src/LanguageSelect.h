/** 
 * @file  LanguageSelect.h
 *
 * @brief Declaration file for CLanguageSelect dialog.
 */
#pragma once

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
	bool LoadLanguageFile(LANGID wLangId, const std::wstring& sLanguagesFolder);
	bool TranslateString(const std::wstring&, std::wstring&) const;
	LANGID GetLangId() const { return m_langId; }

// Implementation data
private:
	std::map<std::wstring, std::wstring> m_map_msgid_to_msgstr;
// Implementation methods
private:
	static std::wstring GetFileName(LANGID wLangId, const std::wstring& sLanguagesFolder);
	LANGID m_langId;
};
