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

/**
 * @brief Ask user a confirmation for copying item(s).
 * Shows a confirmatino dialog for copy operation. Depending ont item count
 * dialog shows full paths to items (single item) or base paths of compare
 * (multiple items).
 * @param [in] origin Origin side of the item(s).
 * @param [in] destination Destination side of the item(s).
 * @param [in] count Number of items.
 * @param [in] src Source path.
 * @param [in] dest Destination path.
 * @return IDYES if copy should proceed, IDNO if aborted.
 */
static BOOL ConfirmCopy(int origin, int destination, int count,
		LPCTSTR src, LPCTSTR dest)
{
	ConfirmFolderCopyDlg dlg;
	CString strQuestion;
	CString sOrig;
	CString sDest;

	if (count == 1)
	{
		strQuestion = LoadResString(IDS_CONFIRM_SINGLE_COPY);
	}
	else
	{
		strQuestion = LoadResString(IDS_CONFIRM_MULTIPLE_COPY);
		strQuestion.Format(strQuestion, count);
	}
	
	if (origin == FileActionItem::UI_LEFT)
		sOrig = LoadResString(IDS_FROM_LEFT);
	else
		sOrig = LoadResString(IDS_FROM_RIGHT);

	if (destination == FileActionItem::UI_LEFT)
		sDest = LoadResString(IDS_TO_LEFT);
	else
		sDest = LoadResString(IDS_TO_RIGHT);

	dlg.m_question = strQuestion;
	dlg.m_fromText = sOrig;
	dlg.m_toText = sDest;
	dlg.m_fromPath = src;
	dlg.m_toPath = dest;

	int rtn = dlg.DoModal();
	return (rtn==IDYES);
}

