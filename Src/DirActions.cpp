/////////////////////////////////////////////////////////////////////////////
//    see Merge.cpp for license (GPLv2+) statement
//
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file DirActions.cpp
 *
 *  @brief Implementation of methods of CDirView that copy/move/delete files
 */

// It would be nice to make this independent of the UI (CDirView)
// but it needs access to the list of selected items.
// One idea would be to provide an iterator over them.
//

#include "pch.h"
#include "DirActions.h"
#include "MergeApp.h"
#include "UnicodeString.h"
#include "7zCommon.h"
#include "ShellFileOperations.h"
#include "DiffItem.h"
#include "FileActionScript.h"
#include "locality.h"
#include "FileFilterHelper.h"
#include "DebugNew.h"

static void ThrowConfirmCopy(const CDiffContext& ctxt, int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide);
static void ThrowConfirmMove(const CDiffContext& ctxt, int origin, int destination, int count,
		const String& src, const String& dest, bool destIsSide);
static void ThrowConfirmationNeededException(const CDiffContext& ctxt, const String &caption, const String &question,
		int origin, int destination, size_t count,
		const String& src, const String& dest, bool destIsSide);

ContentsChangedException::ContentsChangedException(const String& failpath)
	: m_msg(strutils::format_string1(
	        _("Operation aborted!\n\nFolder contents at disks has changed, path\n%1\nwas not found.\n\nPlease refresh the compare."),
	        failpath))
{
}

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
static void ThrowConfirmCopy(const CDiffContext& ctxt, int origin, int destination, size_t count,
		const String& src, const String& dest, bool destIsSide)
{
	String caption = _("Confirm Copy");
	String strQuestion = count == 1 ? _("Are you sure you want to copy:") : 
		strutils::format(_("Are you sure you want to copy %d items:").c_str(), count);

	ThrowConfirmationNeededException(ctxt, caption, strQuestion, origin,
		destination, count,	src, dest, destIsSide);
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
static void ThrowConfirmMove(const CDiffContext& ctxt, int origin, int destination, size_t count,
		const String& src, const String& dest, bool destIsSide)
{
	String caption = _("Confirm Move");
	String strQuestion = count == 1 ? _("Are you sure you want to move:") : 
		strutils::format(_("Are you sure you want to move %d items:").c_str(), count);

	ThrowConfirmationNeededException(ctxt, caption, strQuestion, origin,
		destination, count,	src, dest, destIsSide);
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
static void ThrowConfirmationNeededException(const CDiffContext& ctxt, const String &caption, const String &question,
		int origin, int destination, size_t count,
		const String& src, const String& dest, bool destIsSide)
{
	ConfirmationNeededException exp;
	String sOrig;
	String sDest;
	
	exp.m_caption = caption;
	
	if (origin == 0)
		sOrig = _("From left:");
	else if (origin == ctxt.GetCompareDirs() - 1)
		sOrig = _("From right:");
	else
		sOrig = _("From middle:");

	if (destIsSide)
	{
		// Copy to left / right
		if (destination == 0)
			sDest = _("To left:");
		else if (destination == ctxt.GetCompareDirs() - 1)
			sDest = _("To right:");
		else
			sDest = _("To middle:");
	}
	else
	{
		// Copy left/right to..
		sDest = _("To:");
	}

	String strSrc(src);
	if (paths::DoesPathExist(src) == paths::IS_EXISTING_DIR)
		strSrc = paths::AddTrailingSlash(src);
	String strDest(dest);
	if (paths::DoesPathExist(dest) == paths::IS_EXISTING_DIR)
		strDest = paths::AddTrailingSlash(dest);

	exp.m_question = question;
	exp.m_fromText = sOrig;
	exp.m_toText = sDest;
	exp.m_fromPath = strSrc;
	exp.m_toPath = strDest;

	throw exp;
}

/**
 * @brief Confirm actions with user as appropriate
 * (type, whether single or multiple).
 */
void ConfirmActionList(const CDiffContext& ctxt, const FileActionScript & actionList)
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
			ThrowConfirmCopy(ctxt, item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), item.src, item.dest,
				bDestIsSide);
		}
		else
		{
			String src = ctxt.GetPath(item.UIOrigin);
			String dst;

			if (bDestIsSide)
			{
				dst = ctxt.GetPath(item.UIDestination);
			}
			else
			{
				if (!actionList.m_destBase.empty())
					dst = actionList.m_destBase;
				else
					dst = item.dest;
			}

			ThrowConfirmCopy(ctxt, item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), src, dst, bDestIsSide);
		}
		break;
		
	case FileAction::ACT_DEL:
		break;

	case FileAction::ACT_MOVE:
		bDestIsSide = false;
		if (actionList.GetActionItemCount() == 1)
		{
			ThrowConfirmMove(ctxt, item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), item.src, item.dest,
				bDestIsSide);
		}
		else
		{
			String src = ctxt.GetPath(item.UIOrigin);;
			String dst;

			if (!actionList.m_destBase.empty())
				dst = actionList.m_destBase;
			else
				dst = item.dest;

			ThrowConfirmMove(ctxt, item.UIOrigin, item.UIDestination,
				actionList.GetActionItemCount(), src, dst, bDestIsSide);
		}
		break;

	// Invalid operation
	default: 
		LogErrorString(_T("Unknown fileoperation in CDirView::ConfirmActionList()"));
		throw "Unknown fileoperation in ConfirmActionList()";
		break;
	}
}

/**
 * @brief Update results for FileActionItem.
 * This functions is called to update DIFFITEM after FileActionItem.
 * @param [in] act Action that was done.
 * @param [in] pos List position for DIFFITEM affected.
 */
