/////////////////////////////////////////////////////////////////////////////
//    see Merge.cpp for license (GPLv2+) statement
//
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file DirActions.cpp
 *
 *  @brief Implementation of methods of CDirView that copy/move/delete files
 */
// RCS ID line follows -- this is updated by CVS
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
#include "OutputDlg.h"
#include "paths.h"
#include "CShellFileOp.h"
#include "OptionsDef.h"
#include "WaitStatusCursor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLogFile gLog;

// Prompt user to confirm a multiple item copy
static BOOL ConfirmMultipleCopy(int count, int total)
{
	CString s;
	ASSERT(count>1);
	AfxFormatString2(s, IDS_CONFIRM_COPY2DIR, NumToStr(count), NumToStr(total));
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION|MB_DONT_ASK_AGAIN, IDS_CONFIRM_COPY2DIR);
	return (rtn==IDYES);
}

// Prompt user to confirm a single item copy
static BOOL ConfirmSingleCopy(LPCTSTR src, LPCTSTR dest)
{
	CString s;
	AfxFormatString2(s, IDS_CONFIRM_COPY_SINGLE, src, dest);
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION|MB_DONT_ASK_AGAIN, IDS_CONFIRM_COPY_SINGLE);
	return (rtn==IDYES);
}

// Prompt user to confirm a multiple item delete
static BOOL ConfirmMultipleDelete(int count, int total)
{
	CString s;
	AfxFormatString2(s, IDS_CONFIRM_DELETE_ITEMS, NumToStr(count), NumToStr(total));
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION|MB_DONT_ASK_AGAIN, IDS_CONFIRM_DELETE_ITEMS);
	return (rtn==IDYES);
}

// Prompt user to confirm a single item delete
static BOOL ConfirmSingleDelete(LPCTSTR filepath)
{
	CString s;
	AfxFormatString1(s, IDS_CONFIRM_DELETE_SINGLE, filepath);
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION|MB_DONT_ASK_AGAIN, IDS_CONFIRM_DELETE_SINGLE);
	return (rtn==IDYES);
}

// Prompt & copy item from right to left, if legal
void CDirView::DoCopyRightToLeft()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	// First we build a list of desired actions
	ActionList actionList(ActionList::ACT_COPY);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemCopyableToLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			ActionList::action act;
			act.src = srFile;
			act.dest = slFile;
			act.idx = sel;
			act.code = di.diffcode;
			act.dirflag = di.isDirectory();
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionList);
}
// Prompt & copy item from left to right, if legal
void CDirView::DoCopyLeftToRight()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	// First we build a list of desired actions
	ActionList actionList(ActionList::ACT_COPY);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemCopyableToRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			ActionList::action act;
			act.src = slFile;
			act.dest = srFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionList);
}

