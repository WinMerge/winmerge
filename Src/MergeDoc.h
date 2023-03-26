/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeDoc.h
 *
 * @brief Declaration of CMergeDoc class
 */
#pragma once

#include "IMergeDoc.h"
#include "DiffTextBuffer.h"
#include "DiffWrapper.h"
#include "DiffList.h"
#include "TempFile.h"
#include "PathContext.h"
#include "FileLoadResult.h"
#include <vector>
#include <map>
#include <memory>
#include <optional>

/**
 * @brief Additional action codes for WinMerge.
 * @note Reserve first 100 for CrystalEditor
 */
enum
{
	CE_ACTION_MERGE = 100, /**< Merging action */
};

/**
 * @brief Return statuses of file rescan
 */
enum
{
	RESCAN_OK = 0, /**< Rescan succeeded */
	RESCAN_SUPPRESSED, /**< Rescan not done - suppressed */
	RESCAN_FILE_ERR, /**< Error reading file */
	RESCAN_TEMP_ERR, /**< Error saving to temp file */
};

/**
 * @brief File saving statuses
 */
enum
{
	SAVE_DONE = 0, /**< Saving succeeded */  
	SAVE_FAILED, /**< Saving failed */  
	SAVE_PACK_FAILED, /**< Plugin failed to pack file */  
	SAVE_NO_FILENAME, /**< File has no filename */  
	SAVE_CANCELLED, /**< Saving was cancelled */  
};

/**
 * @brief Types for buffer. Buffer's type defines behavior
 * of buffer when saving etc.
 * 
 * Difference between BUFFERTYPE::NORMAL and BUFFERTYPE::NORMAL_NAMED is
 * that _NAMED has description text given and which is shown
 * instead of filename.
 *
 * BUFFERTYPE::UNNAMED is created empty buffer (scratchpad), it has
 * no filename, and default description is given for it. After
 * this buffer is saved it becomes _SAVED. It is not equal to
 * NORMAL_NAMED, since scratchpads don't have plugins etc.
 */
enum class BUFFERTYPE
{
	NORMAL = 0, /**< Normal, file loaded from disk */
	NORMAL_NAMED, /**< Normal, description given */
	UNNAMED, /**< Empty, created buffer */
	UNNAMED_SAVED, /**< Empty buffer saved with filename */
};

struct WordDiff {
	std::array<int, 3> begin; // 0-based, eg, begin[0] is from str1
	std::array<int, 3> end; // 0-based, eg, end[1] is from str2
	std::array<int, 3> beginline;
	std::array<int, 3> endline;
	int op;

	WordDiff(int s1=0, int e1=0, int bl1=0, int el1=0, int s2=0, int e2=0, int bl2=0, int el2=0, int s3=0, int e3=0, int bl3=0, int el3=0, int op=0)
		: begin{s1, s2, s3}
		, beginline{bl1, bl2, bl3}
		, endline{el1, el2, el3}
		, op(op)
	{
		if (s1>e1) e1=s1;
		if (s2>e2) e2=s2;
		if (s3>e3) e3=s3;
		end[0] = e1;
		end[1] = e2;
		end[2] = e3;
	}
};

struct CurrentWordDiff
{
	int nDiff;
	size_t nWordDiff;
	int nPane;
	CEPoint ptStart;
	CEPoint ptEnd;
};

struct DiffFileInfo;
class CMergeEditView;
class PackingInfo;
class PrediffingInfo;
class CMergeEditFrame;
class CDirDoc;
class CEncodingErrorBar;
class CLocationView;
class CMergeEditSplitterView;

/**
 * @brief Document class for merging two files
 */
class CMergeDoc : public CDocument, public IMergeDoc
{
public:
	struct TableProps { bool istable; tchar_t delimiter; tchar_t quote; bool allowNewlinesInQuotes; };
// Attributes
public:
	static int m_nBuffersTemp;

// Begin declaration of CMergeDoc

