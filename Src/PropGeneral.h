/**
 * @file  PropGeneral.h
 *
 * @brief Declaration of PropGeneral class
 */
#pragma once

#include "OptionsPanel.h"

class COptionsMgr;

/**
 * @brief Class for General options -propertypage.
 */
class PropGeneral : public OptionsPanel
{
// Construction
public:
	explicit PropGeneral(COptionsMgr *optionsMgr);
	~PropGeneral();

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropGeneral)
	enum { IDD = IDD_PROPPAGE_GENERAL };
	bool  m_bScroll;
	bool  m_bSingleInstance;
	bool  m_bVerifyPaths;
	bool  m_bCloseWindowWithEsc;
	bool  m_bAskMultiWindowClose;
	int   m_nAutoCompleteSource;
	bool  m_bPreserveFiletime;
	bool  m_bShowSelectFolderOnStartup;
	bool  m_bCloseWithOK;
	CWinThread *m_pLoadLanguagesThread;
	std::vector<std::pair<LANGID, String>> m_langs;
	CComboBox	m_ctlLangList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static UINT LoadLanguagesThreadProc(void *pParam);
	virtual BOOL OnInitDialog() override;

	// Generated message map functions
	//{{AFX_MSG(PropGeneral)
	afx_msg void OnResetAllMessageBoxes();
	afx_msg LRESULT OnLoadLanguages(WPARAM, LPARAM);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
