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
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_
#pragma once


#include "DirView.h"
#include "DiffContext.h"
#include "diffThread.h"
#include "DiffWrapper.h"

#ifndef PluginManager_h_included
#include "PluginManager.h"
#endif

#ifndef REGEXP_H
#include "RegExp.h"
#endif

class CMergeDoc;
typedef CTypedPtrList<CPtrList, CMergeDoc *> MergeDocPtrList;
class DirDocFilterGlobal;
class DirDocFilterByExtension;
class CustomStatusCursor;

/////////////////////////////////////////////////////////////////////////////
// CDirDoc document

/**
 * @brief Documentclass for directory compare
 */
class CDirDoc : public CDocument
{
protected:
	CDirDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirDoc)

// Attributes
public:
// Operations
public:
	BOOL CloseMergeDocs();
	CDirView * GetMainView();
	CMergeDoc * GetMergeDocForDiff(BOOL * pNew);
	BOOL ReusingDirDoc();
	bool CanFrameClose();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetRecursive() { return m_bRecursive; }
	void SetRecursive(BOOL bRecursive);
	BOOL GetReadOnly(BOOL bLeft) const;
	void SetReadOnly(BOOL bLeft, BOOL bReadOnly);
	void RefreshOptions();
	void CompareReady();
	void UpdateChangedItem(LPCTSTR pathLeft, LPCTSTR pathRight, bool unified);
	POSITION FindItemFromPaths(LPCTSTR pathLeft, LPCTSTR pathRight);
	void SetDiffSide(UINT diffcode, int idx);
	void SetDiffCompare(UINT diffcode, int idx);
	void UpdateResources();
	void InitStatusStrings();
	void ReloadItemStatus(UINT nIdx);
	void Redisplay();
	void Rescan();
	CDiffContext *m_pCtxt;
	virtual ~CDirDoc();
	void SetDirView( CDirView *newView ); // TODO Perry
	void AddMergeDoc(CMergeDoc * pMergeDoc);
	void MergeDocClosing(CMergeDoc * pMergeDoc);
	CDiffThread m_diffThread;
	void SetDiffStatus(UINT diffcode, UINT mask, int idx);
	void SetDiffContext(CDiffContext *pCtxt);
	void UpdateHeaderPath(BOOL bLeft);
	void AbortCurrentScan();
	bool IsCurrentScanAbortable() const;
	void SetDescriptions(CString strLeftDesc, CString strRightDesc);
	void SetPluginPrediffSetting(const CString & filteredFilenames, int newsetting);
	void FetchPluginInfos(const CString& filteredFilenames, 
	                      PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);

protected:
	CDiffWrapper m_diffWrapper;

	// Generated message map functions
	//{{AFX_MSG(CDirDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation data
private:
	DirDocFilterGlobal * m_pFilterGlobal;
	DirDocFilterByExtension * m_pFilterUI;
	CDirView *m_pDirView;
	MergeDocPtrList m_MergeDocs;
	BOOL m_bReuseMergeDocs; // policy to reuse existing merge docs
	BOOL m_bROLeft; /**< Is left side read-only */
	BOOL m_bRORight; /**< Is right side read-only */
	BOOL m_bRecursive; /**< Is current compare recursive? */
	CustomStatusCursor * m_statusCursor;
	CString m_strLeftDesc; /**< Left side desription text */
	CString m_strRightDesc; /**< Left side desription text */
	PluginManager m_pluginman;
};

/**
 * @brief callback for file/directory filtering during diff
 *
 * @note This one uses the current global filter.
 */
class DirDocFilterGlobal : public IDiffFilter
{
public:
	// implement the interface IDiffFilter
	/** @brief Return TRUE unless we're suppressing this directory by filter */
	virtual BOOL includeDir(LPCTSTR szDirName);
	/** @brief Return TRUE unless we're suppressing this file by filter */
	virtual BOOL includeFile(LPCTSTR szFileName);
};

/**
 * @brief callback for file/directory filtering during diff
 *
 * @note This one uses the extension list from the open dialog.
 * Include only the matching files
 */
class DirDocFilterByExtension : public IDiffFilter
{
public:
  DirDocFilterByExtension(LPCTSTR strRegExp);

	// implement the interface IDiffFilter
	/** @brief Return TRUE unless we're suppressing this directory by filter */
	virtual BOOL includeDir(LPCTSTR szDirName);
	/** @brief Return TRUE unless we're suppressing this file by filter */
	virtual BOOL includeFile(LPCTSTR szFileName);
private:
	CRegExp m_rgx;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
