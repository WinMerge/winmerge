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

#if !defined(AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
#define AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ccrystaltextview.h"
#include "wispelld.h"

/////////////////////////////////////////////////////////////////////////////
//  Forward class declarations

class CEditDropTargetImpl;

/////////////////////////////////////////////////////////////////////////////
//  CCrystalEditView view

class EDITPADC_CLASS CCrystalEditView : public CCrystalTextView
  {
    DECLARE_DYNCREATE (CCrystalEditView)

public :
    int m_nLastReplaceLen;
    CMap<CString, LPCTSTR, CString, LPCTSTR> *m_mapExpand;

    static HMODULE hSpellDll;
    static TCHAR szWIspellPath[_MAX_PATH];
    static SpellData spellData;
    static int (*SpellInit) (SpellData*);
    static int (*SpellCheck) (SpellData*);
    static int (*SpellConfig) (SpellData*);
    static BOOL LoadSpellDll (BOOL bAlert = TRUE);
    static int SpellGetLine (struct SpellData_t *pdata);
    static int SpellNotify (int nEvent, struct SpellData_t *pdata);

protected:
    BOOL m_bLastReplace;
    DWORD m_dwLastReplaceFlags;

private :
    BOOL m_bOvrMode;
    BOOL m_bDropPosVisible;
    CPoint m_ptSavedCaretPos;
    CPoint m_ptDropPos;
    BOOL m_bSelectionPushed;
    CPoint m_ptSavedSelStart, m_ptSavedSelEnd;
    BOOL m_bAutoIndent;

    //  [JRT]
    BOOL m_bDisableBSAtSOL;       // Disable BS At Start Of Line


public :
    virtual void ResetView ();
protected :




    CEditDropTargetImpl * m_pDropTarget;
    virtual DROPEFFECT GetDropEffect ();
    virtual void OnDropSource (DROPEFFECT de);
    void Paste ();
    void Cut ();
    BOOL DeleteCurrentSelection ();

    // Attributes
public :
    BOOL GetAutoIndent () const;
    void SetAutoIndent (BOOL bAutoIndent);
    BOOL GetInsertTabs () const;
    void SetInsertTabs (BOOL bInsertTabs);

    //  [JRT]
    void SetDisableBSAtSOL (BOOL bDisableBSAtSOL);
    BOOL GetDisableBSAtSOL () const;

    // Operations
public :
    CCrystalEditView ();
    ~CCrystalEditView ();

    BOOL GetOverwriteMode () const;
    void SetOverwriteMode (BOOL bOvrMode = TRUE);

    void ShowDropIndicator (const CPoint & point);
    void HideDropIndicator ();

    BOOL DoDropText (COleDataObject * pDataObject, const CPoint & ptClient);
    void DoDragScroll (const CPoint & point);

    virtual BOOL QueryEditable ();
    virtual void UpdateView (CCrystalTextView * pSource, CUpdateContext * pContext, DWORD dwFlags, int nLineIndex = -1);

    BOOL ReplaceSelection (LPCTSTR pszNewText, DWORD dwFlags);

    virtual void OnEditOperation (int nAction, LPCTSTR pszText);

    virtual bool DoSetTextType (TextDefinition *def);

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
    afx_msg void OnUpdateEditAutoExpand (CCmdUI * pCmdUI);
    afx_msg void OnEditAutoExpand ();
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
    DECLARE_MESSAGE_MAP ()
  };

/////////////////////////////////////////////////////////////////////////////

#if ! (defined(CE_FROM_DLL) || defined(CE_DLL_BUILD))
#include "ccrystaleditview.inl"
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCRYSTALEDITVIEW_H__8F3F8B63_6F66_11D2_8C34_0080ADB86836__INCLUDED_)
