/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file SelectUnpackerDlg.h : 
 *
 *  @brief Declaration file for the dialog SelectUnpacker
 */ 
#pragma once

#include <vector>
#include <memory>
#include "TrDialogs.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectUnpackerDlgDlg dialog

class PackingInfo;
class PluginInfo;

class CSelectUnpackerDlg : public CTrDialog
{
private:
// Construction
	void Initialize();

public:
// Construction
	CSelectUnpackerDlg(const String& filename, CWnd* pParent /*= nullptr*/);
	CSelectUnpackerDlg(const String& filename1, const String& filename2, CWnd* pParent /*= nullptr*/);
	~CSelectUnpackerDlg();

	void SetInitialInfoHandler(PackingInfo * infoHandler);
	const PackingInfo GetInfoHandler();

// Dialog Data
	//{{AFX_DATA(CSelectUnpackerDlg)
	enum { IDD = IDD_SELECTUNPACKER };
	CComboBox	m_cboUnpackerName;
	bool	m_bNoExtensionCheck;
	String	m_strDescription;
	String	m_strExtensions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectUnpackerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	/// arrays for string describing the available plugins
	CPtrArray  m_UnpackerPlugins;
	std::vector<bool> m_bWithFileFlags;

	// const data "no plugin"
	std::unique_ptr<PluginInfo> noPlugin;
	// const data "automatic plugin"
	std::unique_ptr<PluginInfo> automaticPlugin;

	// input value
	String m_filteredFilenames;

	/// current plugin choice
	PluginInfo * m_pPlugin;
	/// current plugin choice
	String m_strPluginName;

	void prepareListbox();

	// Generated message map functions
	//{{AFX_MSG(CSelectUnpackerDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnUnpackerAllowAll();
	afx_msg void OnSelchangeUnpackerName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