UPDATEITEM_TYPE UpdateDiffAfterOperation(const FileActionItem & act, CDiffContext& ctxt, DIFFITEM &di)
{
	bool bUpdateSrc  = false;
	bool bUpdateDest = false;
	bool bRemoveItem = false;

	// Use FileActionItem types for simplicity for now.
	// Better would be to use FileAction contained, since it is not
	// UI dependent.
	switch (act.UIResult)
	{
	case FileActionItem::UI_SYNC:
		bUpdateSrc = true;
		bUpdateDest = true;
		di.diffcode.setSideFlag(act.UIDestination);
		if (act.dirflag || ctxt.GetCompareDirs() > 2)
			SetDiffCompare(di, DIFFCODE::NOCMP);
		else
			SetDiffCompare(di, DIFFCODE::SAME);
		SetDiffCounts(di, 0, 0);
		break;

	case FileActionItem::UI_DEL:
		if (di.diffcode.isSideOnly(act.UIOrigin))
		{
			bRemoveItem = true;
		}
		else
		{
			di.diffcode.unsetSideFlag(act.UIOrigin);
			SetDiffCompare(di, DIFFCODE::NOCMP);
			bUpdateSrc = true;
		}
		break;
	}

	if (bUpdateSrc)
		ctxt.UpdateStatusFromDisk(&di, act.UIOrigin);
	if (bUpdateDest)
		ctxt.UpdateStatusFromDisk(&di, act.UIDestination);

	if (bRemoveItem)
		return UPDATEITEM_REMOVE;
	if (bUpdateSrc | bUpdateDest)
		return UPDATEITEM_UPDATE;
	return UPDATEITEM_NONE;
}

/**
 * @brief Find the CDiffContext diffpos of an item from its left & right paths
 * @return POSITION to item, `nullptr` if not found.
 * @note Filenames must be same, if they differ `nullptr` is returned.
 */
DIFFITEM *FindItemFromPaths(const CDiffContext& ctxt, const PathContext& paths)
{
	int nBuffer;
	String file[3], path[3], base;
	for (nBuffer = 0; nBuffer < paths.GetSize(); ++nBuffer)
	{
		String p = paths[nBuffer];
		file[nBuffer] = paths::FindFileName(p);
		if (file[nBuffer].empty())
			return 0;
		// Path can contain (because of difftools?) '/' and '\'
		// so for comparing purposes, convert whole path to use '\\'
		path[nBuffer] = paths::ToWindowsPath(String(p, 0, p.length() - file[nBuffer].length())); // include trailing backslash
		base = ctxt.GetPath(nBuffer); // include trailing backslash
		if (path[nBuffer].compare(0, base.length(), base.c_str()) != 0)
			return 0;
		path[nBuffer].erase(0, base.length()); // turn into relative path
		if (String::size_type length = path[nBuffer].length())
			path[nBuffer].resize(length - 1); // remove trailing backslash
	}

	// Filenames must be identical
	if (std::count(file, file + paths.GetSize(), file[0]) < paths.GetSize())
		return 0;

	DIFFITEM *pos = ctxt.GetFirstDiffPosition();
	if (paths.GetSize() == 2)
	{
		while (DIFFITEM *currentPos = pos) // Save our current pos before getting next
		{
			const DIFFITEM &di = ctxt.GetNextDiffPosition(pos);
			if (di.diffFileInfo[0].path == path[0] &&
				di.diffFileInfo[1].path == path[1] &&
				di.diffFileInfo[0].filename == file[0] &&
				di.diffFileInfo[1].filename == file[1])
			{
				return currentPos;
			}
		}
	}
	else
	{
		while (DIFFITEM *currentPos = pos) // Save our current pos before getting next
		{
			const DIFFITEM &di = ctxt.GetNextDiffPosition(pos);
			if (di.diffFileInfo[0].path == path[0] &&
				di.diffFileInfo[1].path == path[1] &&
				di.diffFileInfo[2].path == path[2] &&
				di.diffFileInfo[0].filename == file[0] &&
				di.diffFileInfo[1].filename == file[1] &&
				di.diffFileInfo[2].filename == file[2])
			{
				return currentPos;
			}
		}
	}
	return 0;
}

/// is it possible to copy item to left ?
bool IsItemCopyable(const DIFFITEM &di, int index)
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// can't copy same items
	if (di.diffcode.isResultSame()) return false;
	// impossible if not existing
	if (!di.diffcode.exists(index)) return false;
	// everything else can be copied to other side
	return true;
}

/// is it possible to delete item ?
bool IsItemDeletable(const DIFFITEM &di, int index)
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// impossible if not existing
	if (!di.diffcode.exists(index)) return false;
	// everything else can be deleted
	return true;
}

/// is it possible to delete both items ?
bool IsItemDeletableOnBoth(const CDiffContext& ctxt, const DIFFITEM &di)
{
	// don't let them mess with error items
	if (di.diffcode.isResultError()) return false;
	// impossible if only on right or left
	for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		if (!di.diffcode.exists(i)) return false;

	// everything else can be deleted on both
	return true;
}

/// is it possible to compare these two items?
bool AreItemsOpenable(const CDiffContext& ctxt, SELECTIONTYPE selectionType, const DIFFITEM &di1, const DIFFITEM &di2)
{
	String sLeftBasePath = ctxt.GetPath(0);
	String sRightBasePath = ctxt.GetPath(1);

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
		if (di1.diffcode.exists(0) && di2.diffcode.exists(0))
			return true;
		break;
	case SELECTIONTYPE_RIGHT1RIGHT2:
		if (di1.diffcode.exists(1) && di2.diffcode.exists(1))
			return true;
		break;
	case SELECTIONTYPE_LEFT1RIGHT2:
		if (di1.diffcode.exists(0) && di2.diffcode.exists(1))
			return true;
		break;
	case SELECTIONTYPE_LEFT2RIGHT1:
		if (di1.diffcode.exists(1) && di2.diffcode.exists(0))
			return true;
		break;
	}

	// Allow to compare items if left & right path refer to same directory
	// (which means there is effectively two files involved). No need to check
	// side flags. If files weren't on both sides, we'd have no DIFFITEMs.
	if (strutils::compare_nocase(sLeftBasePath, sRightBasePath) == 0)
		return true;

	return false;
}
/// is it possible to compare these three items?
bool AreItemsOpenable(const CDiffContext& ctxt, const DIFFITEM &di1, const DIFFITEM &di2, const DIFFITEM &di3)
{
	String sLeftBasePath = ctxt.GetPath(0);
	String sMiddleBasePath = ctxt.GetPath(1);
	String sRightBasePath = ctxt.GetPath(2);
	String sLeftPath1 = paths::ConcatPath(di1.getFilepath(0, sLeftBasePath), di1.diffFileInfo[0].filename);
	String sLeftPath2 = paths::ConcatPath(di2.getFilepath(0, sLeftBasePath), di2.diffFileInfo[0].filename);
	String sLeftPath3 = paths::ConcatPath(di3.getFilepath(0, sLeftBasePath), di3.diffFileInfo[0].filename);
	String sMiddlePath1 = paths::ConcatPath(di1.getFilepath(1, sMiddleBasePath), di1.diffFileInfo[1].filename);
	String sMiddlePath2 = paths::ConcatPath(di2.getFilepath(1, sMiddleBasePath), di2.diffFileInfo[1].filename);
	String sMiddlePath3 = paths::ConcatPath(di3.getFilepath(1, sMiddleBasePath), di3.diffFileInfo[1].filename);
	String sRightPath1 = paths::ConcatPath(di1.getFilepath(2, sRightBasePath), di1.diffFileInfo[2].filename);
	String sRightPath2 = paths::ConcatPath(di2.getFilepath(2, sRightBasePath), di2.diffFileInfo[2].filename);
	String sRightPath3 = paths::ConcatPath(di3.getFilepath(2, sRightBasePath), di3.diffFileInfo[2].filename);
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
	if (di1.diffcode.exists(0) && di2.diffcode.exists(1) && di3.diffcode.exists(2))
		return true;
	if (di1.diffcode.exists(0) && di2.diffcode.exists(2) && di3.diffcode.exists(1))
		return true;
	if (di1.diffcode.exists(1) && di2.diffcode.exists(0) && di3.diffcode.exists(2))
		return true;
	if (di1.diffcode.exists(1) && di2.diffcode.exists(2) && di3.diffcode.exists(0))
		return true;
	if (di1.diffcode.exists(2) && di2.diffcode.exists(0) && di3.diffcode.exists(1))
		return true;
	if (di1.diffcode.exists(2) && di2.diffcode.exists(1) && di3.diffcode.exists(0))
		return true;

	// Allow to compare items if left & right path refer to same directory
	// (which means there is effectively two files involved). No need to check
	// side flags. If files weren't on both sides, we'd have no DIFFITEMs.
	if (strutils::compare_nocase(sLeftBasePath, sMiddleBasePath) == 0 && strutils::compare_nocase(sLeftBasePath, sRightBasePath) == 0)
		return true;

	return false;
}
/// is it possible to open item ?
bool IsItemOpenableOn(const DIFFITEM &di, int index)
{
	// impossible if not existing
	if (!di.diffcode.exists(index)) return false;

	// everything else can be opened on right
	return true;
}

