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

#pragma once

#include <vector>
#include "LineInfo.h"
#include "UndoRecord.h"
#include "ccrystaltextview.h"

#ifndef __AFXTEMPL_H__
#pragma message("Include <afxtempl.h> in your stdafx.h to avoid this message")
#include <afxtempl.h>
#endif

enum LINEFLAGS: unsigned long
{
  LF_BOOKMARK_FIRST = 0x00000001UL,
  LF_EXECUTION = 0x00010000UL,
  LF_BREAKPOINT = 0x00020000UL,
  LF_COMPILATION_ERROR = 0x00040000UL,
  LF_BOOKMARKS = 0x00080000UL,
  LF_INVALID_BREAKPOINT = 0x00100000UL,
  LF_INVISIBLE = 0x80000000UL
};

#define LF_BOOKMARK(id)     (LF_BOOKMARK_FIRST << id)

enum class CRLFSTYLE
{
  AUTOMATIC = -1,
  DOS = 0,
  UNIX = 1,
  MAC = 2,
  MIXED = 3
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
    bool IsTextBufferInitialized () const { return m_bInit; }

protected :
    bool m_bInit;
    bool m_bReadOnly;
    bool m_bModified;
    CRLFSTYLE m_nCRLFMode;
    bool m_IgnoreEol;
    bool m_bCreateBackupFile;
    bool m_bInsertTabs;
    int  m_nTabSize;
    int FindLineWithFlag (DWORD dwFlag) const;

protected :
    enum : unsigned
    {
      UNDO_INSERT = 0x0001U,
      UNDO_BEGINGROUP = 0x0100U
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
    bool m_bUndoGroup, m_bUndoBeginGroup;

    //BEGIN SW
    /** Position where the last change was made. */
    CPoint m_ptLastChange;
    //END SW

    //  Connected views
    CList < CCrystalTextView *, CCrystalTextView * >m_lpViews;

    // Table Editing
    bool m_bTableEditing;
    TCHAR m_cFieldDelimiter;
    TCHAR m_cFieldEnclosure;
    bool m_bAllowNewlinesInQuotes;
    struct SharedTableProperties
    {
        std::vector<int> m_aColumnWidths;
        std::vector<CCrystalTextBuffer*> m_textBufferList;
    };
    std::shared_ptr<SharedTableProperties> m_pSharedTableProps;

    //  Helper methods
    void InsertLine (LPCTSTR pszLine, size_t nLength, int nPosition = -1, int nCount = 1);
    void AppendLine (int nLineIndex, LPCTSTR pszChars, size_t nLength, bool bDetectEol = true);
    void MoveLine(int line1, int line2, int newline1);
    void SetEmptyLine(int nPosition, int nCount = 1);

    //  Implementation
    bool InternalInsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, size_t cchText, int &nEndLine, int &nEndChar);
    bool InternalDeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos);
    CString StripTail (int i, size_t bytes);

    //  [JRT] Support For Descriptions On Undo/Redo Actions
    virtual void AddUndoRecord (bool bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
                                LPCTSTR pszText, size_t cchText, int nActionType = CE_ACTION_UNKNOWN, CDWordArray *paSavedRevisionNumbers = nullptr);
    virtual UndoRecord GetUndoRecord (int nUndoPos) const { return m_aUndoBuf[nUndoPos]; }

    virtual CDWordArray *CopyRevisionNumbers(int nStartLine, int nEndLine) const;
    virtual void RestoreRevisionNumbers(int nStartLine, CDWordArray *psaSavedRevisionNumbers);

    //  Overridable: provide action description
    virtual bool GetActionDescription (int nAction, CString & desc) const;

    // Operations
public :
    //  Construction/destruction code
    CCrystalTextBuffer ();
    ~CCrystalTextBuffer ();

    //  Basic functions
    bool InitNew (CRLFSTYLE nCrlfStyle = CRLFSTYLE::DOS);

// WinMerge has own routines for loading and saving
#ifdef CRYSTALEDIT_ENABLELOADER
    bool LoadFromFile (LPCTSTR pszFileName, CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC);
#endif
#ifdef CRYSTALEDIT_ENABLESAVER
    bool SaveToFile(LPCTSTR pszFileName, CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC, 
    bool bClearModifiedFlag = true);
