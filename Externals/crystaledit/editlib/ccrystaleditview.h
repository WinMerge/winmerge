////////////////////////////////////////////////////////////////////////////
//  File:       ccrystaleditview.h
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Interface of the CCrystalEditView class, a part of Crystal Edit - syntax
//  coloring text editor.
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

#pragma once

#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"
#include "wispelld.h"
#include "cepoint.h"

/////////////////////////////////////////////////////////////////////////////
//  Forward class declarations

class CEditDropTargetImpl;
class CEditReplaceDlg;

/////////////////////////////////////////////////////////////////////////////
//  CCrystalEditView view

class EDITPADC_CLASS CCrystalEditView : public CCrystalTextView
  {
    DECLARE_DYNCREATE (CCrystalEditView)

    friend CEditReplaceDlg;

public :
    int m_nLastReplaceLen;

    static HMODULE hSpellDll;
    static tchar_t szWIspellPath[_MAX_PATH];
    static SpellData spellData;
    static int (*SpellInit) (SpellData*);
    static int (*SpellCheck) (SpellData*);
    static int (*SpellConfig) (SpellData*);
    static bool LoadSpellDll (bool bAlert = true);
    static int SpellGetLine (struct SpellData_t *pdata);
    static int SpellNotify (int nEvent, struct SpellData_t *pdata);

protected:
    bool m_bLastReplace;
    findtext_flags_t m_dwLastReplaceFlags;
    CEditReplaceDlg *m_pEditReplaceDlg;

protected:
    bool m_bDropPosVisible;
    CEPoint m_ptSavedCaretPos;
    bool m_bSelectionPushed;
    CEPoint m_ptSavedSelStart, m_ptSavedSelEnd;
private :
    CEPoint m_ptDropPos;
    bool m_bAutoIndent;

    //  [JRT]
    bool m_bDisableBSAtSOL;       // Disable BS At Start Of Line


public :
    virtual void ResetView () override;
protected :




    CEditDropTargetImpl * m_pDropTarget;
    virtual DROPEFFECT GetDropEffect () override;
    virtual void OnDropSource (DROPEFFECT de) override;
    void Paste ();
    void Cut ();
    bool DeleteCurrentSelection ();
    bool DeleteCurrentColumnSelection (int nAction, bool bFlushUndoGroup = true, bool bUpdateCursorPosition = true);
    bool DeleteCurrentColumnSelection2 (int nStartLine, int nEndLine, int nAction);
    bool InsertColumnText (int nLine, int nPos, const tchar_t* pszText, int cchText, int nAction, bool bFlushUndoGroup = true);

    // Attributes
public :
    bool GetAutoIndent () const;
    void SetAutoIndent (bool bAutoIndent);
    bool GetInsertTabs () const;
    void SetInsertTabs (bool bInsertTabs);

    //  [JRT]
    void SetDisableBSAtSOL (bool bDisableBSAtSOL);
    bool GetDisableBSAtSOL () const;

    void CopyProperties (CCrystalTextView* pSource) override;

    // Operations
public :
    CCrystalEditView ();
    ~CCrystalEditView ();

    bool GetOverwriteMode () const;
    void SetOverwriteMode (bool bOvrMode = true);

    void ShowDropIndicator (const CPoint & point);
    void HideDropIndicator ();

    void SetAlternateDropTarget (IDropTarget *pDropTarget);

    bool DoDropText (COleDataObject * pDataObject, const CPoint & ptClient);
    void DoDragScroll (const CPoint & point);

    virtual bool QueryEditable ();
    virtual void UpdateView (CCrystalTextView * pSource, CUpdateContext * pContext, updateview_flags_t dwFlags, int nLineIndex = -1) override;

    void SaveLastSearch(LastSearchInfos *lastSearch);
    bool ReplaceSelection (const tchar_t* pszNewText, size_t cchNewText, DWORD dwFlags, bool bGroupWithPrevious = false);

    virtual void OnEditOperation (int nAction, const tchar_t* pszText, size_t cchText) override;

    virtual bool DoSetTextType (CrystalLineParser::TextDefinition *def) override;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCrystalEditView)
    //}}AFX_VIRTUAL

    // Implementation
protected :

    // Generated message map functions