/// is it possible to open left ... item ?
bool IsItemOpenableOnWith(const DIFFITEM &di, int index)
{
	return (!di.diffcode.isDirectory() && IsItemOpenableOn(di, index));
}
/// is it possible to copy to... left item?
bool IsItemCopyableToOn(const DIFFITEM &di, int index)
{
	// impossible if only on right
	if (!di.diffcode.exists(index)) return false;

	// everything else can be copied to from left
	return true;
}

// When navigating differences, do we stop at this one ?
bool IsItemNavigableDiff(const CDiffContext& ctxt, const DIFFITEM &di)
{
	// Not a valid diffitem, one of special items (e.g "..")
	if (di.diffcode.diffcode == 0)
		return false;
	if (di.diffcode.isResultFiltered() || di.diffcode.isResultError())
		return false;
	if (!di.diffcode.isResultDiff() && IsItemExistAll(ctxt, di))
		return false;
	return true;
}

bool IsItemExistAll(const CDiffContext& ctxt, const DIFFITEM &di)
{
	// Not a valid diffitem, one of special items (e.g "..")
	if (di.diffcode.diffcode == 0)
		return false;
	return di.diffcode.existAll();
}


/**
 * @brief Determines if the user wants to see given item.
 * This function determines what items to show and what items to hide. There
 * are lots of combinations, but basically we check if menuitem is enabled or
 * disabled and show/hide matching items. For non-recursive compare we never
 * hide folders as that would disable user browsing into them. And we even
 * don't really know if folders are identical or different as we haven't
 * compared them.
 * @param [in] di Item to check.
 * @return true if item should be shown, false if not.
 * @sa CDirDoc::Redisplay()
 */
bool IsShowable(const CDiffContext& ctxt, const DIFFITEM &di, const DirViewFilterSettings& filter)
{
	if (di.customFlags & ViewCustomFlags::HIDDEN)
		return false;

	if (di.diffcode.isResultFiltered())
	{
		// Treat SKIPPED as a 'super'-flag. If item is skipped and user
		// wants to see skipped items show item regardless of other flags
		return filter.show_skipped;
	}

	if (di.diffcode.isDirectory())
	{
		// Subfolders in non-recursive compare can only be skipped or unique
		if (!ctxt.m_bRecursive)
		{
			// left/right filters
			if (di.diffcode.isSideFirstOnly() && !filter.show_unique_left)
				return false;
			if (ctxt.GetCompareDirs() < 3)
			{
				if (di.diffcode.isSideSecondOnly() && !filter.show_unique_right)
					return false;
			}
			else
			{
				if (di.diffcode.isSideSecondOnly() && !filter.show_unique_middle)
					return false;
				if (di.diffcode.isSideThirdOnly() && !filter.show_unique_right)
					return false;
				if (di.diffcode.isMissingFirstOnly() && !filter.show_missing_left_only)
					return false;
				if (di.diffcode.isMissingSecondOnly() && !filter.show_missing_middle_only)
					return false;
				if (di.diffcode.isMissingThirdOnly() && !filter.show_missing_right_only)
					return false;
			}

			// result filters
			if (di.diffcode.isResultError() && false/* !GetMainFrame()->m_bShowErrors FIXME:*/)
				return false;
		}
		else // recursive mode (including tree-mode)
		{
			// left/right filters
			if (di.diffcode.isSideFirstOnly() && !filter.show_unique_left)
				return false;
			if (ctxt.GetCompareDirs() < 3)
			{
				if (di.diffcode.isSideSecondOnly() && !filter.show_unique_right)
					return false;
			}
			else
			{
				if (di.diffcode.isSideSecondOnly() && !filter.show_unique_middle)
					return false;
				if (di.diffcode.isSideThirdOnly() && !filter.show_unique_right)
					return false;
				if (di.diffcode.isMissingFirstOnly() && !filter.show_missing_left_only)
					return false;
				if (di.diffcode.isMissingSecondOnly() && !filter.show_missing_middle_only)
					return false;
				if (di.diffcode.isMissingThirdOnly() && !filter.show_missing_right_only)
					return false;
			}

			// ONLY filter folders by result (identical/different) for tree-view.
			// In the tree-view we show subfolders with identical/different
			// status. The flat view only shows files inside folders. So if we
			// filter by status the files inside folder are filtered too and
			// users see files appearing/disappearing without clear logic.		
			if (filter.tree_mode)
			{
				// result filters
				if (di.diffcode.isResultError() && false/* !GetMainFrame()->m_bShowErrors FIXME:*/)
					return false;

				// result filters
				if (di.diffcode.isResultSame() && !filter.show_identical)
					return false;
				if (di.diffcode.isResultDiff() && !filter.show_different)
				{
					DIFFITEM *diffpos = ctxt.GetFirstChildDiffPosition(&di);
					while (diffpos != nullptr)
					{
						const DIFFITEM &dic = ctxt.GetNextSiblingDiffPosition(diffpos);
						if (IsShowable(ctxt, dic, filter))
							return true;
					}
					return false;
				}
			}
		}
	}
	else
	{
		// left/right filters
		if (di.diffcode.isSideFirstOnly() && !filter.show_unique_left)
			return false;
		if (di.diffcode.isSideSecondOnly() && !filter.show_unique_right)
			return false;
		if (ctxt.GetCompareDirs() > 2)
		{
			if (di.diffcode.isMissingFirstOnly() && !filter.show_missing_left_only)
				return false;
			if (di.diffcode.isMissingSecondOnly() && !filter.show_missing_middle_only)
				return false;
			if (di.diffcode.isMissingThirdOnly() && !filter.show_missing_right_only)
				return false;
		}

		// file type filters
		if (di.diffcode.isBin() && !filter.show_binaries)
			return false;

		// result filters
		if (di.diffcode.isResultSame() && !filter.show_identical)
			return false;
		if (di.diffcode.isResultError() && false/* && !GetMainFrame()->m_bShowErrors FIXME:*/)
			return false;
		if (ctxt.GetCompareDirs() < 3)
		{
			if (di.diffcode.isResultDiff() && !filter.show_different)
				return false;
		}
		else
		{
			if ((di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY) == DIFFCODE::DIFF1STONLY)
			{
				if (!filter.show_different_left_only)
					return false;
			}
			else if ((di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY) == DIFFCODE::DIFF2NDONLY)
			{
				if (!filter.show_different_middle_only)
					return false;
			}
			else if ((di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY) == DIFFCODE::DIFF3RDONLY)
			{
				if (!filter.show_different_right_only)
					return false;
			}
			else if (di.diffcode.isResultDiff() && !filter.show_different)
				return false;
		}
	}
	return true;
}

