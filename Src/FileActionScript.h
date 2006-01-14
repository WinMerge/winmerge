/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  FileActionScript.h
 *
 * @brief Declaration file for FileActionScript and related classes
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _FILEACTIONSCRIPT_H_
#define _FILEACTIONSCRIPT_H_

class CShellFileOp;

enum CreateScriptReturn
{
	SCRIPT_FAIL = 0,
	SCRIPT_SUCCESS,
	SCRIPT_USERCANCEL,
	SCRIPT_USERSKIP,
};

/** 
 * @brief FileAction presents one filesystem action we want to do.
 *
 * Action consists of source, destination and action type. This
 * allows us to combine lots of different kind of actions into
 * one list.
 **/
struct FileAction
{
	typedef enum { ACT_COPY = 1, ACT_MOVE, ACT_DEL, } ACT_TYPE;

	CString src; /**< Source for action */
	CString dest; /**< Destination action */
	BOOL dirflag; /**< Is it directory? (TRUE means directory) */
	ACT_TYPE atype; /**< Action's type */
};

/** 
 * @brief FileActionItem presents one filesystem action with UI context.
 *
 * This struct adds UI context and UI action to filesystem action.
 * UI context and action is for storing reference to UI and then updating
 * UI after action script is run.
 */
struct FileActionItem : public FileAction
{
	typedef enum { UI_SYNC = 1, UI_DESYNC, UI_DEL_LEFT, UI_DEL_RIGHT, UI_DEL_BOTH,} UI_RESULT;

	int context; /**< UI context */
	UI_RESULT UIResult; /**< Resulting UI action */
};

typedef CList<FileActionItem, FileActionItem&> FileActionList;

/** 
 * @brief FileActionScript holds list of fileactions and runs those actions.
 *
 * This class holds list of actions we want to make with filesystem. After
 * whole list of actions (script) is composed we can run this sript with
 * one command.
 */
class FileActionScript
{
public:
	FileActionScript();
	~FileActionScript();

	FileActionList actions;

	void SetParentWindow(CWnd * pWnd);
	void UseRecycleBin(BOOL bUseRecycleBin);
	int GetCount() const;
	BOOL Run();

protected:
	int VCSCheckOut(const CString &path, BOOL &bApplyToAll);
	int CreateOperationsScripts();

private:
	CShellFileOp * m_pOperations[3]; /**< One container for every action type */
	BOOL m_bHasOperations[3]; /**< Does container have operations? */
	BOOL m_bUseRecycleBin; /**< Use recycle bin for script actions? */
	CWnd * m_pParentWindow; /**< Parent window for showing messages */
};

#endif // _FILEACTIONSCRIPT_H_
