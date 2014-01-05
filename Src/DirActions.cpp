/////////////////////////////////////////////////////////////////////////////
//    see Merge.cpp for license (GPLv2+) statement
//
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file DirActions.cpp
 *
 *  @brief Implementation of methods of CDirView that copy/move/delete files
 */
// ID line follows -- this is updated by SVN
// $Id: DirActions.cpp 6572 2009-03-18 18:51:20Z kimmov $

// It would be nice to make this independent of the UI (CDirView)
// but it needs access to the list of selected items.
// One idea would be to provide an iterator over them.
//

#include "stdafx.h"
#include "Merge.h"
#include "UnicodeString.h"
#include "DirView.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "coretools.h"
#include "paths.h"
#include "7zCommon.h"
#include "ShellFileOperations.h"
#include "OptionsDef.h"
#include "WaitStatusCursor.h"
#include "DiffItem.h"
#include "FileActionScript.h"
#include "LoadSaveCodepageDlg.h"
#include "IntToIntMap.h"
#include "FileOrFolderSelect.h"
#include "ConfirmFolderCopyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Flags for checking compare items:
// Don't check for existence
#define ALLOW_DONT_CARE 0
// Allow it being folder
#define ALLOW_FOLDER 1
// Allow it being file
#define ALLOW_FILE 2
// Allow all types (currently file and folder)
#define ALLOW_ALL (ALLOW_FOLDER | ALLOW_FILE)

static bool ConfirmCopy(int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide);
static bool ConfirmMove(int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide);
static bool ConfirmDialog(const String &caption, const String &question,
		int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide);

static bool CheckPathsExist(const String &orig, const String& dest, int allowOrig,
		int allowDest, String & failedPath);


/**
 * @brief Ask user a confirmation for copying item(s).
 * Shows a confirmation dialog for copy operation. Depending ont item count
 * dialog shows full paths to items (single item) or base paths of compare
 * (multiple items).
 * @param [in] origin Origin side of the item(s).
 * @param [in] destination Destination side of the item(s).
 * @param [in] count Number of items.
 * @param [in] src Source path.
 * @param [in] dest Destination path.
 * @param [in] destIsSide Is destination path either of compare sides?
 * @return true if copy should proceed, false if aborted.
 */
static bool ConfirmCopy(int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide)
{
	String caption = LoadResString(IDS_CONFIRM_COPY_CAPTION);
	UINT id = count == 1 ? IDS_CONFIRM_SINGLE_COPY : IDS_CONFIRM_MULTIPLE_COPY;
	String strQuestion = string_format(theApp.LoadString(id).c_str(), count);

	bool ret = ConfirmDialog(caption, strQuestion, origin,
		destination, count,	src, dest, destIsSide);
	return ret;
}

/**
 * @brief Ask user a confirmation for moving item(s).
 * Shows a confirmation dialog for move operation. Depending ont item count
 * dialog shows full paths to items (single item) or base paths of compare
 * (multiple items).
 * @param [in] origin Origin side of the item(s).
 * @param [in] destination Destination side of the item(s).
 * @param [in] count Number of items.
 * @param [in] src Source path.
 * @param [in] dest Destination path.
 * @param [in] destIsSide Is destination path either of compare sides?
 * @return true if copy should proceed, false if aborted.
 */
static bool ConfirmMove(int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide)
{
	String caption = LoadResString(IDS_CONFIRM_MOVE_CAPTION);
	UINT id = count == 1 ? IDS_CONFIRM_SINGLE_MOVE : IDS_CONFIRM_MULTIPLE_MOVE;
	String strQuestion = string_format(theApp.LoadString(id).c_str(), count);

	bool ret = ConfirmDialog(caption, strQuestion, origin,
		destination, count,	src, dest, destIsSide);
	return ret;
}

/**
 * @brief Show a (copy/move) confirmation dialog.
 * @param [in] caption Caption of the dialog.
 * @param [in] question Guestion to ask from user.
 * @param [in] origin Origin side of the item(s).
 * @param [in] destination Destination side of the item(s).
 * @param [in] count Number of items.
 * @param [in] src Source path.
 * @param [in] dest Destination path.
 * @param [in] destIsSide Is destination path either of compare sides?
 * @return true if copy should proceed, false if aborted.
 */
static bool ConfirmDialog(const String &caption, const String &question,
		int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide)
{
	ConfirmFolderCopyDlg dlg;
	String sOrig;
	String sDest;
	
	dlg.m_caption = caption.c_str();
	
	if (origin == FileActionItem::UI_LEFT)
		sOrig = theApp.LoadString(IDS_FROM_LEFT);
	else
		sOrig = theApp.LoadString(IDS_FROM_RIGHT);

	if (destIsSide)
	{
		// Copy to left / right
		if (destination == FileActionItem::UI_LEFT)
			sDest = theApp.LoadString(IDS_TO_LEFT);
		else
			sDest = theApp.LoadString(IDS_TO_RIGHT);
	}
	else
	{
		// Copy left/right to..
		sDest = theApp.LoadString(IDS_TO);
	}

	String strSrc(src);
	if (paths_DoesPathExist(src) == IS_EXISTING_DIR)
	{
		if (!paths_EndsWithSlash(src))
			strSrc += _T("\\");
	}
	String strDest(dest);
	if (paths_DoesPathExist(dest) == IS_EXISTING_DIR)
	{
		if (!paths_EndsWithSlash(dest))
			strDest += _T("\\");
	}

	dlg.m_question = question.c_str();
	dlg.m_fromText = sOrig.c_str();
	dlg.m_toText = sDest.c_str();
	dlg.m_fromPath = strSrc.c_str();
	dlg.m_toPath = strDest.c_str();

	return (dlg.DoModal()==IDYES);
}

/**
 * @brief Checks if paths (to be operated) exists.
 * This function checks if one or two given paths exists and are files and
 * or folders as specified by parameters.
 * @param [in] orig Orig side path.
 * @param [in] dest Dest side path.
 * @param [in] allowOrig What kind of paths allowed for orig side.
 * @param [in] allowDest What kind of paths allowed for dest side.
 * @param [out] failedPath If path failed, return it here.
 * @return true if path exists and is of allowed type.
 */
