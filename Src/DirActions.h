#pragma once

#include "UnicodeString.h"
#include "DiffContext.h"
#include "FileActionScript.h"
#include "paths.h"
#include "IntToIntMap.h"
#include <algorithm>
#include "FileTransform.h"

class CDiffContext;
class PathContext;
class PluginManager;
class FileActionScript;
class CTempPathContext;

/**
 * @brief Folder compare icon indexes.
 * This `enum` defines indexes for the imagelist used for folder/file compare icons.
 * Note that this enum must be in synch with code in CDirView::OnInitialUpdate() and
 * GetColImage(). Also remember that icons are in resource file...
 */
enum
{
	DIFFIMG_LUNIQUE,
	DIFFIMG_MUNIQUE,
	DIFFIMG_RUNIQUE,
	DIFFIMG_LMISSING,
	DIFFIMG_MMISSING,
	DIFFIMG_RMISSING,
	DIFFIMG_DIFF,
	DIFFIMG_SAME,
	DIFFIMG_FILE,
	DIFFIMG_BINSAME,
	DIFFIMG_BINDIFF,
	DIFFIMG_LDIRUNIQUE,
	DIFFIMG_MDIRUNIQUE,
	DIFFIMG_RDIRUNIQUE,
	DIFFIMG_LDIRMISSING,
	DIFFIMG_MDIRMISSING,
	DIFFIMG_RDIRMISSING,
	DIFFIMG_SKIP,
	DIFFIMG_DIRSKIP,
	DIFFIMG_DIRDIFF,
	DIFFIMG_DIRSAME,
	DIFFIMG_DIR,
	DIFFIMG_ERROR,
	DIFFIMG_DIRUP,
	DIFFIMG_DIRUP_DISABLE,
	DIFFIMG_ABORT,
	DIFFIMG_TEXTDIFF,
	DIFFIMG_TEXTSAME,
};

typedef enum {
	SIDE_LEFT = 1,
	SIDE_MIDDLE,
	SIDE_RIGHT
} SIDE_TYPE;

typedef enum {
	SELECTIONTYPE_NORMAL,
	SELECTIONTYPE_LEFT1LEFT2,
	SELECTIONTYPE_RIGHT1RIGHT2,
	SELECTIONTYPE_LEFT1RIGHT2,
	SELECTIONTYPE_LEFT2RIGHT1
} SELECTIONTYPE;

typedef enum {
	UPDATEITEM_NONE,
	UPDATEITEM_UPDATE,
	UPDATEITEM_REMOVE
} UPDATEITEM_TYPE;

struct AllowUpwardDirectory
{
	enum ReturnCode
	{
		Never,
		No,
		ParentIsRegularPath,
		ParentIsTempPath
	};
};


struct DirViewFilterSettings
{
	template<class GetOptionBool>
	explicit DirViewFilterSettings(GetOptionBool getoptbool)
	{
		show_skipped = getoptbool(OPT_SHOW_SKIPPED);
		show_unique_left = getoptbool(OPT_SHOW_UNIQUE_LEFT);
		show_unique_middle = getoptbool(OPT_SHOW_UNIQUE_MIDDLE);
		show_unique_right = getoptbool(OPT_SHOW_UNIQUE_RIGHT);
		show_binaries = getoptbool(OPT_SHOW_BINARIES);
		show_identical = getoptbool(OPT_SHOW_IDENTICAL);
		show_different = getoptbool(OPT_SHOW_DIFFERENT);
		show_different_left_only = getoptbool(OPT_SHOW_DIFFERENT_LEFT_ONLY);
		show_different_middle_only = getoptbool(OPT_SHOW_DIFFERENT_MIDDLE_ONLY);
		show_different_right_only = getoptbool(OPT_SHOW_DIFFERENT_RIGHT_ONLY);
		show_missing_left_only = getoptbool(OPT_SHOW_MISSING_LEFT_ONLY);
		show_missing_middle_only = getoptbool(OPT_SHOW_MISSING_MIDDLE_ONLY);
		show_missing_right_only = getoptbool(OPT_SHOW_MISSING_RIGHT_ONLY);
		tree_mode = getoptbool(OPT_TREE_MODE);
	};
	bool show_skipped;
	bool show_unique_left;
	bool show_unique_middle;
	bool show_unique_right;
	bool show_binaries;
	bool show_identical;
	bool show_different;
	bool show_different_left_only;
	bool show_different_middle_only;
	bool show_different_right_only;
	bool show_missing_left_only;
	bool show_missing_middle_only;
	bool show_missing_right_only;
	bool tree_mode;
};

