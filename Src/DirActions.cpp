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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Prompt user to confirm a multiple item copy
static BOOL ConfirmMultipleCopy(int count, int total)
{
	CString s;
	ASSERT(count>1);
	AfxFormatString2(s, IDS_CONFIRM_COPY2DIR, NumToStr(count), NumToStr(total));
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION);
	return (rtn==IDYES);
}

// Prompt user to confirm a single item copy
static BOOL ConfirmSingleCopy(LPCTSTR src, LPCTSTR dest)
{
	CString s;
	AfxFormatString2(s, IDS_CONFIRM_COPY_SINGLE, src, dest);
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION);
	return (rtn==IDYES);
}

// Prompt user to confirm a multiple item delete
static BOOL ConfirmMultipleDelete(int count, int total)
{
	CString s;
	AfxFormatString2(s, IDS_CONFIRM_DELETE_ITEMS, NumToStr(count), NumToStr(total));
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION);
	return (rtn==IDYES);
}

// Prompt user to confirm a single item delete
static BOOL ConfirmSingleDelete(LPCTSTR filepath)
{
	CString s;
	AfxFormatString1(s, IDS_CONFIRM_DELETE_SINGLE, filepath);
	int rtn = AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION);
	return (rtn==IDYES);
}

// Prompt & copy item from right to left, if legal
void CDirView::DoCopyFileToLeft()
{
	ActionList actionList(ACT_COPY);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (IsItemCopyableToLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = srFile;
			act.dest = slFile;
			act.idx = sel;
			act.code = di.diffcode;
			act.dirflag = di.isDirectory();
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	ConfirmAndPerformActions(actionList);
}
// Prompt & copy item from left to right, if legal
void CDirView::DoCopyFileToRight()
{
	ActionList actionList(ACT_COPY);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (IsItemCopyableToRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = slFile;
			act.dest = srFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	ConfirmAndPerformActions(actionList);
}

// Prompt & delete left, if legal
void CDirView::DoDelLeft()
{
	ActionList actionList(ACT_DEL_LEFT);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (IsItemDeletableOnLeft(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = slFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	ConfirmAndPerformActions(actionList);
}
// Prompt & delete right, if legal
void CDirView::DoDelRight()
{
	ActionList actionList(ACT_DEL_RIGHT);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (IsItemDeletableOnRight(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = srFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	ConfirmAndPerformActions(actionList);
}
// Prompt & delete both, if legal
void CDirView::DoDelBoth()
{
	ActionList actionList(ACT_DEL_BOTH);
	int sel=-1;
	CString slFile, srFile;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);

		if (IsItemDeletableOnBoth(di))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = srFile;
			act.dest = slFile;
			act.dirflag = di.isDirectory();
			act.idx = sel;
			act.code = di.diffcode;
			actionList.actions.AddTail(act);
		}
		++actionList.selcount;
	}

	ConfirmAndPerformActions(actionList);
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

// Confirm actions with user as appropriate (type, whether single or multiple)
BOOL CDirView::ConfirmActionList(const ActionList & actionList)
{
	// special handling for the single item case, because it is probably the most common,
	// and we can give the user exact details easily for it
	switch(actionList.atype)
	{
	case ACT_COPY:
		if (actionList.GetCount()==1)
		{
			const action & act = actionList.actions.GetHead();
			if (!ConfirmSingleCopy(act.src, act.dest))
				return FALSE;
		}
		else
		{
			if (!ConfirmMultipleCopy(actionList.GetCount(), actionList.selcount))
				return FALSE;
		}
		break;
	default: // deletes
		if (actionList.GetCount()==1 && actionList.atype!=ACT_DEL_BOTH)
		{
			const action & act = actionList.actions.GetHead();
			if (!ConfirmSingleDelete(act.src))
				return FALSE;
		}
		else
		{
			if (!ConfirmMultipleDelete(actionList.GetCount(), actionList.selcount))
				return FALSE;
		}
		break;
	}
	return TRUE;
}

// Perform an array of actions
void CDirView::PerformActionList(ActionList & actionList)
{
	// Set mainframe variable (VSS):
	mf->m_CheckOutMulti = FALSE;
	mf->m_bVCProjSync = TRUE;

	while (actionList.GetCount()>0)
	{
		PerformAndRemoveTopAction(actionList);
	}

	// must process deleted items in reverse order
	// ie, work our way *up* the display list
	// so we don't invalidate indices of items not done yet
	while (!actionList.deletedItems.IsEmpty())
	{
		int idx = actionList.deletedItems.RemoveTail();
		POSITION diffpos = GetItemKey(idx);
		GetDiffContext()->RemoveDiff(diffpos);
		m_pList->DeleteItem(idx);
	}

	if (!actionList.errors.IsEmpty())
	{
		CString sTitle, sText;
		AfxFormatString1(sTitle, IDS_ERRORS_TITLE, NumToStr(actionList.errors.GetCount()));
		while (!actionList.errors.IsEmpty())
		{
			sText += actionList.errors.RemoveHead() + _T("\r\n\r\n");
		}
		OutputBox(sTitle, sText);
	}
}

static void NormalizeFilepath(CString * pstr)
{
	pstr->Replace('/', '\\');
}

void CDirView::PerformAndRemoveTopAction(ActionList & actionList)
{
	action act = actionList.actions.RemoveHead();
	if (actionList.atype == ACT_COPY)
	{
		// copy
		if (act.dirflag)
		{
			// TODO: copy directory
			// add new entries to an append list to be added to list at end
		}
		else
		{
			CString s;
			// copy single file, and update status immediately
			if (mf->SyncFiles(act.src, act.dest, &s))
			{
				GetDocument()->SetDiffSide(DIFFCODE::BOTH, act.idx);
				GetDocument()->SetDiffCompare(DIFFCODE::SAME, act.idx);
				GetDocument()->ReloadItemStatus(act.idx);
			}
			else
			{
				actionList.errors.AddTail(s);
			}
		}
	}
	else
	{
		// delete
		if (act.dirflag)
		{
			// delete directory
			CString s;
			if (DeleteDirSilently(act.src, &s))
			{
				ASSERT(actionList.atype != ACT_DEL_BOTH); // directories are all unique
				actionList.deletedItems.AddTail(act.idx);
			}
			else
			{
				actionList.errors.AddTail(s);
			}
		}
		else
		{
			// delete file
			CString s;
			if (actionList.atype==ACT_DEL_LEFT || actionList.atype==ACT_DEL_BOTH)
			{
				CString sFile = act.src;
				if (DeleteFileSilently(sFile, &s))
				{
					// figure out if copy on right
					POSITION diffpos = GetItemKey(act.idx);
					const DIFFITEM & di = GetDiffContext()->GetDiffAt(diffpos);
					if (di.isSideLeft())
					{
						actionList.deletedItems.AddTail(act.idx);
					}
					else
					{
						GetDocument()->SetDiffSide(DIFFCODE::RIGHT, act.idx);
						GetDocument()->SetDiffCompare(DIFFCODE::NOCMP, act.idx);
						GetDocument()->ReloadItemStatus(act.idx);
					}
				}
				else
				{
					actionList.errors.AddTail(s);
				}
			}
			s=_T("");
			if (actionList.atype==ACT_DEL_RIGHT || actionList.atype==ACT_DEL_BOTH)
			{
				CString sFile = act.dest.IsEmpty() ? act.src : act.dest;
				if (DeleteFileSilently(sFile, &s))
				{
					// figure out if copy on right
					POSITION diffpos = GetItemKey(act.idx);
					const DIFFITEM & di = GetDiffContext()->GetDiffAt(diffpos);
					if (di.isSideRight())
					{
						actionList.deletedItems.AddTail(act.idx);
					}
					else
					{
						GetDocument()->SetDiffSide(DIFFCODE::LEFT, act.idx);
						GetDocument()->SetDiffCompare(DIFFCODE::NOCMP, act.idx);
						GetDocument()->ReloadItemStatus(act.idx);
					}
				}
				else
				{
					actionList.errors.AddTail(s);
				}
			}
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
BOOL CDirView::IsItemCopyableToLeft(const DIFFITEM & di)
{
	// don't let them mess with error items
	if (di.isResultError()) return FALSE;
	// no directory copying right now
	if (di.isDirectory()) return FALSE; 
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
	// no directory copying right now
	if (di.isDirectory()) return FALSE; 
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

/// get the file names on both sides for specified item
void CDirView::GetItemFileNames(int sel, CString& strLeft, CString& strRight) const
{
	CString name, pathex;

	POSITION diffpos = GetItemKey(sel);
	const CDiffContext * ctxt = GetDiffContext();
	const DIFFITEM & di = ctxt->GetDiffAt(diffpos);

	CString relpath = paths_ConcatPath(di.sSubdir, di.sfilename);
	strLeft = paths_ConcatPath(ctxt->m_strLeft, relpath);
	strRight = paths_ConcatPath(ctxt->m_strRight, relpath);
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