	std::unique_ptr<CDiffTextBuffer> m_ptBuf[3]; /**< Left/Middle/Right side text buffer */
	int m_nBuffers;
	int m_nGroups;

protected: // create from serialization only
	CMergeDoc();
	DECLARE_DYNCREATE(CMergeDoc)

	// Operations
public:	
	std::unique_ptr<DiffFileInfo> m_pSaveFileInfo[3];
	std::unique_ptr<DiffFileInfo> m_pRescanFileInfo[3];
	DiffList m_diffList;
	UINT m_nTrivialDiffs; /**< Amount of trivial (ignored) diffs */
	PathContext m_filePaths; /**< Filepaths for this document */
	/// String of concatenated filenames as text to apply plugins filter to
	String m_strBothFilenames;

	CMergeEditView * GetActiveMergeView();
	CMergeEditView * GetActiveMergeGroupView(int nBuffer);
	void UpdateHeaderPath(int pane);
	void UpdateHeaderActivity(int pane, bool bActivate);
	void RefreshOptions();
	void UpdateResources();
	bool OpenDocs(int nFiles, const FileLocation fileloc[],
		const bool bRO[], const String strDesc[]);
	int LoadFile(const String& sFileName, int nBuffer, bool & readOnly, const FileTextEncoding & encoding);
	void MoveOnLoad(int nPane = -1, int nLinIndex = -1, bool bRealLine = false, int nCharIndex = -1);
	void ChangeFile(int nBuffer, const String& path, int nLineIndex = -1);
	void RescanIfNeeded(float timeOutInSecond);
	int Rescan(bool &bBinary, IDENTLEVEL &identical, bool bForced = false);
	void CheckFileChanged(void) override;
	int ShowMessageBox(const String& sText, unsigned nType = MB_OK, unsigned nIDHelp = 0);
	void ShowRescanError(int nRescanResult, IDENTLEVEL identical);
	bool Undo();
	void CopyAllList(int srcPane, int dstPane);
	void CopyMultipleList(int srcPane, int dstPane, int firstDiff, int lastDiff, int firstWordDiff = -1, int lastWordDiff = -1);
	void CopyMultiplePartialList(int srcPane, int dstPane, int firstDiff, int lastDiff, int firstLineDiff = -1, int lastLineDiff = -1);
	void DoAutoMerge(int dstPane);
	bool SanityCheckDiff(const DIFFRANGE& dr) const;
	bool WordListCopy(int srcPane, int dstPane, int nDiff, int nFirstWordDiff, int nLastWordDiff, const std::vector<int> *pWordDiffIndice, bool bGroupWithPrevious = false, bool bUpdateView = true);
	bool PartialListCopy(int srcPane, int dstPane, int nDiff, int firstLine, int lastLine = -1, bool bGroupWithPrevious = false, bool bUpdateView = true);
	bool ListCopy(int srcPane, int dstPane, int nDiff = -1, bool bGroupWithPrevious = false, bool bUpdateView = true);
	bool TrySaveAs(String& strPath, int &nLastErrorCode, String & sError,
		int nBuffer, PackingInfo& infoTempUnpacker);
	bool DoSave(const tchar_t* szPath, bool &bSaveSuccess, int nBuffer);
	bool DoSaveAs(const tchar_t* szPath, bool &bSaveSuccess, int nBuffer);
	int RightLineInMovedBlock(int pane, int line);
	int LeftLineInMovedBlock(int pane, int line);
	void SetEditedAfterRescan(int nBuffer);
	bool IsEditedAfterRescan(int nBuffer = -1) const;

	const PackingInfo* GetUnpacker() const override { return &m_infoUnpacker; }
	void SetUnpacker(const PackingInfo * infoUnpacker) override;
	void SetPrediffer(const PrediffingInfo * infoPrediffer);
	void GetPrediffer(PrediffingInfo * infoPrediffer) const;
	const PrediffingInfo *GetPrediffer() const override;
	void AddMergeViews(CMergeEditSplitterView* pMergeEditSplitterView, CMergeEditView* pView[3]);
	void RemoveMergeViews(CMergeEditSplitterView* pMergeEditSplitterView);
	void SetLocationView(CLocationView *pLocationView) { m_pLocationView = pLocationView; }