typedef std::map<String, bool> DirViewTreeState;

String NumToStr(int n);
String FormatFilesAffectedString(int nFilesAffected, int nFilesTotal);
String FormatMenuItemString(SIDE_TYPE src, int count, int total);
String FormatMenuItemString(SIDE_TYPE src, SIDE_TYPE dst, int count, int total);
String FormatMenuItemStringAll(int nDirs, int count, int total);
String FormatMenuItemString(const String& fmt1, const String& fmt2, int count, int total);
String FormatMenuItemStringTo(SIDE_TYPE src, int count, int total);
String FormatMenuItemStringAllTo(int nDirs, int count, int total);
String FormatMenuItemStringDifferencesTo(int count, int total);

void ConfirmActionList(const CDiffContext& ctxt, const FileActionScript & actionList);
UPDATEITEM_TYPE UpdateDiffAfterOperation(const FileActionItem & act, CDiffContext& ctxt, DIFFITEM &di);

DIFFITEM *FindItemFromPaths(const CDiffContext& ctxt, const PathContext& paths);

bool IsItemCopyable(const DIFFITEM &di, int index);
bool IsItemDeletable(const DIFFITEM &di, int index);
bool IsItemDeletableOnBoth(const CDiffContext& ctxt, const DIFFITEM &di);
bool AreItemsOpenable(const CDiffContext& ctxt, SELECTIONTYPE selectionType, const DIFFITEM &di1, const DIFFITEM &di2);
bool AreItemsOpenable(const CDiffContext& ctxt, const DIFFITEM &di1, const DIFFITEM &di2, const DIFFITEM &di3);
bool IsItemOpenableOn(const DIFFITEM &di, int index);
bool IsItemOpenableOnWith(const DIFFITEM &di, int index);
bool IsItemCopyableToOn(const DIFFITEM &di, int index);
bool IsItemNavigableDiff(const CDiffContext& ctxt, const DIFFITEM &di);
bool IsItemExistAll(const CDiffContext& ctxt, const DIFFITEM &di);
bool IsShowable(const CDiffContext& ctxt, const DIFFITEM &di, const DirViewFilterSettings& filter);

bool GetOpenOneItem(const CDiffContext& ctxt, DIFFITEM *pos1, const DIFFITEM *pdi[3],
		PathContext &paths, int & sel1, bool & isDir, int nPane[3], String& errmsg);
bool GetOpenTwoItems(const CDiffContext& ctxt, SELECTIONTYPE selectionType, DIFFITEM *pos1, DIFFITEM *pos2, const DIFFITEM *pdi[3],
		PathContext &paths, int & sel1, int & sel2, bool & isDir, int nPane[3], String& errmsg);
bool GetOpenThreeItems(const CDiffContext& ctxt, DIFFITEM *pos1, DIFFITEM *pos2, DIFFITEM *pos3, const DIFFITEM *pdi[3],
		PathContext &paths, int & sel1, int & sel2, int & sel3, bool & isDir, int nPane[3], String& errmsg);

void GetItemFileNames(const CDiffContext& ctxt, const DIFFITEM& di, String& strLeft, String& strRight);
PathContext GetItemFileNames(const CDiffContext& ctxt, const DIFFITEM& di);
String GetItemFileName(const CDiffContext& ctx, const DIFFITEM &di, int index);
int GetColImage(const DIFFITEM &di);

