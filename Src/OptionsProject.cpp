#include "pch.h"
#include "OptionsProject.h"
#include "OptionsMgr.h"

namespace Options { namespace Project
{

/** @brief Setting name for project options. */
const tchar_t Section[] = _T("Project");

/** @brief Setting name for operations */
const String OperationKeyword[OperationCount] = { _T("Open"), _T("Load"), _T("Save") };

/** @brief Setting name for items */
const String ItemKeyword[ItemCount] = { _T("FileFilter"), _T("IncludeSubfolders"), _T("UnpackerPlugin"), _T("CompareOptions"), _T("HiddenItems") };

/**
 * @brief Initialize project settings.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 */
void Init(COptionsMgr *pOptionsMgr)
{
	if (pOptionsMgr == nullptr)
		return;

	for (int i = 0; i < OperationCount; i++)
		for (int j = 0; j < ItemCount; j++)
		{
			String name = strutils::format(_T("%s/%s.%s"), Section, OperationKeyword[i], ItemKeyword[j]);
			pOptionsMgr->InitOption(name, true);
		}
}

/**
 * @brief Load the project settings from OptionsMgr.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [out] settings Loaded project settings
 */
void Load(COptionsMgr *pOptionsMgr, Settings settings)
{

	if (pOptionsMgr == nullptr || settings == nullptr)
		return;

	for (int i = 0; i < OperationCount; i++)
		for (int j = 0; j < ItemCount; j++)
		{
			String name = strutils::format(_T("%s/%s.%s"), Section, OperationKeyword[i], ItemKeyword[j]);
			settings[i][j] = pOptionsMgr->GetBool(name);
		}
}

/**
 * @brief Save the project settings to OptionsMgr.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [in] settings Project settings
 */
void Save(COptionsMgr* pOptionsMgr, const Settings& settings)
{

	if (pOptionsMgr == nullptr)
		return;

	for (int i = 0; i < OperationCount; i++)
		for (int j = 0; j < ItemCount; j++)
		{
			String name = strutils::format(_T("%s/%s.%s"), Section, OperationKeyword[i], ItemKeyword[j]);
			pOptionsMgr->SaveOption(name, settings[i][j]);
		}
}

/**
 * @brief Get the project setting from OptionsMgr.
 * @param [in] pOptionsMgr Pointer to OptionsMgr
 * @param [in] operation Operation for project files
 * @param [in] item Item to save to or restore from the project file
 */
bool Get(COptionsMgr* pOptionsMgr, Operation operation, Item item)
{
	if (pOptionsMgr == nullptr)
		return false;

	String name = strutils::format(_T("%s/%s.%s"), Section, OperationKeyword[static_cast<int>(operation)], ItemKeyword[static_cast<int>(item)]);
	return pOptionsMgr->GetBool(name);
}

}}