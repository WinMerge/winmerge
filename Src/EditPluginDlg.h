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
// Construction
	void Initialize(PluginType pluginType);

public:
// Construction
	CEditPluginDlg(internal_plugin::Info& info, CWnd* pParent = nullptr);
	~CEditPluginDlg();

// Dialog Data
	//{{AFX_DATA(CEditPluginDlg)
	enum { IDD = IDD_PLUGINS_EDITPLUGIN };
	String	m_strPluginName;
	String	m_strDescription;
	String	m_strExtensions;
	String	m_strArguments;
	String	m_strPluginPipeline;
	CSuperComboBox m_ctlPluginPipeline;
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	PluginType m_pluginType;
	internal_plugin::Info& m_info;
	bool m_userDefined;
};
