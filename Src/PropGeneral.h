/**
 * @file  PropGeneral.h
 *
 * @brief Declaration of PropGeneral class
 */
#pragma once

#include "OptionsPanel.h"
#include "Concurrent.h"

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
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropGeneral)
	enum { IDD = IDD_PROPPAGE_GENERAL };
	bool  m_bScroll;
	bool  m_bScrollToFirstInlineDiff;
	int   m_nSingleInstance;
	int   m_nFileReloadMode;
	bool  m_bVerifyPaths;
	int   m_nCloseWindowWithEsc;
	bool  m_bAskMultiWindowClose;
	int   m_nAutoCompleteSource;
	bool  m_bPreserveFiletime;
	bool  m_bShowSelectFolderOnStartup;
	bool  m_bCloseWithOK;
	Concurrent::Task<std::vector<std::pair<LANGID, String>>> m_asyncLanguagesLoader;
	CComboBox	m_ctlLangList;
	//}}AFX_DATA

private:
	// Implementation methods
	void SetCursorSelectForLanguage(LANGID lang);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PropGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog() override;

	// Generated message map functions
	//{{AFX_MSG(PropGeneral)
	afx_msg void OnDefaults() override;
	afx_msg LRESULT OnLoadLanguages(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
