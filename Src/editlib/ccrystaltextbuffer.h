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

#if !defined(AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_)
#define AFX_CCRYSTALTEXTBUFFER_H__AD7F2F49_6CB3_11D2_8C32_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ccrystaltextview.h"

#ifndef __AFXTEMPL_H__
#pragma message("Include <afxtempl.h> in your stdafx.h to avoid this message")
#include <afxtempl.h>
#endif

#define UNDO_DESCRIP_BUF        32

enum LINEFLAGS
{
  LF_BOOKMARK_FIRST = 0x00000001L,
  LF_EXECUTION = 0x00010000L,
  LF_BREAKPOINT = 0x00020000L,
  LF_COMPILATION_ERROR = 0x00040000L,
  LF_BOOKMARKS = 0x00080000L,
  LF_INVALID_BREAKPOINT = 0x00100000L,
  LF_DIFF = 0x00200000L,
  LF_DELETED = 0x00400000L,
  LF_LEFT_ONLY = 0x00800000L,
  LF_RIGHT_ONLY = 0x01000000L
};
#define LF_WINMERGE_FLAGS	0x01e00000

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

protected :
    BOOL m_bInit;
    BOOL m_bReadOnly;
    BOOL m_bModified;
    int m_nCRLFMode;
    BOOL m_EolSensitive;
    BOOL m_bCreateBackupFile;
    int m_nUndoBufSize;
    BOOL m_bInsertTabs;
    int  m_nTabSize;
    int FindLineWithFlag (DWORD dwFlag);

protected :
#pragma pack(push, 1)
    //  Nested class declarations
    struct SLineInfo
      {
        TCHAR *m_pcLine;
        int m_nLength, m_nMax;
        int m_nEolChars; // # of eolchars
        DWORD m_dwFlags;

        int FullLength() const { return m_nLength+m_nEolChars; }
        int Length() const { return m_nLength; }

        SLineInfo ()
        {
          memset (this, 0, sizeof (SLineInfo));
        };
      };

    enum
    {
      UNDO_INSERT = 0x0001,
      UNDO_BEGINGROUP = 0x0100
    };

    //  [JRT] Support For Descriptions On Undo/Redo Actions
    struct SUndoRecord
      {
        DWORD m_dwFlags;
        
        // Undo records store file line numbers, not screen line numbers
        // File line numbers do not count ghost lines
        // (ghost lines are lines with no text and no EOL chars, which are
        //  used by WinMerge as left-only or right-only placeholders)

        CPoint m_ptStartPos, m_ptEndPos;  //  Block of text participating

        int m_nAction;            //  For information only: action type

private :
        //  TCHAR   *m_pcText;
        //  Since in most cases we have 1 character here,
        //  we should invent a better way. Note: 2 * sizeof(WORD) <= sizeof(TCHAR*)
        //
        //  Here we will use the following trick: on Win32 platforms high-order word
        //  of any pointer will be != 0. So we can store 1 character strings without
        //  allocating memory.
        //

        union
          {
            TCHAR *m_pszText;     //  For cases when we have > 1 character strings

            TCHAR m_szText[2];    //  For single-character strings

          };

public :
        SUndoRecord () // default constructor
        {
          memset (this, 0, sizeof (SUndoRecord));
        }
        SUndoRecord (const SUndoRecord & src) // copy constructor
        {
          memset (this, 0, sizeof (SUndoRecord));
          (*this)=src;
        }
        SUndoRecord & operator=(const SUndoRecord & src) // copy assignment
        {
          m_dwFlags = src.m_dwFlags;
          m_ptStartPos = src.m_ptStartPos;
          m_ptEndPos = src.m_ptEndPos;
          m_nAction = src.m_nAction;
          SetText(src.GetText());
	   return *this;
        }
        ~SUndoRecord () // destructor
        {
          FreeText();
        }

        void SetText (LPCTSTR pszText);
        void FreeText ();

        LPCTSTR GetText () const
        {
          if (HIWORD ((DWORD) m_pszText) != 0)
            return m_pszText;
          return m_szText;
        };
      };