/**
 * @brief Open one selected item.
 * @param [in] pos1 Item position.
 * @param [in,out] di1 Pointer to first diffitem.
 * @param [in,out] di2 Pointer to second diffitem.
 * @param [in,out] di3 Pointer to third diffitem.
 * @param [out] paths First/Second/Third paths.
 * @param [out] sel1 Item's selection index in listview.
 * @param [in,out] isDir Is item folder?
 * return false if there was error or item was completely processed.
 */
bool GetOpenOneItem(const CDiffContext& ctxt, DIFFITEM *pos1, const DIFFITEM *pdi[3],
		PathContext & paths, int & sel1, bool & isdir, int nPane[3], String& errmsg)
{
	pdi[0] = &ctxt.GetDiffAt(pos1);
	pdi[1] = pdi[0];
	pdi[2] = pdi[0];

	paths = GetItemFileNames(ctxt, *pdi[0]);

	for (int nIndex = 0; nIndex < paths.GetSize(); ++nIndex)
		nPane[nIndex] = nIndex;

	if (pdi[0]->diffcode.isDirectory())
		isdir = true;

	if (isdir && (pdi[0]->diffcode.existsFirst() && pdi[1]->diffcode.existsSecond() && pdi[2]->diffcode.existsThird()))
	{
		// Check both folders exist. If either folder is missing that means
		// folder has been changed behind our back, so we just tell user to
		// refresh the compare.
		paths::PATH_EXISTENCE path1Exists = paths::DoesPathExist(paths[0]);
		paths::PATH_EXISTENCE path2Exists = paths::DoesPathExist(paths[1]);
		if (path1Exists != paths::IS_EXISTING_DIR || path2Exists != paths::IS_EXISTING_DIR)
		{
			String invalid = path1Exists == paths::IS_EXISTING_DIR ? paths[0] : paths[1];
			errmsg = strutils::format_string1(
				_("Operation aborted!\n\nFolder contents at disks has changed, path\n%1\nwas not found.\n\nPlease refresh the compare."),
				invalid);
			return false;
		}
	}

	return true;
}

/**
 * @brief Open two selected items.
 * @param [in] pos1 First item position.
 * @param [in] pos2 Second item position.
 * @param [in,out] di1 Pointer to first diffitem.
 * @param [in,out] di2 Pointer to second diffitem.
 * @param [out] paths First/Second/Third paths.
 * @param [out] sel1 First item's selection index in listview.
 * @param [out] sel2 Second item's selection index in listview.
 * @param [in,out] isDir Is item folder?
 * return false if there was error or item was completely processed.
 */
bool GetOpenTwoItems(const CDiffContext& ctxt, SELECTIONTYPE selectionType, DIFFITEM *pos1, DIFFITEM *pos2, const DIFFITEM *pdi[3],
		PathContext & paths, int & sel1, int & sel2, bool & isDir, int nPane[3], String& errmsg)
{
	// Two items selected, get their info
	pdi[0] = &ctxt.GetDiffAt(pos1);
	pdi[1] = &ctxt.GetDiffAt(pos2);
	nPane[0] = 0;
	nPane[1] = 1;

	// Check for binary & side compatibility & file/dir compatibility
	if (!AreItemsOpenable(ctxt, selectionType, *pdi[0], *pdi[1]))
	{
		return false;
	}

	switch (selectionType)
	{
	case SELECTIONTYPE_NORMAL:
		// Ensure that di1 is on left (swap if needed)
		if (pdi[0]->diffcode.isSideSecondOnly() || (pdi[0]->diffcode.isSideBoth() &&
				pdi[1]->diffcode.isSideFirstOnly()))
		{
			std::swap(pdi[0], pdi[1]);
			std::swap(sel1, sel2);
		}
		break;
	case SELECTIONTYPE_LEFT1LEFT2:
		nPane[0] = nPane[1] = 0;
		break;
	case SELECTIONTYPE_RIGHT1RIGHT2:
		nPane[0] = nPane[1] = 1;
		break;
	case SELECTIONTYPE_LEFT1RIGHT2:
		break;
	case SELECTIONTYPE_LEFT2RIGHT1:
		std::swap(pdi[0], pdi[1]);
		std::swap(sel1, sel2);
		break;
	}

	PathContext files1, files2;
	files1 = GetItemFileNames(ctxt, *pdi[0]);
	files2 = GetItemFileNames(ctxt, *pdi[1]);
	paths.SetLeft(files1[nPane[0]]);
	paths.SetRight(files2[nPane[1]]);

	if (pdi[0]->diffcode.isDirectory())
	{
		isDir = true;
		if (paths::GetPairComparability(paths) != paths::IS_EXISTING_DIR)
		{
			errmsg = _("The selected folder is invalid.");
			return false;
		}
	}

	return true;
}