/// Prompt & copy item from right to left, if legal
void CDirView::DoCopyRightToLeft()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemCopyableToLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			FileActionItem act;
			act.src = srFile;
			act.dest = slFile;
			act.context = sel;
			act.dirflag = di.isDirectory();
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
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemCopyableToRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			FileActionItem act;
			act.src = slFile;
			act.dest = srFile;
			act.dirflag = di.isDirectory();
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
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_DELETEFILES));

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemDeletableOnLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			FileActionItem act;
			act.src = slFile;
			act.dirflag = di.isDirectory();
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
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_DELETEFILES));

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemDeletableOnRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			FileActionItem act;
			act.src = srFile;
			act.dirflag = di.isDirectory();
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
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_DELETEFILES));

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemDeletableOnBoth(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			FileActionItem act;
			act.src = srFile;
			act.dest = slFile;
			act.dirflag = di.isDirectory();
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
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_DELETEFILES));

	// First we build a list of desired actions
	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_DEL;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0)
		{
			GetItemFileNames(sel, slFile, srFile);
			FileActionItem act;
			if (IsItemDeletableOnBoth(di))
			{
				act.src = srFile;
				act.dest = slFile;
			}
			else if (IsItemDeletableOnLeft(di))
			{
				act.src = slFile;
			}
			else if (IsItemDeletableOnRight(di))
			{
				act.src = srFile;
			}
			act.dirflag = di.isDirectory();
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
	CString msg;

	VERIFY(msg.LoadString(IDS_SELECT_DEST_LEFT));
	if (!SelectFolder(destPath, startPath, msg))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemCopyableToOnLeft(di))
		{
			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");
			if (GetDocument()->GetRecursive())
			{
				if (!di.sLeftSubdir.IsEmpty())
					sFullDest += di.sLeftSubdir + _T("\\");
			}
			sFullDest += di.sLeftFilename;
			act.dest = sFullDest;

			GetItemFileNames(sel, slFile, srFile);
			act.src = slFile;
			act.dirflag = di.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DESYNC;
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
	CString msg;

	VERIFY(msg.LoadString(IDS_SELECT_DEST_RIGHT));
	if (!SelectFolder(destPath, startPath, msg))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_COPY;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemCopyableToOnRight(di))
		{
			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");
			if (GetDocument()->GetRecursive())
			{
				if (!di.sRightSubdir.IsEmpty())
					sFullDest += di.sRightSubdir + _T("\\");
			}
			sFullDest += di.sRightFilename;
			act.dest = sFullDest;

			GetItemFileNames(sel, slFile, srFile);
			act.src = srFile;
			act.dirflag = di.isDirectory();
			act.context = sel;
			act.atype = actType;
			act.UIResult = FileActionItem::UI_DESYNC;
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
	CString msg;

	VERIFY(msg.LoadString(IDS_SELECT_DEST_LEFT));
	if (!SelectFolder(destPath, startPath, msg))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_MOVEFILES));

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_MOVE;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemCopyableToOnLeft(di) && IsItemDeletableOnLeft(di))
		{
			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");
			if (GetDocument()->GetRecursive())
			{
				if (!di.sLeftSubdir.IsEmpty())
					sFullDest += di.sLeftSubdir + _T("\\");
			}
			sFullDest += di.sLeftFilename;
			act.dest = sFullDest;

			GetItemFileNames(sel, slFile, srFile);
			act.src = slFile;
			act.dirflag = di.isDirectory();
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
	CString msg;

	VERIFY(msg.LoadString(IDS_SELECT_DEST_RIGHT));
	if (!SelectFolder(destPath, startPath, msg))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_MOVEFILES));

	FileActionScript actionScript;
	const FileAction::ACT_TYPE actType = FileAction::ACT_MOVE;
	int selCount = 0;
	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemCopyableToOnRight(di) && IsItemDeletableOnRight(di))
		{
			FileActionItem act;
			CString sFullDest(destPath);
			sFullDest += _T("\\");
			if (GetDocument()->GetRecursive())
			{
				if (!di.sRightSubdir.IsEmpty())
					sFullDest += di.sRightSubdir + _T("\\");
			}
			sFullDest += di.sRightFilename;
			act.dest = sFullDest;

			GetItemFileNames(sel, slFile, srFile);
			act.src = srFile;
			act.dirflag = di.isDirectory();
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
	actionList.SetParentWindow(this);
	
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

	// special handling for the single item case, because it is probably the most common,
	// and we can give the user exact details easily for it
	switch(item.atype)
	{
	case FileAction::ACT_COPY:
		if (actionList.GetActionItemCount() == 1)
		{
			if (!ConfirmCopy(item.UIOrigin, item.UIDestination,
                actionList.GetActionItemCount(), item.src, item.dest))
			{
				return FALSE;
			}
		}
		else
		{
			CString src;
			CString dst;

			if (item.UIOrigin == FileActionItem::UI_LEFT)
				src = GetDocument()->GetLeftBasePath();
			else
				src = GetDocument()->GetRightBasePath();
			if (item.UIDestination == FileActionItem::UI_LEFT)
				dst = GetDocument()->GetLeftBasePath();
			else
				dst = GetDocument()->GetRightBasePath();
			if (!ConfirmCopy(item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), src, dst))
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

	actionScript.SetParentWindow(this);

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
				GetMainFrame()->CheckinToClearCase(act.dest);
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
			if (di.isSideLeftOnly())
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
			if (di.isSideRightOnly())
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
BOOL CDirView::GetSelectedDirNames(CString& strLeft, CString& strRight) const
{
	BOOL bResult = GetSelectedFileNames(strLeft, strRight);

	if (bResult)
	{
		strLeft = GetPathOnly(strLeft);
		strRight = GetPathOnly(strRight);
	}
	return bResult;
}

/// is it possible to copy item to left ?
BOOL CDirView::IsItemCopyableToLeft(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// can't copy same items
	if (di.isResultSame()) return FALSE;
	// impossible if only on left
	if (di.isSideLeftOnly()) return FALSE;

	// everything else can be copied to left
	return TRUE;
}
/// is it possible to copy item to right ?
BOOL CDirView::IsItemCopyableToRight(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// can't copy same items
	if (di.isResultSame()) return FALSE;
	// impossible if only on right
	if (di.isSideRightOnly()) return FALSE;

	// everything else can be copied to right
	return TRUE;
}
/// is it possible to delete left item ?
BOOL CDirView::IsItemDeletableOnLeft(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// impossible if only on right
	if (di.isSideRightOnly()) return FALSE;
	// everything else can be deleted on left
	return TRUE;
}
/// is it possible to delete right item ?
BOOL CDirView::IsItemDeletableOnRight(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// impossible if only on right
	if (di.isSideLeftOnly()) return FALSE;

	// everything else can be deleted on right
	return TRUE;
}
/// is it possible to delete both items ?
BOOL CDirView::IsItemDeletableOnBoth(const DIFFITEM & di) const
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// impossible if only on right or left
	if (di.isSideLeftOnly() || di.isSideRightOnly()) return FALSE;

	// everything else can be deleted on both
	return TRUE;
}

/// is it possible to open item for compare ?
BOOL CDirView::IsItemOpenable(const DIFFITEM & di) const
{
	// impossible if unique or binary
	if (di.isSideRightOnly() || di.isSideLeftOnly() || di.isBin()) return FALSE;

	// everything else can be opened
	return TRUE;
}
/// is it possible to compare these two items?
BOOL CDirView::AreItemsOpenable(const DIFFITEM & di1, const DIFFITEM & di2) const
{
	CString sLeftBasePath = GetDocument()->GetLeftBasePath();
	CString sRightBasePath = GetDocument()->GetRightBasePath();
	CString sLeftPath1 = paths_ConcatPath(di1.getLeftFilepath(sLeftBasePath), di1.sLeftFilename);
	CString sLeftPath2 = paths_ConcatPath(di2.getLeftFilepath(sLeftBasePath), di2.sLeftFilename);
	CString sRightPath1 = paths_ConcatPath(di1.getRightFilepath(sRightBasePath), di1.sRightFilename);
	CString sRightPath2 = paths_ConcatPath(di2.getRightFilepath(sRightBasePath), di2.sRightFilename);
	// Must not be binary (unless archive)
	if
	(
		(di1.isBin() || di2.isBin())
	&&!	(
			HasZipSupport()
		&&	(sLeftPath1.IsEmpty() || ArchiveGuessFormat(sLeftPath1))
		&&	(sRightPath1.IsEmpty() || ArchiveGuessFormat(sRightPath1))
		&&	(sLeftPath2.IsEmpty() || ArchiveGuessFormat(sLeftPath2))
		&&	(sRightPath2.IsEmpty() || ArchiveGuessFormat(sRightPath2))
		)
	)
	{
		return FALSE;
	}

	// Must be both directory or neither
	if (di1.isDirectory() != di2.isDirectory()) return FALSE;

	// Must be on different sides, or one on one side & one on both
	if (di1.isSideLeftOnly() && (di2.isSideRightOnly() || di2.isSideBoth()))
		return TRUE;
	if (di1.isSideRightOnly() && (di2.isSideLeftOnly() || di2.isSideBoth()))
		return TRUE;
	if (di1.isSideBoth() && (di2.isSideLeftOnly() || di2.isSideRightOnly()))
		return TRUE;

	// Allow to compare items if left & right path refer to same directory
	// (which means there is effectively two files involved). No need to check
	// side flags. If files weren't on both sides, we'd have no DIFFITEMs.
	if (sLeftBasePath.CompareNoCase(sRightBasePath) == 0)
		return TRUE;

	return FALSE;
}
/// is it possible to open left item ?
BOOL CDirView::IsItemOpenableOnLeft(const DIFFITEM & di) const
{
	// impossible if only on right
	if (di.isSideRightOnly()) return FALSE;

	// everything else can be opened on right
	return TRUE;
}
/// is it possible to open right item ?
BOOL CDirView::IsItemOpenableOnRight(const DIFFITEM & di) const
{
	// impossible if only on left
	if (di.isSideLeftOnly()) return FALSE;

	// everything else can be opened on left
	return TRUE;
}
/// is it possible to open left ... item ?
BOOL CDirView::IsItemOpenableOnLeftWith(const DIFFITEM & di) const
{
	return (!di.isDirectory() && IsItemOpenableOnLeft(di));
}
/// is it possible to open with ... right item ?
BOOL CDirView::IsItemOpenableOnRightWith(const DIFFITEM & di) const
{
	return (!di.isDirectory() && IsItemOpenableOnRight(di));
}
/// is it possible to copy to... left item?
BOOL CDirView::IsItemCopyableToOnLeft(const DIFFITEM & di) const
{
	// impossible if only on right
	if (di.isSideRightOnly()) return FALSE;

	// everything else can be copied to from left
	return TRUE;
}
/// is it possible to copy to... right item?
BOOL CDirView::IsItemCopyableToOnRight(const DIFFITEM & di) const
{
	// impossible if only on left
	if (di.isSideLeftOnly()) return FALSE;

	// everything else can be copied to from right
	return TRUE;
}

/// get the file names on both sides for first selected item
BOOL CDirView::GetSelectedFileNames(CString& strLeft, CString& strRight) const
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
		return FALSE;
	GetItemFileNames(sel, strLeft, strRight);
	return TRUE;
}
/// get file name on specified side for first selected item
CString CDirView::GetSelectedFileName(SIDE_TYPE stype) const
{
	CString left, right;
	if (!GetSelectedFileNames(left, right)) return _T("");
	return stype==SIDE_LEFT ? left : right;
}
/**
 * @brief Get the file names on both sides for specified item.
 * @note Return empty strings if item is special item.
 */
