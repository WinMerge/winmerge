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

#include "parsers/crystallineparser.h"
#include "LineInfo.h"
#include "UndoRecord.h"
#include "cepoint.h"
#include <memory>
#include <vector>
#include <list>

class CCrystalTextView;

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
    virtual void RecalcPoint (CEPoint & ptPoint) = 0;
  };


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer command target

class EDITPADC_CLASS CCrystalTextBuffer
  {
public:
    int m_nSourceEncoding;
    static int m_nDefaultEncoding;
    uint32_t m_dwCurrentRevisionNumber;
    uint32_t m_dwRevisionNumberOnSave;
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
    int FindLineWithFlag (lineflags_t dwFlag) const;

protected :

class EDITPADC_CLASS CInsertContext : public CUpdateContext
      {
public :
        CEPoint m_ptStart, m_ptEnd;
        virtual void RecalcPoint (CEPoint & ptPoint);
      };

class EDITPADC_CLASS CDeleteContext : public CUpdateContext
      {
public :
        CEPoint m_ptStart, m_ptEnd;
        virtual void RecalcPoint (CEPoint & ptPoint);
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
    CEPoint m_ptLastChange;
    //END SW

    //  Connected views
    std::list < CCrystalTextView * > m_lpViews;

    // Table Editing
    bool m_bTableEditing;
    tchar_t m_cFieldDelimiter;
    tchar_t m_cFieldEnclosure;
    bool m_bAllowNewlinesInQuotes;
    struct SharedTableProperties
    {
        std::vector<int> m_aColumnWidths;
        std::vector<CCrystalTextBuffer*> m_textBufferList;
    };
    std::shared_ptr<SharedTableProperties> m_pSharedTableProps;

    //  Helper methods
    void InsertLine (const tchar_t* pszLine, size_t nLength, int nPosition = -1, int nCount = 1);
    void AppendLine (int nLineIndex, const tchar_t* pszChars, size_t nLength, bool bDetectEol = true);
    void MoveLine(int line1, int line2, int newline1);
    void SetEmptyLine(int nPosition, int nCount = 1);

    //  Implementation
    bool InternalInsertText (CCrystalTextView * pSource, int nLine, int nPos, const tchar_t* pszText, size_t cchText, int &nEndLine, int &nEndChar);
    bool InternalDeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos);
    std::basic_string<tchar_t> StripTail (int i, size_t bytes);

    //  [JRT] Support For Descriptions On Undo/Redo Actions
    virtual void AddUndoRecord (bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos,
                                const tchar_t* pszText, size_t cchText, int nActionType = CE_ACTION_UNKNOWN, std::vector<uint32_t> *paSavedRevisionNumbers = nullptr);
    virtual UndoRecord GetUndoRecord (int nUndoPos) const { return m_aUndoBuf[nUndoPos]; }

    virtual std::vector<uint32_t> *CopyRevisionNumbers(int nStartLine, int nEndLine) const;
    virtual void RestoreRevisionNumbers(int nStartLine, std::vector<uint32_t> *psaSavedRevisionNumbers);

    //  Overridable: provide action description
    virtual bool GetActionDescription (int nAction, std::basic_string<tchar_t>& desc) const;

    // Operations
public :
    //  Construction/destruction code
    CCrystalTextBuffer ();
    ~CCrystalTextBuffer ();

    //  Basic functions
    bool InitNew (CRLFSTYLE nCrlfStyle = CRLFSTYLE::DOS);

// WinMerge has own routines for loading and saving
#ifdef CRYSTALEDIT_ENABLELOADER
    bool LoadFromFile (const tchar_t* pszFileName, CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC);
#endif
#ifdef CRYSTALEDIT_ENABLESAVER
    bool SaveToFile(const tchar_t* pszFileName, CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC, 
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
    const tchar_t* GetLineEol (int nLine) const;
    bool ChangeLineEol (int nLine, const tchar_t* lpEOL);
    const tchar_t* GetLineChars (int nLine) const;
    lineflags_t GetLineFlags (int nLine) const;
    uint32_t GetLineRevisionNumber (int nLine) const;
    int GetLineWithFlag (lineflags_t dwFlag) const;
    void SetLineFlag (int nLine, lineflags_t dwFlag, bool bSet,
            bool bRemoveFromPreviousLine = true, bool bUpdate=true);
    void GetText (int nStartLine, int nStartChar, int nEndLine, int nEndChar,
            std::basic_string<tchar_t>& text, const tchar_t* pszCRLF = nullptr, bool bExcludeInvisibleLines = true) const;
    virtual void GetTextWithoutEmptys (int nStartLine, int nStartChar,
            int nEndLine, int nEndChar, std::basic_string<tchar_t>& text,
            CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC, bool bExcludeInvisibleLines = true) const;
    virtual bool IsIndentableLine(int nLine) const { return true; }

    //  Attributes
    CRLFSTYLE GetCRLFMode () const { return m_nCRLFMode; }
    void SetCRLFMode (CRLFSTYLE nCRLFMode);
    /// Adjust all the lines in the buffer to the buffer default EOL Mode
    virtual bool applyEOLMode();
    const tchar_t* GetDefaultEol() const;
    static const tchar_t* GetStringEol(CRLFSTYLE nCRLFMode);
    bool GetReadOnly () const;
    void SetReadOnly (bool bReadOnly = true);

    void SetIgnoreEol(bool IgnoreEol) { m_IgnoreEol = IgnoreEol; }

    //  Text modification functions
    virtual bool InsertText (CCrystalTextView * pSource, int nLine, int nPos, const tchar_t* pszText, size_t cchText, int &nEndLine, int &nEndChar, int nAction = CE_ACTION_UNKNOWN, bool bHistory = true);
    virtual bool DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN, bool bHistory = true, bool bExcludeInvisibleLines = true);
    virtual bool DeleteText2 (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN, bool bHistory = true);

    //  Undo/Redo
    bool CanUndo () const;
    bool CanRedo () const;
    virtual bool Undo (CCrystalTextView * pSource, CEPoint & ptCursorPos);
    virtual bool UndoInsert (CCrystalTextView * pSource, CEPoint & ptCursorPos, const CEPoint apparent_ptStartPos, CEPoint const apparent_ptEndPos, const UndoRecord & ur);
    virtual bool Redo (CCrystalTextView * pSource, CEPoint & ptCursorPos);

    //  Undo grouping
    virtual void BeginUndoGroup (bool bMergeWithPrevious = false);
    virtual void FlushUndoGroup (CCrystalTextView * pSource);

    //BEGIN SW
    /**
    Returns the position where the last changes where made.
    */
    CEPoint GetLastChangePos() const;
    //END SW
    void RestoreLastChangePos(CEPoint pt);
    void DeleteLine(int line, int nCount = 1);


    //  Browse undo sequence
    size_t GetUndoActionCode (int & nAction, size_t pos = 0) const;
    size_t GetRedoActionCode (int & nAction, size_t pos = 0) const;
    size_t GetUndoDescription (std::basic_string<tchar_t>& desc, size_t pos = 0) const;
    size_t GetRedoDescription (std::basic_string<tchar_t>& desc, size_t pos = 0) const;

    //  Notify all connected views about changes in name of file
    CrystalLineParser::TextDefinition *RetypeViews (const tchar_t* lpszFileName);
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
    const std::vector<int>& GetColumnWidths () const { return m_pSharedTableProps->m_aColumnWidths; }
    void SetColumnWidths (const std::vector<int>& columnWidths);
    int  GetColumnCount (int nLineIndex) const;
    std::basic_string<tchar_t> GetCellText (int nLineIndex, int nColumnIndex) const;
    void SetAllowNewlinesInQuotes (bool bAllowNewlinesInQuotes) { m_bAllowNewlinesInQuotes = bAllowNewlinesInQuotes; }
    tchar_t GetAllowNewlinesInQuotes () const { return m_bAllowNewlinesInQuotes; }
    void SetFieldDelimiter (tchar_t cFieldDelimiter) { m_cFieldDelimiter = cFieldDelimiter; }
    tchar_t GetFieldDelimiter () const { return m_cFieldDelimiter; }
    void SetFieldEnclosure (tchar_t cFieldEnclosure) { m_cFieldEnclosure = cFieldEnclosure; }
    tchar_t GetFieldEnclosure () const { return m_cFieldEnclosure; }
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