/**
 * @brief Open three selected items.
 * @param [in] pos1 First item position.
 * @param [in] pos2 Second item position.
 * @param [in] pos3 Third item position.
 * @param [in,out] di1 Pointer to first diffitem.
 * @param [in,out] di2 Pointer to second diffitem.
 * @param [in,out] di3 Pointer to third diffitem.
 * @param [out] paths First/Second/Third paths.
 * @param [out] sel1 First item's selection index in listview.
 * @param [out] sel2 Second item's selection index in listview.
 * @param [out] sel3 Third item's selection index in listview.
 * @param [in,out] isDir Is item folder?
 * return false if there was error or item was completely processed.
 */
bool GetOpenThreeItems(const CDiffContext& ctxt, DIFFITEM *pos1, DIFFITEM *pos2, DIFFITEM *pos3, const DIFFITEM *pdi[3],
	PathContext & paths, int & sel1, int & sel2, int & sel3, bool & isDir, int nPane[3], String& errmsg)
{
	String pathLeft, pathMiddle, pathRight;

	// FIXME:
	for (int nIndex = 0; nIndex < 3; ++nIndex)
		nPane[nIndex] = nIndex;
	if (pos3 == 0)
	{
		// Two items selected, get their info
		pdi[0] = &ctxt.GetDiffAt(pos1);
		pdi[1] = &ctxt.GetDiffAt(pos2);

		// Check for binary & side compatibility & file/dir compatibility
		if (!::AreItemsOpenable(ctxt, *pdi[0], *pdi[1], *pdi[1]) && 
			!::AreItemsOpenable(ctxt, *pdi[0], *pdi[0], *pdi[1]))
		{
			return false;
		}
		// Ensure that pdi[0] is on left (swap if needed)
		if (pdi[0]->diffcode.exists(0) && pdi[0]->diffcode.exists(1) && pdi[1]->diffcode.exists(2))
		{
			pdi[2] = pdi[1];
			pdi[1] = pdi[0];
			sel3 = sel2;
			sel2 = sel1;
		}
		else if (pdi[0]->diffcode.exists(0) && pdi[0]->diffcode.exists(2) && pdi[1]->diffcode.exists(1))
		{
			pdi[2] = pdi[0];
			sel3 = sel1;
		}
		else if (pdi[0]->diffcode.exists(1) && pdi[0]->diffcode.exists(2) && pdi[1]->diffcode.exists(0))
		{
			std::swap(pdi[0], pdi[1]);
			std::swap(sel1, sel2);
			pdi[2] = pdi[1];
			sel3 = sel2;
		}
		else if (pdi[1]->diffcode.exists(0) && pdi[1]->diffcode.exists(1) && pdi[0]->diffcode.exists(2))
		{
			std::swap(pdi[0], pdi[1]);
			std::swap(sel1, sel2);
			pdi[2] = pdi[1];
			pdi[1] = pdi[0];
			sel3 = sel2;
			sel2 = sel1;
		}
		else if (pdi[1]->diffcode.exists(0) && pdi[1]->diffcode.exists(2) && pdi[0]->diffcode.exists(1))
		{
			std::swap(pdi[0], pdi[1]);
			std::swap(sel1, sel2);
			pdi[2] = pdi[0];
			sel3 = sel1;
		}
		else if (pdi[1]->diffcode.exists(1) && pdi[1]->diffcode.exists(2) && pdi[0]->diffcode.exists(0))
		{
			pdi[2] = pdi[1];
			sel3 = sel2;
		}
	}
	else
	{
		// Three items selected, get their info
		pdi[0] = &ctxt.GetDiffAt(pos1);
		pdi[1] = &ctxt.GetDiffAt(pos2);
		pdi[2] = &ctxt.GetDiffAt(pos3);

		// Check for binary & side compatibility & file/dir compatibility
		if (!::AreItemsOpenable(ctxt, *pdi[0], *pdi[1], *pdi[2]))
		{
			return false;
		}
		// Ensure that pdi[0] is on left (swap if needed)
		if (pdi[0]->diffcode.exists(0) && pdi[1]->diffcode.exists(1) && pdi[2]->diffcode.exists(2))
		{
		}
		else if (pdi[0]->diffcode.exists(0) && pdi[1]->diffcode.exists(2) && pdi[2]->diffcode.exists(1))
		{
			std::swap(pdi[1], pdi[2]);
			std::swap(sel2, sel3);
		}
		else if (pdi[0]->diffcode.exists(1) && pdi[1]->diffcode.exists(0) && pdi[2]->diffcode.exists(2))
		{
			std::swap(pdi[0], pdi[1]);
			std::swap(sel1, sel2);
		}
		else if (pdi[0]->diffcode.exists(1) && pdi[1]->diffcode.exists(2) && pdi[2]->diffcode.exists(0))
		{
			std::swap(pdi[0], pdi[2]);
			std::swap(sel1, sel3);
			std::swap(pdi[1], pdi[2]);
			std::swap(sel2, sel3);
		}
		else if (pdi[0]->diffcode.exists(2) && pdi[1]->diffcode.exists(0) && pdi[2]->diffcode.exists(1))
		{
			std::swap(pdi[0], pdi[1]);
			std::swap(sel1, sel2);
			std::swap(pdi[1], pdi[2]);
			std::swap(sel2, sel3);
		}
		else if (pdi[0]->diffcode.exists(2) && pdi[1]->diffcode.exists(1) && pdi[2]->diffcode.exists(0))
		{
			std::swap(pdi[0], pdi[2]);
			std::swap(sel1, sel3);
		}
	}

	// Fill in pathLeft & & pathMiddle & pathRight
	PathContext pathsTemp = GetItemFileNames(ctxt, *pdi[0]);
	pathLeft = pathsTemp[0];
	pathsTemp = GetItemFileNames(ctxt, *pdi[1]);
	pathMiddle = pathsTemp[1];
	pathsTemp = GetItemFileNames(ctxt, *pdi[2]);
	pathRight = pathsTemp[2];

	paths.SetLeft(pathLeft);
	paths.SetMiddle(pathMiddle);
	paths.SetRight(pathRight);

	if (pdi[0]->diffcode.isDirectory())
	{
		isDir = true;
		if (paths::GetPairComparability(paths) != paths::IS_EXISTING_DIR)
		{
			errmsg = _("The selected folder is invalid.");
			return false;
		} 
	}

	return true;
}