protected :
    bool m_bMergeUndo;
    //{{AFX_MSG(CCrystalEditView)
    afx_msg void OnEditPaste ();
    afx_msg void OnUpdateEditCut (CCmdUI * pCmdUI);
    afx_msg void OnEditCut ();
    afx_msg void OnUpdateEditPaste (CCmdUI * pCmdUI);
    afx_msg void OnEditDelete ();
    afx_msg void OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnEditDeleteBack ();
    afx_msg void OnEditUntab ();
    afx_msg void OnEditTab ();
    afx_msg void OnEditSwitchOvrmode ();
    afx_msg void OnUpdateEditSwitchOvrmode (CCmdUI * pCmdUI);
    afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy ();
    afx_msg void OnEditReplace ();
    afx_msg void OnUpdateEditUndo (CCmdUI * pCmdUI);
    afx_msg void OnEditUndo ();
    afx_msg void OnUpdateEditRedo (CCmdUI * pCmdUI);
    afx_msg void OnEditRedo ();
    afx_msg void OnUpdateEditAutoComplete (CCmdUI * pCmdUI);
    afx_msg void OnEditAutoComplete ();
    afx_msg void OnUpdateEditLowerCase (CCmdUI * pCmdUI);
    afx_msg void OnEditLowerCase ();
    afx_msg void OnUpdateEditUpperCase (CCmdUI * pCmdUI);
    afx_msg void OnEditUpperCase ();
    afx_msg void OnUpdateEditSwapCase (CCmdUI * pCmdUI);
    afx_msg void OnEditSwapCase ();
    afx_msg void OnUpdateEditCapitalize (CCmdUI * pCmdUI);
    afx_msg void OnEditCapitalize ();
    afx_msg void OnUpdateEditSentence (CCmdUI * pCmdUI);
    afx_msg void OnEditSentence ();
    afx_msg void OnKillFocus (CWnd * pNewWnd);
    //}}AFX_MSG
    afx_msg void OnUpdateIndicatorCol (CCmdUI * pCmdUI);
    afx_msg void OnUpdateIndicatorOvr (CCmdUI * pCmdUI);
    afx_msg void OnUpdateIndicatorRead (CCmdUI * pCmdUI);
    //BEGIN SW
    afx_msg void OnUpdateEditGotoLastChange(CCmdUI* pCmdUI);
    afx_msg void OnEditGotoLastChange();
    //END SW
    afx_msg void OnUpdateToolsSpelling (CCmdUI * pCmdUI);
    afx_msg void OnToolsSpelling ();
    afx_msg void OnUpdateToolsCharCoding (CCmdUI * pCmdUI);
    afx_msg void OnToolsCharCoding ();
    afx_msg void OnEditDeleteWord ();
    afx_msg void OnEditDeleteWordBack ();
    // cursor movement
    afx_msg void OnCharLeft();
    afx_msg void OnExtCharLeft();
    afx_msg void OnCharRight();
    afx_msg void OnExtCharRight();
    afx_msg void OnWordLeft();
    afx_msg void OnExtWordLeft();
    afx_msg void OnWordRight();
    afx_msg void OnExtWordRight();
    afx_msg void OnLineUp();
    afx_msg void OnExtLineUp();
    afx_msg void OnLineDown();
    afx_msg void OnExtLineDown();
    afx_msg void OnPageUp();
    afx_msg void OnExtPageUp();
    afx_msg void OnPageDown();
    afx_msg void OnExtPageDown();
    afx_msg void OnLineEnd();
    afx_msg void OnExtLineEnd();
    afx_msg void OnHome();
    afx_msg void OnExtHome();
    afx_msg void OnTextBegin();
    afx_msg void OnExtTextBegin();
    afx_msg void OnTextEnd();
    afx_msg void OnExtTextEnd();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    bool DoEditUndo();
    bool DoEditRedo();
    DECLARE_MESSAGE_MAP ()
  };

/////////////////////////////////////////////////////////////////////////////

inline bool CCrystalEditView::
GetOverwriteMode ()
const
{
  return m_bOvrMode;
}

inline void CCrystalEditView::SetOverwriteMode (bool bOvrMode /*= true*/ )
{
  m_bOvrMode = bOvrMode;
}

inline bool CCrystalEditView::
GetDisableBSAtSOL ()
const
{
  return m_bDisableBSAtSOL;
}

inline bool CCrystalEditView::GetAutoIndent ()
const
{
  return m_bAutoIndent;
}

inline void CCrystalEditView::SetAutoIndent (bool bAutoIndent)
{
  m_bAutoIndent = bAutoIndent;
}

inline bool CCrystalEditView::GetInsertTabs ()
const
{
  return m_pTextBuffer->GetInsertTabs();
}

inline void CCrystalEditView::SetInsertTabs (bool bInsertTabs)
{
  m_pTextBuffer->SetInsertTabs(bInsertTabs);
}
