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
// MergeDoc.h : interface of the CMergeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CCrystalTextBuffer.h"
#include <vector>
#include "afxtempl.h"

#define OP_NONE			0
#define OP_LEFTONLY		1
#define OP_DIFF			2
#define OP_RIGHTONLY	3

typedef struct tagDIFFRANGE {
	UINT begin0,end0,begin1,end1;
	UINT dbegin0,dend0,dbegin1,dend1;
	int blank0,blank1;
	BYTE op;
}DIFFRANGE;

enum
{
	RESCAN_OK = 0,
	RESCAN_SUPPRESSED,
	RESCAN_IDENTICAL,
	RESCAN_BINARIES,
	RESCAN_FILE_ERR
};

class CMergeEditView;

class CUndoItem
{
public:
	UINT begin,end,diffidx;
	int blank;
	BOOL bInsert;
	CMergeEditView *m_pList;
	CStringList list;
};

class CChildFrame;
class CDirDoc;

class CMergeDoc : public CDocument
{
// Attributes
public:
	public :
class CDiffTextBuffer : public CCrystalTextBuffer
      {
friend class CMergeDoc;
private :
        CMergeDoc * m_pOwnerDoc;
		BOOL m_bIsLeft;
		BOOL FlagIsSet(UINT line, DWORD flag);
		CString m_strTempPath;

		int DetermineCRLFStyle(LPVOID lpBuf, DWORD dwLength);
		void ReadLineFromBuffer(TCHAR *lpLineBegin, DWORD dwLineLen = 0);
public :
		void SetTempPath(CString path);
		bool curUndoGroup();
		void ReplaceLine(int nLine, const CString& strText);
		void ReplaceFullLine(int nLine, const CString& strText);

		UINT GetTextWithoutEmptys(int nStartLine, int nStartChar, int nEndLine, int nEndChar, 
				CString &text, BOOL bLeft, int nCrlfStyle = CRLF_STYLE_AUTOMATIC);
		BOOL LoadFromFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC);
		BOOL SaveToFile (LPCTSTR pszFileName, BOOL bTempFile,
				int nCrlfStyle = CRLF_STYLE_AUTOMATIC, 
											 BOOL bClearModifiedFlag = TRUE );

		CDiffTextBuffer (CMergeDoc * pDoc, BOOL bLeft)
		{
			m_pOwnerDoc = pDoc;
			m_bIsLeft=bLeft;
		}
		// If line has text (excluding eol), set strLine to text (excluding eol)
		BOOL GetLine( int nLineIndex, CString &strLine ) 
		{ 
			int nLineLength = CCrystalTextBuffer::GetLineLength 
				( nLineIndex ); 
			
			if( nLineLength < 0 ) 
				return FALSE; 
			else if( nLineLength == 0 ) 
				strLine.Empty(); 
			else 
			{ 
				_tcsncpy ( strLine.GetBuffer( nLineLength + 1 ), 
					CCrystalTextBuffer::GetLineChars( nLineIndex ), 
					nLineLength ); 
				strLine.ReleaseBuffer( nLineLength ); 
			} 
			return TRUE; 
		} 
		// if line has any text (including eol), set strLine to text (including eol)
		BOOL GetFullLine(int nLineIndex, CString &strLine)
		{
			if (!GetFullLineLength(nLineIndex))
				return FALSE;
			strLine = GetLineChars(nLineIndex);
			return TRUE;
		}

		virtual void SetModified (BOOL bModified = TRUE)
		{
			CCrystalTextBuffer::SetModified (bModified);
			m_pOwnerDoc->SetModifiedFlag (bModified);
		}
		void InsertLine (LPCTSTR pszLine, int nLength = -1, int nPosition = -1)
		{
			CCrystalTextBuffer::InsertLine(pszLine, nLength, nPosition);
		}

      };

// End declaration of CMergeDoc::CDiffTextBuffer

// Begin declaration of CMergeDoc

    CDiffTextBuffer m_ltBuf;
    CDiffTextBuffer m_rtBuf;

protected: // create from serialization only
	CMergeDoc();
	DECLARE_DYNCREATE(CMergeDoc)

	
	// Operations
public:	
	CPtrList m_undoList;
	CArray<DIFFRANGE,DIFFRANGE> m_diffs;
	UINT m_nDiffs;
	CString m_strLeftFile, m_strRightFile;

	void SetNeedRescan();
	void RescanIfNeeded();
	int Rescan(BOOL bForced = FALSE);
	void AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op);
	void FixLastDiffRange(BOOL left);
	void ShowRescanError(int nRescanResult);
	void AddUndoAction(UINT nBegin, UINT nEnd, UINT nDiff, int nBlanks, BOOL bInsert, CMergeEditView *pList);
	BOOL Undo();
	void ListCopy(bool bSrcLeft);
	BOOL TrySaveAs(CString strPath, BOOL &bSaveSuccess, BOOL bLeft);
	BOOL DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft);
	//CString ExpandTabs(LPCTSTR szText);
	//CString Tabify(LPCTSTR szText);
	int LineToDiff(UINT nLine);
	BOOL LineInDiff(UINT nLine, UINT nDiff);
	void SetDiffViewMode(BOOL bEnable);
	
	void SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight);
	void SetDirDoc(CDirDoc * pDirDoc);
	void DirDocClosing(CDirDoc * pDirDoc);
	BOOL CloseNow();

	CMergeEditView * GetLeftView() { return m_pLeftView; }
	CMergeEditView * GetRightView() { return m_pRightView; }
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

// Implementation
public:
	BOOL SaveHelper();
	std::vector<CMergeEditView*> undoTgt;
	std::vector<CMergeEditView*>::iterator curUndo;
	void FlushAndRescan(BOOL bForced = FALSE);
	BOOL TempFilesExist();
	void CleanupTempFiles();
	BOOL InitTempFiles(const CString& srcPathL, const CString& strPathR);
	void SetCurrentDiff(int nDiff);
	int GetCurrentDiff() { return m_nCurDiff; }
	UINT CountPrevBlanks(UINT nCurLine, BOOL bLeft);
	virtual ~CMergeDoc();
  	virtual void OnFileEvent (WPARAM wEvent, LPCTSTR pszPathName);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation data
protected:
	int m_nCurDiff;		// 0-based index, -1 if no diff selected
	CString m_strTempRightFile;
	CString m_strTempLeftFile;
	CMergeEditView * m_pLeftView;
	CMergeEditView * m_pRightView;
	CDirDoc * m_pDirDoc;
	BOOL m_bEnableRescan;
	BOOL m_bNeedIdleRescan;
	COleDateTime m_LastRescan;

// friend access
	friend class RescanSuppress;


// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PrimeTextBuffers();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
