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

static BOOL
IsItemCodeDir(int code)
{
	return code==FILE_LDIRUNIQUE || code==FILE_RDIRUNIQUE;
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
		if (IsItemCopyableToLeft(di.code))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = srFile;
			act.dest = slFile;
			act.idx = sel;
			act.code = di.code;
			act.dirflag = IsItemCodeDir(di.code);
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
		if (IsItemCopyableToRight(di.code))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = slFile;
			act.dest = srFile;
			act.dirflag = IsItemCodeDir(di.code);
			act.idx = sel;
			act.code = di.code;
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
		if (IsItemDeletableOnLeft(di.code))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = slFile;
			act.dirflag = IsItemCodeDir(di.code);
			act.idx = sel;
			act.code = di.code;
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

		if (IsItemDeletableOnRight(di.code))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = srFile;
			act.dirflag = IsItemCodeDir(di.code);
			act.idx = sel;
			act.code = di.code;
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

		if (IsItemDeletableOnBoth(di.code))
		{
			GetItemFileNames(sel, slFile, srFile);
			action act;
			act.src = srFile;
			act.dest = slFile;
			act.dirflag = IsItemCodeDir(di.code);
			act.idx = sel;
			act.code = di.code;
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
				if (act.code == FILE_BINDIFF)
					mf->UpdateCurrentFileStatus(GetDocument(), FILE_BINSAME, act.idx);
				else
					mf->UpdateCurrentFileStatus(GetDocument(), FILE_SAME, act.idx);
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
					BYTE code = GetDiffContext()->GetDiffStatus(diffpos);
					if (IsItemLeftOnly(code))
						actionList.deletedItems.AddTail(act.idx);
					else
						mf->UpdateCurrentFileStatus(GetDocument(), FILE_RUNIQUE, act.idx);
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
					BYTE code = GetDiffContext()->GetDiffStatus(diffpos);
					if (IsItemRightOnly(code))
						actionList.deletedItems.AddTail(act.idx);
					else
						mf->UpdateCurrentFileStatus(GetDocument(), FILE_LUNIQUE, act.idx);
				}
				else
				{
					actionList.errors.AddTail(s);
				}
			}
		}
	}
}

// Get directories of first selected item
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

// Does item exist only on the left ?
BOOL CDirView::IsItemLeftOnly(int code)
{
	return code==FILE_LUNIQUE || code==FILE_LDIRUNIQUE;
}
// Does item exist only on the right ?
BOOL CDirView::IsItemRightOnly(int code)
{
	return code==FILE_RUNIQUE || code==FILE_RDIRUNIQUE;
}

// is it possible to copy item to left ?
BOOL CDirView::IsItemCopyableToLeft(int code)
{
	switch(code)
	{
	case FILE_RUNIQUE:
	case FILE_DIFF:
	case FILE_BINDIFF:
		return TRUE;
	// no point in allowing FILE_SAME
	// TODO: FILE_RDIRUNIQUE: add code for directory copy
	}
	return FALSE;
}
// is it possible to copy item to right ?
BOOL CDirView::IsItemCopyableToRight(int code)
{
	switch(code)
	{
	case FILE_LUNIQUE:
	case FILE_DIFF:
	case FILE_BINDIFF:
		return TRUE;
	// no point in allowing FILE_SAME
	// TODO: FILE_LDIRUNIQUE: add code for directory copy
	}
	return FALSE;
}
// is it possible to delete left item ?
BOOL CDirView::IsItemDeletableOnLeft(int code)
{
	switch(code)
	{
	case FILE_LUNIQUE:
	case FILE_DIFF:
	case FILE_BINSAME:
	case FILE_BINDIFF:
	case FILE_SAME:
	case FILE_LDIRUNIQUE:
		return TRUE;
	}
	return FALSE;
}
// is it possible to delete right item ?
BOOL CDirView::IsItemDeletableOnRight(int code)
{
	switch(code)
	{
	case FILE_RUNIQUE:
	case FILE_DIFF:
	case FILE_BINSAME:
	case FILE_BINDIFF:
	case FILE_SAME:
	case FILE_RDIRUNIQUE:
		return TRUE;
	}
	return FALSE;
}
// is it possible to delete both items ?
BOOL CDirView::IsItemDeletableOnBoth(int code)
{
	switch(code)
	{
	case FILE_DIFF:
	case FILE_BINSAME:
	case FILE_BINDIFF:
	case FILE_SAME:
		return TRUE;
	}
	return FALSE;
}

// is it possible to open left item ?
BOOL CDirView::IsItemOpenableOnLeft(int code)
{
	switch(code)
	{
	case FILE_LUNIQUE:
	case FILE_LDIRUNIQUE:
	case FILE_DIFF:
	case FILE_BINSAME:
	case FILE_BINDIFF:
	case FILE_SAME:
		return TRUE;
	}
	return FALSE;
}
// is it possible to open right item ?
BOOL CDirView::IsItemOpenableOnRight(int code)
{
	switch(code)
	{
	case FILE_RUNIQUE:
	case FILE_RDIRUNIQUE:
	case FILE_DIFF:
	case FILE_BINSAME:
	case FILE_BINDIFF:
	case FILE_SAME:
		return TRUE;
	}
	return FALSE;
}
// is it possible to open left ... item ?
BOOL CDirView::IsItemOpenableOnLeftWith(int code)
{
	switch(code)
	{
	case FILE_LUNIQUE:
	case FILE_DIFF:
	case FILE_BINSAME:
	case FILE_BINDIFF:
	case FILE_SAME:
		return TRUE;
	}
	return FALSE;
}
// is it possible to open with ... right item ?
BOOL CDirView::IsItemOpenableOnRightWith(int code)
{
	switch(code)
	{
	case FILE_RUNIQUE:
	case FILE_DIFF:
	case FILE_BINSAME:
	case FILE_BINDIFF:
	case FILE_SAME:
		return TRUE;
	}
	return FALSE;
}

// get the file names on both sides for first selected item
BOOL CDirView::GetSelectedFileNames(CString& strLeft, CString& strRight) const
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel == -1)
		return FALSE;
	GetItemFileNames(sel, strLeft, strRight);
	return TRUE;
}
// get file name on specified side for first selected item
CString CDirView::GetSelectedFileName(SIDE_TYPE stype) const
{
	CString left, right;
	if (!GetSelectedFileNames(left, right)) return _T("");
	return stype==SIDE_LEFT ? left : right;
}

// get the file names on both sides for specified item
void CDirView::GetItemFileNames(int sel, CString& strLeft, CString& strRight) const
{
	const CDirDoc *pd = GetDocument();
	CString name, pathex;

	POSITION diffpos = GetItemKey(sel);
	const CDiffContext * ctxt = GetDiffContext();
	const DIFFITEM & di = ctxt->GetDiffAt(diffpos);

	strLeft = di.getLeftFilepath();
	strRight = di.getRightFilepath();
}

// Open selected file on specified side
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

// Open with dialog for file on selected side
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
