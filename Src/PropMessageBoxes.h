/** 
 * @file  PropMessageBoxes.h
 *
 * @brief Declaration file for PropMessageBoxes propertyheet
 *
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"
#include "SubeditList.h"

class COptionsMgr;

/**
 * @brief Property page to set file type extension options for WinMerge.
 */
class PropMessageBoxes : public OptionsPanel
{
// Construction
public:
	explicit PropMessageBoxes(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropEditor)
	enum { IDD = IDD_PROPPAGE_MESSAGEBOXES};
	CSubeditList m_list;
	std::vector<int> m_answers;
	//}}AFX_DATA

private:

// Implementation methods

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
	afx_msg void OnLVNItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnResetAllMessageBoxes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