void SetDiffStatus(DIFFITEM& di, unsigned  diffcode, unsigned mask);
void SetDiffCompare(DIFFITEM& di, unsigned diffcode);
void SetDiffSide(DIFFITEM& di, unsigned diffcode);
void SetDiffCounts(DIFFITEM& di, unsigned diffs, unsigned ignored);
void SetItemViewFlag(DIFFITEM& di, unsigned flag, unsigned mask);
void SetItemViewFlag(CDiffContext& ctxt, unsigned flag, unsigned mask);
void MarkForRescan(DIFFITEM& di);

bool RenameOnSameDir(const String& szOldFileName, const String& szNewFileName);

void ExpandSubdirs(CDiffContext& ctxt, DIFFITEM& dip);
void ExpandAllSubdirs(CDiffContext &ctxt);
void CollapseAllSubdirs(CDiffContext &ctxt);
DirViewTreeState *SaveTreeState(const CDiffContext& ctxt);
void RestoreTreeState(CDiffContext &ctxt, DirViewTreeState *pTreeState);

AllowUpwardDirectory::ReturnCode
CheckAllowUpwardDirectory(const CDiffContext& ctxt, const CTempPathContext *pTempPathContext, PathContext &pathsParent);

inline int SideToIndex(const CDiffContext& ctxt, SIDE_TYPE stype)
{
	switch (stype)
	{
	case SIDE_MIDDLE: return ctxt.GetCompareDirs() == 3 ? 1 : -1;
	case SIDE_RIGHT: return ctxt.GetCompareDirs() - 1;
	default: return 0;
	}
}

struct ConfirmationNeededException
{
	String m_caption;
	String m_question;
	String m_fromText;
	String m_toText;
	String m_fromPath;
	String m_toPath;
};

struct ContentsChangedException
{
	explicit ContentsChangedException(const String& failpath);
	String m_msg;
};

struct DirActions
{
	typedef bool (DirActions::*method_type2)(const DIFFITEM& di) const;
	typedef FileActionScript *(DirActions::*method_type)(FileActionScript *, const std::pair<int, const DIFFITEM *>& it) const;

	DirActions(const CDiffContext& ctxt, const bool RO[], method_type func = nullptr, method_type2 func2 = nullptr) : 
		m_ctxt(ctxt), m_RO(RO), m_cur_method(func), m_cur_method2(func2) {}

	template <SIDE_TYPE src, SIDE_TYPE dst>
	bool IsItemCopyableOnTo(const DIFFITEM& di) const
	{
		return (di.diffcode.diffcode != 0 && !m_RO[SideToIndex(m_ctxt, dst)] && ::IsItemCopyable(di, SideToIndex(m_ctxt, src)));
	}

	template <SIDE_TYPE src>
	bool IsItemCopyableToOn(const DIFFITEM& di) const
	{
		return (di.diffcode.diffcode != 0 && ::IsItemCopyableToOn(di, SideToIndex(m_ctxt, src)));
	}

	bool IsItemCopyableBothToOn(const DIFFITEM& di) const
	{
		if (di.diffcode.diffcode != 0)
		{
			int i;
			for (i = 0; i < m_ctxt.GetCompareDirs(); ++i)
			{
				if (!::IsItemCopyableToOn(di, i))
					break;
			}
			return (i == m_ctxt.GetCompareDirs());
		}
		return false;
	}

	template <SIDE_TYPE src>
	bool IsItemMovableToOn(const DIFFITEM& di) const
	{
		const int idx = SideToIndex(m_ctxt, src);
		return (di.diffcode.diffcode != 0 && !m_RO[idx] && IsItemDeletable(di, idx) && ::IsItemCopyableToOn(di, idx));
	}

	template <SIDE_TYPE src>
	bool IsItemDeletableOn(const DIFFITEM& di) const
	{ 
		const int idx = SideToIndex(m_ctxt, src);
		return (di.diffcode.diffcode != 0 && !m_RO[idx] && IsItemDeletable(di, idx));
	}