/**
 * @brief Get the file names on both sides for specified item.
 * @note Return empty strings if item is special item.
 */
void GetItemFileNames(const CDiffContext& ctxt, const DIFFITEM &di, String& strLeft, String& strRight)
{
	const String leftrelpath = paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename);
	const String rightrelpath = paths::ConcatPath(di.diffFileInfo[1].path, di.diffFileInfo[1].filename);
	const String & leftpath = ctxt.GetPath(0);
	const String & rightpath = ctxt.GetPath(1);
	strLeft = paths::ConcatPath(leftpath, leftrelpath);
	strRight = paths::ConcatPath(rightpath, rightrelpath);
}

String GetItemFileName(const CDiffContext& ctxt, const DIFFITEM &di, int index)
{
	return paths::ConcatPath(ctxt.GetPath(index), paths::ConcatPath(di.diffFileInfo[index].path, di.diffFileInfo[index].filename));
}

PathContext GetItemFileNames(const CDiffContext& ctxt, const DIFFITEM &di)
{
	PathContext paths;
	for (int nIndex = 0; nIndex < ctxt.GetCompareDirs(); nIndex++)
	{
		const String relpath = paths::ConcatPath(di.diffFileInfo[nIndex].path, di.diffFileInfo[nIndex].filename);
		const String & path = ctxt.GetPath(nIndex);
		paths.SetPath(nIndex, paths::ConcatPath(path, relpath));
	}
	return paths;
}

/**
 * @brief Return image index appropriate for this row
 */
int GetColImage(const DIFFITEM &di)
{
	// Must return an image index into image list created above in OnInitDialog
	if (di.diffcode.isResultError())
		return DIFFIMG_ERROR;
	if (di.diffcode.isResultAbort())
		return DIFFIMG_ABORT;
	if (di.diffcode.isResultFiltered())
		return (di.diffcode.isDirectory() ? DIFFIMG_DIRSKIP : DIFFIMG_SKIP);
	if (di.diffcode.isSideFirstOnly())
		return (di.diffcode.isDirectory() ? DIFFIMG_LDIRUNIQUE : DIFFIMG_LUNIQUE);
	if (di.diffcode.isSideSecondOnly())
		return ((di.diffcode.diffcode & DIFFCODE::THREEWAY) == 0 ? 
			(di.diffcode.isDirectory() ? DIFFIMG_RDIRUNIQUE : DIFFIMG_RUNIQUE) :
			(di.diffcode.isDirectory() ? DIFFIMG_MDIRUNIQUE : DIFFIMG_MUNIQUE));
	if (di.diffcode.isSideThirdOnly())
		return (di.diffcode.isDirectory() ? DIFFIMG_RDIRUNIQUE : DIFFIMG_RUNIQUE);
	if ((di.diffcode.diffcode & DIFFCODE::THREEWAY) != 0)
	{
		if (!di.diffcode.exists(0))
			return (di.diffcode.isDirectory() ? DIFFIMG_LDIRMISSING : DIFFIMG_LMISSING);
		if (!di.diffcode.exists(1))
			return (di.diffcode.isDirectory() ? DIFFIMG_MDIRMISSING : DIFFIMG_MMISSING);
		if (!di.diffcode.exists(2))
			return (di.diffcode.isDirectory() ? DIFFIMG_RDIRMISSING : DIFFIMG_RMISSING);
	}
	if (di.diffcode.isResultSame())
	{
		if (di.diffcode.isDirectory())
			return DIFFIMG_DIRSAME;
		else
		{
			if (di.diffcode.isText())
				return DIFFIMG_TEXTSAME;
			else if (di.diffcode.isBin())
				return DIFFIMG_BINSAME;
			else
				return DIFFIMG_SAME;
		}
	}
	// diff
	if (di.diffcode.isResultDiff())
	{
		if (di.diffcode.isDirectory())
			return DIFFIMG_DIRDIFF;
		else
		{
			if (di.diffcode.isText())
				return DIFFIMG_TEXTDIFF;
			else if (di.diffcode.isBin())
				return DIFFIMG_BINDIFF;
			else
				return DIFFIMG_DIFF;
		}
	}
	return (di.diffcode.isDirectory() ? DIFFIMG_DIR : DIFFIMG_FILE );
}

/**
 * @brief Set side status of diffitem
 * @note This does not update UI - ReloadItemStatus() does
 * @sa CDirDoc::ReloadItemStatus()
 */
void SetDiffSide(DIFFITEM& di, unsigned diffcode)
{
	SetDiffStatus(di, diffcode, DIFFCODE::SIDEFLAGS);
}

/**
 * @brief Set compare status of diffitem
 * @note This does not update UI - ReloadItemStatus() does
 * @sa CDirDoc::ReloadItemStatus()
 */
void SetDiffCompare(DIFFITEM& di, unsigned diffcode)
{
	SetDiffStatus(di, diffcode, DIFFCODE::COMPAREFLAGS);
}

/**
 * @brief Set status for diffitem
 * @param diffcode New code
 * @param mask Defines allowed set of flags to change
 * @param idx Item's index to list in UI
 */
void SetDiffStatus(DIFFITEM& di, unsigned  diffcode, unsigned mask)
{
	// TODO: Why is the update broken into these pieces ?
	// Someone could figure out these pieces and probably simplify this.

	// Update DIFFITEM code (comparison result)
	assert( ((~mask) & diffcode) == 0 ); // make sure they only set flags in their mask
	di.diffcode.diffcode &= (~mask); // remove current data
	di.diffcode.diffcode |= diffcode; // add new data

	// update DIFFITEM time (and other disk info), and tell views
}

void SetDiffCounts(DIFFITEM& di, unsigned diffs, unsigned ignored)
{
	di.nidiffs = ignored; // see StoreDiffResult() in DirScan.cpp
	di.nsdiffs = diffs;
}

