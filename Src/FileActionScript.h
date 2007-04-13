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
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _FILEACTIONSCRIPT_H_
#define _FILEACTIONSCRIPT_H_

class CShellFileOp;

/** 
 * @brief Return values for FileActionScript functions.
 */
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
	typedef enum
	{
		UI_SYNC = 1,
		UI_DESYNC,
		UI_DEL_LEFT,
		UI_DEL_RIGHT,
		UI_DEL_BOTH,
		UI_DONT_CARE,
	} UI_RESULT;

	typedef enum { UI_LEFT, UI_RIGHT } UI_SIDE;

	int context; /**< UI context */
	UI_RESULT UIResult; /**< Resulting UI action */
	UI_SIDE UIOrigin; /**< Original UI-side */
	UI_SIDE UIDestination; /**< Destination UI-side */
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

	void SetParentWindow(CWnd * pWnd);
	void UseRecycleBin(BOOL bUseRecycleBin);
	BOOL Run();

	// Manipulate the FileActionList
	int GetActionItemCount() const;
	void AddActionItem(FileActionItem & item) { m_actions.AddTail(item); }
	FileActionItem RemoveTailActionItem() { return m_actions.RemoveTail(); }
	FileActionItem GetHeadActionItem() const { return m_actions.GetHead(); }

protected:
	int VCSCheckOut(const CString &path, BOOL &bApplyToAll);
	int CreateOperationsScripts();

private:
	FileActionList m_actions; /**< List of all actions for this script. */
	CShellFileOp * m_pCopyOperations; /**< Copy operations. */
	BOOL m_bHasCopyOperations; /**< flag if we've put anything into m_pCopyOperations */
	CShellFileOp * m_pMoveOperations; /**< Move operations. */
	BOOL m_bHasMoveOperations; /**< flag if we've put anything into m_pMoveOperations */
	CShellFileOp * m_pDelOperations; /**< Delete operations. */
	BOOL m_bHasDelOperations; /**< flag if we've put anything into m_pDelOperations */
	BOOL m_bUseRecycleBin; /**< Use recycle bin for script actions? */
	CWnd * m_pParentWindow; /**< Parent window for showing messages */
};

#endif // _FILEACTIONSCRIPT_H_