	bool IsItemDeletableOnBoth(const DIFFITEM& di) const
	{
		if (di.diffcode.diffcode != 0)
		{
			int i;
			for (i = 0; i < m_ctxt.GetCompareDirs(); ++i)
			{
				if (m_RO[i] || !IsItemDeletable(di, i))
					break;
			}
			return (i == m_ctxt.GetCompareDirs());
		}
		return false;
	}
	bool IsItemDeletableOnEitherOrBoth(const DIFFITEM& di) const
	{
		if (di.diffcode.diffcode != 0)
		{
			int i;
			for (i = 0; i < m_ctxt.GetCompareDirs(); ++i)
			{
				if (!m_RO[i] && IsItemDeletable(di, i))
					break;
			}
			return (i < m_ctxt.GetCompareDirs());
		}
		return false;
	}

	template <SIDE_TYPE src>
	bool IsItemOpenableOn(const DIFFITEM& di) const
	{
		return (di.diffcode.diffcode != 0 && ::IsItemOpenableOn(di, SideToIndex(m_ctxt, src)));
	}

	template <SIDE_TYPE src>
	bool IsItemOpenableOnWith(const DIFFITEM& di) const
	{
		return (di.diffcode.diffcode != 0 && ::IsItemOpenableOnWith(di, SideToIndex(m_ctxt, src)));
	}

	template <SIDE_TYPE src>
	bool IsParentFolderOpenable(const DIFFITEM& di) const
	{
		return (di.diffcode.diffcode != 0 && di.diffcode.exists(SideToIndex(m_ctxt, src)));
	}

	bool IsItemFile(const DIFFITEM& di) const
	{
		return (di.diffcode.diffcode != 0 && !di.diffcode.isDirectory());
	}

	template <SIDE_TYPE src>
	bool IsItemExist(const DIFFITEM& di) const
	{
		return (di.diffcode.diffcode != 0 && di.diffcode.exists(SideToIndex(m_ctxt, src)));
	}

	template <SIDE_TYPE src>
	bool IsItemEditableEncoding(const DIFFITEM& di) const
	{
		const int index = SideToIndex(m_ctxt, src);
		return (di.diffcode.diffcode != 0 && di.diffcode.exists(index) && di.diffFileInfo[index].IsEditableEncoding());
	}

	bool IsItemNavigableDiff(const DIFFITEM& di) const
	{
		return ::IsItemNavigableDiff(m_ctxt, di);
	}

	FileActionScript *CopyItem(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it, SIDE_TYPE src, SIDE_TYPE dst) const
	{
		const DIFFITEM& di = *it.second;
		const int srcidx = SideToIndex(m_ctxt, src);
		const int dstidx = SideToIndex(m_ctxt, dst);
		if (di.diffcode.diffcode != 0 && !m_RO[dstidx] && IsItemCopyable(di, srcidx))
		{
			FileActionItem act;
			act.src  = GetItemFileName(m_ctxt, di, srcidx);
			act.dest = GetItemFileName(m_ctxt, di, dstidx);
			
			// We must check that paths still exists
			if (paths::DoesPathExist(act.src) == paths::DOES_NOT_EXIST)
				throw ContentsChangedException(act.src);

			act.context = it.first;
			act.dirflag = di.diffcode.isDirectory();
			act.atype = FileAction::ACT_COPY;
			act.UIResult = FileActionItem::UI_SYNC;
			act.UIOrigin = srcidx;
			act.UIDestination = dstidx;
			pscript->AddActionItem(act);
		}
		return pscript;
	}

	template<SIDE_TYPE src, SIDE_TYPE to>
	FileActionScript *Copy(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it) const
	{
		return CopyItem(pscript, it, src, to);
	}

	FileActionScript *DeleteItem(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it, SIDE_TYPE src) const
	{
		const DIFFITEM& di = *it.second;
		const int index = SideToIndex(m_ctxt, src);
		if (di.diffcode.diffcode != 0 && !m_RO[index] && IsItemDeletable(di, index))
		{
			FileActionItem act;
			act.src = GetItemFileName(m_ctxt, di, index);

			// We must check that path still exists
			if (paths::DoesPathExist(act.src) == paths::DOES_NOT_EXIST)
				throw ContentsChangedException(act.src);

			act.context = it.first;
			act.dirflag = di.diffcode.isDirectory();
			act.atype = FileAction::ACT_DEL;
			act.UIOrigin = index;
			act.UIResult = FileActionItem::UI_DEL;
			pscript->AddActionItem(act);
		}
		return pscript;
	}

