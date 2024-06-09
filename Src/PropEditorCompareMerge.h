/** 
 * @file  PropEditorCompareMerge.h
 *
 * @brief Declaration file for PropEditorCompareMerge propertyheet
 *
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/**
 * @brief Property page for editor options.
 *
 * Editor options affect to editor behavior. For example syntax highlighting
 * and tabs.
 */
class PropEditorCompareMerge : public OptionsPanel
{
// Construction
public:
	explicit PropEditorCompareMerge(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropEditorCompareMerge)
	enum { IDD = IDD_PROPPAGE_EDITOR_COMPAREMERGE };
	bool    m_bAutomaticRescan;
	int     m_nCopyGranularity;
	bool    m_bViewLineDifferences;
	bool    m_bBreakOnWords;
	int     m_nBreakType;
	String m_breakChars;
	//}}AFX_DATA

private:
// Implementation methods
	void LoadComboBoxStrings();
	void UpdateDataToWindow() { UpdateData(FALSE); }
	void UpdateDataFromWindow() { UpdateData(TRUE); }
	void UpdateLineDiffControls();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropEditorCompareMerge)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropEditorCompareMerge)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnDefaults();
	afx_msg void OnLineDiffControlClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
