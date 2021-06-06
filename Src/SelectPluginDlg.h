/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file SelectPluginDlg.h : 
 *
 *  @brief Declaration file for the dialog SelectUnpacker
 */ 
#pragma once

#include <vector>
#include <memory>
#include "TrDialogs.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectPluginDlgDlg dialog

class PluginInfo;

class CSelectPluginDlg : public CTrDialog
{
private:
// Construction
	void Initialize();

public:
// Construction
	CSelectPluginDlg(const String& pluginPipeline, const String& filename, CWnd* pParent /*= nullptr*/);
	CSelectPluginDlg(const String& pluginPipeline, const String& filename1, const String& filename2, CWnd* pParent /*= nullptr*/);
	~CSelectPluginDlg();

	const String& CSelectPluginDlg::GetPluginPipeline() const { return m_strPluginPipeline; }

// Dialog Data
	//{{AFX_DATA(CSelectPluginDlg)
	enum { IDD = IDD_PLUGINS_SELECTPLUGIN };
	CComboBox	m_cboPluginName;
	bool	m_bNoExtensionCheck;
	String	m_strDescription;
	String	m_strExtensions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectPluginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	/// arrays for string describing the available plugins
	std::map<String, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>> m_Plugins;

	// const data "no plugin"
	std::unique_ptr<PluginInfo> noPlugin;
	// const data "automatic plugin"
	std::unique_ptr<PluginInfo> automaticPlugin;

	// input value
	String m_filteredFilenames;

	/// current plugin choice
	String m_strPluginPipeline;

	void prepareListbox();

	// Generated message map functions
	//{{AFX_MSG(CSelectPluginDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnUnpackerAllowAll();
	afx_msg void OnSelchangeUnpackerName();
	afx_msg void OnClickedAddPipe();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