static bool CheckPathsExist(const String& orig, const String& dest, int allowOrig,
		int allowDest, String & failedPath)
{
	// Either of the paths must be checked!
	ASSERT(allowOrig != ALLOW_DONT_CARE || allowDest != ALLOW_DONT_CARE);
	bool origSuccess = false;
	bool destSuccess = false;

	if (allowOrig != ALLOW_DONT_CARE)
	{
		// Check that source exists
		PATH_EXISTENCE exists = paths_DoesPathExist(orig);
		if (((allowOrig & ALLOW_FOLDER) != 0) && exists == IS_EXISTING_DIR)
			origSuccess = true;
		if (((allowOrig & ALLOW_FILE) != 0) && exists == IS_EXISTING_FILE)
			origSuccess = true;

		// Original item failed, don't bother checking dest item
		if (origSuccess == false)
		{
			failedPath = orig;
			return false;
		}
	}

	if (allowDest != ALLOW_DONT_CARE)
	{
		// Check that destination exists
		PATH_EXISTENCE exists = paths_DoesPathExist(dest);
		if (((allowDest & ALLOW_FOLDER) != 0) && exists == IS_EXISTING_DIR)
			destSuccess = true;
		if (((allowDest & ALLOW_FILE) != 0) && exists == IS_EXISTING_FILE)
			destSuccess = true;

		if (destSuccess == false)
		{
			failedPath = dest;
			return false;
		}
	}
	return true;
}

/**
 * @brief Format warning message about invalid folder compare contents.
 * @param [in] failedPath Path that failed (didn't exist).
 */
void CDirView::WarnContentsChanged(const String & failedPath)
{
	ResMsgBox1(IDS_DIRCMP_NOTSYNC, failedPath.c_str(), MB_ICONWARNING);
}