	CDirDoc * GetDirDoc() const override { return m_pDirDoc; }
	void SetDirDoc(CDirDoc * pDirDoc) override;
	void DirDocClosing(CDirDoc * pDirDoc) override;
	bool CloseNow() override;
	int GetFileCount() const override { return m_filePaths.GetSize(); }
	String GetPath(int pane) const override { return m_filePaths[pane]; } 
	bool GetReadOnly(int pane) const override { return m_ptBuf[pane]->m_bReadOnly; }
	CString GetTooltipString() const override;
	void SwapFiles(int nFromIndex, int nToIndex);

	CMergeEditView * GetView(int group, int buffer) const { return m_pView[group][buffer]; }
	CLocationView * GetLocationView() { return m_pLocationView; }
	std::vector<CMergeEditView *> GetViewList(int nGroup = -1, int nBuffer = -1) const {
		std::vector<CMergeEditView *> list;
		if (nGroup != -1)
			for (int nBuffer2 = 0; nBuffer2 < m_nBuffers; ++nBuffer2)
				list.push_back(m_pView[nGroup][nBuffer2]);
		else if (nBuffer != -1)
			for (int nGroup2 = 0; nGroup2 < m_nGroups; ++nGroup2)
				list.push_back(m_pView[nGroup2][nBuffer]);
		else
		{
			for (int nGroup3 = 0; nGroup3 < m_nGroups; nGroup3++)
				for (int nBuffer3 = 0; nBuffer3 < m_nBuffers; ++nBuffer3)
					list.push_back(m_pView[nGroup3][nBuffer3]);
		}
		return list;
	}
	template <typename Function>
	void ForEachView(int nBuffer, Function func) {
		for (int nGroup = 0; nGroup < m_nGroups; nGroup++)
			func(m_pView[nGroup][nBuffer]);
	}
	template <typename Function>
	void ForEachView(Function func) {
		for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
		{
			for (int nGroup = 0; nGroup < m_nGroups; nGroup++)
				func(m_pView[nGroup][nBuffer]);
		}
	}
	template <typename Function>
	void ForEachActiveGroupView(Function func) {
		int nGroup = GetActiveMergeView()->m_nThisGroup;
		for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
			func(m_pView[nGroup][nBuffer]);
	}
	CMergeEditFrame * GetParentFrame();