#endif

    void FreeAll ();

    void ResetInit() { FreeAll(); InitNew(); } // reset new or used buffer

    //  'Dirty' flag
    virtual void SetModified (bool bModified = true);
    bool IsModified () const;

    //  Connect/disconnect views
    void AddView (CCrystalTextView * pView);
    void RemoveView (CCrystalTextView * pView);

    //  Text access functions
    int GetLineCount () const;
    int GetLineLength (int nLine) const;
    int GetFullLineLength (int nLine) const; // including EOLs
    LPCTSTR GetLineEol (int nLine) const;
    bool ChangeLineEol (int nLine, LPCTSTR lpEOL);
    LPCTSTR GetLineChars (int nLine) const;
    DWORD GetLineFlags (int nLine) const;
    DWORD GetLineRevisionNumber (int nLine) const;
    int GetLineWithFlag (DWORD dwFlag) const;
    void SetLineFlag (int nLine, DWORD dwFlag, bool bSet,
            bool bRemoveFromPreviousLine = true, bool bUpdate=true);
    void GetText (int nStartLine, int nStartChar, int nEndLine, int nEndChar,
            CString & text, LPCTSTR pszCRLF = nullptr, bool bExcludeInvisibleLines = true) const;
    virtual void GetTextWithoutEmptys (int nStartLine, int nStartChar,
            int nEndLine, int nEndChar, CString &text,
            CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC, bool bExcludeInvisibleLines = true) const;
    virtual bool IsIndentableLine(int nLine) const { return true; }

    //  Attributes
    CRLFSTYLE GetCRLFMode () const { return m_nCRLFMode; }
    void SetCRLFMode (CRLFSTYLE nCRLFMode);
    /// Adjust all the lines in the buffer to the buffer default EOL Mode
    virtual bool applyEOLMode();
    LPCTSTR GetDefaultEol() const;
    static LPCTSTR GetStringEol(CRLFSTYLE nCRLFMode);
    bool GetReadOnly () const;
    void SetReadOnly (bool bReadOnly = true);

    void SetIgnoreEol(bool IgnoreEol) { m_IgnoreEol = IgnoreEol; }

    //  Text modification functions
    virtual bool InsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, size_t cchText, int &nEndLine, int &nEndChar, int nAction = CE_ACTION_UNKNOWN, bool bHistory = true);
    virtual bool DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN, bool bHistory = true, bool bExcludeInvisibleLines = true);
    virtual bool DeleteText2 (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN, bool bHistory = true);

    //  Undo/Redo
    bool CanUndo () const;
    bool CanRedo () const;
    virtual bool Undo (CCrystalTextView * pSource, CPoint & ptCursorPos);
    virtual bool UndoInsert (CCrystalTextView * pSource, CPoint & ptCursorPos, const CPoint apparent_ptStartPos, CPoint const apparent_ptEndPos, const UndoRecord & ur);
    virtual bool Redo (CCrystalTextView * pSource, CPoint & ptCursorPos);

    //  Undo grouping
    virtual void BeginUndoGroup (bool bMergeWithPrevious = false);
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
    POSITION GetUndoActionCode (int & nAction, POSITION pos = nullptr) const;
    POSITION GetRedoActionCode (int & nAction, POSITION pos = nullptr) const;
    POSITION GetUndoDescription (CString & desc, POSITION pos = nullptr) const;
    POSITION GetRedoDescription (CString & desc, POSITION pos = nullptr) const;

    //  Notify all connected views about changes in name of file
    CrystalLineParser::TextDefinition *RetypeViews (LPCTSTR lpszFileName);
    //  Notify all connected views about changes in text
    void UpdateViews (CCrystalTextView * pSource, CUpdateContext * pContext,
                      DWORD dwUpdateFlags, int nLineIndex = -1);

    // Tabs/space inserting
    bool GetInsertTabs() const;
    void SetInsertTabs(bool bInsertTabs);

    // Tabbing
    int  GetTabSize() const;
    void SetTabSize(int nTabSize);

    // Table Editing
    void ShareColumnWidths (CCrystalTextBuffer& other)
    {
      if (this == &other)
        return;
      m_pSharedTableProps = other.m_pSharedTableProps;
      m_pSharedTableProps->m_textBufferList.push_back (this);
    }
    int  GetColumnWidth (int nColumnIndex) const;
    void SetColumnWidth (int nColumnIndex, int nColumnWidth);
    int  GetColumnCount (int nLineIndex) const;
    void SetAllowNewlinesInQuotes (bool bAllowNewlinesInQuotes) { m_bAllowNewlinesInQuotes = bAllowNewlinesInQuotes; }
    TCHAR GetAllowNewlinesInQuotes () const { return m_bAllowNewlinesInQuotes; }
    void SetFieldDelimiter (TCHAR cFieldDelimiter) { m_cFieldDelimiter = cFieldDelimiter; }
    TCHAR GetFieldDelimiter () const { return m_cFieldDelimiter; }
    void SetFieldEnclosure (TCHAR cFieldEnclosure) { m_cFieldEnclosure = cFieldEnclosure; }
    TCHAR GetFieldEnclosure () const { return m_cFieldEnclosure; }
    bool GetTableEditing () const { return m_bTableEditing; }
    void SetTableEditing (bool bTableEditing) { m_bTableEditing = bTableEditing; }
    void JoinLinesForTableEditingMode ();
    void SplitLinesForTableEditingMode ();
    void InvalidateColumns ();
    std::vector<CCrystalTextBuffer*> GetTextBufferList () const { return m_pSharedTableProps->m_textBufferList; }

    // More bookmarks
    int FindNextBookmarkLine (int nCurrentLine = 0) const;
    int FindPrevBookmarkLine (int nCurrentLine = 0) const;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCrystalTextBuffer)
    //}}AFX_VIRTUAL


    // Generated message map functions
    //{{AFX_MSG(CCrystalTextBuffer)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP ()
  };


inline bool CCrystalTextBuffer::IsModified () const
{
  return m_bModified;
}

inline bool CCrystalTextBuffer::GetInsertTabs() const          //UPDATE-BEGIN
{
    return m_bInsertTabs;
}

inline void CCrystalTextBuffer::SetInsertTabs(bool bInsertTabs)
{
    m_bInsertTabs = bInsertTabs;
}