/// Prompt & copy item from right to left, if legal
void CDirView::DoCopyRightToLeft()
{
	WaitStatusCursor waitstatus(IDS_STATUS_COPYFILES);

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode.diffcode != 0 && IsItemCopyableToLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			
			// We must check that paths still exists
			String failpath;
			bool succeed = CheckPathsExist(srFile, slFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			String sDest(slFile);

			if (GetDocument()->GetRecursive())
			{
				// If destination sides's relative path is empty it means we
				// are copying unique items and need to get the real relative
				// path from original side.
				if (di.diffFileInfo[0].path.get().empty())
				{
					sDest = GetDocument()->GetLeftBasePath();
					sDest = paths_ConcatPath(sDest, di.diffFileInfo[1].GetFile());
				}
			}

			act.src = srFile;
			act.dest = sDest;
			act.context = sel;
			act.dirflag = di.diffcode.isDirectory();
			act.atype = actType;
			act.UIResult = FileActionItem::UI_SYNC;
			act.UIOrigin = FileActionItem::UI_RIGHT;
			act.UIDestination = FileActionItem::UI_LEFT;
			actionScript.AddActionItem(act);
		}
		++selCount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

/// Prompt & copy item from left to right, if legal
void CDirView::DoCopyLeftToRight()
{
	WaitStatusCursor waitstatus(IDS_STATUS_COPYFILES);

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode.diffcode != 0 && IsItemCopyableToRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must first check that paths still exists
			String failpath;
			bool succeed = CheckPathsExist(slFile, srFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			String sDest(srFile);

			if (GetDocument()->GetRecursive())
			{
				// If destination sides's relative path is empty it means we
				// are copying unique items and need to get the real relative
				// path from original side.
				if (di.diffFileInfo[1].path.get().empty())
				{
					sDest = GetDocument()->GetRightBasePath();
					sDest = paths_ConcatPath(sDest, di.diffFileInfo[0].GetFile());
				}
			}

			act.src = slFile;
			act.dest = sDest;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_SYNC;
			act.UIOrigin = FileActionItem::UI_LEFT;
			act.UIDestination = FileActionItem::UI_RIGHT;
			actionScript.AddActionItem(act);
		}
		++selCount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

/// Prompt & delete left, if legal
void CDirView::DoDelLeft()
{
	WaitStatusCursor waitstatus(IDS_STATUS_DELETEFILES);

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel=-1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode.diffcode != 0 && IsItemDeletableOnLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must check that path still exists
			String failpath;
			bool succeed = CheckPathsExist(slFile, srFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			act.src = slFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DEL_LEFT;
			actionScript.AddActionItem(act);
		}
		++selCount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}
/// Prompt & delete right, if legal
void CDirView::DoDelRight()
{
	WaitStatusCursor waitstatus(IDS_STATUS_DELETEFILES);

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode.diffcode != 0 && IsItemDeletableOnRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must first check that path still exists
			String failpath;
			bool succeed = CheckPathsExist(srFile, slFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			act.src = srFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DEL_RIGHT;
			actionScript.AddActionItem(act);
		}
		++selCount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

/**
 * @brief Prompt & delete both, if legal.
 */
void CDirView::DoDelBoth()
{
	WaitStatusCursor waitstatus(IDS_STATUS_DELETEFILES);

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode.diffcode != 0 && IsItemDeletableOnBoth(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must first check that paths still exists
			String failpath;
			bool succeed = CheckPathsExist(srFile, slFile, ALLOW_ALL,
					ALLOW_ALL, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			act.src = srFile;
			act.dest = slFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DEL_BOTH;
			actionScript.AddActionItem(act);
		}
		++selCount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

/**
 * @brief Delete left, right or both items.
 * @note Usually we don't need to check for read-only in this level of code.
 *   Usually we can disable handling read-only items/sides by disabling GUI
 *   element. But in this case the GUI element effects to both sides and can
 *   be selected when another side is read-only.
 */
void CDirView::DoDelAll()
{
	WaitStatusCursor waitstatus(IDS_STATUS_DELETEFILES);

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode.diffcode != 0)
		{
			GetItemFileNames(sel, slFile, srFile);

			int leftFlags = ALLOW_DONT_CARE;
			int rightFlags = ALLOW_DONT_CARE;
			bool leftRO = GetDocument()->GetReadOnly(true);
			bool rightRO = GetDocument()->GetReadOnly(false);
			FileActionItem act;
			if (IsItemDeletableOnBoth(di) && !leftRO && !rightRO)
			{
				leftFlags = ALLOW_ALL;
				rightFlags = ALLOW_ALL;
				act.src = srFile;
				act.dest = slFile;
				act.UIResult = FileActionItem::UI_DEL_BOTH;
			}
			else if (IsItemDeletableOnLeft(di) && !leftRO)
			{
				leftFlags = ALLOW_ALL;
				act.src = slFile;
				act.UIResult = FileActionItem::UI_DEL_LEFT;
			}
			else if (IsItemDeletableOnRight(di) && !rightRO)
			{
				rightFlags = ALLOW_ALL;
				act.src = srFile;
				act.UIResult = FileActionItem::UI_DEL_RIGHT;
			}

			// Check one of sides is actually being added to removal list
			if (leftFlags != ALLOW_DONT_CARE || rightFlags != ALLOW_DONT_CARE)
			{
				// We must first check that paths still exists
				String failpath;
				bool succeed = CheckPathsExist(slFile, srFile, leftFlags,
						rightFlags, failpath);
				if (succeed == false)
				{
					WarnContentsChanged(failpath);
					return;
				}

				act.dirflag = di.diffcode.isDirectory();
				act.context = sel;
				act.atype = actType;
				actionScript.AddActionItem(act);
				++selCount;
			}
		}
	}

	if (selCount > 0)
	{
		// Now we prompt, and execute actions
		ConfirmAndPerformActions(actionScript, selCount);
	}
}

/**
 * @brief Copy selected left-side files to user-specified directory
 *
 * When copying files from recursive compare file subdirectory is also
 * read so directory structure is preserved.
 */
void CDirView::DoCopyLeftTo()
{
	String destPath;
	String startPath(m_lastCopyFolder);

	if (!SelectFolder(destPath, startPath.c_str(), IDS_SELECT_DEST_LEFT))
		return;

	m_lastCopyFolder = destPath;
	WaitStatusCursor waitstatus(IDS_STATUS_COPYFILES);

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode.diffcode != 0 && IsItemCopyableToOnLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must check that path still exists
			String failpath;
			bool succeed = CheckPathsExist(slFile, srFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			String sFullDest(destPath);
			sFullDest += _T("\\");

			actionScript.m_destBase = sFullDest;

			if (GetDocument()->GetRecursive())
			{
				if (!di.diffFileInfo[0].path.get().empty())
				{
					sFullDest += di.diffFileInfo[0].path;
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.diffFileInfo[0].filename;
			act.dest = sFullDest;

			act.src = slFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DONT_CARE;
			act.UIOrigin = FileActionItem::UI_LEFT;
			actionScript.AddActionItem(act);
			++selCount;
		}
	}
	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

/**
 * @brief Copy selected righ-side files to user-specified directory
 *
 * When copying files from recursive compare file subdirectory is also
 * read so directory structure is preserved.
 */
void CDirView::DoCopyRightTo()
{
	String destPath;
	String startPath(m_lastCopyFolder);

	if (!SelectFolder(destPath, startPath.c_str(), IDS_SELECT_DEST_RIGHT))
		return;

	m_lastCopyFolder = destPath;
	WaitStatusCursor waitstatus(IDS_STATUS_COPYFILES);

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode.diffcode != 0 && IsItemCopyableToOnRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must check that path still exists
			String failpath;
			bool succeed = CheckPathsExist(srFile, slFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			String sFullDest(destPath);
			sFullDest += _T("\\");

			actionScript.m_destBase = sFullDest;

			if (GetDocument()->GetRecursive())
			{
				if (!di.diffFileInfo[1].path.get().empty())
				{
					sFullDest += di.diffFileInfo[1].path;
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.diffFileInfo[1].filename;
			act.dest = sFullDest;

			act.src = srFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DONT_CARE;
			act.UIOrigin = FileActionItem::UI_RIGHT;
			actionScript.AddActionItem(act);
			++selCount;
		}
	}
	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

/**
 * @brief Move selected left-side files to user-specified directory
 *
 * When moving files from recursive compare file subdirectory is also
 * read so directory structure is preserved.
 */
void CDirView::DoMoveLeftTo()
{
	String destPath;
	String startPath(m_lastCopyFolder);

	if (!SelectFolder(destPath, startPath.c_str(), IDS_SELECT_DEST_LEFT))
		return;

	m_lastCopyFolder = destPath;
	WaitStatusCursor waitstatus(IDS_STATUS_MOVEFILES);

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_MOVE;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode.diffcode != 0 && IsItemCopyableToOnLeft(di) && IsItemDeletableOnLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must check that path still exists
			String failpath;
			bool succeed = CheckPathsExist(slFile, srFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			String sFullDest(destPath);
			sFullDest += _T("\\");
			actionScript.m_destBase = sFullDest;
			if (GetDocument()->GetRecursive())
			{
				if (!di.diffFileInfo[0].path.get().empty())
				{
					sFullDest += di.diffFileInfo[0].path;
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.diffFileInfo[0].filename;
			act.dest = sFullDest;

			act.src = slFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIOrigin = FileActionItem::UI_LEFT;
			act.UIResult = FileActionItem::UI_DEL_LEFT;
			actionScript.AddActionItem(act);
			++selCount;
		}
	}
	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

/**
 * @brief Move selected right-side files to user-specified directory
 *
 * When moving files from recursive compare file subdirectory is also
 * read so directory structure is preserved.
 */
void CDirView::DoMoveRightTo()
{
	String destPath;
	String startPath(m_lastCopyFolder);

	if (!SelectFolder(destPath, startPath.c_str(), IDS_SELECT_DEST_RIGHT))
		return;

	m_lastCopyFolder = destPath;
	WaitStatusCursor waitstatus(IDS_STATUS_MOVEFILES);

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_MOVE;
	int selCount = 0;
	int sel = -1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode.diffcode != 0 && IsItemCopyableToOnRight(di) && IsItemDeletableOnRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);

			// We must check that path still exists
			String failpath;
			bool succeed = CheckPathsExist(srFile, slFile, ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == false)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			String sFullDest(destPath);
			sFullDest += _T("\\");
			actionScript.m_destBase = sFullDest;
			if (GetDocument()->GetRecursive())
			{
				if (!di.diffFileInfo[1].path.get().empty())
				{
					sFullDest += di.diffFileInfo[1].path;
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.diffFileInfo[1].filename;
			act.dest = sFullDest;

			act.src = srFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIOrigin = FileActionItem::UI_RIGHT;
			act.UIResult = FileActionItem::UI_DEL_RIGHT;
			actionScript.AddActionItem(act);
			++selCount;
		}
	}
	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionScript, selCount);
}

// Confirm with user, then perform the action list
void CDirView::ConfirmAndPerformActions(FileActionScript & actionList, int selCount)
{
	if (selCount == 0) // Not sure it is possible to get right-click menu without
		return;    // any selected items, but may as well be safe

	ASSERT(actionList.GetActionItemCount()>0); // Or else the update handler got it wrong

	// Set parent window so modality is correct and correct window gets focus
	// after dialogs.
	actionList.SetParentWindow(this->GetSafeHwnd());
	
	if (!ConfirmActionList(actionList, selCount))
		return;

	PerformActionList(actionList);
}

/**
 * @brief Confirm actions with user as appropriate
 * (type, whether single or multiple).
 */
bool CDirView::ConfirmActionList(const FileActionScript & actionList, int selCount)
{
	// TODO: We need better confirmation for file actions.
	// Maybe we should show a list of files with actions done..
	FileActionItem item = actionList.GetHeadActionItem();

	bool bDestIsSide = true;

	// special handling for the single item case, because it is probably the most common,
	// and we can give the user exact details easily for it
	switch(item.atype)
	{
	case FileAction::ACT_COPY:
		if (item.UIResult == FileActionItem::UI_DONT_CARE)
			bDestIsSide = false;

		if (actionList.GetActionItemCount() == 1)
		{
			if (!ConfirmCopy(item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), item.src, item.dest,
				bDestIsSide))
			{
				return false;
			}
		}
		else
		{
			String src;
			String dst;

			if (item.UIOrigin == FileActionItem::UI_LEFT)
				src = GetDocument()->GetLeftBasePath();
			else
				src = GetDocument()->GetRightBasePath();

			if (bDestIsSide)
			{
				if (item.UIDestination == FileActionItem::UI_LEFT)
					dst = GetDocument()->GetLeftBasePath();
				else
					dst = GetDocument()->GetRightBasePath();
			}
			else
			{
				if (!actionList.m_destBase.empty())
					dst = actionList.m_destBase;
				else
					dst = item.dest;
			}

			if (!ConfirmCopy(item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), src, dst, bDestIsSide))
			{
				return false;
			}
		}
		break;
		
	case FileAction::ACT_DEL:
		break;

	case FileAction::ACT_MOVE:
		bDestIsSide = false;
		if (actionList.GetActionItemCount() == 1)
		{
			if (!ConfirmMove(item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), item.src, item.dest,
				bDestIsSide))
			{
				return false;
			}
		}
		else
		{
			String src;
			String dst;

			if (item.UIOrigin == FileActionItem::UI_LEFT)
				src = GetDocument()->GetLeftBasePath();
			else
				src = GetDocument()->GetRightBasePath();

			if (!actionList.m_destBase.empty())
				dst = actionList.m_destBase;
			else
				dst = item.dest;

			if (!ConfirmMove(item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), src, dst, bDestIsSide))
			{
				return false;
			}
		}
		break;

	// Invalid operation
	default: 
		LogErrorString(_T("Unknown fileoperation in CDirView::ConfirmActionList()"));
		_RPTF0(_CRT_ERROR, "Unknown fileoperation in CDirView::ConfirmActionList()");
		break;
	}
	return true;
}

/**
 * @brief Perform an array of actions
 * @note There can be only COPY or DELETE actions, not both!
 * @sa CMainFrame::SaveToVersionControl()
 * @sa CMainFrame::SyncFilesToVCS()
 */
void CDirView::PerformActionList(FileActionScript & actionScript)
{
	// Reset suppressing VSS dialog for multiple files.
	// Set in CMainFrame::SaveToVersionControl().
	GetMainFrame()->m_CheckOutMulti = false;
	GetMainFrame()->m_bVssSuppressPathCheck = false;

	// Check option and enable putting deleted items to Recycle Bin
	if (GetOptionsMgr()->GetBool(OPT_USE_RECYCLE_BIN))
		actionScript.UseRecycleBin(true);
	else
		actionScript.UseRecycleBin(false);

	actionScript.SetParentWindow(this->GetSafeHwnd());

	theApp.AddOperation();
	if (actionScript.Run())
		UpdateAfterFileScript(actionScript);
	theApp.RemoveOperation();
}

/**
 * @brief Update results after running FileActionScript.
 * This functions is called after script is finished to update
 * results (including UI).
 * @param [in] actionlist Script that was run.
 */
void CDirView::UpdateAfterFileScript(FileActionScript & actionList)
{
	bool bItemsRemoved = false;
	int curSel = GetFirstSelectedInd();
	CDirDoc *pDoc = GetDocument();
	while (actionList.GetActionItemCount()>0)
	{
		// Start handling from tail of list, so removing items
		// doesn't invalidate our item indexes.
		FileActionItem act = actionList.RemoveTailActionItem();
		UINT_PTR diffpos = GetItemKey(act.context);
		DIFFCODE diffcode = pDoc->GetDiffByKey(diffpos).diffcode;
		bool bUpdateLeft = false;
		bool bUpdateRight = false;

		// Synchronized items may need VCS operations
		if (act.UIResult == FileActionItem::UI_SYNC)
		{
			if (GetMainFrame()->m_bCheckinVCS)
				GetMainFrame()->CheckinToClearCase(act.dest);
		}

		// Update UI
		switch (act.UIResult)
		{
		case FileActionItem::UI_SYNC:
			bUpdateLeft = true;
			bUpdateRight = true;
			break;
		
		case FileActionItem::UI_DESYNC:
			// Cannot happen yet since we have only "simple" operations
			break;

		case FileActionItem::UI_DEL_LEFT:
			if (diffcode.isSideFirstOnly())
			{
				if (m_bTreeMode)
					CollapseSubdir(act.context);
				m_pList->DeleteItem(act.context);
				bItemsRemoved = true;
			}
			else
			{
				bUpdateLeft = true;
			}
			break;

		case FileActionItem::UI_DEL_RIGHT:
			if (diffcode.isSideSecondOnly())
			{
				if (m_bTreeMode)
					CollapseSubdir(act.context);
				m_pList->DeleteItem(act.context);
				bItemsRemoved = true;
			}
			else
			{
				bUpdateRight = true;
			}
			break;

		case FileActionItem::UI_DEL_BOTH:
			if (m_bTreeMode)
				CollapseSubdir(act.context);
			m_pList->DeleteItem(act.context);
			bItemsRemoved = true;
			break;
		}

		// Update doc (difflist)
		pDoc->UpdateDiffAfterOperation(act, diffpos);

		if (bUpdateLeft || bUpdateRight)
		{
			pDoc->UpdateStatusFromDisk(diffpos, bUpdateLeft, bUpdateRight);
			UpdateDiffItemStatus(act.context);
		}
	}
	
	// Make sure selection is at sensible place if all selected items
	// were removed.
	if (bItemsRemoved == true)
	{
		UINT selected = GetSelectedCount();
		if (selected == 0)
		{
			if (curSel < 1)
				++curSel;
			MoveFocus(0, curSel - 1, selected);
		}
	}
}

/// Get directories of first selected item
bool CDirView::GetSelectedDirNames(String& strLeft, String& strRight) const
{
	bool bResult = GetSelectedFileNames(strLeft, strRight);

	if (bResult)
	{
		strLeft = paths_GetPathOnly(strLeft);
		strRight = paths_GetPathOnly(strRight);
	}
	return bResult;
}

/// is it possible to copy item to left ?
bool CDirView::IsItemCopyableToLeft(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// can't copy same items
	if (di.diffcode.isResultSame()) return false;
	// impossible if only on left
	if (di.diffcode.isSideFirstOnly()) return false;

	// everything else can be copied to left
	return true;
}
/// is it possible to copy item to right ?
bool CDirView::IsItemCopyableToRight(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// can't copy same items
	if (di.diffcode.isResultSame()) return false;
	// impossible if only on right
	if (di.diffcode.isSideSecondOnly()) return false;

	// everything else can be copied to right
	return true;
}
/// is it possible to delete left item ?
bool CDirView::IsItemDeletableOnLeft(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// impossible if only on right
	if (di.diffcode.isSideSecondOnly()) return false;
	// everything else can be deleted on left
	return true;
}
/// is it possible to delete right item ?
bool CDirView::IsItemDeletableOnRight(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// impossible if only on right
	if (di.diffcode.isSideFirstOnly()) return false;

	// everything else can be deleted on right
	return true;
}
/// is it possible to delete both items ?
bool CDirView::IsItemDeletableOnBoth(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// impossible if only on right or left
	if (di.diffcode.isSideFirstOnly() || di.diffcode.isSideSecondOnly()) return false;

	// everything else can be deleted on both
	return true;
}

/**
 * @brief Determine if item can be opened.
 * Basically we only disable opening unique files at the moment.
 * Unique folders can be opened since we ask for creating matching folder
 * to another side.
 * @param [in] di DIFFITEM for item to check.
 * @return true if the item can be opened, false otherwise.
 */
bool CDirView::IsItemOpenable(const DIFFITEM & di) const
{
	if (m_bTreeMode && GetDocument()->GetRecursive())
	{
		if (di.diffcode.isDirectory() ||
			(!di.diffcode.isExistsFirst() || !di.diffcode.isExistsSecond())) /* FIXME: 3-pane */
		{
			return false;
		}
	}
	else 
	{
		if (!di.diffcode.isDirectory() &&
			(!di.diffcode.isExistsFirst() || !di.diffcode.isExistsSecond())) /* FIXME: 3-pane */
		{
			return false;
		}
	}
	return true;
}
/// is it possible to compare these two items?
bool CDirView::AreItemsOpenable(SELECTIONTYPE selectionType, const DIFFITEM & di1, const DIFFITEM & di2) const
{
	String sLeftBasePath = GetDocument()->GetBasePath(0);
	String sRightBasePath = GetDocument()->GetBasePath(1);

	// Must be both directory or neither
	if (di1.diffcode.isDirectory() != di2.diffcode.isDirectory()) return false;

	switch (selectionType)
	{
	case SELECTIONTYPE_NORMAL:
		// Must be on different sides, or one on one side & one on both
		if (di1.diffcode.isSideFirstOnly() && (di2.diffcode.isSideSecondOnly() ||
			di2.diffcode.isSideBoth()))
			return true;
		if (di1.diffcode.isSideSecondOnly() && (di2.diffcode.isSideFirstOnly() ||
			di2.diffcode.isSideBoth()))
			return true;
		if (di1.diffcode.isSideBoth() && (di2.diffcode.isSideFirstOnly() ||
			di2.diffcode.isSideSecondOnly()))
			return true;
		break;
	case SELECTIONTYPE_LEFT1LEFT2:
		if (di1.diffcode.isExists(0) && di2.diffcode.isExists(0))
			return true;
		break;
	case SELECTIONTYPE_RIGHT1RIGHT2:
		if (di1.diffcode.isExists(1) && di2.diffcode.isExists(1))
			return true;
		break;
	case SELECTIONTYPE_LEFT1RIGHT2:
		if (di1.diffcode.isExists(0) && di2.diffcode.isExists(1))
			return true;
		break;
	case SELECTIONTYPE_LEFT2RIGHT1:
		if (di1.diffcode.isExists(1) && di2.diffcode.isExists(0))
			return true;
		break;
	}

	// Allow to compare items if left & right path refer to same directory
	// (which means there is effectively two files involved). No need to check
	// side flags. If files weren't on both sides, we'd have no DIFFITEMs.
	if (string_compare_nocase(sLeftBasePath, sRightBasePath) == 0)
		return true;

	return false;
}
/// is it possible to compare these three items?
bool CDirView::AreItemsOpenable(const DIFFITEM & di1, const DIFFITEM & di2, const DIFFITEM & di3) const
{
	String sLeftBasePath = GetDocument()->GetBasePath(0);
	String sMiddleBasePath = GetDocument()->GetBasePath(1);
	String sRightBasePath = GetDocument()->GetBasePath(2);
	String sLeftPath1 = paths_ConcatPath(di1.getFilepath(0, sLeftBasePath), di1.diffFileInfo[0].filename);
	String sLeftPath2 = paths_ConcatPath(di2.getFilepath(0, sLeftBasePath), di2.diffFileInfo[0].filename);
	String sLeftPath3 = paths_ConcatPath(di3.getFilepath(0, sLeftBasePath), di3.diffFileInfo[0].filename);
	String sMiddlePath1 = paths_ConcatPath(di1.getFilepath(1, sMiddleBasePath), di1.diffFileInfo[1].filename);
	String sMiddlePath2 = paths_ConcatPath(di2.getFilepath(1, sMiddleBasePath), di2.diffFileInfo[1].filename);
	String sMiddlePath3 = paths_ConcatPath(di3.getFilepath(1, sMiddleBasePath), di3.diffFileInfo[1].filename);
	String sRightPath1 = paths_ConcatPath(di1.getFilepath(2, sRightBasePath), di1.diffFileInfo[2].filename);
	String sRightPath2 = paths_ConcatPath(di2.getFilepath(2, sRightBasePath), di2.diffFileInfo[2].filename);
	String sRightPath3 = paths_ConcatPath(di3.getFilepath(2, sRightBasePath), di3.diffFileInfo[2].filename);
	// Must not be binary (unless archive)
	if
	(
		(di1.diffcode.isBin() || di2.diffcode.isBin() || di3.diffcode.isBin())
	&&!	(
			HasZipSupport()
		&&	(sLeftPath1.empty() || ArchiveGuessFormat(sLeftPath1))
		&&	(sMiddlePath1.empty() || ArchiveGuessFormat(sMiddlePath1))
		&&	(sLeftPath2.empty() || ArchiveGuessFormat(sLeftPath2))
		&&	(sMiddlePath2.empty() || ArchiveGuessFormat(sMiddlePath2))
		&&	(sLeftPath2.empty() || ArchiveGuessFormat(sLeftPath2))
		&&	(sMiddlePath2.empty() || ArchiveGuessFormat(sMiddlePath2)) /* FIXME: */
		)
	)
	{
		return false;
	}

	// Must be both directory or neither
	if (di1.diffcode.isDirectory() != di2.diffcode.isDirectory() && di1.diffcode.isDirectory() != di3.diffcode.isDirectory()) return false;

	// Must be on different sides, or one on one side & one on both
	if (di1.diffcode.isExists(0) && di2.diffcode.isExists(1) && di3.diffcode.isExists(2))
		return true;
	if (di1.diffcode.isExists(0) && di2.diffcode.isExists(2) && di3.diffcode.isExists(1))
		return true;
	if (di1.diffcode.isExists(1) && di2.diffcode.isExists(0) && di3.diffcode.isExists(2))
		return true;
	if (di1.diffcode.isExists(1) && di2.diffcode.isExists(2) && di3.diffcode.isExists(0))
		return true;
	if (di1.diffcode.isExists(2) && di2.diffcode.isExists(0) && di3.diffcode.isExists(1))
		return true;
	if (di1.diffcode.isExists(2) && di2.diffcode.isExists(1) && di3.diffcode.isExists(0))
		return true;

	// Allow to compare items if left & right path refer to same directory
	// (which means there is effectively two files involved). No need to check
	// side flags. If files weren't on both sides, we'd have no DIFFITEMs.
	if (lstrcmpi(sLeftBasePath.c_str(), sMiddleBasePath.c_str()) == 0 && lstrcmpi(sLeftBasePath.c_str(), sRightBasePath.c_str()) == 0)
		return true;

	return false;
}
/// is it possible to open left item ?
bool CDirView::IsItemOpenableOnLeft(const DIFFITEM & di) const
{
	// impossible if only on right
	if (di.diffcode.isSideSecondOnly()) return false;

	// everything else can be opened on right
	return true;
}
/// is it possible to open right item ?
bool CDirView::IsItemOpenableOnRight(const DIFFITEM & di) const
{
	// impossible if only on left
	if (di.diffcode.isSideFirstOnly()) return false;

	// everything else can be opened on left
	return true;
}
/// is it possible to open left ... item ?
bool CDirView::IsItemOpenableOnLeftWith(const DIFFITEM & di) const
{
	return (!di.diffcode.isDirectory() && IsItemOpenableOnLeft(di));
}
/// is it possible to open with ... right item ?
bool CDirView::IsItemOpenableOnRightWith(const DIFFITEM & di) const
{
	return (!di.diffcode.isDirectory() && IsItemOpenableOnRight(di));
}
/// is it possible to copy to... left item?
bool CDirView::IsItemCopyableToOnLeft(const DIFFITEM & di) const
{
	// impossible if only on right
	if (di.diffcode.isSideSecondOnly()) return false;

	// everything else can be copied to from left
	return true;
}
/// is it possible to copy to... right item?
bool CDirView::IsItemCopyableToOnRight(const DIFFITEM & di) const
{
	// impossible if only on left
	if (di.diffcode.isSideFirstOnly()) return false;

	// everything else can be copied to from right
	return true;
}

/// get the file names on both sides for first selected item
bool CDirView::GetSelectedFileNames(String& strLeft, String& strRight) const
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
		return false;
	GetItemFileNames(sel, strLeft, strRight);
	return true;
}
/// get file name on specified side for first selected item
String CDirView::GetSelectedFileName(SIDE_TYPE stype) const
{
	String left, right;
	if (!GetSelectedFileNames(left, right)) return _T("");
	return stype==SIDE_LEFT ? left : right;
}
/**
 * @brief Get the file names on both sides for specified item.
 * @note Return empty strings if item is special item.
 */
void CDirView::GetItemFileNames(int sel, String& strLeft, String& strRight) const
{
	UINT_PTR diffpos = GetItemKey(sel);
	if (diffpos == (UINT_PTR)SPECIAL_ITEM_POS)
	{
		strLeft.erase();
		strRight.erase();
	}
	else
	{
		const DIFFITEM & di = GetDocument()->GetDiffByKey(diffpos);
		const String leftrelpath = di.diffFileInfo[0].GetFile();
		const String rightrelpath = di.diffFileInfo[1].GetFile();
		const String & leftpath = GetDocument()->GetBasePath(0);
		const String & rightpath = GetDocument()->GetBasePath(1);
		strLeft = paths_ConcatPath(leftpath, leftrelpath);
		strRight = paths_ConcatPath(rightpath, rightrelpath);
	}
}

/**
 * @brief Get the file names on both sides for specified item.
 * @note Return empty strings if item is special item.
 */
void CDirView::GetItemFileNames(int sel, PathContext * paths) const
{
	String strPath[3];
	UINT_PTR diffpos = GetItemKey(sel);
	if (diffpos == SPECIAL_ITEM_POS)
	{
		for (int nIndex = 0; nIndex < GetDocument()->m_nDirs; nIndex++)
			paths->SetPath(nIndex, _T(""));
	}
	else
	{
		const DIFFITEM & di = GetDocument()->GetDiffByKey(diffpos);
		for (int nIndex = 0; nIndex < GetDocument()->m_nDirs; nIndex++)
		{
			const String relpath = di.diffFileInfo[nIndex].GetFile();
			const String & path = GetDocument()->GetBasePath(nIndex);
			paths->SetPath(nIndex, paths_ConcatPath(path, relpath));
		}
	}
}

/**
 * @brief Open selected file with registered application.
 * Uses shell file associations to open file with registered
 * application. We first try to use "Edit" action which should
 * open file to editor, since we are more interested editing
 * files than running them (scripts).
 * @param [in] stype Side of file to open.
 */
void CDirView::DoOpen(SIDE_TYPE stype)
{
	int sel = GetSingleSelectedItem();
	if (sel == -1) return;
	String file = GetSelectedFileName(stype);
	if (file.empty()) return;
	int rtn = (int)ShellExecute(::GetDesktopWindow(), _T("edit"), file.c_str(), 0, 0, SW_SHOWNORMAL);
	if (rtn==SE_ERR_NOASSOC)
		rtn = (int)ShellExecute(::GetDesktopWindow(), _T("open"), file.c_str(), 0, 0, SW_SHOWNORMAL);
	if (rtn==SE_ERR_NOASSOC)
		DoOpenWith(stype);
}

/// Open with dialog for file on selected side
void CDirView::DoOpenWith(SIDE_TYPE stype)
{
	int sel = GetSingleSelectedItem();
	if (sel == -1) return;
	String file = GetSelectedFileName(stype);
	if (file.empty()) return;
	CString sysdir;
	if (!GetSystemDirectory(sysdir.GetBuffer(MAX_PATH), MAX_PATH)) return;
	sysdir.ReleaseBuffer();
	CString arg = (CString)_T("shell32.dll,OpenAs_RunDLL ") + file.c_str();
	ShellExecute(::GetDesktopWindow(), 0, _T("RUNDLL32.EXE"), arg, sysdir, SW_SHOWNORMAL);
}

/// Open selected file  on specified side to external editor
void CDirView::DoOpenWithEditor(SIDE_TYPE stype)
{
	int sel = GetSingleSelectedItem();
	if (sel == -1) return;
	String file = GetSelectedFileName(stype);
	if (file.empty()) return;

	GetMainFrame()->OpenFileToExternalEditor(file);
}

/**
 * @brief Apply specified setting for prediffing to all selected items
 */
void CDirView::ApplyPluginPrediffSetting(int newsetting)
{
	// Unlike other group actions, here we don't build an action list
	// to execute; we just apply this change directly
	int sel=-1;
	String slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!di.diffcode.isDirectory() && !di.diffcode.isSideFirstOnly() &&
			!di.diffcode.isSideSecondOnly())
		{
			GetItemFileNames(sel, slFile, srFile);
			String filteredFilenames = slFile + _T("|") + srFile;
			GetDocument()->SetPluginPrediffSetting(filteredFilenames, newsetting);
		}
	}
}

