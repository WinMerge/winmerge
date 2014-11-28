/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DirDoc.h
 *
 * @brief Declaration file for CDirDoc
 *
 */
// ID line follows -- this is updated by SVN
// $Id: DirDoc.h 6136 2008-12-01 17:04:25Z kimmov $

#if !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_
#pragma once

#include <memory>
#include "DiffThread.h"
#include "PluginManager.h"

class CDirView;
struct IMergeDoc;
typedef CTypedPtrList<CPtrList, IMergeDoc *> MergeDocPtrList;
class DirDocFilterGlobal;
class DirDocFilterByExtension;
class CustomStatusCursor;
class CTempPathContext;
struct FileActionItem;

/////////////////////////////////////////////////////////////////////////////
// CDirDoc document

/**
 * @brief Class for folder compare data.
 * This class is "document" class for folder compare. It has compare context,
 * which in turn has a list of differences and other compare result data.
 * This class also has compare statistics which are updated during compare.
 * GUI calls this class to operate with results.
 */
class CDirDoc : public CDocument
{
protected:
	CDirDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirDoc)

// Attributes
public:
	CTempPathContext *m_pTempPathContext;
	int m_nDirs;
	static int m_nDirsTemp;

// Operations
public:
	BOOL CloseMergeDocs();
	CDirView * GetMainView() const;

	BOOL ReusingDirDoc();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void SetTitle(LPCTSTR lpszTitle);
	protected:
	virtual BOOL OnNewDocument();
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitCompare(const PathContext & paths, bool bRecursive, CTempPathContext *);
	void DiffThreadCallback(int& state);
	void Rescan();
	bool GetRecursive() const { return m_bRecursive; }
	bool GetReadOnly(int nIndex) const;
	void SetReadOnly(int nIndex, bool bReadOnly);
	bool HasDirView() { return m_pDirView != NULL; }
	void RefreshOptions();
	void CompareReady();
	void UpdateChangedItem(PathContext & paths,
		UINT nDiffs, UINT nTrivialDiffs, BOOL bIdentical);
	Poco::UIntPtr FindItemFromPaths(const String& pathLeft, const String& pathRight);
	void SetDiffSide(UINT diffcode, int idx);
	void SetDiffCompare(UINT diffcode, int idx);
	void UpdateResources();
	void InitStatusStrings();
	void UpdateStatusFromDisk(Poco::UIntPtr diffPos, bool bLeft, bool bRight);
	void ReloadItemStatus(Poco::UIntPtr diffPos, bool bLeft, bool bRight);
	void Redisplay();
	virtual ~CDirDoc();
	void SetDirView( CDirView *newView ); // TODO Perry
	void AddMergeDoc(IMergeDoc * pMergeDoc);
	void MergeDocClosing(IMergeDoc * pMergeDoc);
	CDiffThread m_diffThread;
	void SetDiffStatus(UINT diffcode, UINT mask, int idx);
	void SetDiffCounts(UINT diffs, UINT ignored, int idx);
	void UpdateDiffAfterOperation(const FileActionItem & act, Poco::UIntPtr pos);
	void UpdateHeaderPath(BOOL bLeft);
	void AbortCurrentScan();
	bool IsCurrentScanAbortable() const;
	void SetDescriptions(const String strDesc[]);
	void ApplyDisplayRoot(int nIndex, String &);

	void SetPluginPrediffSetting(const String& filteredFilenames, int newsetting);
	void SetPluginPrediffSettingAll(int newsetting) { m_pluginman.SetPrediffSettingAll(newsetting); }
	void SetPluginPrediffer(const String& filteredFilenames, const String & prediffer);
	void FetchPluginInfos(const String& filteredFilenames, 
	                      PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);
	bool IsShowable(const DIFFITEM & di) const;

	bool HasDiffs() const { return m_pCtxt != NULL; }
	const CDiffContext & GetDiffContext() const { return *m_pCtxt; }
	const DIFFITEM & GetDiffByKey(Poco::UIntPtr key) const { return m_pCtxt->GetDiffAt(key); }
	DIFFITEM & GetDiffRefByKey(Poco::UIntPtr key) { return m_pCtxt->GetDiffRefAt(key); }
	String GetLeftBasePath() const { return m_pCtxt->GetNormalizedLeft(); }
	String GetRightBasePath() const { return m_pCtxt->GetNormalizedRight(); }
	String GetBasePath(int nIndex) const { return m_pCtxt->GetNormalizedPath(nIndex); }
	void RemoveDiffByKey(Poco::UIntPtr key) { m_pCtxt->RemoveDiff(key); }
	void SetMarkedRescan() {m_bMarkedRescan = TRUE; }
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
	AllowUpwardDirectory::ReturnCode AllowUpwardDirectory(PathContext &paths);
	void SetItemViewFlag(Poco::UIntPtr key, UINT flag, UINT mask);
	void SetItemViewFlag(UINT flag, UINT mask);
	const CompareStats * GetCompareStats() const { return m_pCompareStats.get(); };
	bool IsArchiveFolders() const;

protected:
	void LoadLineFilterList();

	// Generated message map functions
	//{{AFX_MSG(CDirDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation data
private:
	std::unique_ptr<CDiffContext> m_pCtxt; /**< Pointer to diff-data */
	CDirView *m_pDirView; /**< Pointer to GUI */
	std::unique_ptr<CompareStats> m_pCompareStats; /**< Compare statistics */
	MergeDocPtrList m_MergeDocs; /**< List of file compares opened from this compare */
	bool m_bRO[3]; /**< Is left/middle/right side read-only */
	bool m_bRecursive; /**< Is current compare recursive? */
	std::unique_ptr<CustomStatusCursor> m_statusCursor;
	String m_strDesc[3]; /**< Left/middle/right side desription text */
	PluginManager m_pluginman;
	bool m_bReuseCloses; /**< Are we closing because of reuse? */
	bool m_bMarkedRescan; /**< If TRUE next rescan scans only marked items */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
