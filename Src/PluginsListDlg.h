/**
 * @file  PluginsListDlg.h
 *
 * @brief Declaration file for PluginsList dialog
 */
#pragma once

/**
 * @brief A dialog listing plugins.
 * This dialog shows a list of currently available plugins. In addition
 * to the plugin (file)name it shows plugin type and description. The
 * dialog also has a control to enable/disable plugins.
 */
class PluginsListDlg : public CDialog
{
	DECLARE_DYNAMIC(PluginsListDlg)

public:
	PluginsListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~PluginsListDlg();

// Dialog Data
	enum { IDD = IDD_PLUGINS_LIST };

protected:
	void InitList();
	void AddPlugins();
	void AddPluginsToList(LPCWSTR pluginEvent, LPCTSTR pluginType);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedPluginSettings();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_list; /**< A list control for plugins. */	
};