	template<SIDE_TYPE src>
	FileActionScript *DeleteOn(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it) const
	{
		return DeleteItem(pscript, it, src);
	}

	FileActionScript *DeleteOnBoth(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it) const
	{
		const DIFFITEM& di = *it.second;

		if (di.diffcode.diffcode != 0 && IsItemDeletableOnBoth(di) && 
			(std::count(m_RO, m_RO + m_ctxt.GetCompareDirs(), true) == 0))
		{
			for (int i = 0; i < m_ctxt.GetCompareDirs(); ++i)
			{
				FileActionItem act;
				act.src = GetItemFileName(m_ctxt, di, i);
				// We must first check that paths still exists
				if (paths::DoesPathExist(act.src) == paths::DOES_NOT_EXIST)
					throw ContentsChangedException(act.src);
				act.context = it.first;
				act.dirflag = di.diffcode.isDirectory();
				act.atype = FileAction::ACT_DEL;
				act.UIOrigin = i;
				act.UIResult = FileActionItem::UI_DEL;
				pscript->AddActionItem(act);
			}
		}
		return pscript;
	}

	FileActionScript *DeleteOnEitherOrBoth(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it) const
	{
		const DIFFITEM& di = *it.second;
		if (di.diffcode.diffcode != 0)
		{
			for (int i = 0; i < m_ctxt.GetCompareDirs(); ++i)
			{
				if (IsItemDeletable(di, i) && !m_RO[i])
				{
					FileActionItem act;
					act.src = GetItemFileName(m_ctxt, di, i);
					act.UIResult = FileActionItem::UI_DEL;
					act.dirflag = di.diffcode.isDirectory();
					act.context = it.first;
					act.UIOrigin = i;
					act.atype = FileAction::ACT_DEL;
					pscript->AddActionItem(act);
				}
			}
		}
		return pscript;
	}

	FileActionScript *CopyOrMoveItemTo(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it, FileAction::ACT_TYPE atype, SIDE_TYPE src) const
	{
		const int index = SideToIndex(m_ctxt, src);
		const DIFFITEM& di = *it.second;

		if (di.diffcode.diffcode != 0 && di.diffcode.exists(index) &&
			(atype == FileAction::ACT_MOVE ? (!m_RO[index] && IsItemDeletable(di, index)) : true))
		{
			FileActionItem act;
			act.src = GetItemFileName(m_ctxt, di, index);
			 
			// We must check that path still exists
			if (paths::DoesPathExist(act.src) == paths::DOES_NOT_EXIST)
				throw ContentsChangedException(act.src);

			act.dest = paths::ConcatPath(pscript->m_destBase, di.diffFileInfo[index].GetFile());
			act.dirflag = di.diffcode.isDirectory();
			act.context = it.first;
			act.atype = atype;
			act.UIResult = (atype == FileAction::ACT_COPY) ? FileActionItem::UI_DONT_CARE : FileActionItem::UI_DEL;
			act.UIOrigin = index;
			pscript->AddActionItem(act);
		}
		return pscript;
	}

	template<SIDE_TYPE src>
	FileActionScript *CopyTo(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it) const
	{
		return CopyOrMoveItemTo(pscript, it, FileAction::ACT_COPY, src);
	}

	template<SIDE_TYPE src>
	FileActionScript *MoveTo(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it) const
	{
		return CopyOrMoveItemTo(pscript, it, FileAction::ACT_MOVE, src);
	}

	FileActionScript *operator()(FileActionScript *pscript, const std::pair<int, const DIFFITEM *>& it) const
	{
		return ((*this).*m_cur_method)(pscript, it);
	}

	bool operator()(const DIFFITEM &di) const
	{
		return ((*this).*m_cur_method2)(di);
	}

	method_type m_cur_method;
	method_type2 m_cur_method2;
	const CDiffContext& m_ctxt;
	const bool *m_RO;
};

