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
 * @file  MergeDoc.h
 *
 * @brief Declaration of CMergeDoc class
 */
#pragma once

#include "DiffTextBuffer.h"
#include <vector>
#include <map>
#include <memory>
#include "DiffWrapper.h"
#include "DiffList.h"
#include "TempFile.h"
#include "PathContext.h"
#include "IMergeDoc.h"

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
 * Difference between BUFFER_NORMAL and BUFFER_NORMAL_NAMED is
 * that _NAMED has description text given and which is shown
 * instead of filename.
 *
 * BUFFER_UNNAMED is created empty buffer (scratchpad), it has
 * no filename, and default description is given for it. After
 * this buffer is saved it becomes _SAVED. It is not equal to
 * NORMAL_NAMED, since scratchpads don't have plugins etc.
 */
enum BUFFERTYPE
{
	BUFFER_NORMAL = 0, /**< Normal, file loaded from disk */
	BUFFER_NORMAL_NAMED, /**< Normal, description given */
	BUFFER_UNNAMED, /**< Empty, created buffer */
	BUFFER_UNNAMED_SAVED, /**< Empty buffer saved with filename */
};

struct WordDiff {
	int begin[3]; // 0-based, eg, begin[0] is from str1
	int end[3]; // 0-based, eg, end[1] is from str2
	int beginline[3];
	int endline[3];
	int op;
	WordDiff(int s1=0, int e1=0, int bl1=0, int el1=0, int s2=0, int e2=0, int bl2=0, int el2=0, int s3=0, int e3=0, int bl3=0, int el3=0, int op=0)
		: op(op)
	{
		if (s1>e1) e1=s1;
		if (s2>e2) e2=s2;
		if (s3>e3) e3=s3;
		begin[0] = s1;
		begin[1] = s2;
		begin[2] = s3;
		end[0] = e1;
		end[1] = e2;
		end[2] = e3;
		beginline[0] = bl1;
		beginline[1] = bl2;
		beginline[2] = bl3;
		endline[0] = el1;
		endline[1] = el2;
		endline[2] = el3;
	}
	WordDiff(const WordDiff & src)
	{
		for (int i=0; i<3; ++i)
		{
			begin[i] = src.begin[i];
			end[i] = src.end[i];
			beginline[i] = src.beginline[i];
			endline[i] = src.endline[i];
		}
		op = src.op;
	}
};

struct DiffFileInfo;
class CMergeEditView;
class PackingInfo;
class PrediffingInfo;
class CMergeEditFrame;
class CDirDoc;
class CEncodingErrorBar;
class CLocationView;

/**
 * @brief Document class for merging two files
 */
class CMergeDoc : public CDocument, public IMergeDoc
{
public:
	enum FileChange
	{
		FileNoChange,
		FileChanged,
		FileRemoved,
	};

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
	int LoadFile(CString sFileName, int nBuffer, bool & readOnly, const FileTextEncoding & encoding);
	void MoveOnLoad(int nPane = -1, int nLinIndex = -1);
	void ChangeFile(int nBuffer, const String& path);
	void RescanIfNeeded(float timeOutInSecond);
	int Rescan(bool &bBinary, IDENTLEVEL &identical, bool bForced = false);
	void CheckFileChanged(void) override;
	int ShowMessageBox(const String& sText, unsigned nType = MB_OK, unsigned nIDHelp = 0);
	void ShowRescanError(int nRescanResult, IDENTLEVEL identical);
	bool Undo();
	void CopyAllList(int srcPane, int dstPane);
	void CopyMultipleList(int srcPane, int dstPane, int firstDiff, int lastDiff, int firstWordDiff = -1, int lastWordDiff = -1);
	void DoAutoMerge(int dstPane);
	bool SanityCheckDiff(DIFFRANGE dr) const;
	bool WordListCopy(int srcPane, int dstPane, int nDiff, int nFirstWordDiff, int nLastWordDiff, const std::vector<int> *pWordDiffIndice, bool bGroupWithPrevious = false, bool bUpdateView = true);
	bool ListCopy(int srcPane, int dstPane, int nDiff = -1, bool bGroupWithPrevious = false, bool bUpdateView = true);
	bool TrySaveAs(String& strPath, int &nLastErrorCode, String & sError,
		int nBuffer, PackingInfo * pInfoTempUnpacker);
	bool DoSave(LPCTSTR szPath, bool &bSaveSuccess, int nBuffer);
	bool DoSaveAs(LPCTSTR szPath, bool &bSaveSuccess, int nBuffer);
	int RightLineInMovedBlock(int pane, int line);
	int LeftLineInMovedBlock(int pane, int line);
	void SetEditedAfterRescan(int nBuffer);
	bool IsEditedAfterRescan(int nBuffer) { return m_bEditAfterRescan[nBuffer]; }

	void SetUnpacker(const PackingInfo * infoUnpacker);
	void SetPrediffer(const PrediffingInfo * infoPrediffer);
	void GetPrediffer(PrediffingInfo * infoPrediffer);
	void AddMergeViews(CMergeEditView * pView[3]);
	void RemoveMergeViews(int nGroup);
	void SetLocationView(CLocationView *pLocationView) { m_pLocationView = pLocationView; }

