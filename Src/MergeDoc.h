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
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#include "GhostTextBuffer.h"
#include <vector>
#include "DiffWrapper.h"

#ifndef _DIFF_FILE_INFO_H_INCLUDED
#include "DiffFileInfo.h"
#endif

/**
 * @brief additionnal action code for WinMerge (reserve 100 first codes for CrystalEdit)
 */enum
{
	CE_ACTION_MERGE = 100,
};

/**
 * @brief Return statuses of file rescan
 */
enum
{
	RESCAN_OK = 0,
	RESCAN_SUPPRESSED,
	RESCAN_IDENTICAL,
	RESCAN_BINARIES,
	RESCAN_FILE_ERR,
	RESCAN_TEMP_ERR,
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

struct DiffFileInfo;
class CMergeEditView;
class CMergeDiffDetailView;
class PackingInfo;
class PrediffingInfo;
class CChildFrame;
class CDirDoc;

//<jtuc 2003-06-28>
/*
class CUndoItem
{
public:
	UINT begin,end,diffidx;
	int blank;
	BOOL bInsert;
	CMergeEditView *m_pList;
	CStringList list;
};
*/
//<jtuc>

/**
 * @brief Document class for merging two files
 */
class CMergeDoc : public CDocument
{
// Attributes
public:

/**
 * @brief Specialized buffer to save file data
 */
class CDiffTextBuffer : public CGhostTextBuffer
	{
		friend class CMergeDoc;
private :
		CMergeDoc * m_pOwnerDoc;
		BOOL m_bIsLeft; /**< Left/Right side */
		BOOL FlagIsSet(UINT line, DWORD flag);
		CString m_strTempPath;
		int unpackerSubcode;
		/* 
		 * @brief Unicode encoding from ucr::UNICODESET 
		 *
		 * @note m_unicoding and m_codepage are indications of how the buffer is supposed to be saved on disk
		 * In memory, it is invariant, depending on build:
		 * ANSI:
		 *   in memory it is CP_ACP/CP_THREAD_ACP 8-bit characters
		 * Unicode:
		 *   in memory it is wchars
		 */
		int m_unicoding; 
		int m_codepage; /**< @brief 8-bit codepage, if relevant m_unicoding==ucr::NONE */

		int NoteCRLFStyleFromBuffer(TCHAR *lpLineBegin, DWORD dwLineLen = 0);
		void ReadLineFromBuffer(TCHAR *lpLineBegin, DWORD dwLineNum, DWORD dwLineLen = 0);
public :
		void SetTempPath(CString path);
		virtual void AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
                        LPCTSTR pszText, int nLinesToValidate, int nActionType = CE_ACTION_UNKNOWN);
		bool curUndoGroup();
		void ReplaceLine(CCrystalTextView * pSource, int nLine, const CString& strText, int nAction =CE_ACTION_UNKNOWN);
		void ReplaceFullLine(CCrystalTextView * pSource, int nLine, const CString& strText, int nAction =CE_ACTION_UNKNOWN);

		int LoadFromFile(LPCTSTR pszFileName, PackingInfo * infoUnpacker, CString filteredFilenames, BOOL & readOnly, int nCrlfStyle, int codepage);
		int SaveToFile (LPCTSTR pszFileName, BOOL bTempFile, PackingInfo * infoUnpacker = NULL,
                    int nCrlfStyle = CRLF_STYLE_AUTOMATIC, BOOL bClearModifiedFlag = TRUE );
		int getUnicoding() const { return m_unicoding; }
		void setUnicoding(int value) { m_unicoding = value; }
		int getCodepage() const { return m_codepage; }
		void setCodepage(int value) { m_codepage = value; }

		CDiffTextBuffer(CMergeDoc * pDoc, BOOL bLeft);

		// If line has text (excluding eol), set strLine to text (excluding eol)
		BOOL GetLine(int nLineIndex, CString &strLine);

		// if line has any text (including eol), set strLine to text (including eol)
		BOOL GetFullLine(int nLineIndex, CString &strLine);

		virtual void SetModified (BOOL bModified = TRUE);

		void prepareForRescan();

		/** 
		After editing a line, we don't know if there is a diff or not.
		So we clear the LF_DIFF flag (and it is more easy to read during edition).
		Rescan will set the proper color
		*/
		virtual void OnNotifyLineHasBeenEdited(int nLine);


	} friend;

// End declaration of CMergeDoc::CDiffTextBuffer

// Begin declaration of CMergeDoc

	CDiffTextBuffer m_ltBuf; /**< Left side text buffer */
	CDiffTextBuffer m_rtBuf; /**< Right side text buffer */

protected: // create from serialization only
	CMergeDoc();
	DECLARE_DYNCREATE(CMergeDoc)

	
	// Operations
public:	
	DiffFileInfo m_leftSaveFileInfo;
	DiffFileInfo m_rightSaveFileInfo;
	CPtrList m_undoList;
	CArray<DIFFRANGE,DIFFRANGE> m_diffs;
	UINT m_nDiffs; /**< Amount of diffs */
	UINT m_nTrivialDiffs; /**< Amount of trivial (ignored) diffs */
	CString m_strLeftFile, m_strRightFile;
	/// String of concatenated filenames as text to apply plugins filter to
	CString m_strBothFilenames;

