/** 
 * @file  PropEditor.h
 *
 * @brief Declaration file for PropEditor propertyheet
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
class PropEditor : public OptionsPanel
{
// Construction
public:
	explicit PropEditor(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropEditor)
	enum { IDD = IDD_PROPPAGE_EDITOR };
	bool    m_bHiliteSyntax;
	int	    m_nTabType;
	unsigned m_nTabSize;
	bool    m_bAutomaticRescan;
	bool    m_bAllowMixedEol;
	bool    m_bViewLineDifferences;
	bool    m_bBreakOnWords;
	int     m_nBreakType;
	String m_breakChars;
	//}}AFX_DATA

private:
// Implementation methods
	void LoadBreakTypeStrings();
	void UpdateDataToWindow() { UpdateData(FALSE); }
	void UpdateDataFromWindow() { UpdateData(TRUE); }
	void UpdateLineDiffControls();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropEditor)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnLineDiffControlClicked();
	afx_msg void OnEnKillfocusTabEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
