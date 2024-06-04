#include "pch.h"
#include "OptionsEditorSyntax.h"
#include "OptionsMgr.h"
#include "parsers/crystallineparser.h"

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

	for (int i = CrystalLineParser::SRC_ABAP; i <= CrystalLineParser::SRC_XML; i++)
	{
		CrystalLineParser::TextDefinition* def = CrystalLineParser::GetTextType(i);
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

	for (int i = CrystalLineParser::SRC_ABAP; i <= CrystalLineParser::SRC_XML; i++)
	{
		// Register the extension settings defined in CrystalLineParser in OptionsMgr as the default value.
		CrystalLineParser::TextDefinition* def = CrystalLineParser::GetTextType(i);
		if (def != nullptr)
		{
			String name = strutils::format(_T("%s/%s.exts"), Section, def->name);
			pOptionsMgr->InitOption(name, String(def->exts));

			// Register the settings read from the registry in CrystalLineParser.
			String exts = pOptionsMgr->GetString(name);
			CrystalLineParser::SetExtension(i, exts.c_str());
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

	for (int i = CrystalLineParser::SRC_ABAP; i <= CrystalLineParser::SRC_XML; i++)
	{
		CrystalLineParser::TextDefinition* def = CrystalLineParser::GetTextType(i);
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

	for (int i = CrystalLineParser::SRC_ABAP; i <= CrystalLineParser::SRC_XML; i++)
	{
		// Save the extension settings to OptionsMgr.
		CrystalLineParser::TextDefinition* def = CrystalLineParser::GetTextType(i);
		if (def != nullptr)
		{
			String name = strutils::format(_T("%s/%s.exts"), Section, def->name);
			pOptionsMgr->SaveOption(name, pExtension[i-1]);

			// Save the extension settings to CrystalLineParser.
			CrystalLineParser::SetExtension(i, pExtension[i-1].c_str());
		}
	}
}

}}