/**
 * @brief Set item's view-flag.
 * @param [in] key Item fow which flag is set.
 * @param [in] flag Flag value to set.
 * @param [in] mask Mask for possible flag values.
 */
void SetItemViewFlag(DIFFITEM& di, unsigned flag, unsigned mask)
{
	unsigned curFlags = di.customFlags;
	curFlags &= ~mask; // Zero bits masked
	curFlags |= flag;
	di.customFlags = curFlags;
}

/**
 * @brief Set all item's view-flag.
 * @param [in] flag Flag value to set.
 * @param [in] mask Mask for possible flag values.
 */
void SetItemViewFlag(CDiffContext& ctxt, unsigned flag, unsigned mask)
{
	DIFFITEM *pos = ctxt.GetFirstDiffPosition();

	while (pos != nullptr)
	{
		unsigned curFlags = ctxt.GetCustomFlags1(pos);
		curFlags &= ~mask; // Zero bits masked
		curFlags |= flag;
		ctxt.SetCustomFlags1(pos, curFlags);
		ctxt.GetNextDiffPosition(pos);
	}
}

/**
 * @brief Mark selected items as needing for rescan.
 * @return Count of items to rescan.
 */
void MarkForRescan(DIFFITEM &di)
{
	SetDiffStatus(di, 0, DIFFCODE::TEXTFLAGS | DIFFCODE::SIDEFLAGS | DIFFCODE::COMPAREFLAGS);
	SetDiffStatus(di, DIFFCODE::NEEDSCAN, DIFFCODE::SCANFLAGS);
}

/**
 * @brief Return string such as "15 of 30 Files Affected" or "30 Files Affected"
 */
String FormatFilesAffectedString(int nFilesAffected, int nFilesTotal)
{
	if (nFilesAffected == nFilesTotal)
		return strutils::format_string1(_("(%1 Files Affected)"), NumToStr(nFilesTotal));
	else
		return strutils::format_string2(_("(%1 of %2 Files Affected)"), NumToStr(nFilesAffected), NumToStr(nFilesTotal));
}

String FormatMenuItemString(const String& fmt1, const String& fmt2, int count, int total)
{
	if (count == total)
		return strutils::format_string1(fmt1, NumToStr(total));
	else
		return strutils::format_string2(fmt2, NumToStr(count), NumToStr(total));
}

String FormatMenuItemString(SIDE_TYPE src, SIDE_TYPE dst, int count, int total)
{
	String fmt1, fmt2;
	if (src == SIDE_LEFT && dst == SIDE_RIGHT)
	{
		fmt1 = _("Left to Right (%1)");
		fmt2 = _("Left to Right (%1 of %2)");
	}
	else if (src == SIDE_LEFT && dst == SIDE_MIDDLE)
	{
		fmt1 = _("Left to Middle (%1)");
		fmt2 = _("Left to Middle (%1 of %2)");
	}
	else if (src == SIDE_MIDDLE && dst == SIDE_LEFT)
	{
		fmt1 = _("Middle to Left (%1)");
		fmt2 = _("Middle to Left (%1 of %2)");
	}
	else if (src == SIDE_MIDDLE && dst == SIDE_RIGHT)
	{
		fmt1 = _("Middle to Right (%1)");
		fmt2 = _("Middle to Right (%1 of %2)");
	}
	else if (src == SIDE_RIGHT && dst == SIDE_LEFT)
	{
		fmt1 = _("Right to Left (%1)");
		fmt2 = _("Right to Left (%1 of %2)");
	}
	else if (src == SIDE_RIGHT && dst == SIDE_MIDDLE)
	{
		fmt1 = _("Right to Middle (%1)");
		fmt2 = _("Right to Middle (%1 of %2)");
	}
	return FormatMenuItemString(fmt1, fmt2, count, total);
}

String FormatMenuItemString(SIDE_TYPE src, int count, int total)
{
	String fmt1, fmt2;
	if (src == SIDE_LEFT)
	{
		fmt1 = _("Left (%1)");
		fmt2 = _("Left (%1 of %2)");
	}
	else if (src == SIDE_MIDDLE)
	{
		fmt1 = _("Middle (%1)");
		fmt2 = _("Middle (%1 of %2)");
	}
	else if (src == SIDE_RIGHT)
	{
		fmt1 = _("Right (%1)");
		fmt2 = _("Right (%1 of %2)");
	}
	return FormatMenuItemString(fmt1, fmt2, count, total);
}

String FormatMenuItemStringAll(int nDirs, int count, int total)
{
	if (nDirs < 3)
		return FormatMenuItemString(_("Both (%1)"), _("Both (%1 of %2)"), count, total);
	else
		return FormatMenuItemString(_("All (%1)"), _("All (%1 of %2)"), count, total);
}

String FormatMenuItemStringTo(SIDE_TYPE src, int count, int total)
{
	String fmt1, fmt2;
	if (src == SIDE_LEFT)
	{
		fmt1 = _("Left to... (%1)");
		fmt2 = _("Left to... (%1 of %2)");
	}
	else if (src == SIDE_MIDDLE)
	{
		fmt1 = _("Middle to... (%1)");
		fmt2 = _("Middle to... (%1 of %2)");
	}
	else if (src == SIDE_RIGHT)
	{
		fmt1 = _("Right to... (%1)");
		fmt2 = _("Right to... (%1 of %2)");
	}
	return FormatMenuItemString(fmt1, fmt2, count, total);
}

String FormatMenuItemStringAllTo(int nDirs, int count, int total)
{
	if (nDirs < 3)
		return FormatMenuItemString(_("Both to... (%1)"), _("Both to... (%1 of %2)"), count, total);
	else
		return FormatMenuItemString(_("All to... (%1)"), _("All to... (%1 of %2)"), count, total);
}

String FormatMenuItemStringDifferencesTo(int count, int total)
{
	return FormatMenuItemString(_("Differences to... (%1)"), _("Differences to... (%1 of %2)"), count, total);
}

/**
 * @brief Rename a file without moving it to different directory.
 *
 * @param szOldFileName [in] Full path of file to rename.
 * @param szNewFileName [in] New file name (without the path).
 *
 * @return true if file was renamed successfully.
 */
