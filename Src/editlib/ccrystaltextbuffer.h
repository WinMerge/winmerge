////////////////////////////////////////////////////////////////////////////
//  File:       ccrystaltextbuffer.h
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Interface of the CCrystalTextBuffer class, a part of Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: see cpps ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////
/**
 * @file  ccrystaltextbuffer.h
 *
 * @brief Declaration file for CCrystalTextBuffer.
 */
// ID line follows -- this is updated by SVN
// $Id$


#if !defined(AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_)
#define AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_

#pragma once

#include <vector>
#include "LineInfo.h"
#include "UndoRecord.h"
#include "ccrystaltextview.h"

#ifndef __AFXTEMPL_H__
#pragma message("Include <afxtempl.h> in your stdafx.h to avoid this message")
#include <afxtempl.h>
#endif

enum LINEFLAGS
{
  LF_BOOKMARK_FIRST = 0x00000001L,
  LF_EXECUTION = 0x00010000L,
  LF_BREAKPOINT = 0x00020000L,
  LF_COMPILATION_ERROR = 0x00040000L,
  LF_BOOKMARKS = 0x00080000L,
  LF_INVALID_BREAKPOINT = 0x00100000L
};

#define LF_BOOKMARK(id)     (LF_BOOKMARK_FIRST << id)

enum CRLFSTYLE
{
  CRLF_STYLE_AUTOMATIC = -1,
  CRLF_STYLE_DOS = 0,
  CRLF_STYLE_UNIX = 1,
  CRLF_STYLE_MAC = 2
};

enum
{
  CE_ACTION_UNKNOWN = 0,
  CE_ACTION_PASTE = 1,
  CE_ACTION_DELSEL = 2,
  CE_ACTION_CUT = 3,
  CE_ACTION_TYPING = 4,
  CE_ACTION_BACKSPACE = 5,
  CE_ACTION_INDENT = 6,
  CE_ACTION_DRAGDROP = 7,
  CE_ACTION_REPLACE = 8,
  CE_ACTION_DELETE = 9,
  CE_ACTION_AUTOINDENT = 10,
  CE_ACTION_AUTOCOMPLETE = 11,
  CE_ACTION_AUTOEXPAND = 12,
  CE_ACTION_LOWERCASE = 13,
  CE_ACTION_UPPERCASE = 14,
  CE_ACTION_SWAPCASE = 15,
  CE_ACTION_CAPITALIZE = 16,
  CE_ACTION_SENTENCIZE = 17,
  CE_ACTION_RECODE  = 18, 
  CE_ACTION_SPELL = 19
                         //  ...
                         //  Expandable: user actions allowed
};


/////////////////////////////////////////////////////////////////////////////
// CUpdateContext class

class EDITPADC_CLASS CUpdateContext
  {
public :
    virtual void RecalcPoint (CPoint & ptPoint) = 0;
  };


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer command target

class EDITPADC_CLASS CCrystalTextBuffer : public CCmdTarget
  {
public:
    DECLARE_DYNCREATE (CCrystalTextBuffer)

    int m_nSourceEncoding;
    static int m_nDefaultEncoding;
    DWORD m_dwCurrentRevisionNumber;
    DWORD m_dwRevisionNumberOnSave;
    BOOL IsTextBufferInitialized () const { return m_bInit; }

protected :
    BOOL m_bInit;
    BOOL m_bReadOnly;
    BOOL m_bModified;
    CRLFSTYLE m_nCRLFMode;
    BOOL m_IgnoreEol;
    BOOL m_bCreateBackupFile;
    BOOL m_bInsertTabs;
    int  m_nTabSize;
    int FindLineWithFlag (DWORD dwFlag);

protected :
    enum
    {
      UNDO_INSERT = 0x0001,
      UNDO_BEGINGROUP = 0x0100
    };

class EDITPADC_CLASS CInsertContext : public CUpdateContext
      {
public :
        CPoint m_ptStart, m_ptEnd;
        virtual void RecalcPoint (CPoint & ptPoint);
      };

class EDITPADC_CLASS CDeleteContext : public CUpdateContext
      {
public :
        CPoint m_ptStart, m_ptEnd;
        virtual void RecalcPoint (CPoint & ptPoint);
      };

    //  Lines of text
    std::vector<LineInfo> m_aLines; /**< Text lines. */

    //  Undo
    std::vector<UndoRecord> m_aUndoBuf; /**< Undo records. */
    int m_nUndoPosition;
    int m_nSyncPosition;
    BOOL m_bUndoGroup, m_bUndoBeginGroup;

    //BEGIN SW
    /** Position where the last change was made. */
    CPoint m_ptLastChange;
    //END SW

    //  Connected views
    CList < CCrystalTextView *, CCrystalTextView * >m_lpViews;

    //  Helper methods
    void InsertLine (LPCTSTR pszLine, int nLength, int nPosition = -1, int nCount = 1);
    void AppendLine (int nLineIndex, LPCTSTR pszChars, int nLength);
    void MoveLine(int line1, int line2, int newline1);
    void SetEmptyLine(int nPosition, int nCount = 1);

    //  Implementation
    BOOL InternalInsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, int cchText, int &nEndLine, int &nEndChar);
    BOOL InternalDeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos);
    CString StripTail (int i, int bytes);

    //  [JRT] Support For Descriptions On Undo/Redo Actions
    virtual void AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
                                LPCTSTR pszText, int cchText, int nActionType = CE_ACTION_UNKNOWN, CDWordArray *paSavedRevisonNumbers = NULL);

    //  Overridable: provide action description
    virtual BOOL GetActionDescription (int nAction, CString & desc);

    // Operations
