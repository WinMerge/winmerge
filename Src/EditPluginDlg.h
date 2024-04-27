// SPDX-License-Identifier: GPL-2.0-or-later
/**
 *  @file EditPluginDlg.h : 
 *
 *  @brief Declaration file for the dialog SelectUnpacker
 */ 
#pragma once

#include <vector>
#include <memory>
#include "TrDialogs.h"
#include "CMoveConstraint.h"
#include "SuperComboBox.h"
#include "UnicodeString.h"
#include "InternalPlugins.h"

/////////////////////////////////////////////////////////////////////////////
// CEditPluginDlgDlg dialog

class PluginInfo;

class CEditPluginDlg : public CTrDialog
{
public:
	enum class PluginType { Unpacker, Prediffer, EditorScript };
private:
	void LoadMethod(const internal_plugin::Method& method, int index);
	void SaveMethod(std::unique_ptr<internal_plugin::Method>& method, int index);
	void ResizeDialog(bool alias, bool hasScript);
	void UpdateControls();
	void ShowMenu(unsigned menuid, unsigned ctlid);
	void InsertMacro(unsigned menuid, unsigned id, unsigned ctlid);
	bool HasScript();
	void RemoveUnwantedCharacters(unsigned id, const String& validChars, const String& invalidChars);

public:
// Construction
	CEditPluginDlg(internal_plugin::Info& info, CWnd* pParent = nullptr);
	~CEditPluginDlg();

// Dialog Data
	//{{AFX_DATA(CEditPluginDlg)
	enum { IDD = IDD_PLUGINS_EDITPLUGIN };
	CComboBox m_ctlEvent;
	String	m_strEvent;
	String	m_strPluginName;
	String	m_strDescription;
	String	m_strExtensions;
	String	m_strArguments;
	String	m_strProcessType;
	String	m_strMenuCaption;
	String	m_strWindowType;
	String	m_strUnpackedFileExtension;
	String	m_strPluginPipeline;
	CTabCtrl m_ctlTab;
	String	m_strCommandline;
	String	m_strScriptFileExtension;
	String	m_strScriptBody;
	String	m_strCommandlineAry[5];
	String	m_strScriptFileExtensionAry[5];
	String	m_strScriptBodyAry[5];
	int m_nWindowType;
	bool m_bIsAutomatic;
	bool m_bArgumentsRequired;
	bool m_bGenerateEditorScript;
	prdlg::CMoveConstraint m_constraint; //*< Resizes dialog controls when dialog resized */
	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPluginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEditPluginDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog() override;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedPipelineMenu();
	afx_msg void OnBnClickedCommandLineMenu();
	afx_msg void OnPluginPipelineMenu(unsigned id);
	afx_msg void OnPluginCommandlineMenu(unsigned id);
	afx_msg void OnSelchangePluginType();
	afx_msg void OnTcnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangePluginName();
	afx_msg void OnEnChangePluginExtendedProperty(unsigned id);
	afx_msg void OnEnChangePluginCommandLine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	internal_plugin::Info& m_info;
	bool m_bInEnChange;
};