// Prompt & delete left, if legal
void CDirView::DoDelLeft()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_DELETEFILES));

	// First we build a list of desired actions
	ActionList actionList(ActionList::ACT_DEL_LEFT);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemDeletableOnLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			ActionList::action act;
			act.src = slFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionList);
}
// Prompt & delete right, if legal
void CDirView::DoDelRight()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_DELETEFILES));

	// First we build a list of desired actions
	ActionList actionList(ActionList::ACT_DEL_RIGHT);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemDeletableOnRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			ActionList::action act;
			act.src = srFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionList);
}
// Prompt & delete both, if legal
void CDirView::DoDelBoth()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_DELETEFILES));

	// First we build a list of desired actions
	ActionList actionList(ActionList::ACT_DEL_BOTH);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemDeletableOnBoth(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			ActionList::action act;
			act.src = srFile;
			act.dest = slFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	// Now we prompt, and execute actions
	ConfirmAndPerformActions(actionList);
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
	CShellFileOp fileOp;
	CString destPath;
	CString startPath;
	CString msg;

	VERIFY(msg.LoadString(IDS_SELECT_DESTFOLDER));
	if (!SelectFolder(destPath, startPath, msg))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	fileOp.SetOperationFlags(FO_COPY, this, FOF_NOCONFIRMMKDIR);
	if (!GetDocument()->GetRecursive())
		fileOp.AddDestFile(destPath);

	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemCopyableToOnLeft(di))
		{
			if (GetDocument()->GetRecursive())
			{
				CString sFullDest(destPath);
				sFullDest += _T("\\");
				if (!di.sSubdir.IsEmpty())
					sFullDest += di.sSubdir + _T("\\");
				sFullDest += di.sfilename;
				fileOp.AddDestFile(sFullDest);
			}
			GetItemFileNames(sel, slFile, srFile);
			fileOp.AddSourceFile(slFile);
		}
	}

	BOOL bSuccess = FALSE;
	BOOL bAPICalled = FALSE;
	BOOL bAborted = FALSE;
	int  nAPIReturn = 0;
	bSuccess = fileOp.Go(&bAPICalled, &nAPIReturn, &bAborted);
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
	CShellFileOp fileOp;
	CString destPath;
	CString startPath;
	CString msg;

	VERIFY(msg.LoadString(IDS_SELECT_DESTFOLDER));
	if (!SelectFolder(destPath, startPath, msg))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYFILES));

	fileOp.SetOperationFlags(FO_COPY, this, FOF_NOCONFIRMMKDIR);
	if (!GetDocument()->GetRecursive())
		fileOp.AddDestFile(destPath);

	int sel = -1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (di.diffcode != 0 && IsItemCopyableToOnRight(di))
		{
			if (GetDocument()->GetRecursive())
			{
				CString sFullDest(destPath);
				sFullDest += _T("\\");
				if (!di.sSubdir.IsEmpty())
					sFullDest += di.sSubdir + _T("\\");
				sFullDest += di.sfilename;
				fileOp.AddDestFile(sFullDest);
			}
			GetItemFileNames(sel, slFile, srFile);
			fileOp.AddSourceFile(srFile);
		}
	}

	BOOL bSuccess = FALSE;
	BOOL bAPICalled = FALSE;
	BOOL bAborted = FALSE;
	int  nAPIReturn = 0;
	bSuccess = fileOp.Go( &bAPICalled, &nAPIReturn, &bAborted );
}

// Confirm with user, then perform the action list
void CDirView::ConfirmAndPerformActions(ActionList & actionList)
{
	if (!actionList.selcount) // Not sure it is possible to get right-click menu without
		return;    // any selected items, but may as well be safe

	ASSERT(actionList.actions.GetCount()>0); // Or else the update handler got it wrong

	if (!ConfirmActionList(actionList))
		return;

	PerformActionList(actionList);
}

/**
 * @brief Confirm actions with user as appropriate
 * (type, whether single or multiple).
 */