void CDirView::GetItemFileNames(int sel, CString& strLeft, CString& strRight) const
{
	POSITION diffpos = GetItemKey(sel);
	if (diffpos == (POSITION)SPECIAL_ITEM_POS)
	{
		strLeft.Empty();
		strRight.Empty();
	}
	else
	{
		const DIFFITEM & di = GetDocument()->GetDiffByKey(diffpos);
		const CString leftrelpath = paths_ConcatPath(di.sLeftSubdir, di.sLeftFilename);
		const CString rightrelpath = paths_ConcatPath(di.sRightSubdir, di.sRightFilename);
		const CString & leftpath = GetDocument()->GetLeftBasePath();
		const CString & rightpath = GetDocument()->GetRightBasePath();
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
	CString strLeft;
	CString strRight;
	GetItemFileNames(sel, strLeft, strRight);
	paths->SetLeft(strLeft);
	paths->SetRight(strRight);
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
	CString file = GetSelectedFileName(stype);
	if (file.IsEmpty()) return;
	int rtn = (int)ShellExecute(::GetDesktopWindow(), _T("edit"), file, 0, 0, SW_SHOWNORMAL);
	if (rtn==SE_ERR_NOASSOC)
		rtn = (int)ShellExecute(::GetDesktopWindow(), _T("open"), file, 0, 0, SW_SHOWNORMAL);
	if (rtn==SE_ERR_NOASSOC)
		DoOpenWith(stype);
}

/// Open with dialog for file on selected side
void CDirView::DoOpenWith(SIDE_TYPE stype)
{
	int sel = GetSingleSelectedItem();
	if (sel == -1) return;
	CString file = GetSelectedFileName(stype);
	if (file.IsEmpty()) return;
	CString sysdir;
	if (!GetSystemDirectory(sysdir.GetBuffer(MAX_PATH), MAX_PATH)) return;
	sysdir.ReleaseBuffer();
	CString arg = (CString)_T("shell32.dll,OpenAs_RunDLL ") + file;
	ShellExecute(::GetDesktopWindow(), 0, _T("RUNDLL32.EXE"), arg, sysdir, SW_SHOWNORMAL);
}

/// Open selected file  on specified side to external editor
void CDirView::DoOpenWithEditor(SIDE_TYPE stype)
{
	int sel = GetSingleSelectedItem();
	if (sel == -1) return;
	CString file = GetSelectedFileName(stype);
	if (file.IsEmpty()) return;

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
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!di.isDirectory() && !di.isSideLeftOnly() && !di.isSideRightOnly())
		{
			GetItemFileNames(sel, slFile, srFile);
			CString filteredFilenames = slFile + (CString)_T("|") + srFile;
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
	CString slFile, srFile;
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
		AfxFormatString1(fmt, IDS_FILES_AFFECTED_FMT, NumToStr(nFilesTotal));
	else
		AfxFormatString2(fmt, IDS_FILES_AFFECTED_FMT2, NumToStr(nFilesAffected), NumToStr(nFilesTotal));
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
		if (di.diffcode == 0) // Invalid value, this must be special item
			continue;
		if (di.isDirectory())
			continue;

		if (di.isSideLeftOrBoth())
		{
			// exists on left
			++nLeft;
			if (di.left.IsEditableEncoding())
				++nLeftAffected;
			int codepage = di.left.encoding.m_codepage;
			currentCodepages.Increment(codepage);
		}
		if (di.isSideRightOrBoth())
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
	if (rtn != IDOK) return;

	int nCodepage = dlg.GetLoadCodepage();

	bool doLeft = dlg.DoesAffectLeft();
	bool doRight = dlg.DoesAffectRight();

	int i=-1;
	while ((i = m_pList->GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		DIFFITEM & di = GetDiffItemRef(i);
		if (di.diffcode == 0) // Invalid value, this must be special item
			continue;
		if (di.isDirectory())
			continue;

		// Does it exist on left? (ie, right or both)
		if (doLeft && di.isSideLeftOrBoth() && di.left.IsEditableEncoding())
		{
			di.left.encoding.SetCodepage(nCodepage);
		}
		// Does it exist on right (ie, left or both)
		if (doRight && di.isSideRightOrBoth() && di.right.IsEditableEncoding())
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
		CString sFullName;

		SplitFilename(szOldFileName, &sFullName, NULL, NULL);
		sFullName += _T('\\') + CString(szNewFileName);

		// No need to rename if new file already exist.
		if ((sFullName.Compare(szOldFileName)) ||
			(DOES_NOT_EXIST == paths_DoesPathExist(sFullName)))
		{
			CShellFileOp fileOp;

			fileOp.SetOperationFlags(FO_RENAME, this, 0);
			fileOp.AddSourceFile(szOldFileName);
			fileOp.AddDestFile(sFullName);
			
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
	
	CString sLeftFile, sRightFile;

	int nSelItem = m_pList->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(-1 != nSelItem);
	GetItemFileNames(nSelItem, sLeftFile, sRightFile);

	BOOL bRenameLeft = RenameOnSameDir(sLeftFile, szNewItemName);
	BOOL bRenameRight = RenameOnSameDir(sRightFile, szNewItemName);

	POSITION key = GetItemKey(nSelItem);
	ASSERT(key != SPECIAL_ITEM_POS);
	DIFFITEM& di = GetDocument()->GetDiffRefByKey(key);

	if ((TRUE == bRenameLeft)  && (TRUE == bRenameRight))
	{
		di.sLeftFilename = szNewItemName;
		di.sRightFilename = szNewItemName;
	}
	else if (TRUE == bRenameLeft)
	{
		di.sLeftFilename = szNewItemName;
		di.sRightFilename.Empty();
	}
	else if (TRUE == bRenameRight)
	{
		di.sLeftFilename.Empty();
		di.sRightFilename = szNewItemName;
	}

	return (bRenameLeft || bRenameRight);
}