struct Counts {
	Counts() : count(0), total(0) {}
	Counts(int c, int t): count(c), total(t) {}
	int count;
	int total;
};

template<class InputIterator, class Predicate>
Counts Count(const InputIterator& begin, const InputIterator& end, const Predicate& pred) 
{
	int count = 0, total = 0;
	for (InputIterator it = begin; it != end; ++it)
	{
		if (pred(*it))
			++count;
		++total;
	}
	return Counts(count, total);
}

/**
 * @brief Rename selected item on both left and right sides.
 *
 * @param szNewItemName [in] New item name.
 *
 * @return true if at least one file was renamed successfully.
 */
template<class InputIterator>
bool DoItemRename(InputIterator& it, const CDiffContext& ctxt, const String& szNewItemName)
{
	PathContext paths;
	int nDirs = ctxt.GetCompareDirs();

	assert(it != InputIterator());

	// We must check that paths still exists
	String failpath;
	DIFFITEM &di = *it;
	paths = ::GetItemFileNames(ctxt, di);
	for (int index = 0; index < nDirs; index++)
	{
		if (di.diffcode.exists(index) && paths::DoesPathExist(paths[index]) == paths::DOES_NOT_EXIST)
			throw ContentsChangedException(failpath);
	}

	bool bRename[3] = {false};
	for (int index = 0; index < nDirs; index++)
	{
		if (di.diffcode.exists(index))
			bRename[index] = RenameOnSameDir(paths[index], szNewItemName);
	}

	if (std::count(bRename, bRename + nDirs, true) == 0)
		return false;
	
	di.diffcode.setSideNone();
	for (int index = 0; index < nDirs; index++)
	{
		di.diffFileInfo[index].filename = szNewItemName;
		if (bRename[index] || paths::DoesPathExist(GetItemFileName(ctxt, di, index)))
			di.diffcode.setSideFlag(index);
	}
	return true;
}

template<class InputIterator, class OutputIterator>
OutputIterator CopyPathnames(const InputIterator& begin, const InputIterator& end, OutputIterator result, SIDE_TYPE stype, const CDiffContext& ctxt)
{
	const int index = SideToIndex(ctxt, stype);
	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;
		if (di.diffcode.exists(index))
		{
			*result = GetItemFileName(ctxt, di, index);
			++result;
		}
	}
	return result;
}

template<class InputIterator, class OutputIterator>
OutputIterator CopyBothPathnames(const InputIterator& begin, const InputIterator& end, OutputIterator result, const CDiffContext& ctxt)
{
	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;
		for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		{
			if (di.diffcode.exists(i))
			{
				*result = GetItemFileName(ctxt, di, i);
				++result;
			}
		}
	}
	return result;
}

template<class InputIterator, class OutputIterator>
OutputIterator CopyFilenames(const InputIterator& begin, const InputIterator& end, OutputIterator result)
{
	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;
		if (!di.diffcode.isDirectory())
		{
			*result = di.diffFileInfo[0].filename;
			++result;
		}
	}
	return result;
}

template<class InputIterator, class OutputIterator>
OutputIterator CopyPathnamesForDragAndDrop(const InputIterator& begin, const InputIterator& end, OutputIterator result, const CDiffContext& ctxt)
{
	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;

		// check for special items (e.g not "..")
		if (di.diffcode.diffcode == 0)
			continue;

		for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		{
			if (di.diffcode.exists(i))
			{
				*result = GetItemFileName(ctxt, di, i);
				++result;
			}
		}
	}
	return result;
}

template<class InputIterator, class BinaryFunction>
void ApplyFolderNameAndFileName(const InputIterator& begin, const InputIterator& end, SIDE_TYPE stype,
	const CDiffContext& ctxt, BinaryFunction func)
{
	int index = SideToIndex(ctxt, stype);
	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;
		if (di.diffcode.diffcode == 0) // Invalid value, this must be special item
			continue;
		String filename = di.diffFileInfo[index].filename;
		String currentDir = di.getFilepath(index, ctxt.GetNormalizedPath(index));
		func(currentDir, filename);
	}
}