BOOL CDirView::ConfirmActionList(const ActionList & actionList)
{
	// special handling for the single item case, because it is probably the most common,
	// and we can give the user exact details easily for it
	switch(actionList.atype)
	{
	case ActionList::ACT_COPY:
		if (actionList.GetCount()==1)
		{
			const ActionList::action & act = actionList.actions.GetHead();
			if (!ConfirmSingleCopy(act.src, act.dest))
				return FALSE;
		}
		else
		{
			if (!ConfirmMultipleCopy(actionList.GetCount(), actionList.selcount))
				return FALSE;
		}
		break;
		
	// Deleting does not need confirmation, CShellFileOp takes care of it
	case ActionList::ACT_DEL_LEFT:
	case ActionList::ACT_DEL_RIGHT:
	case ActionList::ACT_DEL_BOTH:
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
void CDirView::PerformActionList(ActionList & actionList)
{
	CShellFileOp fileOp;
	CString destPath;
	CString startPath;
	UINT operation = 0;
	UINT operFlags = 0;

	// Reset suppressing VSS dialog for multiple files.
	// Set in CMainFrame::SaveToVersionControl().
	mf->m_CheckOutMulti = FALSE;
	mf->m_bVssSuppressPathCheck = FALSE;

	switch (actionList.atype)
	{
	case ActionList::ACT_COPY:
		operation = FO_COPY;
		operFlags |= FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOCONFIRMATION;
		break;
	case ActionList::ACT_DEL_LEFT:
		operation = FO_DELETE;
		break;
	case ActionList::ACT_DEL_RIGHT:
		operation = FO_DELETE;
		break;
	case ActionList::ACT_DEL_BOTH:
		operation = FO_DELETE;
		break;
	default:
		LogErrorString(_T("Unknown fileoperation in CDirView::PerformActionList()"));
		_RPTF0(_CRT_ERROR, "Unknown fileoperation in CDirView::PerformActionList()");
		break;
	}
	
	// Check option and enable putting deleted items to Recycle Bin
	if (mf->m_options.GetInt(OPT_USE_RECYCLE_BIN) == TRUE)
		operFlags |= FOF_ALLOWUNDO;
	fileOp.SetOperationFlags(operation, this, operFlags);
	
	// Add files/directories
	BOOL bSucceed = TRUE;
	const BOOL bMultiFile = (actionList.actions.GetCount() > 1);
	BOOL bApplyToAll = FALSE;
	POSITION pos = actionList.actions.GetHeadPosition();
	POSITION curPos = pos;
	int nItemCount = 0;
	while (bSucceed && pos != NULL)
	{
		BOOL bSkip = FALSE;
		curPos = pos; // Save current position for later use
		const ActionList::action act = actionList.actions.GetNext(pos);

		// If copying files, try to sync files to VCS too
		if (actionList.atype == ActionList::ACT_COPY && !act.dirflag)
		{
			CString strErr;
			int nRetVal = mf->SyncFileToVCS(act.src, act.dest, bApplyToAll, &strErr);
			if (nRetVal == -1)
			{
				bSucceed = FALSE;
				AfxMessageBox(strErr, MB_OK | MB_ICONERROR);
			}
			else if (nRetVal == IDCANCEL)
				bSucceed = FALSE; // User canceled, so we don't continue
			else if (nRetVal == IDNO)
			{
				actionList.actions.RemoveAt(curPos);
				if (actionList.actions.GetCount() == 0)
					pos = NULL;
				bSkip = TRUE;  // User wants to skip this item
			}
		}

		if (bSucceed && !bSkip) // No error from VCS sync (propably just not called)
		{
			try
			{
				switch (actionList.atype)
				{
				case ActionList::ACT_COPY:
					fileOp.AddSourceFile(act.src);
					fileOp.AddDestFile(act.dest);
					gLog.Write(_T("Copy file(s) from: %s\n\tto: %s"), act.src, act.dest);
					break;
				case ActionList::ACT_DEL_LEFT:
					fileOp.AddSourceFile(act.src);
					gLog.Write(_T("Delete file(s) from LEFT: %s"), act.src);
					break;
				case ActionList::ACT_DEL_RIGHT:
					fileOp.AddSourceFile(act.src);
					gLog.Write(_T("Delete file(s) from RIGHT: %s"), act.src);
					break;
				case ActionList::ACT_DEL_BOTH:
					fileOp.AddSourceFile(act.src);
					fileOp.AddSourceFile(act.dest);
					gLog.Write(_T("Delete BOTH file(s) from: %s\n\tto: %s"), act.src, act.dest);
					break;
				}
			}
			catch (CMemoryException *ex)
			{
				bSucceed = FALSE;
				LogErrorString(_T("CDirView::PerformActionList(): ")
					_T("Adding files to buffer failed!"));
				ex->ReportError();
				ex->Delete();
			}
			if (bSucceed)
				nItemCount++;
		}
	} 

	// Abort if no items to process
	if (nItemCount == 0)
	{
		gLog.Write(_T("Fileoperation aborted, no items to process."));
		return;
	}

	// Now process files/directories that got added to list
	BOOL bOpStarted = FALSE;
	int apiRetVal = 0;
	BOOL bUserCancelled = FALSE;
	BOOL bFileOpSucceed = fileOp.Go(&bOpStarted, &apiRetVal, &bUserCancelled);

	// All succeeded
	if (bFileOpSucceed && !bUserCancelled)
	{
		gLog.Write(_T("Fileoperation succeeded, %d item processed."), nItemCount);
		UpdateCopiedItems(actionList);
		UpdateDeletedItems(actionList);
	}
	else if (!bOpStarted)
	{
		// Invalid parameters - is this programmer error only?
		LogErrorString(_T("Invalid usage of CShellFileOp in ")
			_T("CDirView::PerformActionList()"));
		_RPTF0(_CRT_ERROR, "Invalid usage of CShellFileOp in "
			"CDirView::PerformActionList()");
	}
	else if (bUserCancelled)
	{
		// User cancelled, we have a problem as we don't know which
		// items were processed!
		// User could cancel operation before it was done or during operation
		gLog.Write(LOGLEVEL::LWARNING, _T("User cancelled fileoperation!"));
	}
	else
	{
		// CShellFileOp shows errors to user, so just write log
		LogErrorString(Fmt(_T("File operation failed: %s"),
			GetSysError(GetLastError())));
	}
}

/**
 * @brief Update copied items after fileactions
 */
void CDirView::UpdateCopiedItems(ActionList & actionList)
{
	while (actionList.GetCount()>0)
	{
		ActionList::action act = actionList.actions.RemoveHead();
		POSITION diffpos = GetItemKey(act.idx);
		const DIFFITEM & di = GetDiffContext()->GetDiffAt(diffpos);

		if (actionList.atype == ActionList::ACT_COPY)
		{
			// Copy files and folders
			CDirDoc *pDoc = GetDocument();
			pDoc->SetDiffSide(DIFFCODE::BOTH, act.idx);
			
			// Folders don't have compare flag set!!
			if (act.dirflag)
				pDoc->SetDiffCompare(DIFFCODE::NOCMP, act.idx);
			else
				pDoc->SetDiffCompare(DIFFCODE::SAME, act.idx);
			pDoc->ReloadItemStatus(act.idx);
		}
		else
		{
			// Delete files and folders
			// If both items or unique item is deleted, don't bother updating
			// statuses, just remove from list
			CDirDoc *pDoc = GetDocument();
			if (actionList.atype == ActionList::ACT_DEL_LEFT)
			{
				if (di.isSideLeft())
				{
					actionList.deletedItems.AddTail(act.idx);
				}
				else
				{
					pDoc->SetDiffSide(DIFFCODE::RIGHT, act.idx);
					pDoc->SetDiffCompare(DIFFCODE::NOCMP, act.idx);
					pDoc->ReloadItemStatus(act.idx);
				}
			}
			
			if (actionList.atype == ActionList::ACT_DEL_RIGHT)
			{
				if (di.isSideRight())
				{
					actionList.deletedItems.AddTail(act.idx);
				}
				else
				{
					pDoc->SetDiffSide(DIFFCODE::LEFT, act.idx);
					pDoc->SetDiffCompare(DIFFCODE::NOCMP, act.idx);
					pDoc->ReloadItemStatus(act.idx);
				}
			}

			if (actionList.atype == ActionList::ACT_DEL_BOTH)
			{
				actionList.deletedItems.AddTail(act.idx);
			}
		}
	}
}

/**
 * @brief Update deleted items after fileactions
 */
void CDirView::UpdateDeletedItems(ActionList & actionList)
{
	while (!actionList.deletedItems.IsEmpty())
	{
		int idx = actionList.deletedItems.RemoveTail();
		POSITION diffpos = GetItemKey(idx);
		GetDiffContext()->RemoveDiff(diffpos);
		m_pList->DeleteItem(idx);
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
BOOL CDirView::IsItemCopyableToLeft(const DIFFITEM & di)
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// can't copy same items
	if (di.isResultSame()) return FALSE;
	// impossible if only on left
	if (di.isSideLeft()) return FALSE;

	// everything else can be copied to left
	return TRUE;
}
/// is it possible to copy item to right ?
BOOL CDirView::IsItemCopyableToRight(const DIFFITEM & di)
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// can't copy same items
	if (di.isResultSame()) return FALSE;
	// impossible if only on right
	if (di.isSideRight()) return FALSE;

	// everything else can be copied to right
	return TRUE;
}
/// is it possible to delete left item ?
BOOL CDirView::IsItemDeletableOnLeft(const DIFFITEM & di)
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// impossible if only on right
	if (di.isSideRight()) return FALSE;
	// everything else can be deleted on left
	return TRUE;
}
/// is it possible to delete right item ?
BOOL CDirView::IsItemDeletableOnRight(const DIFFITEM & di)
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// impossible if only on right
	if (di.isSideLeft()) return FALSE;

	// everything else can be deleted on right
	return TRUE;
}
/// is it possible to delete both items ?
BOOL CDirView::IsItemDeletableOnBoth(const DIFFITEM & di)
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// impossible if only on right or left
	if (di.isSideLeft() || di.isSideRight()) return FALSE;

	// everything else can be deleted on both
	return TRUE;
}

