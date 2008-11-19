/////////////////////////////////////////////////////////////////////////////
//	  WinMerge:  an interactive diff/merge utility
//	  Copyright (C) 1997  Dean P. Grimm
//
//	  This program is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  This program is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this program; if not, write to the Free Software
//	  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/**
 *	@file ShellContextMenu.h
 *
 *	@brief Declaration of class CShellContextMenu
 */ 
//
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _SHELLCONTEXTMENU_H_
#define _SHELLCONTEXTMENU_H_

#include <comdef.h>
#include <list>
#include "UnicodeString.h"

/**
 * @brief Explorer's context menu
 *
 * Allows to query shell context menu for a group of files
 *
 * Usage:
 * <ol>
 * <li>Initialize menu by calling Initialize().
 *	   This should be called each time user wants to show menu - i.e. user does not have to create 
 *	   new instance of CShellContextMenu each time, just needs to call Initialize().
 * <li>Add files using AddItem().</li>
 * <li>Call RequeryShellContextMenu().
 *	   If you have several CShellContextMenu objects for different groups of files and want to show context menu 
 *	   for these groups in turns you should call RequeryShellContextMenu() for appropriate object before menu is shown.
 *	   You can call RequeryShellContextMenu() in WM_INITMENUPOPUP message handler.
 *	   This is probably a "feature" of Windows Shell, but without it menus won't work correctly.</li>
 * <li>Show popup menu via TrackPopupMenu[Ex]() with TPM_RETURNCMD flag using handle that is returned by GetHMENU().
 *	   Handle WM_INITMENUPOPUP, WM_DRAWITEM, WM_MEASUREITEM and WM_MENUCHAR in window procedure of the menu owner 
 *	   and pass them to HandleMenuMessage().</li>
 * <li>Call InvokeCommand() with nCmd returned by TrackPopupMenu[Ex]().</li>
 * </ol>
 *
 * @note Current implementation can successfully query context menu from the shell only if all the files are in the same folder.
 *		 This is checked in QueryShellContextMenu() so caller can simply add items and check return value of RequeryShellContextMenu()
 */
class CShellContextMenu
{
public:
	/**
	 * @brief	Helper describing file entry in the group of files to show shell context menu for
	 */
	struct FileEntry
	{
		FileEntry(const String& aPath, const String& aFilename)
			: path(aPath), filename(aFilename) {}
		String path; /**< path to file, including all but file name */
		String filename; /**< file name */
	};

	/**
	 * @brief Constructor
	 *
	 * @param[in]	cmdFirst	minimum value for a menu item identifier
	 * @param[in]	cmdLast		maximum value for a menu item identifier
	 *
	 * @pre		cmdFirst < cmdLast <= 0xffff
	 * @pre		[cmdFirst, cmdLast] range should not intersect with available command IDs
	 */
	CShellContextMenu(UINT cmdFirst, UINT cmdLast);

	/**
	 * @brief Destructor
	 */
	~CShellContextMenu();

	/**
	 * @brief Initializes menu
	 *
	 * Should be called each time menu is going to be shown.
	 * Initializes m_hShellContextMenu and clears all previously added items.
	 */
	void Initialize();

	/**
	 * @brief	Adds a file to a group of files for which shell context menu is queried
	 *
	 * @param[in]	fileEntry	FileEntry object describing a file to add
	 */
	void AddItem(const FileEntry& fileEntry);

	/**
	 * @brief	Adds a file to a group of files for which shell context menu is queried
	 *
	 * @param[in]	path		path to file, including all but file name
	 * @param[in]	filename	file name without path
	 */
	void AddItem(const String& path,
				 const String& filename);

	/**
	 * @brief	Returns HMENU handle to context menu
	 *
	 * Menu is created by CShellContextMenu using CreatePopupMenu()
	 *
	 * @return	Handle to shell context menu.
	 *			Menu is created in constructor and does not normally change when requesting context menu via RequeryShellContextMenu()
	 *			It may though change if destroyed by someone. In this case it is recreated in RequeryShellContextMenu()
	 */
	HMENU GetHMENU() const;

	/**
	 * @brief	Clears current menu and queries context menu from the shell
	 *
	 * @retval	true	Context menu is queried successfully. User can track menu using handle returned by GetHMENU()
	 * @retval	false	Failed to query context menu
	 */
	bool RequeryShellContextMenu();

	/**
	 * @brief	Forwards certain messages to context menu so it works properly
	 *
	 * Handles WM_INITMENUPOPUP, WM_DRAWITEM, WM_MEASUREITEM and WM_MENUCHAR messages
	 *
	 * @param[in]		message		Message to handle
	 * @param[in]		wParam		Additional message-specific information
	 * @param[in]		lParam		Additional message-specific information
	 * @param[out]		retval		Value returned by message handler
	 *
	 * @retval	true	message was handled
	 * @retval	false	message was not handled
	 */
	bool HandleMenuMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& retval);

	/**
	 * @brief	Handles selected menu command
	 *
	 * @param[in]	nCmd	Menu item identifier returned by TrackPopupMenu[Ex]()
	 * @param[in]	hWnd	Handle to the window that owns popup menu ( window handle passed to TrackPopupMenu[Ex]() )
	 *
	 * @retval	true	Everything is OK
	 * @retval	false	Something failed
	 */
	bool InvokeCommand(UINT nCmd, HWND hWnd);

private:
	/**
	 * @brief	Queries context menu from the shell
	 *
	 * Initializes \ref m_pPreferredMenu, \ref m_pShellContextMenu2, \ref m_pShellContextMenu3 and \ref m_hShellContextMenu
	 * that are used later for showing menu and handling commands
	 *
	 * @retval	true	Menu is queried successfully
	 * @retval	false	Failed to query context menu
	 */
	bool QueryShellContextMenu();

private:
	typedef std::list<FileEntry> FilenamesContainer;

	FilenamesContainer m_files; /**< List of files to show context menu for */

	LPCONTEXTMENU m_pPreferredMenu; /**< Shell context menu for group of files. 
										 Points either to IContextMenu, IContextMenu2 or IContextMenu3 interface, 
										 the highest available. Used to process command in InvokeCommand() */
	IContextMenu2Ptr m_pShellContextMenu2; /**< IContextMenu2 interface of current preferred context menu */
	IContextMenu3Ptr m_pShellContextMenu3; /**< IContextMenu3 interface of current preferred context menu */

	HMENU m_hShellContextMenu; /**< HMENU handle of shell context menu for group of files */

	const UINT m_cmdFirst; /**< minimum value for a menu item identifier */
	const UINT m_cmdLast; /**< maximum value for a menu item identifier */
};

#endif // _SHELLCONTEXTMENU_H_