bool RenameOnSameDir(const String& szOldFileName, const String& szNewFileName)
{
	bool bSuccess = false;

	if (paths::DOES_NOT_EXIST != paths::DoesPathExist(szOldFileName))
	{
		String sFullName = paths::ConcatPath(paths::GetPathOnly(szOldFileName), szNewFileName);

		// No need to rename if new file already exist.
		if ((sFullName != szOldFileName) ||
			(paths::DOES_NOT_EXIST == paths::DoesPathExist(sFullName)))
		{
			ShellFileOperations fileOp;
			fileOp.SetOperation(FO_RENAME, 0);
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
 * @brief Convert number to string.
 * Converts number to string, with commas between digits in
 * locale-appropriate manner.
*/
String NumToStr(int n)
{
	return locality::NumToLocaleStr(n);
}

void ExpandSubdirs(CDiffContext& ctxt, DIFFITEM& dip)
{
	dip.customFlags |= ViewCustomFlags::EXPANDED;
	DIFFITEM *diffpos = ctxt.GetFirstChildDiffPosition(&dip);
	while (diffpos != nullptr)
	{
		DIFFITEM &di = ctxt.GetNextDiffRefPosition(diffpos);
		if (!di.IsAncestor(&dip))
			break;
		if (di.HasChildren())
			di.customFlags |= ViewCustomFlags::EXPANDED;
	}
}

void ExpandAllSubdirs(CDiffContext& ctxt)
{
	DIFFITEM *diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM &di = ctxt.GetNextDiffRefPosition(diffpos);
		di.customFlags |= ViewCustomFlags::EXPANDED;
	}
}

void CollapseAllSubdirs(CDiffContext& ctxt)
{
	DIFFITEM *diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM &di = ctxt.GetNextDiffRefPosition(diffpos);
		di.customFlags &= ~ViewCustomFlags::EXPANDED;
	}
}

DirViewTreeState *SaveTreeState(const CDiffContext& ctxt)
{
	DirViewTreeState *pTreeState = new DirViewTreeState();
	DIFFITEM *diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		const DIFFITEM &di = ctxt.GetNextDiffPosition(diffpos);
		if (di.HasChildren())
		{
			String relpath = paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename);
			pTreeState->insert(std::pair<String, bool>(relpath, !!(di.customFlags & ViewCustomFlags::EXPANDED)));
		}
	}
	return pTreeState;
}

void RestoreTreeState(CDiffContext& ctxt, DirViewTreeState *pTreeState)
{
	DIFFITEM *diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos != nullptr)
	{
		DIFFITEM &di = ctxt.GetNextDiffRefPosition(diffpos);
		if (di.HasChildren())
		{
			String relpath = paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename);
			std::map<String, bool>::iterator p = pTreeState->find(relpath);
			if (p != pTreeState->end())
			{
				di.customFlags &= ~ViewCustomFlags::EXPANDED;
				di.customFlags |= (p->second ? ViewCustomFlags::EXPANDED : 0);
			}
		}
	}
}

/**
 * @brief Tell if user may use ".." and move to parents directory.
 * This function checks if current compare's parent folders should
 * be allowed to open. I.e. if current compare folders are:
 * - C:\Work\Project1 and
 * - C:\Work\Project2
 * we check if C:\Work and C:\Work should be allowed to opened.
 * For regular folders we allow opening if both folders exist.
 * @param [out] leftParent Left parent folder to open.
 * @param [out] rightParent Right parent folder to open.
 * @return Info if opening parent folders should be enabled:
 * - No : upward RESTRICTED
 * - ParentIsRegularPath : upward ENABLED
 * - ParentIsTempPath : upward ENABLED
 */
AllowUpwardDirectory::ReturnCode
CheckAllowUpwardDirectory(const CDiffContext& ctxt, const CTempPathContext *pTempPathContext, PathContext &pathsParent)
{
	std::vector<String> path(ctxt.GetCompareDirs());
	for (int i = 0; i < static_cast<int>(path.size()); ++i)
		path[i] = ctxt.GetNormalizedPath(i);

	// If we have temp context it means we are comparing archives
	if (pTempPathContext != nullptr)
	{
		std::vector<String> name(path.size());
		for (int i = 0; i < static_cast<int>(path.size()); ++i)
			name[i] = paths::FindFileName(path[i]);

		String::size_type cchLeftRoot = pTempPathContext->m_strRoot[0].length();
		if (path[0].length() <= cchLeftRoot)
		{
			pathsParent.SetSize(ctxt.GetCompareDirs());
			if (pTempPathContext->m_pParent)
			{
				for (int i = 0; i < static_cast<int>(path.size()); ++i)
					pathsParent[i] = pTempPathContext->m_pParent->m_strRoot[i];
				if (paths::GetPairComparability(pathsParent) != paths::IS_EXISTING_DIR)
					return AllowUpwardDirectory::Never;
				return AllowUpwardDirectory::ParentIsTempPath;
			}
			for (int i = 0; i < static_cast<int>(path.size()); ++i)
				pathsParent[i] = pTempPathContext->m_strDisplayRoot[i];
			if (pathsParent.GetSize() < 3)
			{
				if (!ctxt.m_piFilterGlobal->includeFile(pathsParent[0], pathsParent[1]))
					return AllowUpwardDirectory::Never;
			}
			else
			{
				if (!ctxt.m_piFilterGlobal->includeFile(pathsParent[0], pathsParent[1], pathsParent[2]))
					return AllowUpwardDirectory::Never;
			}
			if (path.size() == 2 && 
				strutils::compare_nocase(name[0], _T("ORIGINAL")) == 0 && 
				strutils::compare_nocase(name[1], _T("ALTERED")) == 0)
			{
				for (int i = 0; i < static_cast<int>(path.size()); ++i)
					pathsParent[i] = paths::GetParentPath(pathsParent[i]);
				for (int i = 0; i < static_cast<int>(path.size()); ++i)
					name[i] = paths::FindFileName(pathsParent[i]);
				if (strutils::compare_nocase(name[0], name[1]) == 0)
				{
					if (paths::GetPairComparability(pathsParent) != paths::IS_EXISTING_DIR)
						return AllowUpwardDirectory::Never;
					return AllowUpwardDirectory::ParentIsTempPath;
				}
			}
			return AllowUpwardDirectory::No;
		}
	}

	// If regular parent folders exist, allow opening them
	pathsParent.SetSize(ctxt.GetCompareDirs());
	for (int i = 0; i < static_cast<int>(path.size()); ++i)
		pathsParent[i] = paths::GetParentPath(path[i]);
	if (paths::GetPairComparability(pathsParent) != paths::IS_EXISTING_DIR)
		return AllowUpwardDirectory::Never;
	return AllowUpwardDirectory::ParentIsRegularPath;
}
