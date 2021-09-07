#pragma once

#include <tchar.h>

/**
 * @brief Flags for enabling and other settings of context menu.
 */
enum ExtensionFlags
{
	EXT_ENABLED = 0x01, /**< ShellExtension enabled/disabled. */
	EXT_ADVANCED = 0x02, /**< Advanced menuitems enabled/disabled. */
};

enum
{
	CMD_COMPARE,
	CMD_COMPARE_ELLIPSIS,
	CMD_SELECT_LEFT,
	CMD_SELECT_MIDDLE,
	CMD_RESELECT_LEFT,
	CMD_LAST = CMD_RESELECT_LEFT,
};

/**
 * @brief The states in which the menu can be.
 * These states define what items are added to the menu and how those
 * items work.
 */
enum
{
	MENU_HIDDEN = -1,
	MENU_SIMPLE = 0,  /**< Simple menu, only "Compare item" is shown. */
	MENU_ONESEL_NOPREV,  /**< One item selected, no previous selections. */
	MENU_ONESEL_PREV,  /**< One item selected, previous selection exists. */
	MENU_ONESEL_TWO_PREV,  /**< One item selected, two previous selections exist. */
	MENU_TWOSEL,  /**< Two items are selected. */
	MENU_THREESEL
};

/// Max. filecount to select
inline const int MaxFileCount = 3;

/// Registry path to WinMerge
#define REGDIR _T("Software\\Thingamahoochie\\WinMerge")
inline const TCHAR f_RegDir[] = REGDIR;
inline const TCHAR f_RegLocaleDir[] = REGDIR _T("\\Locale");
inline const TCHAR f_RegSettingsDir[] = REGDIR _T("\\Settings");

/**
 * @name Registry valuenames.
 */
 /*@{*/
 /** Shell context menuitem enabled/disabled */
inline const TCHAR f_RegValueEnabled[] = _T("ContextMenuEnabled");
/** 'Saved' path in advanced mode */
inline const TCHAR f_FirstSelection[] = _T("FirstSelection");
/** 'Saved' path in advanced mode */
inline const TCHAR f_SecondSelection[] = _T("SecondSelection");
/** Path to WinMergeU.exe */
inline const TCHAR f_RegValuePath[] = _T("Executable");
/** Path to WinMergeU.exe, overwrites f_RegValuePath if present. */
inline const TCHAR f_RegValuePriPath[] = _T("PriExecutable");
/** LanguageId */
inline const TCHAR f_LanguageId[] = _T("LanguageId");
/** Recurse */
inline const TCHAR f_Recurse[] = _T("Recurse");
/*@}*/