	void AddSyncPoint();
	bool DeleteSyncPoint(int pane, int nLine, bool bRescan = true);
	void ClearSyncPoints();
	bool HasSyncPoints();
	std::vector<std::vector<int> > GetSyncPointList();
	String GetDescription(int pane) const override { return m_strDesc[pane]; }
	void SetDescription(int pane, const String& sText) { m_strDesc[pane] = sText; }

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL SaveModified();
	virtual void DeleteContents ();
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation in MergeDocLineDiffs.cpp
public:
	typedef enum { BYTEDIFF, WORDDIFF } DIFFLEVEL;
	void Showlinediff(CMergeEditView *pView, bool bReversed = false);
	void AddToSubstitutionFilters(CMergeEditView* pView, bool bReversed = false);
	void AddToLineFilters(const String& text);
	std::vector<WordDiff> GetWordDiffArrayInDiffBlock(int nDiff);
	std::vector<WordDiff> GetWordDiffArray(int nLineIndex);
	std::vector<WordDiff> GetWordDiffArrayInRange(const int begin[3], const int end[3], int pane1 = -1, int pane2 = -1);
	void ClearWordDiffCache(int nDiff = -1);
private:
	void Computelinediff(CMergeEditView *pView, std::pair<CEPoint, CEPoint> rc[], bool bReversed);
	std::map<int, std::vector<WordDiff> > m_cacheWordDiffs;
// End MergeDocLineDiffs.cpp

// Implementation in MergeDocEncoding.cpp
public:
	bool DoFileEncodingDialog(int pane = -1);
// End MergeDocEncoding.cpp

// Implementation
public:
	FileChange IsFileChangedOnDisk(const tchar_t* szPath, DiffFileInfo &dfi,
		bool bSave, int nBuffer);
	bool PromptAndSaveIfNeeded(bool bAllowCancel);
	std::vector<int> undoTgt;
	std::vector<int>::iterator curUndo;
	void FlushAndRescan(bool bForced = false);
	void SetCurrentDiff(int nDiff);
	int GetCurrentDiff() const { return m_nCurDiff; }
	const CurrentWordDiff& GetCurrentWordDiff() const { return m_CurWordDiff; }
	bool EqualCurrentWordDiff(int nBuffer, const CEPoint& ptStart, const CEPoint& ptEnd) const
	{
		return (m_CurWordDiff.nPane == nBuffer && m_CurWordDiff.ptStart == ptStart && m_CurWordDiff.ptEnd == ptEnd);
	}
	virtual ~CMergeDoc();
	void SetDetectMovedBlocks(bool bDetectMovedBlocks);
	bool IsMixedEOL(int nBuffer) const;
	bool GenerateReport(const String& sFileName) const override;
	void SetAutoMerged(bool bAutoMerged) { m_bAutoMerged = bAutoMerged; }
	bool GetAutoMerged() const { return m_bAutoMerged; };
	bool IsModified() const
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
			if (m_ptBuf[nBuffer]->IsModified())
				return true;
		return false;
	}
	bool CanUndo() const
	{
		for (int nBuffer = 0; nBuffer < m_nBuffers; ++nBuffer)
			if (m_ptBuf[nBuffer]->CanUndo())
				return true;
		return false;
	}

	std::optional<bool> GetEnableTableEditing() const { return m_bEnableTableEditing; }
	void SetEnableTableEditing(std::optional<bool> bEnableTableEditing) { m_bEnableTableEditing = bEnableTableEditing; }
	static TableProps MakeTablePropertiesByFileName(const String& path, const std::optional<bool>& enableTableEditing, bool showDialog = true);
	void SetPreparedTableProperties(const TableProps& props) { m_pTablePropsPrepared.reset(new TableProps(props)); }

	void SetTextType(int textType);
	void SetTextType(const String& ext);
	bool GetChangedSchemeManually() const { return m_bChangedSchemeManually; }

	bool GetAutomaticRescan() const { return m_bAutomaticRescan; }
	// to customize the mergeview menu
	HMENU createPrediffersSubmenu(HMENU hMenu);

// implementation methods
private:
	bool GetBreakType() const;
	bool GetByteColoringOption() const;
	bool IsValidCodepageForMergeEditor(unsigned cp) const;
	void SanityCheckCodepage(FileLocation & fileinfo);
	FileLoadResult::flags_t LoadOneFile(int index, const String& filename, bool readOnly, const String& strDesc, const FileTextEncoding & encoding);
	void SetTableProperties();

// Implementation data
protected:
	int m_nCurDiff; /**< Selected diff, 0-based index, -1 if no diff selected */
	CurrentWordDiff m_CurWordDiff;
	CMergeEditView * m_pView[3][3]; /**< Pointer to left/middle/right view */
	CMergeEditSplitterView * m_pMergeEditSplitterView[3];
	CLocationView * m_pLocationView; /**< Pointer to locationview */
	CDirDoc * m_pDirDoc;
	bool m_bEnableRescan; /**< Automatic rescan enabled/disabled */
	COleDateTime m_LastRescan; /**< Time of last rescan (for delaying) */ 
	CDiffWrapper m_diffWrapper;
	/// information about the file packer/unpacker
	PackingInfo m_infoUnpacker;
	String m_strDesc[3]; /**< Left/Middle/Right side description text */
	BUFFERTYPE m_nBufferType[3];
	bool m_bEditAfterRescan[3]; /**< Left/middle/right doc edited after rescanning */
	TempFile m_tempFiles[3]; /**< Temp files for compared files */
	int m_nDiffContext;
	bool m_bInvertDiffContext;
	bool m_bMixedEol; /**< Does this document have mixed EOL style? */
	std::unique_ptr<CEncodingErrorBar> m_pEncodingErrorBar;
	bool m_bHasSyncPoints;
	bool m_bAutoMerged;
	std::optional<bool> m_bEnableTableEditing;
	std::unique_ptr<TableProps> m_pTablePropsPrepared;
	/**
	 * Are automatic rescans enabled?
	 * If automatic rescans are enabled then we rescan files after edit
	 * events, unless timer suppresses rescan. We suppress rescans within
	 * certain time from previous rescan.
	 */
	bool m_bAutomaticRescan;
	/// active prediffer ID : helper to check the radio button
	int m_CurrentPredifferID;
	bool m_bChangedSchemeManually;	/**< `true` if the syntax highlighting scheme is changed manually */
	String m_sCurrentHeaderTitle[3];

