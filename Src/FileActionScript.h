// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  FileActionScript.h
 *
 * @brief Declaration file for FileActionScript and related classes
 */
#pragma once

#include <vector>
#include <memory>

class ShellFileOperations;

/** 
 * @brief Return values for FileActionScript functions.
 */
enum CreateScriptReturn
{
	SCRIPT_FAIL = 0,    /**< The script failed. */
	SCRIPT_SUCCESS,     /**< The script succeeded. */
	SCRIPT_USERCANCEL,  /**< The user cancelled the action. */
	SCRIPT_USERSKIP,    /**< The user wanted to skip one or more items. */
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
	/**
	 * @brief the type of the action.
	 * These action types are low level actions for filesystem, not
	 * higher level actions user is doing (e.g. synchronizing).
	 */
	enum ACT_TYPE
	{ 
		ACT_NONE = 0,
		ACT_COPY = 1, /**< Copy the item(s). */
		ACT_MOVE,     /**< Move the item(s). */
		ACT_DEL,      /**< Delete the item(s). */
		ACT_RENAME,   /**< Rename the item(s). */
	};

	String src; /**< Source for action */
	String dest; /**< Destination action */
	bool dirflag = false; /**< Is it directory? (`true` means directory) */
	ACT_TYPE atype = ACT_NONE; /**< Action's type */
};

/** 
 * @brief FileActionItem presents one filesystem action from GUI perspective.
 */
struct FileActionItem : public FileAction
{
	/**
	 * @brief UI result of the action done.
	 * These values present the change in the UI happening, due to lower
	 * level actions. E.g. delete operation may cause left item to be removed
	 * from the list.
	 */
	enum UI_RESULT
	{
		UI_COPY = 1,       /**< Copy items. */
		UI_COPY_DIFFITEMS, /**< Copy diff items. */
		UI_MOVE,           /**< Move items. */
		UI_DEL,            /**< Remove left item. */
		UI_DONT_CARE,      /**< Ignore the GUI change. */
		UI_RENAME          /**< Rename item. */
	};

	/**
	 * Optional context value for the item.
	 * This is an arbitrary value that can be used to associate the item with
	 * other items. This can be e.g. indext of the item in the GUI.
	 */
	int context = 0;
	int UIResult = 0; /**< Resulting UI action */
	int UIOrigin = 0; /**< Original UI-side */
	int UIDestination = 0; /**< Destination UI-side */
};

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

	void SetParentWindow(HWND hWnd);
	void UseRecycleBin(bool bUseRecycleBin);
	bool Run();

	// Manipulate the FileActionList
	size_t GetActionItemCount() const;

	/**
	 * Add new item to the action list.
	 * @param [in] item Item to add to the list.
	 */
	void AddActionItem(FileActionItem & item) { m_actions.push_back(item); }

	FileActionItem RemoveTailActionItem();
	void RemoveDuplicates();

	/**
	 * Get first action item in the list.
	 * @return First item in the list.
	 */
	FileActionItem GetHeadActionItem() const { return m_actions[0]; }

	bool IsCanceled() const { return m_bCanceled; }

	String m_destBase; /**< Base destination path for some operations */

protected:
	int CreateOperationsScripts();
	bool RunOp(ShellFileOperations *oplist, bool & userCancelled);

private:
	std::vector<FileActionItem> m_actions; /**< List of all actions for this script. */
	std::unique_ptr<ShellFileOperations> m_pCopyOperations; /**< Copy operations. */
	bool m_bHasCopyOperations; /**< flag if we've put anything into m_pCopyOperations */
	std::unique_ptr<ShellFileOperations> m_pMoveOperations; /**< Move operations. */
	bool m_bHasMoveOperations; /**< flag if we've put anything into m_pMoveOperations */
	std::unique_ptr<ShellFileOperations> m_pRenameOperations; /**< Rename operations. */
	bool m_bHasRenameOperations; /**< flag if we've put anything into m_pRenameOperations */
	std::unique_ptr<ShellFileOperations> m_pDelOperations; /**< Delete operations. */
	bool m_bHasDelOperations; /**< flag if we've put anything into m_pDelOperations */
	bool m_bUseRecycleBin; /**< Use recycle bin for script actions? */
	HWND m_hParentWindow; /**< Parent window for showing messages */
	bool m_bCanceled;
};

/**
 * @brief Set parent window used for showing MessageBoxes.
 * @param [in] hWnd Handle to parent window.
 */
inline void FileActionScript::SetParentWindow(HWND hWnd)
{
	m_hParentWindow = hWnd;
}

/**
 * @brief Does user want to move deleted files to Recycle Bin?
 * @param [in] bUseRecycleBin If `true` deleted files are moved to Recycle Bin.
 */
inline void FileActionScript::UseRecycleBin(bool bUseRecycleBin)
{
	m_bUseRecycleBin = bUseRecycleBin;
}

/**
 * @brief Return amount of actions (copy, move, etc) in script.
 * @return Amount of actions.
 */
inline size_t FileActionScript::GetActionItemCount() const
{
	return m_actions.size();
}