/**
 * @brief Apply specified setting for prediffing to all selected items
 */
template<class InputIterator>
void ApplyPluginPrediffSetting(const InputIterator& begin, const InputIterator& end, const CDiffContext& ctxt, PLUGIN_MODE newsetting)
{
	// Unlike other group actions, here we don't build an action list
	// to execute; we just apply this change directly
	if( !ctxt.m_bPluginsEnabled || ctxt.m_piPluginInfos == nullptr )
		return;
	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;
		if (!di.diffcode.isDirectory())
		{
			String filteredFilenames;
			for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
			{
				if (di.diffcode.exists(i))
				{
					if (!filteredFilenames.empty()) filteredFilenames += _T("|");
					filteredFilenames += ::GetItemFileName(ctxt, di, i);
				}
			}
			PackingInfo * infoUnpacker = nullptr;
			PrediffingInfo * infoPrediffer = nullptr;
			const_cast<CDiffContext&>(ctxt).FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);
			infoPrediffer->Initialize(newsetting);
		}
	}
}

/**
 * @brief Updates just before displaying plugin context view in list
 */
template<class InputIterator>
std::pair<int, int> CountPredifferYesNo(const InputIterator& begin, const InputIterator& end, const CDiffContext& ctxt)
{
	int nPredifferYes = 0;
	int nPredifferNo = 0;
	if( !ctxt.m_bPluginsEnabled || ctxt.m_piPluginInfos == nullptr ) 
		return std::make_pair(nPredifferYes, nPredifferNo);

	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;
		if (di.diffcode.diffcode == 0) // Invalid value, this must be special item
			continue;

		// note the prediffer flag for 'files present on both sides and not skipped'
		if (!di.diffcode.isDirectory() && !di.diffcode.isBin() && IsItemExistAll(ctxt, di)
			&& !di.diffcode.isResultFiltered())
		{
			PathContext tFiles = GetItemFileNames(ctxt, di);
			String filteredFilenames = strutils::join(tFiles.begin(), tFiles.end(), _T("|"));
			PackingInfo * unpacker;
			PrediffingInfo * prediffer;
			const_cast<CDiffContext&>(ctxt).FetchPluginInfos(filteredFilenames, &unpacker, &prediffer);
			if (prediffer->m_PluginOrPredifferMode == PLUGIN_AUTO || !prediffer->m_PluginName.empty())
				nPredifferYes ++;
			else
				nPredifferNo ++;
		}
	}
	return std::make_pair(nPredifferYes, nPredifferNo);
}

template<class InputIterator>
IntToIntMap CountCodepages(const InputIterator& begin, const InputIterator& end, const CDiffContext& ctxt)
{
	IntToIntMap map;
	for (InputIterator it = begin; it != end; ++it)
	{
		const DIFFITEM& di = *it;
		for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		{
			if (di.diffcode.diffcode != 0 && di.diffcode.exists(i))
				map.Increment(di.diffFileInfo[i].encoding.m_codepage);
		}
	}
	return map;
}

template<class InputIterator>
void ApplyCodepage(const InputIterator& begin, const InputIterator& end, CDiffContext& ctxt, const bool affect[3], int nCodepage)
{
	for (InputIterator it = begin; it != end; ++it)
	{
		DIFFITEM& di = *it;
		if (di.diffcode.diffcode == 0) // Invalid value, this must be special item
			continue;
		if (di.diffcode.isDirectory())
			continue;

		for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
		{
			// Does it exist on left? (ie, right or both)
			if (affect[i] && di.diffcode.exists(i) && di.diffFileInfo[i].IsEditableEncoding())
			{
				di.diffFileInfo[i].encoding.SetCodepage(nCodepage);
			}
		}
	}
}

/// get file name on specified side for first selected item
template<class InputIterator>
String GetSelectedFileName(InputIterator& it, SIDE_TYPE stype, const CDiffContext& ctxt)
{
	if (it == InputIterator())
		return _T("");
	return GetItemFileName(ctxt, *it, SideToIndex(ctxt, stype));
}