	void SetDirDoc(CDirDoc * pDirDoc) override;
	CDirDoc * GetDirDoc() const { return m_pDirDoc; }
	void DirDocClosing(CDirDoc * pDirDoc) override;
	bool CloseNow() override;
	void SwapFiles();

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
	String GetDescription(int pane) const { return m_strDesc[pane]; }

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
	std::vector<WordDiff> GetWordDiffArrayInDiffBlock(int nDiff);
	std::vector<WordDiff> GetWordDiffArray(int nLineIndex);
	void ClearWordDiffCache(int nDiff = -1);
private:
	void Computelinediff(CMergeEditView *pView, CRect rc[], bool bReversed);
	std::map<int, std::vector<WordDiff> > m_cacheWordDiffs;
// End MergeDocLineDiffs.cpp

// Implementation in MergeDocEncoding.cpp
public:
	bool DoFileEncodingDialog();
// End MergeDocEncoding.cpp

// Implementation
public:
	FileChange IsFileChangedOnDisk(LPCTSTR szPath, DiffFileInfo &dfi,
		bool bSave, int nBuffer);
	bool PromptAndSaveIfNeeded(bool bAllowCancel);
	std::vector<int> undoTgt;
	std::vector<int>::iterator curUndo;
	void FlushAndRescan(bool bForced = false);
	void SetCurrentDiff(int nDiff);
	int GetCurrentDiff() const { return m_nCurDiff; }
	virtual ~CMergeDoc();
	void SetDetectMovedBlocks(bool bDetectMovedBlocks);
	bool IsMixedEOL(int nBuffer) const;
	bool OpenWithUnpackerDialog();
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

// implementation methods
private:
	bool GetBreakType() const;
	bool GetByteColoringOption() const;
	bool IsValidCodepageForMergeEditor(unsigned cp) const;
	void SanityCheckCodepage(FileLocation & fileinfo);
	DWORD LoadOneFile(int index, String filename, bool readOnly, const String& strDesc, const FileTextEncoding & encoding);

// Implementation data
protected:
	int m_nCurDiff; /**< Selected diff, 0-based index, -1 if no diff selected */
	CMergeEditView * m_pView[3][3]; /**< Pointer to left/middle/right view */
	CLocationView * m_pLocationView; /**< Pointer to locationview */
	CDirDoc * m_pDirDoc;
	bool m_bEnableRescan; /**< Automatic rescan enabled/disabled */
	COleDateTime m_LastRescan; /**< Time of last rescan (for delaying) */ 
	CDiffWrapper m_diffWrapper;
	/// information about the file packer/unpacker
	std::unique_ptr<PackingInfo> m_pInfoUnpacker;
	String m_strDesc[3]; /**< Left/Middle/Right side description text */
	BUFFERTYPE m_nBufferType[3];
	bool m_bEditAfterRescan[3]; /**< Left/middle/right doc edited after rescanning */
	TempFile m_tempFiles[3]; /**< Temp files for compared files */
	int m_nDiffContext;
	bool m_bMixedEol; /**< Does this document have mixed EOL style? */
	std::unique_ptr<CEncodingErrorBar> m_pEncodingErrorBar;
	bool m_bHasSyncPoints;
	bool m_bAutoMerged;
// friend access
	friend class RescanSuppress;


// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveLeft();
	afx_msg void OnFileSaveMiddle();
	afx_msg void OnFileSaveRight();
	afx_msg void OnFileSaveAsLeft();
	afx_msg void OnUpdateFileSaveAsMiddle(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAsMiddle();
	afx_msg void OnFileSaveAsRight();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePluginName(CCmdUI* pCmdUI);
	afx_msg void OnFileReload();
	afx_msg void OnFileEncoding();
	afx_msg void OnDiffContext(UINT nID);
	afx_msg void OnUpdateDiffContext(CCmdUI* pCmdUI);
	afx_msg void OnToolsGenerateReport();
	afx_msg void OnToolsGeneratePatch();
	afx_msg void OnCtxtOpenWithUnpacker();
	afx_msg void OnBnClickedFileEncoding();
	afx_msg void OnBnClickedPlugin();
	afx_msg void OnBnClickedHexView();
	afx_msg void OnOK();
	afx_msg void OnFileRecompareAsXML();
	afx_msg void OnFileRecompareAs(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PrimeTextBuffers();
	void HideLines();
	void AdjustDiffBlocks();
	void AdjustDiffBlock(DiffMap & diffmap, const DIFFRANGE & diffrange, int lo0, int hi0, int lo1, int hi1);
	int GetMatchCost(const String &Line0, const String &Line1);
	void FlagTrivialLines();
	void FlagMovedLines();
	String GetFileExt(LPCTSTR sFileName, LPCTSTR sDescription) const;
	void DoFileSave(int pane);
};

/**
 * @brief return true if there are synchronization points
 */
inline bool CMergeDoc::HasSyncPoints()
{
	return m_bHasSyncPoints;
}

