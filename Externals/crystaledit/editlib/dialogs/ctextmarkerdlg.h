/**
 *  @file ctextmarkerdlg.h
 *
 *  @brief Declaration of Marker-dialog.
 */
#pragma once

#include "resource.h"
#include "memcombo.h"
#include "../ccrystaltextmarkers.h"


/////////////////////////////////////////////////////////////////////////////
// CTextMarkerDlg dialog

class EDITPADC_CLASS CTextMarkerDlg : public CDialog
  {
private :
    CCrystalTextMarkers m_tempMarkers;
    CCrystalTextMarkers& m_markers;
    int GetSelectedItemIndex() const;
    void UpdateDataListView(bool bSaveAndValidate);

    // Construction
public :
    explicit CTextMarkerDlg (CCrystalTextMarkers& markers, const CString& sPattern = _T(""), findtext_flags_t dwFlags = 0);
    findtext_flags_t GetLastSearchFlags() const;

    // Dialog Data
    //{{AFX_DATA(CTextMarkerDlg)
    enum { IDD = IDD_EDIT_MARKER };
    CComboBox m_ctlBgColorIdx;
    CButton m_ctlWholeWord;
    CListCtrl m_listMarkers;
    CString m_sFindWhat;
    int m_nBgColorIndex;
    int m_nCurItemIndex;
    bool m_bMatchCase;
    bool m_bWholeWord;
    bool m_bRegExp;
    bool m_bMarkersEnabled;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTextMarkerDlg)
  public:
  protected:
    virtual void DoDataExchange (CDataExchange * pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

    // Implementation
protected :
    // Generated message map functions
    //{{AFX_MSG(CTextMarkerDlg)
    virtual BOOL OnInitDialog ();
    virtual void OnOK();
    afx_msg void OnBnclickedEditRegExp ();
    afx_msg void OnBnClickedEditMarkerNew();
    afx_msg void OnBnClickedEditMarkerDelete();
    afx_msg void OnBnClickedApplyNow();
    afx_msg void OnEditchangeEditFindtext();
    afx_msg void OnItemchangingEditMarkerList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnItemchangedEditMarkerList(NMHDR *pNMHDR, LRESULT *pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
public:
};