	int GetLineCount(BOOL bLeft) const;
	void UpdateHeaderPath(BOOL bLeft);
	void UpdateHeaderActivity(BOOL bLeft, BOOL bActivate);
	void RefreshOptions();
	BOOL OpenDocs(CString sLeftFile, CString sRightFile,
		BOOL bROLeft, BOOL bRORight, int cpleft, int cpright);
	int LoadFile(CString sFileName, BOOL bLeft, BOOL & readOnly, int codepage);
	void RescanIfNeeded(float timeOutInSecond);
	int Rescan(BOOL bForced = FALSE);
	void ShowRescanError(int nRescanResult);
	void AddUndoAction(UINT nBegin, UINT nEnd, UINT nDiff, int nBlanks, BOOL bInsert, CMergeEditView *pList);
	BOOL Undo();
	void CopyAllList(bool bSrcLeft);
	void CopyMultipleList(bool bSrcLeft, int firstDiff, int lastDiff);
	void ListCopy(bool bSrcLeft);
	BOOL TrySaveAs(CString &strPath, int &nLastErrorCode, BOOL bLeft, PackingInfo * pInfoTempUnpacker);
	BOOL DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft);
	BOOL DoSaveAs(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft);
	//CString ExpandTabs(LPCTSTR szText);
	//CString Tabify(LPCTSTR szText);
	int LineToDiff(UINT nLine);
	BOOL LineInDiff(UINT nLine, UINT nDiff);
	void SetDiffViewMode(BOOL bEnable);

	void SetUnpacker(PackingInfo * infoUnpacker);
	void SetPrediffer(PrediffingInfo * infoPrediffer);
	void GetPrediffer(PrediffingInfo * infoPrediffer);
	void SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight);
	void SetMergeDetailViews(CMergeDiffDetailView * pLeft, CMergeDiffDetailView * pRight);
	void SetDirDoc(CDirDoc * pDirDoc);
	void DirDocClosing(CDirDoc * pDirDoc);
	BOOL CloseNow();

	CMergeEditView * GetLeftView() { return m_pLeftView; }
	CMergeEditView * GetRightView() { return m_pRightView; }
	CMergeDiffDetailView * GetLeftDetailView() { return m_pLeftDetailView; }
	CMergeDiffDetailView * GetRightDetailView() { return m_pRightDetailView; }
	CChildFrame * GetParentFrame();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL SaveModified();
	virtual void DeleteContents ();
	//}}AFX_VIRTUAL

// Implementation in MergeDocLineDiffs.cpp
public:
	void Showlinediff(CMergeEditView * pView);
	void Showlinediff(CMergeDiffDetailView * pView);
private:
	void Computelinediff(CCrystalTextView * pView, CCrystalTextView * pOther, int line, CRect * rc1, CRect * rc2);
// End MergeDocLineDiffs.cpp

// Implementation
public:
	BOOL IsFileChangedOnDisk(LPCTSTR szPath, DiffFileInfo &dfi,
		BOOL bSave, BOOL bLeft);
	BOOL SaveHelper(BOOL bAllowCancel);
	std::vector<CMergeEditView*> undoTgt;
	std::vector<CMergeEditView*>::iterator curUndo;
	void FlushAndRescan(BOOL bForced = FALSE);
	BOOL TempFilesExist();
	void CleanupTempFiles();
	BOOL InitTempFiles(const CString& strPathL, const CString& strPathR);
	void SetCurrentDiff(int nDiff);
	int GetCurrentDiff() { return m_nCurDiff; }
	UINT CountPrevBlanks(UINT nCurLine, BOOL bLeft);
	virtual ~CMergeDoc();
	virtual void OnFileEvent (WPARAM wEvent, LPCTSTR pszPathName);
	BOOL GetMergingMode() const;
	void SetMergingMode(BOOL bMergingMode);
	BOOL GetNextDiff(int nLine, int &nDiff);
	BOOL GetDiff(int nDiff, DIFFRANGE &di) const;

// Implementation data
protected:
	int m_nCurDiff; /**< Selected diff, 0-based index, -1 if no diff selected */
	CString m_strTempRightFile; /**< Left buffer temp file for diffengine */
	CString m_strTempLeftFile; /**< Right buffer temp file for diffengine */
	CMergeEditView * m_pLeftView; /**< Pointer to left view */
	CMergeEditView * m_pRightView; /**< Pointer to right view */
	CMergeDiffDetailView * m_pLeftDetailView;
	CMergeDiffDetailView * m_pRightDetailView;
	CDirDoc * m_pDirDoc;
	BOOL m_bEnableRescan; /**< Automatic rescan enabled/disabled */
	COleDateTime m_LastRescan; /**< Time of last rescan (for delaying) */ 
	CDiffWrapper m_diffWrapper;
	/// information about the file packer/unpacker
	PackingInfo * m_pInfoUnpacker;
	CString m_strLeftDesc; /**< Left side description text */
	CString m_strRightDesc; /**< Right side description text */
	BUFFERTYPE m_nLeftBufferType;
	BUFFERTYPE m_nRightBufferType;
	BOOL m_bMergingMode; /**< Merging or Edit mode */

// friend access
	friend class RescanSuppress;


// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveLeft();
	afx_msg void OnFileSaveRight();
	afx_msg void OnFileSaveAsLeft();
	afx_msg void OnFileSaveAsRight();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PrimeTextBuffers();
	void FlagMovedLines(const CMap<int, int, int, int> * movedLines, CDiffTextBuffer * pBuffer);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