/// is it possible to open left item ?
BOOL CDirView::IsItemOpenableOnLeft(const DIFFITEM & di)
{
	// impossible if only on right
	if (di.isSideRight()) return FALSE;

	// everything else can be opened on right
	return TRUE;
}
/// is it possible to open right item ?
BOOL CDirView::IsItemOpenableOnRight(const DIFFITEM & di)
{
	// impossible if only on left
	if (di.isSideLeft()) return FALSE;

	// everything else can be opened on left
	return TRUE;
}
/// is it possible to open left ... item ?
BOOL CDirView::IsItemOpenableOnLeftWith(const DIFFITEM & di)
{
	return (!di.isDirectory() && IsItemOpenableOnLeft(di));
}
/// is it possible to open with ... right item ?
BOOL CDirView::IsItemOpenableOnRightWith(const DIFFITEM & di)
{
	return (!di.isDirectory() && IsItemOpenableOnRight(di));
}
/// is it possible to copy to... left item?
BOOL CDirView::IsItemCopyableToOnLeft(const DIFFITEM & di)
{
	// impossible if only on right
	if (di.isSideRight()) return FALSE;

	// everything else can be copied to from left
	return TRUE;
}
/// is it possible to copy to... right item?
BOOL CDirView::IsItemCopyableToOnRight(const DIFFITEM & di)
{
	// impossible if only on left
	if (di.isSideLeft()) return FALSE;

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
	CString name, pathex;

	POSITION diffpos = GetItemKey(sel);
	if (diffpos == (POSITION)-1)
	{
		strLeft.Empty();
		strRight.Empty();
	}
	else
	{
		const CDiffContext * ctxt = GetDiffContext();
		const DIFFITEM & di = ctxt->GetDiffAt(diffpos);
		CString relpath = paths_ConcatPath(di.sSubdir, di.sfilename);
		strLeft = paths_ConcatPath(ctxt->m_strLeft, relpath);
		strRight = paths_ConcatPath(ctxt->m_strRight, relpath);
	}
}

/// Open selected file on specified side
void CDirView::DoOpen(SIDE_TYPE stype)
{
	int sel = GetSingleSelectedItem();
	if (sel == -1) return;
	CString file = GetSelectedFileName(stype);
	if (file.IsEmpty()) return;
	int rtn = (int)ShellExecute(::GetDesktopWindow(), _T("open"), file, 0, 0, SW_SHOWNORMAL);
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

	mf->OpenFileToExternalEditor(file);
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
		if (!di.isDirectory() && !di.isSideLeft() && !di.isSideRight())
		{
			GetItemFileNames(sel, slFile, srFile);
			CString filteredFilenames = slFile + (CString)_T("|") + srFile;
			GetDocument()->SetPluginPrediffSetting(filteredFilenames, newsetting);
		}
	}
}