public :
    //  Construction/destruction code
    CCrystalTextBuffer ();
    ~CCrystalTextBuffer ();

    //  Basic functions
    BOOL InitNew (CRLFSTYLE nCrlfStyle = CRLF_STYLE_DOS);

// WinMerge has own routines for loading and saving
#ifdef CRYSTALEDIT_ENABLELOADER
    BOOL LoadFromFile (LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC);
#endif
#ifdef CRYSTALEDIT_ENABLESAVER
    BOOL SaveToFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC, 
    BOOL bClearModifiedFlag = TRUE);
#endif

    void FreeAll ();

    void ResetInit() { FreeAll(); InitNew(); } // reset new or used buffer

    //  'Dirty' flag
    virtual void SetModified (BOOL bModified = TRUE);
    BOOL IsModified () const;

    //  Connect/disconnect views
    void AddView (CCrystalTextView * pView);
    void RemoveView (CCrystalTextView * pView);

    //  Text access functions
    int GetLineCount () const;
    int GetLineLength (int nLine) const;
    int GetFullLineLength (int nLine) const; // including EOLs
    LPCTSTR GetLineEol (int nLine) const;
    BOOL ChangeLineEol (int nLine, LPCTSTR lpEOL);
    LPCTSTR GetLineChars (int nLine) const;
    DWORD GetLineFlags (int nLine) const;
    DWORD GetLineRevisionNumber (int nLine) const;
    int GetLineWithFlag (DWORD dwFlag);
    void SetLineFlag (int nLine, DWORD dwFlag, BOOL bSet, BOOL bRemoveFromPreviousLine = TRUE, BOOL bUpdate=TRUE);
    void GetText (int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString & text, LPCTSTR pszCRLF = NULL);
    virtual void GetTextWithoutEmptys (int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString &text, CRLFSTYLE nCrlfStyle =CRLF_STYLE_AUTOMATIC );

    //  Attributes
    CRLFSTYLE GetCRLFMode ();
    void SetCRLFMode (CRLFSTYLE nCRLFMode);
    /// Adjust all the lines in the buffer to the buffer default EOL Mode
    virtual BOOL applyEOLMode();
    LPCTSTR GetDefaultEol() const;
    static LPCTSTR GetStringEol(CRLFSTYLE nCRLFMode);
    BOOL GetReadOnly () const;
    void SetReadOnly (BOOL bReadOnly = TRUE);

    void SetIgnoreEol(BOOL IgnoreEol) { m_IgnoreEol = IgnoreEol; }

    //  Text modification functions
    virtual BOOL InsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, int cchText, int &nEndLine, int &nEndChar, int nAction = CE_ACTION_UNKNOWN, BOOL bHistory =TRUE);
    virtual BOOL DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN, BOOL bHistory =TRUE);

    //  Undo/Redo
    BOOL CanUndo ();
    BOOL CanRedo ();
    virtual BOOL Undo (CCrystalTextView * pSource, CPoint & ptCursorPos);
    virtual BOOL Redo (CCrystalTextView * pSource, CPoint & ptCursorPos);

    //  Undo grouping
    virtual void BeginUndoGroup (BOOL bMergeWithPrevious = FALSE);
    virtual void FlushUndoGroup (CCrystalTextView * pSource);

    //BEGIN SW
    /**
    Returns the position where the last changes where made.
    */
    CPoint GetLastChangePos() const;
    //END SW
    void RestoreLastChangePos(CPoint pt);
    void DeleteLine(int line, int nCount = 1);


    //  Browse undo sequence
    POSITION GetUndoActionCode (int & nAction, POSITION pos = NULL);
    POSITION GetRedoActionCode (int & nAction, POSITION pos = NULL);
    POSITION GetUndoDescription (CString & desc, POSITION pos = NULL);
    POSITION GetRedoDescription (CString & desc, POSITION pos = NULL);

    //  Notify all connected views about changes in name of file
    CCrystalTextView::TextDefinition *RetypeViews (LPCTSTR lpszFileName);
    //  Notify all connected views about changes in text
    void UpdateViews (CCrystalTextView * pSource, CUpdateContext * pContext,
                      DWORD dwUpdateFlags, int nLineIndex = -1);

    // Tabs/space inserting
    BOOL GetInsertTabs() const;
    void SetInsertTabs(BOOL bInsertTabs);

    // Tabbing
    int  GetTabSize();
    void SetTabSize(int nTabSize);

    // More bookmarks
    int FindNextBookmarkLine (int nCurrentLine = 0);
    int FindPrevBookmarkLine (int nCurrentLine = 0);

    BOOL IsMBSLead (int nLine, int nCol);
    BOOL IsMBSTrail (int nLine, int nCol);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCrystalTextBuffer)
    //}}AFX_VIRTUAL


    // Generated message map functions
    //{{AFX_MSG(CCrystalTextBuffer)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP ()
  };

#if ! (defined(CE_FROM_DLL) || defined(CE_DLL_BUILD))
#include "ccrystaltextbuffer.inl"
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_)
