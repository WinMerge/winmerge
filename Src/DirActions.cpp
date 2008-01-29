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
// $Id$

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
#include "CShellFileOp.h"
#include "OptionsDef.h"
#include "WaitStatusCursor.h"
#include "LogFile.h"
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

static BOOL ConfirmCopy(int origin, int destination, int count,
		LPCTSTR src, LPCTSTR dest, BOOL destIsSide);

static BOOL CheckPathsExist(LPCTSTR orig, LPCTSTR dest, int allowOrig,
		int allowDest, CString & failedPath);


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
 * @return IDYES if copy should proceed, IDNO if aborted.
 */
static BOOL ConfirmCopy(int origin, int destination, int count,
		LPCTSTR src, LPCTSTR dest, BOOL destIsSide)
{
	ConfirmFolderCopyDlg dlg;
	CString strQuestion;
	String sOrig;
	String sDest;

	UINT id = count == 1 ? IDS_CONFIRM_SINGLE_COPY : IDS_CONFIRM_MULTIPLE_COPY;
	strQuestion.Format(theApp.LoadString(id).c_str(), count);
	
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

	dlg.m_question = strQuestion;
	dlg.m_fromText = sOrig.c_str();
	dlg.m_toText = sDest.c_str();
	dlg.m_fromPath = strSrc.c_str();
	dlg.m_toPath = strDest.c_str();

	int rtn = dlg.DoModal();
	return (rtn==IDYES);
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
 * @return TRUE if path exists and is of allowed type.
 */
static BOOL CheckPathsExist(LPCTSTR orig, LPCTSTR dest, int allowOrig,
		int allowDest, CString & failedPath)
{
	// Either of the paths must be checked!
	ASSERT(allowOrig != ALLOW_DONT_CARE || allowDest != ALLOW_DONT_CARE);
	BOOL origSuccess = FALSE;
	BOOL destSuccess = FALSE;

	if (allowOrig != ALLOW_DONT_CARE)
	{
		// Check that source exists
		PATH_EXISTENCE exists = paths_DoesPathExist(orig);
		if (((allowOrig & ALLOW_FOLDER) != 0) && exists == IS_EXISTING_DIR)
			origSuccess = TRUE;
		if (((allowOrig & ALLOW_FILE) != 0) && exists == IS_EXISTING_FILE)
			origSuccess = TRUE;

		// Original item failed, don't bother checking dest item
		if (origSuccess == FALSE)
		{
			failedPath = orig;
			return FALSE;
		}
	}

	if (allowDest != ALLOW_DONT_CARE)
	{
		// Check that destination exists
		PATH_EXISTENCE exists = paths_DoesPathExist(dest);
		if (((allowDest & ALLOW_FOLDER) != 0) && exists == IS_EXISTING_DIR)
			destSuccess = TRUE;
		if (((allowDest & ALLOW_FILE) != 0) && exists == IS_EXISTING_FILE)
			destSuccess = TRUE;

		if (destSuccess == FALSE)
		{
			failedPath = dest;
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * @brief Format warning message about invalid folder compare contents.
 * @param [in] failedPath Path that failed (didn't exist).
 */
void CDirView::WarnContentsChanged(const CString & failedPath)
{
	ResMsgBox1(IDS_DIRCMP_NOTSYNC, failedPath, MB_ICONWARNING);
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
			CString failpath;
			BOOL succeed = CheckPathsExist(srFile.c_str(), slFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			act.src = srFile;
			act.dest = slFile;
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
			CString failpath;
			BOOL succeed = CheckPathsExist(slFile.c_str(), srFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			act.src = slFile;
			act.dest = srFile;
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
			CString failpath;
			BOOL succeed = CheckPathsExist(slFile.c_str(), srFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
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
			CString failpath;
			BOOL succeed = CheckPathsExist(srFile.c_str(), slFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
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
			CString failpath;
			BOOL succeed = CheckPathsExist(srFile.c_str(), slFile.c_str(), ALLOW_ALL,
					ALLOW_ALL, failpath);
			if (succeed == FALSE)
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
			FileActionItem act;
			if (IsItemDeletableOnBoth(di))
			{
				leftFlags = ALLOW_ALL;
				rightFlags = ALLOW_ALL;
				act.src = srFile;
				act.dest = slFile;
			}
			else if (IsItemDeletableOnLeft(di))
			{
				leftFlags = ALLOW_ALL;
				act.src = slFile;
			}
			else if (IsItemDeletableOnRight(di))
			{
				rightFlags = ALLOW_ALL;
				act.src = srFile;
			}

			// We must first check that paths still exists
			CString failpath;
			BOOL succeed = CheckPathsExist(slFile.c_str(), srFile.c_str(), leftFlags,
					rightFlags, failpath);
			if (succeed == FALSE)
			{
				WarnContentsChanged(failpath);
				return;
			}

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
 * @brief Copy selected left-side files to user-specified directory
 *
 * When copying files from recursive compare file subdirectory is also
 * read so directory structure is preserved.
 * @note CShellFileOp takes care of much of error handling
 */
void CDirView::DoCopyLeftTo()
{
	CString destPath;
	CString startPath;

	if (!SelectFolder(destPath, startPath, IDS_SELECT_DEST_LEFT))
		return;

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
			CString failpath;
			BOOL succeed = CheckPathsExist(slFile.c_str(), srFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");

			actionScript.m_destBase = sFullDest;

			if (GetDocument()->GetRecursive())
			{
				if (!di.sLeftSubdir.empty())
				{
					sFullDest += di.sLeftSubdir.c_str();
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.sLeftFilename.c_str();
			act.dest = sFullDest;

			act.src = slFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DONT_CARE;
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
 * @note CShellFileOp takes care of much of error handling
 */
void CDirView::DoCopyRightTo()
{
	CString destPath;
	CString startPath;

	if (!SelectFolder(destPath, startPath, IDS_SELECT_DEST_RIGHT))
		return;

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
			CString failpath;
			BOOL succeed = CheckPathsExist(srFile.c_str(), slFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");

			actionScript.m_destBase = sFullDest;

			if (GetDocument()->GetRecursive())
			{
				if (!di.sRightSubdir.empty())
				{
					sFullDest += di.sRightSubdir.c_str();
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.sRightFilename.c_str();
			act.dest = sFullDest;

			act.src = srFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DONT_CARE;
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
 * @note CShellFileOp takes care of much of error handling
 */
void CDirView::DoMoveLeftTo()
{
	CString destPath;
	CString startPath;

	if (!SelectFolder(destPath, startPath, IDS_SELECT_DEST_LEFT))
		return;

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
			CString failpath;
			BOOL succeed = CheckPathsExist(slFile.c_str(), srFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");
			if (GetDocument()->GetRecursive())
			{
				if (!di.sLeftSubdir.empty())
				{
					sFullDest += di.sLeftSubdir.c_str();
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.sLeftFilename.c_str();
			act.dest = sFullDest;

			act.src = slFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
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
 * @note CShellFileOp takes care of much of error handling
 */
void CDirView::DoMoveRightTo()
{
	CString destPath;
	CString startPath;

	if (!SelectFolder(destPath, startPath, IDS_SELECT_DEST_RIGHT))
		return;

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
			CString failpath;
			BOOL succeed = CheckPathsExist(srFile.c_str(), slFile.c_str(), ALLOW_ALL,
					ALLOW_DONT_CARE, failpath);
			if (succeed == FALSE)
			{
				WarnContentsChanged(failpath);
				return;
			}

			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");
			if (GetDocument()->GetRecursive())
			{
				if (!di.sRightSubdir.empty())
				{
					sFullDest += di.sRightSubdir.c_str();
					sFullDest += _T("\\");
				}
			}
			sFullDest += di.sRightFilename.c_str();
			act.dest = sFullDest;

			act.src = srFile;
			act.dirflag = di.diffcode.isDirectory();
			act.context = sel;
			act.atype = actType;
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
BOOL CDirView::ConfirmActionList(const FileActionScript & actionList, int selCount)
{
	// TODO: We need better confirmation for file actions.
	// Maybe we should show a list of files with actions done..
	FileActionItem item = actionList.GetHeadActionItem();

	BOOL bDestIsSide = TRUE;

	// special handling for the single item case, because it is probably the most common,
	// and we can give the user exact details easily for it
	switch(item.atype)
	{
	case FileAction::ACT_COPY:
		if (item.UIResult == FileActionItem::UI_DONT_CARE)
			bDestIsSide = FALSE;

		if (actionList.GetActionItemCount() == 1)
		{
			if (!ConfirmCopy(item.UIOrigin, item.UIDestination,
                actionList.GetActionItemCount(), item.src.c_str(), item.dest.c_str(),
				bDestIsSide))
			{
				return FALSE;
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
				if (!actionList.m_destBase.IsEmpty())
					dst = actionList.m_destBase;
				else
					item.dest;

			}

			if (!ConfirmCopy(item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), src.c_str(), dst.c_str(), bDestIsSide))
			{
				return FALSE;
			}
		}
		break;
		
	// Deleting does not need confirmation, CShellFileOp takes care of it
	case FileAction::ACT_DEL:
	// Moving does not need confirmation, CShellFileOp takes care of it
	case FileAction::ACT_MOVE:
		break;

	// Invalid operation
	default: 
		LogErrorString(_T("Unknown fileoperation in CDirView::ConfirmActionList()"));
		_RPTF0(_CRT_ERROR, "Unknown fileoperation in CDirView::ConfirmActionList()");
		break;
	}
	return TRUE;
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
	GetMainFrame()->m_CheckOutMulti = FALSE;
	GetMainFrame()->m_bVssSuppressPathCheck = FALSE;

	// Check option and enable putting deleted items to Recycle Bin
	if (GetOptionsMgr()->GetBool(OPT_USE_RECYCLE_BIN))
		actionScript.UseRecycleBin(TRUE);
	else
		actionScript.UseRecycleBin(FALSE);

	actionScript.SetParentWindow(this->GetSafeHwnd());

	if (actionScript.Run())
		UpdateAfterFileScript(actionScript);
}

/**
 * @brief Update results after running FileActionScript.
 * This functions is called after script is finished to update
 * results (including UI).
 * @param [in] actionlist Script that was run.
 */
void CDirView::UpdateAfterFileScript(FileActionScript & actionList)
{
	BOOL bItemsRemoved = FALSE;
	int curSel = GetFirstSelectedInd();
	CDirDoc *pDoc = GetDocument();
	while (actionList.GetActionItemCount()>0)
	{
		// Start handling from tail of list, so removing items
		// doesn't invalidate our item indexes.
		FileActionItem act = actionList.RemoveTailActionItem();
		POSITION diffpos = GetItemKey(act.context);
		const DIFFITEM & di = pDoc->GetDiffByKey(diffpos);
		BOOL bUpdateLeft = FALSE;
		BOOL bUpdateRight = FALSE;

		// Synchronized items may need VCS operations
		if (act.UIResult == FileActionItem::UI_SYNC)
		{
			if (GetMainFrame()->m_bCheckinVCS)
				GetMainFrame()->CheckinToClearCase(act.dest.c_str());
		}

		// Update doc (difflist)
		pDoc->UpdateDiffAfterOperation(act, diffpos);

		// Update UI
		switch (act.UIResult)
		{
		case FileActionItem::UI_SYNC:
			bUpdateLeft = TRUE;
			bUpdateRight = TRUE;
			break;
		
		case FileActionItem::UI_DESYNC:
			// Cannot happen yet since we have only "simple" operations
			break;

		case FileActionItem::UI_DEL_LEFT:
			if (di.diffcode.isSideLeftOnly())
			{
				m_pList->DeleteItem(act.context);
				bItemsRemoved = TRUE;
			}
			else
			{
				bUpdateLeft = TRUE;
			}
			break;

		case FileActionItem::UI_DEL_RIGHT:
			if (di.diffcode.isSideRightOnly())
			{
				m_pList->DeleteItem(act.context);
				bItemsRemoved = TRUE;
			}
			else
			{
				bUpdateRight = TRUE;
			}
			break;

		case FileActionItem::UI_DEL_BOTH:
			m_pList->DeleteItem(act.context);
			bItemsRemoved = TRUE;
			break;
		}

		if (bUpdateLeft || bUpdateRight)
		{
			pDoc->UpdateStatusFromDisk(diffpos, bUpdateLeft, bUpdateRight);
			UpdateDiffItemStatus(act.context);
		}
	}
	
	// Make sure selection is at sensible place if all selected items
	// were removed.
	if (bItemsRemoved == TRUE)
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
BOOL CDirView::GetSelectedDirNames(String& strLeft, String& strRight) const
{
	BOOL bResult = GetSelectedFileNames(strLeft, strRight);

	if (bResult)
	{
		strLeft = GetPathOnly(strLeft.c_str());
		strRight = GetPathOnly(strRight.c_str());
	}
	return bResult;
}

/// is it possible to copy item to left ?
BOOL CDirView::IsItemCopyableToLeft(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return FALSE;
	// can't copy same items
	if (di.diffcode.isResultSame()) return FALSE;
	// impossible if only on left
	if (di.diffcode.isSideLeftOnly()) return FALSE;

	// everything else can be copied to left
	return TRUE;
}
/// is it possible to copy item to right ?
BOOL CDirView::IsItemCopyableToRight(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return FALSE;
	// can't copy same items
	if (di.diffcode.isResultSame()) return FALSE;
	// impossible if only on right
	if (di.diffcode.isSideRightOnly()) return FALSE;

	// everything else can be copied to right
	return TRUE;
}
/// is it possible to delete left item ?
BOOL CDirView::IsItemDeletableOnLeft(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return FALSE;
	// impossible if only on right
	if (di.diffcode.isSideRightOnly()) return FALSE;
	// everything else can be deleted on left
	return TRUE;
}
/// is it possible to delete right item ?
BOOL CDirView::IsItemDeletableOnRight(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return FALSE;
	// impossible if only on right
	if (di.diffcode.isSideLeftOnly()) return FALSE;

	// everything else can be deleted on right
	return TRUE;
}
/// is it possible to delete both items ?
BOOL CDirView::IsItemDeletableOnBoth(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return FALSE;
	// impossible if only on right or left
	if (di.diffcode.isSideLeftOnly() || di.diffcode.isSideRightOnly()) return FALSE;

	// everything else can be deleted on both
	return TRUE;
}

/**
 * @brief Determine if item can be opened.
 * Basically we only disable opening unique files at the moment.
 * Unique folders can be opened since we ask for creating matching folder
 * to another side.
 * @param [in] di DIFFITEM for item to check.
 * @return TRUE if the item can be opened, FALSE otherwise.
 */
BOOL CDirView::IsItemOpenable(const DIFFITEM & di) const
{
	if (!di.diffcode.isDirectory() &&
		(di.diffcode.isSideRightOnly() || di.diffcode.isSideLeftOnly()))
	{
		return FALSE;
	}
	return TRUE;
}
/// is it possible to compare these two items?
BOOL CDirView::AreItemsOpenable(const DIFFITEM & di1, const DIFFITEM & di2) const
{
	String sLeftBasePath = GetDocument()->GetLeftBasePath();
	String sRightBasePath = GetDocument()->GetRightBasePath();

	// Must be both directory or neither
	if (di1.diffcode.isDirectory() != di2.diffcode.isDirectory()) return FALSE;

	// Must be on different sides, or one on one side & one on both
	if (di1.diffcode.isSideLeftOnly() && (di2.diffcode.isSideRightOnly() ||
			di2.diffcode.isSideBoth()))
		return TRUE;
	if (di1.diffcode.isSideRightOnly() && (di2.diffcode.isSideLeftOnly() ||
			di2.diffcode.isSideBoth()))
		return TRUE;
	if (di1.diffcode.isSideBoth() && (di2.diffcode.isSideLeftOnly() ||
			di2.diffcode.isSideRightOnly()))
		return TRUE;

	// Allow to compare items if left & right path refer to same directory
	// (which means there is effectively two files involved). No need to check
	// side flags. If files weren't on both sides, we'd have no DIFFITEMs.
	if (lstrcmpi(sLeftBasePath.c_str(), sRightBasePath.c_str()) == 0)
		return TRUE;

	return FALSE;
}
/// is it possible to open left item ?
BOOL CDirView::IsItemOpenableOnLeft(const DIFFITEM & di) const
{
	// impossible if only on right
	if (di.diffcode.isSideRightOnly()) return FALSE;

	// everything else can be opened on right
	return TRUE;
}
/// is it possible to open right item ?
BOOL CDirView::IsItemOpenableOnRight(const DIFFITEM & di) const
{
	// impossible if only on left
	if (di.diffcode.isSideLeftOnly()) return FALSE;

	// everything else can be opened on left
	return TRUE;
}
/// is it possible to open left ... item ?
BOOL CDirView::IsItemOpenableOnLeftWith(const DIFFITEM & di) const
{
	return (!di.diffcode.isDirectory() && IsItemOpenableOnLeft(di));
}
/// is it possible to open with ... right item ?
BOOL CDirView::IsItemOpenableOnRightWith(const DIFFITEM & di) const
{
	return (!di.diffcode.isDirectory() && IsItemOpenableOnRight(di));
}
/// is it possible to copy to... left item?
BOOL CDirView::IsItemCopyableToOnLeft(const DIFFITEM & di) const
{
	// impossible if only on right
	if (di.diffcode.isSideRightOnly()) return FALSE;

	// everything else can be copied to from left
	return TRUE;
}
/// is it possible to copy to... right item?
BOOL CDirView::IsItemCopyableToOnRight(const DIFFITEM & di) const
{
	// impossible if only on left
	if (di.diffcode.isSideLeftOnly()) return FALSE;

	// everything else can be copied to from right
	return TRUE;
}

/// get the file names on both sides for first selected item
BOOL CDirView::GetSelectedFileNames(String& strLeft, String& strRight) const
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
		return FALSE;
	GetItemFileNames(sel, strLeft, strRight);
	return TRUE;
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
	POSITION diffpos = GetItemKey(sel);
	if (diffpos == (POSITION)SPECIAL_ITEM_POS)
	{
		strLeft.empty();
		strRight.empty();
	}
	else
	{
		const DIFFITEM & di = GetDocument()->GetDiffByKey(diffpos);
		const String leftrelpath = paths_ConcatPath(di.sLeftSubdir, di.sLeftFilename);
		const String rightrelpath = paths_ConcatPath(di.sRightSubdir, di.sRightFilename);
		const String & leftpath = GetDocument()->GetLeftBasePath();
		const String & rightpath = GetDocument()->GetRightBasePath();
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
	String strLeft;
	String strRight;
	GetItemFileNames(sel, strLeft, strRight);
	paths->SetLeft(strLeft.c_str());
	paths->SetRight(strRight.c_str());
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

	GetMainFrame()->OpenFileToExternalEditor(file.c_str());
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
		if (!di.diffcode.isDirectory() && !di.diffcode.isSideLeftOnly() &&
			!di.diffcode.isSideRightOnly())
		{
			GetItemFileNames(sel, slFile, srFile);
			String filteredFilenames = slFile + _T("|") + srFile;
			GetDocument()->SetPluginPrediffSetting(filteredFilenames.c_str(), newsetting);
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

		DIFFITEM di = GetDiffItem(sel);
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
static CString
FormatFilesAffectedString(int nFilesAffected, int nFilesTotal)
{
	CString fmt;
	if (nFilesAffected == nFilesTotal)
		LangFormatString1(fmt, IDS_FILES_AFFECTED_FMT, NumToStr(nFilesTotal).c_str());
	else
		LangFormatString2(fmt, IDS_FILES_AFFECTED_FMT2, NumToStr(nFilesAffected).c_str(), NumToStr(nFilesTotal).c_str());
	return fmt;
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
	int nLeft=0, nLeftAffected=0, nRight=0, nRightAffected=0;
	int i = -1;
	while ((i = m_pList->GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(i);
		if (di.diffcode.diffcode == 0) // Invalid value, this must be special item
			continue;
		if (di.diffcode.isDirectory())
			continue;

		if (di.diffcode.isSideLeftOrBoth())
		{
			// exists on left
			++nLeft;
			if (di.left.IsEditableEncoding())
				++nLeftAffected;
			int codepage = di.left.encoding.m_codepage;
			currentCodepages.Increment(codepage);
		}
		if (di.diffcode.isSideRightOrBoth())
		{
			++nRight;
			if (di.right.IsEditableEncoding())
				++nRightAffected;
			int codepage = di.right.encoding.m_codepage;
			currentCodepages.Increment(codepage);
		}
	}

	// Format strings such as "25 of 30 Files Affected"
	CString sLeftAffected = FormatFilesAffectedString(nLeftAffected, nLeft);
	CString sRightAffected = FormatFilesAffectedString(nRightAffected, nRight);
	dlg->SetLeftRightAffectStrings(sLeftAffected, sRightAffected);
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
	CLoadSaveCodepageDlg dlg;
	// set up labels about what will be affected
	FormatEncodingDialogDisplays(&dlg);
	dlg.EnableSaveCodepage(false); // disallow setting a separate codepage for saving

	// Invoke dialog
	int rtn = dlg.DoModal();
	if (rtn != IDOK)
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
		if (doLeft && di.diffcode.isSideLeftOrBoth() && di.left.IsEditableEncoding())
		{
			di.left.encoding.SetCodepage(nCodepage);
		}
		// Does it exist on right (ie, left or both)
		if (doRight && di.diffcode.isSideRightOrBoth() && di.right.IsEditableEncoding())
		{
			di.right.encoding.SetCodepage(nCodepage);
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
	BOOL haveSelectedItems = (m_pList->GetNextItem(-1, LVNI_SELECTED) != -1);
	pCmdUI->Enable(haveSelectedItems);
}

/**
 * @brief Rename a file without moving it to different directory.
 *
 * @param szOldFileName [in] Full path of file to rename.
 * @param szNewFileName [in] New file name (without the path).
 *
 * @return TRUE if file was renamed successfully.
 */
BOOL CDirView::RenameOnSameDir(LPCTSTR szOldFileName, LPCTSTR szNewFileName)
{
	ASSERT(NULL != szOldFileName);
	ASSERT(NULL != szNewFileName);

	BOOL bSuccess = FALSE;

	if (DOES_NOT_EXIST != paths_DoesPathExist(szOldFileName))
	{
		String sFullName;

		SplitFilename(szOldFileName, &sFullName, NULL, NULL);
		sFullName += _T('\\');
		sFullName += szNewFileName;

		// No need to rename if new file already exist.
		if ((sFullName.compare(szOldFileName)) ||
			(DOES_NOT_EXIST == paths_DoesPathExist(sFullName.c_str())))
		{
			CShellFileOp fileOp;

			fileOp.SetOperationFlags(FO_RENAME, this->GetSafeHwnd(), 0);
			fileOp.AddSourceFile(szOldFileName);
			fileOp.AddDestFile(sFullName.c_str());
			
			BOOL bOpStarted = FALSE;
			bSuccess = fileOp.Go(&bOpStarted);
		}
		else
		{
			bSuccess = TRUE;
		}
	}

	return bSuccess;
}

/**
 * @brief Rename selected item on both left and right sides.
 *
 * @param szNewItemName [in] New item name.
 *
 * @return TRUE if at least one file was renamed successfully.
 */
BOOL CDirView::DoItemRename(LPCTSTR szNewItemName)
{
	ASSERT(NULL != szNewItemName);
	
	String sLeftFile, sRightFile;

	int nSelItem = m_pList->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(-1 != nSelItem);
	GetItemFileNames(nSelItem, sLeftFile, sRightFile);

	// We must check that paths still exists
	CString failpath;
	DIFFITEM &di = GetDiffItemRef(nSelItem);
	BOOL succeed = CheckPathsExist(sLeftFile.c_str(), sRightFile.c_str(),
		di.diffcode.isSideLeftOrBoth()  ? ALLOW_FILE | ALLOW_FOLDER : ALLOW_DONT_CARE,
		di.diffcode.isSideRightOrBoth() ? ALLOW_FILE | ALLOW_FOLDER : ALLOW_DONT_CARE,
		failpath);
	if (succeed == FALSE)
	{
		WarnContentsChanged(failpath);
		return FALSE;
	}

	POSITION key = GetItemKey(nSelItem);
	ASSERT(key != SPECIAL_ITEM_POS);
	di = GetDocument()->GetDiffRefByKey(key);

	BOOL bRenameLeft = FALSE;
	BOOL bRenameRight = FALSE;
	if (di.diffcode.isSideLeftOrBoth())
		bRenameLeft = RenameOnSameDir(sLeftFile.c_str(), szNewItemName);
	if (di.diffcode.isSideRightOrBoth())
		bRenameRight = RenameOnSameDir(sRightFile.c_str(), szNewItemName);

	if ((TRUE == bRenameLeft) && (TRUE == bRenameRight))
	{
		di.sLeftFilename = szNewItemName;
		di.sRightFilename = szNewItemName;
	}
	else if (TRUE == bRenameLeft)
	{
		di.sLeftFilename = szNewItemName;
		di.sRightFilename.erase();
	}
	else if (TRUE == bRenameRight)
	{
		di.sLeftFilename.erase();
		di.sRightFilename = szNewItemName;
	}

	return (bRenameLeft || bRenameRight);
}
