#pragma once

#include "UnicodeString.h"

class PathContext;
struct IMergeDoc;
class CTempPathContext;
struct FileLocation;

struct IDirDoc
{
	virtual void InitCompare(const PathContext& paths, bool bRecursive, CTempPathContext*) = 0;
	virtual void SetHiddenItems(const std::vector<String>& hiddenItems) = 0;
	virtual void SetReportFile(const String& sReportFile) = 0;
	virtual void SetDescriptions(const String strDesc[]) = 0;
	virtual void SetTitle(const tchar_t* pszTitle) = 0;
	virtual void SetReadOnly(int nIndex, bool bRO) = 0;
	virtual void Rescan() = 0;
	virtual void AddMergeDoc(IMergeDoc* pMergeDoc) = 0;
	virtual void MergeDocClosing(IMergeDoc* pMergeDoc) = 0;
	virtual bool CloseMergeDocs() = 0;
	virtual bool IsArchiveFolders() const = 0;
	virtual bool HasDiffs() const = 0;
	virtual void UpdateChangedItem(const PathContext& paths, unsigned nDiffs, unsigned nTrivialDiffs, bool bIdentical) = 0;
	virtual void ApplyDisplayRoot(int nIndex, String& sText) = 0;
	virtual bool CompareFilesIfFilesAreLarge(int nFiles, const FileLocation ifileloc[]) = 0;
	virtual bool MoveableToNextDiff() = 0;
	virtual bool MoveableToPrevDiff() = 0;
	virtual void MoveToPrevDiff(IMergeDoc* pMergeDoc) = 0;
	virtual void MoveToNextDiff(IMergeDoc* pMergeDoc) = 0;
	virtual void MoveToFirstFile(IMergeDoc* pMergeDoc) = 0;
	virtual void MoveToLastFile(IMergeDoc* pMergeDoc) = 0;
	virtual void MoveToNextFile(IMergeDoc* pMergeDoc) = 0;
	virtual void MoveToPrevFile(IMergeDoc* pMergeDoc) = 0;
	virtual bool IsFirstFile() = 0;
	virtual bool IsLastFile() = 0;
};