/**
 * @brief Mark selected items as needing for rescan.
 * @return Count of items to rescan.
 */
UINT CDirView::MarkSelectedForRescan()
{
	int sel = -1;
	int items = 0;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		// Don't try to rescan special items
		if (GetItemKey(sel) == SPECIAL_ITEM_POS)
			continue;

		const DIFFITEM &di = GetDiffItem(sel);
		GetDocument()->SetDiffStatus(0, DIFFCODE::TEXTFLAGS | DIFFCODE::SIDEFLAGS | DIFFCODE::COMPAREFLAGS, sel);		
		GetDocument()->SetDiffStatus(DIFFCODE::NEEDSCAN, DIFFCODE::SCANFLAGS, sel);
		++items;
	}
	if (items > 0)
		GetDocument()->SetMarkedRescan();
	return items;
}

/**
 * @brief Return string such as "15 of 30 Files Affected" or "30 Files Affected"
 */
static String
FormatFilesAffectedString(int nFilesAffected, int nFilesTotal)
{
	if (nFilesAffected == nFilesTotal)
		return LangFormatString1(IDS_FILES_AFFECTED_FMT, NumToStr(nFilesTotal).c_str());
	else
		return LangFormatString2(IDS_FILES_AFFECTED_FMT2, NumToStr(nFilesAffected).c_str(), NumToStr(nFilesTotal).c_str());
}

