/**
 * @file  PluginsListDlg.h
 *
 * @brief Declaration file for PluginsList dialog
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _PLUGINS_LIST_DIALOG_H_
#define _PLUGINS_LIST_DIALOG_H_

#include "UnicodeString.h"

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
	void AddPluginsToList(const wchar_t *pluginEvent, const String& pluginType);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_list; /**< A list control for plugins. */	
};

#endif // _PLUGINS_LIST_DIALOG_H_
