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
#pragma once

#include <memory>
#include "DiffThread.h"
#include "PluginManager.h"
#include "DirCmpReport.h"

class CDirView;
struct IMergeDoc;
typedef CTypedPtrList<CPtrList, IMergeDoc *> MergeDocPtrList;
class DirDocFilterGlobal;
class DirDocFilterByExtension;
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
	bool CloseMergeDocs();
	CDirView * GetMainView() const;

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
	bool GetReadOnly(int nIndex) const;
	const bool *GetReadOnly(void) const;
	void SetReadOnly(int nIndex, bool bReadOnly);
	String GetReportFile() const { return m_sReportFile; }
	void SetReportFile(const String& sReportFile) { m_sReportFile = sReportFile; }
	bool GetGeneratingReport() const { return m_bGeneratingReport; }
	void SetGeneratingReport(bool bGeneratingReport) { m_bGeneratingReport = bGeneratingReport; }
	void SetReport(DirCmpReport* pReport) { m_pReport.reset(pReport);  }
	bool HasDirView() const { return m_pDirView != nullptr; }
	void RefreshOptions();
	void CompareReady();
	void UpdateChangedItem(const PathContext & paths,
		UINT nDiffs, UINT nTrivialDiffs, bool bIdentical);
	void UpdateResources();
	void InitStatusStrings();
	void ReloadItemStatus(DIFFITEM *diffPos, int idx);
	void Redisplay();
	virtual ~CDirDoc();
	void SetDirView( CDirView *newView ); // TODO Perry
	void AddMergeDoc(IMergeDoc * pMergeDoc);
	void MergeDocClosing(IMergeDoc * pMergeDoc);
	CDiffThread m_diffThread;
	void UpdateHeaderPath(int nIndex);
	void AbortCurrentScan();
	void PauseCurrentScan();
	void ContinueCurrentScan();
	bool IsCurrentScanAbortable() const;
	void SetDescriptions(const String strDesc[]);
	void ApplyDisplayRoot(int nIndex, String &);

	bool HasDiffs() const { return m_pCtxt != nullptr; }
	const CDiffContext & GetDiffContext() const { return *m_pCtxt; }
	CDiffContext& GetDiffContext() { return *m_pCtxt.get(); }
	void SetMarkedRescan() {m_bMarkedRescan = true; }
	const CompareStats * GetCompareStats() const { return m_pCompareStats.get(); };
	bool IsArchiveFolders() const;
	PluginManager& GetPluginManager() { return m_pluginman; };
	void Swap(int idx1, int idx2);
	bool MoveableToNextDiff();
	bool MoveableToPrevDiff();
	void MoveToNextDiff(IMergeDoc *pMergeDoc);
	void MoveToPrevDiff(IMergeDoc *pMergeDoc);

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
	String m_strDesc[3]; /**< Left/middle/right side desription text */
	String m_sReportFile;
	PluginManager m_pluginman;
	bool m_bMarkedRescan; /**< If `true` next rescan scans only marked items */
	bool m_bGeneratingReport;
	std::unique_ptr<DirCmpReport> m_pReport;
};

/**
 * @brief Set left/middle/right side readonly-status
 * @param nIndex Select side to set 
 * @param bReadOnly New status of selected side
 */
inline void CDirDoc::SetReadOnly(int nIndex, bool bReadOnly)
{
	m_bRO[nIndex] = bReadOnly;
}

/**
 * @brief Return left/middle/right side readonly-status
 * @param nIndex Select side to ask
 */
inline bool CDirDoc::GetReadOnly(int nIndex) const
{
	return m_bRO[nIndex];
}

inline const bool *CDirDoc::GetReadOnly(void) const
{
	return m_bRO;
}

