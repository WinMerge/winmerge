/** 
 * @file  PropEditorSyntax.h
 *
 * @brief Declaration file for PropEditorSyntax propertyheet
 *
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"
#include "SubeditList.h"
#include "ccrystaltextview.h"

class COptionsMgr;

/**
 * @brief Property page to set file type extension options for WinMerge.
 */
class PropEditorSyntax : public OptionsPanel
{
// Construction
public:
	explicit PropEditorSyntax(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropEditor)
	enum { IDD = IDD_PROPPAGE_EDITOR_SYNTAX };
	CSubeditList m_listSyntax;
	CComboBox m_comboSqlDialect;
	//}}AFX_DATA

private:
	bool m_init;
	String m_Extension[LangServices::LanguageId::SRC_MAX_ENTRY];

// Implementation methods
	void InitList();
	void MoveSqlExtension(int dialect);
	static bool HasSqlExtension(const String& extensions);
	static String RemoveSqlExtension(const String& extensions);

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
	afx_msg void OnDefaults();
	afx_msg void OnSqlDialectChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
