#include "pch.h"
#include "OptionsEditorSyntax.h"
#include "OptionsMgr.h"
#include "TextDefinition.h"

namespace Options { namespace EditorSyntax
{

/** @brief Setting name for file type extension. */
const tchar_t Section[] = _T("FileTypes");

/**
 * @brief Name of the setting holding the variation chosen last for a base text type, e.g. FileTypes/SQL.type
 * holds SQL, PostgreSQL or MySQL, or nothing when none was chosen ("Auto Syntax").
 */
static String VariationOptionName(const LangServices::TextDefinition* base)
{
	return strutils::format(_T("%s/%s.type"), Section, base->name);
}

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

	// The variation chosen last for each base type that has variations, stored by name (empty for none)
	for (int i = LangServices::LanguageId::SRC_ABAP; i < LangServices::LanguageId::SRC_MAX_ENTRY; i++)
	{
		LangServices::TextDefinition* base = LangServices::GetTextType(i);
		if (base == nullptr || LangServices::GetTextTypeVariationBase(i) != i)
			continue;
		const String option = VariationOptionName(base);
		pOptionsMgr->InitOption(option, String());
		const String variationName = pOptionsMgr->GetString(option);
		for (int j = LangServices::LanguageId::SRC_ABAP; !variationName.empty() && j < LangServices::LanguageId::SRC_MAX_ENTRY; j++)
		{
			LangServices::TextDefinition* def = LangServices::GetTextType(j);
			if (def != nullptr && LangServices::GetTextTypeVariationBase(j) == i && variationName == def->name)
				LangServices::SetTextTypeVariation(j);
		}
	}
}

/**
 * @brief Keep a text type variation for the files the extensions of its base type match (see Init).
 * Types without a variation group are ignored.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [in] nTextType Text type index
 */
void SaveTextTypeVariation(COptionsMgr *pOptionsMgr, int nTextType)
{
	LangServices::TextDefinition* def = LangServices::GetTextType(nTextType);
	LangServices::TextDefinition* base = LangServices::GetTextType(LangServices::GetTextTypeVariationBase(nTextType));
	if (pOptionsMgr == nullptr || def == nullptr || base == nullptr)
		return;
	LangServices::SetTextTypeVariation(nTextType);
	pOptionsMgr->SaveOption(VariationOptionName(base), String(def->name));
}

/**
 * @brief Forget the variation chosen for a base text type: its files get the base type again ("Auto Syntax").
 * Types without variations are ignored.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [in] nBase Base text type index
 */
void ResetTextTypeVariation(COptionsMgr *pOptionsMgr, int nBase)
{
	LangServices::TextDefinition* base = LangServices::GetTextType(nBase);
	if (pOptionsMgr == nullptr || base == nullptr || LangServices::GetTextTypeVariationBase(nBase) != nBase)
		return;
	LangServices::ResetTextTypeVariation(nBase);
	pOptionsMgr->SaveOption(VariationOptionName(base), String());
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