// friend access
	friend class RescanSuppress;


// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveLeft();
	afx_msg void OnUpdateFileSaveLeft(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveMiddle();
	afx_msg void OnUpdateFileSaveMiddle(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveRight();
	afx_msg void OnUpdateFileSaveRight(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAsLeft();
	afx_msg void OnUpdateFileSaveAsMiddle(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAsMiddle();
	afx_msg void OnFileSaveAsRight();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnFileReload();
	afx_msg void OnFileEncoding();
	afx_msg void OnFileReadOnlyLeft();
	afx_msg void OnUpdateFileReadOnlyLeft(CCmdUI* pCmdUI);
	afx_msg void OnFileReadOnlyMiddle();
	afx_msg void OnUpdateFileReadOnlyMiddle(CCmdUI* pCmdUI);
	afx_msg void OnFileReadOnlyRight();
	afx_msg void OnUpdateFileReadOnlyRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusRO(CCmdUI* pCmdUI);
	afx_msg void OnDiffContext(UINT nID);
	afx_msg void OnUpdateDiffContext(CCmdUI* pCmdUI);
	afx_msg void OnToolsGenerateReport();
	afx_msg void OnToolsGeneratePatch();
	afx_msg void OnOpenWithUnpacker();
	afx_msg void OnApplyPrediffer();
	afx_msg void OnBnClickedFileEncoding();
	afx_msg void OnBnClickedPlugin();
	afx_msg void OnBnClickedHexView();
	afx_msg void OnOK();
	afx_msg void OnFileRecompareAsText();
	afx_msg void OnFileRecompareAsTable();
	afx_msg void OnUpdateFileRecompareAsText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRecompareAsTable(CCmdUI* pCmdUI);
	afx_msg void OnFileRecompareAs(UINT nID);
	template<int srcPane, int dstPane>
	afx_msg void OnViewSwapPanes();
	afx_msg void OnUpdateSwapContext(CCmdUI* pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnUpdatePrediffer(CCmdUI* pCmdUI);
	afx_msg void OnPrediffer(UINT nID );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PrimeTextBuffers();
	void HideLines();
	void AdjustDiffBlocks();
	void AdjustDiffBlocks3way();
	void AdjustDiffBlock(DiffMap & diffmap, const DIFFRANGE & diffrange,
		const std::vector<WordDiff>& worddiffs,
		int i0, int i1, int lo0, int hi0, int lo1, int hi1);
	int GetMatchCost(const DIFFRANGE& dr, int i0, int i1, int line0, int line1, const std::vector<WordDiff>& worddiffs);
	OP_TYPE ComputeOpType3way(const std::vector<std::array<int, 3>>& vlines, size_t index,
		const DIFFRANGE& diffrange, const DIFFOPTIONS& diffOptions);
	void FlagTrivialLines();
	void FlagMovedLines();
	String GetFileExt(const tchar_t* sFileName, const tchar_t* sDescription) const;
	void DoFileSave(int pane);
	void SetPredifferByMenu(UINT nID);
};

/**
 * @brief return true if there are synchronization points
 */
inline bool CMergeDoc::HasSyncPoints()
{
	return m_bHasSyncPoints;
}

