#include "pch.h"
#include "OptionsEditorSyntax.h"
#include "OptionsMgr.h"
#include "TextDefinition.h"

namespace Options { namespace EditorSyntax
{

/** @brief Setting name for file type extension. */
const tchar_t Section[] = _T("FileTypes");

/**
 * @brief Get the default value of the extension settings from OptionsMgr.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [out] pExtension Default value for extension settings
 */
void GetDefaults(COptionsMgr* pOptionsMgr, String* pExtension)
{
	if (pOptionsMgr == nullptr || pExtension == nullptr)
		return;

	for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; i++)
	{
		LangServices::TextDefinition* def = LangServices::GetTextType(i);
		if (def != nullptr)
		{
			String name = strutils::format(_T("%s/%s.exts"), Section, def->name);
			String exts = pOptionsMgr->GetDefault<String>(name);
			pExtension[i-1] = std::move(exts);
		}
	}
}

/**
 * @brief Initialize file type extension settings.
 * Register the extension settings defined in CrystalLineParser in OptionsMgr as the default value.
 * Register the settings read from the registry in CrystalLineParser.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 */
void Init(COptionsMgr *pOptionsMgr)
{
	if (pOptionsMgr == nullptr)
		return;

	for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; i++)
	{
		// Register the extension settings defined in CrystalLineParser in OptionsMgr as the default value.
		LangServices::TextDefinition* def = LangServices::GetTextType(i);
		if (def != nullptr)
		{
			String name = strutils::format(_T("%s/%s.exts"), Section, def->name);
			pOptionsMgr->InitOption(name, String(def->exts));

			// Register the settings read from the registry in CrystalLineParser.
			String exts = pOptionsMgr->GetString(name);
			LangServices::SetExtension(i, exts.c_str());
		}
	}
}

/**
 * @brief Load extension settings from OptionsMgr.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [out] pExtension Loaded extension settings
 */
void Load(COptionsMgr *pOptionsMgr, String* pExtension)
{
	if (pOptionsMgr == nullptr || pExtension == nullptr)
		return;

	for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; i++)
	{
		LangServices::TextDefinition* def = LangServices::GetTextType(i);
		if (def != nullptr)
		{
			String name = strutils::format(_T("%s/%s.exts"), Section, def->name);
			String exts = pOptionsMgr->GetString(name);
			pExtension[i-1] = std::move(exts);
		}
	}
}

/**
 * @brief Save the extension settings to OptionsMgr and CrystalLineParser.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [in] pExtension Extension settings
 */
void Save(COptionsMgr* pOptionsMgr, const String* const pExtension)
{
	if (pOptionsMgr == nullptr || pExtension == nullptr)
		return;

	for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; i++)
	{
		// Save the extension settings to OptionsMgr.
		LangServices::TextDefinition* def = LangServices::GetTextType(i);
		if (def != nullptr)
		{
			String name = strutils::format(_T("%s/%s.exts"), Section, def->name);
			pOptionsMgr->SaveOption(name, pExtension[i-1]);

			// Save the extension settings to CrystalLineParser.
			LangServices::SetExtension(i, pExtension[i-1].c_str());
		}
	}
}

}}