#pragma pack(pop)

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
    CArray < SLineInfo, SLineInfo & >m_aLines;
    // A RealityBlock is a block of lines with no ghost lines
    struct RealityBlock { int nStartReal; int nStartApparent; int nCount; };
    // The array of reality blocks is kept in order
    CArray < RealityBlock, RealityBlock& > m_RealityBlocks;

    //  Undo
    CArray < SUndoRecord, SUndoRecord & >m_aUndoBuf;
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
    void InsertLine (LPCTSTR pszLine, int nLength = -1, int nPosition = -1);
    void AppendLine (int nLineIndex, LPCTSTR pszChars, int nLength = -1);

    //  Implementation
    BOOL InternalInsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar);
    BOOL InternalDeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int * pRealStart=0, int * pRealEnd=0);
    CString StripTail (int i, int bytes);

    //  [JRT] Support For Descriptions On Undo/Redo Actions
    void AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
                        LPCTSTR pszText, int nActionType = CE_ACTION_UNKNOWN);

    //  Overridable: provide action description
    virtual BOOL GetActionDescription (int nAction, CString & desc);

    // Operations
public :
    //  Construction/destruction code
    CCrystalTextBuffer ();
    ~CCrystalTextBuffer ();

    //  Basic functions
    BOOL InitNew (int nCrlfStyle = CRLF_STYLE_DOS);

// WinMerge has own routines for loading and saving
#if 0
    BOOL LoadFromFile (LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC);
    BOOL SaveToFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC, 
    BOOL bClearModifiedFlag = TRUE);
#endif

    void FreeAll ();

    //  'Dirty' flag
    virtual void SetModified (BOOL bModified = TRUE);
    BOOL IsModified () const;

    //  Connect/disconnect views
    void AddView (CCrystalTextView * pView);
    void RemoveView (CCrystalTextView * pView);

    //  Text access functions
    int GetLineCount ();
    int GetLineLength (int nLine);
    int GetFullLineLength (int nLine); // including EOLs
    LPCTSTR GetLineEol (int nLine);
    LPTSTR GetLineChars (int nLine);
    DWORD GetLineFlags (int nLine);
    int GetLineWithFlag (DWORD dwFlag);
    void SetLineFlag (int nLine, DWORD dwFlag, BOOL bSet, BOOL bRemoveFromPreviousLine = TRUE, BOOL bUpdate=TRUE);
    void GetText (int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString & text, LPCTSTR pszCRLF = NULL);

    //  Attributes
    int GetCRLFMode ();
    void SetCRLFMode (int nCRLFMode);
    LPCTSTR CCrystalTextBuffer::GetDefaultEol() const;
    BOOL GetReadOnly () const;
    void SetReadOnly (BOOL bReadOnly = TRUE);

    void SetEolSensitivity(BOOL EolSensitive) { m_EolSensitive = EolSensitive; }

    //  Text modification functions
    BOOL InsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar, int nAction = CE_ACTION_UNKNOWN, BOOL bUpdate =TRUE);
    BOOL DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartPos, int nEndLine, int nEndPos, int nAction = CE_ACTION_UNKNOWN, BOOL bUpdate =TRUE);
    void FinishLoading();

    //  Undo/Redo
    BOOL CanUndo ();
    BOOL CanRedo ();
    BOOL Undo (CPoint & ptCursorPos);
    BOOL Redo (CPoint & ptCursorPos);

    //  Undo grouping
    void BeginUndoGroup (BOOL bMergeWithPrevious = FALSE);
    void FlushUndoGroup (CCrystalTextView * pSource);

    //BEGIN SW
    /**
    Returns the position where the last changes where made.
    */
    CPoint GetLastChangePos() const;
    //END SW
    void DeleteLine(int line);


    //  Browse undo sequence
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

    // Implementation
protected :
    // Code for mapping between file line numbers (real line numbers)
    // and screen line numbers (apparent line numbers)
    // This is needed to handle ghost lines (ones with no text or EOL chars)
    // which WinMerge uses for left-only or right-only lines.
    int ComputeRealLine(int nApparentLine) const;
    int ComputeApparentLine(int nRealLine) const;
    int LastRealLine() const;
    void RecomputeRealityMapping();

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