/**
 * @brief Count left & right files, and number with editable text encoding
 * @param nLeft [out]  #files on left side selected
 * @param nLeftAffected [out]  #files on left side selected which can have text encoding changed
 * @param nRight [out]  #files on right side selected
 * @param nRightAffected [out]  #files on right side selected which can have text encoding changed
 *
 * Affected files include all except unicode files
 */
void CDirView::FormatEncodingDialogDisplays(CLoadSaveCodepageDlg * dlg)
{
	IntToIntMap currentCodepages;
	int nFirst=0, nFirstAffected=0, nSecond=0, nSecondAffected=0, nThird=0, nThirdAffected=0;
	int i = -1;
	while ((i = m_pList->GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(i);
		if (di.diffcode.diffcode == 0) // Invalid value, this must be special item
			continue;
		if (di.diffcode.isDirectory())
			continue;

		if (di.diffcode.isExistsFirst())
		{
			// exists on First
			++nFirst;
			if (di.diffFileInfo[0].IsEditableEncoding())
				++nFirstAffected;
			int codepage = di.diffFileInfo[0].encoding.m_codepage;
			currentCodepages.Increment(codepage);
		}
		if (di.diffcode.isExistsSecond())
		{
			++nSecond;
			if (di.diffFileInfo[1].IsEditableEncoding())
				++nSecondAffected;
			int codepage = di.diffFileInfo[1].encoding.m_codepage;
			currentCodepages.Increment(codepage);
		}
		if (GetDocument()->m_nDirs > 2 && di.diffcode.isExistsThird())
		{
			++nThird;
			if (di.diffFileInfo[1].IsEditableEncoding())
				++nThirdAffected;
			int codepage = di.diffFileInfo[2].encoding.m_codepage;
			currentCodepages.Increment(codepage);
		}
	}

	// Format strings such as "25 of 30 Files Affected"
	String sFirstAffected = FormatFilesAffectedString(nFirstAffected, nFirst);
	String sSecondAffected = FormatFilesAffectedString(nSecondAffected, nSecond);
	String sThirdAffected = FormatFilesAffectedString(nThirdAffected, nThird);
	if (GetDocument()->m_nDirs < 3)
		dlg->SetLeftRightAffectStrings(sFirstAffected.c_str(), _T(""), sSecondAffected.c_str());
	else
		dlg->SetLeftRightAffectStrings(sFirstAffected.c_str(), sSecondAffected.c_str(), sThirdAffected.c_str());
	int codepage = currentCodepages.FindMaxKey();
	dlg->SetCodepages(codepage);
}

/**
 * @brief Display file encoding dialog to user & handle user's choices
 *
 * This handles DirView invocation, so multiple files may be affected
 */
void CDirView::DoFileEncodingDialog()
{
	CLoadSaveCodepageDlg dlg(GetDocument()->m_nDirs);
	// set up labels about what will be affected
	FormatEncodingDialogDisplays(&dlg);
	dlg.EnableSaveCodepage(false); // disallow setting a separate codepage for saving

	// Invoke dialog
	if (dlg.DoModal() != IDOK)
		return;

	int nCodepage = dlg.GetLoadCodepage();

	bool doLeft = dlg.DoesAffectLeft();
	bool doRight = dlg.DoesAffectRight();

	int i=-1;
	while ((i = m_pList->GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		DIFFITEM & di = GetDiffItemRef(i);
		if (di.diffcode.diffcode == 0) // Invalid value, this must be special item
			continue;
		if (di.diffcode.isDirectory())
			continue;

		// Does it exist on left? (ie, right or both)
		if (doLeft && di.diffcode.isExistsFirst() && di.diffFileInfo[0].IsEditableEncoding())
		{
			di.diffFileInfo[0].encoding.SetCodepage(nCodepage);
		}
		// Does it exist on right (ie, left or both)
		if (doRight && di.diffcode.isExistsSecond() && di.diffFileInfo[1].IsEditableEncoding())
		{
			di.diffFileInfo[1].encoding.SetCodepage(nCodepage);
		}
	}
	m_pList->InvalidateRect(NULL);
	m_pList->UpdateWindow();

	// TODO: We could loop through any active merge windows belonging to us
	// and see if any of their files are affected
	// but, if they've been edited, we cannot throw away the user's work?
}

void CDirView::DoUpdateFileEncodingDialog(CCmdUI* pCmdUI)
{
	bool haveSelectedItems = (m_pList->GetNextItem(-1, LVNI_SELECTED) != -1);
	pCmdUI->Enable(haveSelectedItems);
}

/**
 * @brief Rename a file without moving it to different directory.
 *
 * @param szOldFileName [in] Full path of file to rename.
 * @param szNewFileName [in] New file name (without the path).
 *
 * @return true if file was renamed successfully.
 */
bool CDirView::RenameOnSameDir(const String& szOldFileName, const String& szNewFileName)
{
	bool bSuccess = false;

	if (DOES_NOT_EXIST != paths_DoesPathExist(szOldFileName))
	{
		String sFullName;

		paths_SplitFilename(szOldFileName, &sFullName, NULL, NULL);
		sFullName = paths_ConcatPath(sFullName, szNewFileName);

		// No need to rename if new file already exist.
		if ((sFullName != szOldFileName) ||
			(DOES_NOT_EXIST == paths_DoesPathExist(sFullName)))
		{
			ShellFileOperations fileOp;
			fileOp.SetOperation(FO_RENAME, 0, this->GetSafeHwnd());
			fileOp.AddSourceAndDestination(szOldFileName, sFullName);
			bSuccess = fileOp.Run();
		}
		else
		{
			bSuccess = true;
		}
	}

	return bSuccess;
}

/**
 * @brief Rename selected item on both left and right sides.
 *
 * @param szNewItemName [in] New item name.
 *
 * @return true if at least one file was renamed successfully.
 */
bool CDirView::DoItemRename(const String& szNewItemName)
{
	PathContext paths;
	int nDirs = GetDocument()->m_nDirs;

	int nSelItem = m_pList->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(-1 != nSelItem);
	GetItemFileNames(nSelItem, &paths);

	// We must check that paths still exists
	String failpath;
	DIFFITEM &di = GetDiffItemRef(nSelItem);
	bool succeed = CheckPathsExist(paths.GetLeft(), paths.GetRight(), 
		di.diffcode.isExistsFirst() ? ALLOW_FILE | ALLOW_FOLDER : ALLOW_DONT_CARE,
		di.diffcode.isExistsSecond() ? ALLOW_FILE | ALLOW_FOLDER : ALLOW_DONT_CARE,
		failpath);
	if (succeed == false)
	{
		WarnContentsChanged(failpath);
		return false;
	}

	UINT_PTR key = GetItemKey(nSelItem);
	ASSERT(key != SPECIAL_ITEM_POS);
	ASSERT(&di == &GetDocument()->GetDiffRefByKey(key));

	bool bRename[3] = {false};
	int index;
	for (index = 0; index < nDirs; index++)
	{
		if (di.diffcode.isExists(index))
			bRename[index] = RenameOnSameDir(paths[index], szNewItemName);
	}

	int nSuccessCount = 0;
	for (index = 0; index < nDirs; index++)
		nSuccessCount += bRename[index] ? 1 : 0;

	if (nSuccessCount > 0)
	{
		for (index = 0; index < nDirs; index++)
		{
			if (bRename[index])
				di.diffFileInfo[index].filename = szNewItemName;
			else
				di.diffFileInfo[index].filename = _T("");
		}
	}

	return (bRename[0] || bRename[1] || (nDirs > 2 && bRename[2]));
}

/**
 * @brief Copy selected item left side to clipboard.
 * @param[in] flags 0:left, 1:right, 2:both
 */
void CDirView::DoCopyItemsToClipboard(int flags)
{
	CString strPaths, strPathsSepSpc;
	int sel = -1;

	strPaths.GetBufferSetLength(GetSelectedCount() * MAX_PATH);
	strPaths = _T("");
	strPathsSepSpc.GetBufferSetLength(GetSelectedCount() * MAX_PATH);
	strPathsSepSpc = _T("");

	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		String path;
		for (int nIndex = 0; nIndex < 3; nIndex++)
		{
			if (di.diffcode.isExists(nIndex) && ((flags >> nIndex) & 0x1))
			{
				path = di.getFilepath(nIndex, GetDocument()->GetBasePath(nIndex));
				path += '\\';
				// If item is a folder then subfolder (relative to base folder)
				// is in filename member.
				path += di.diffFileInfo[nIndex].filename;

				strPaths += path.c_str();
				strPaths += '\0';

				strPathsSepSpc += _T("\"");
				strPathsSepSpc += path.c_str();
				strPathsSepSpc += _T("\" ");
			}
		}
	}
	strPaths += '\0';
	strPathsSepSpc.TrimRight();

	// CF_HDROP
	HGLOBAL hDrop = GlobalAlloc(GHND, sizeof(DROPFILES) + sizeof(TCHAR) * strPaths.GetLength());
	if (!hDrop)
		return;
	TCHAR *pDrop = (TCHAR *)GlobalLock(hDrop);
	DROPFILES df = {0};
	df.pFiles = sizeof(DROPFILES);
	df.fWide = (sizeof(TCHAR) > 1);
	memcpy(pDrop, &df, sizeof(DROPFILES));
	memcpy((BYTE *)pDrop + sizeof(DROPFILES), (LPCTSTR)strPaths, sizeof(TCHAR) * strPaths.GetLength());
	GlobalUnlock(hDrop);

	// CF_DROPEFFECT
	HGLOBAL hDropEffect = GlobalAlloc(GHND, sizeof(DWORD));
	if (!hDropEffect)
	{
		GlobalFree(hDrop);
		return;
	}
	*((DWORD *)(GlobalLock(hDropEffect))) = DROPEFFECT_COPY;
	GlobalUnlock(hDropEffect);

	// CF_UNICODETEXT
	HGLOBAL hPathnames = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(TCHAR) * (strPathsSepSpc.GetLength() + 1));
	if (!hPathnames)
	{
		GlobalFree(hDrop);
		GlobalFree(hDropEffect);
		return;
	}
	void *pPathnames = GlobalLock(hPathnames);
	memcpy((BYTE *)pPathnames, (LPCTSTR)strPathsSepSpc, sizeof(TCHAR) * strPathsSepSpc.GetLength());
	((TCHAR *)pPathnames)[strPathsSepSpc.GetLength()] = 0;
	GlobalUnlock(hPathnames);

	UINT CF_DROPEFFECT = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	if (::OpenClipboard(AfxGetMainWnd()->GetSafeHwnd()))
	{
		EmptyClipboard();
		SetClipboardData(CF_HDROP, hDrop);
		SetClipboardData(CF_DROPEFFECT, hDropEffect);
		SetClipboardData(GetClipTcharTextFormat(), hPathnames);
		CloseClipboard();
	}
}
