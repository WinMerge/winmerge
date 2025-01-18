/**
 * @file  PluginsListDlg.h
 *
 * @brief Declaration file for PluginsList dialog
 */
#pragma once

#include "TrDialogs.h"
#include "UnicodeString.h"
#include "CMoveConstraint.h"

class PluginInfo;
namespace internal_plugin { struct Info; }

/**
 * @brief A dialog listing plugins.
 * This dialog shows a list of currently available plugins. In addition
 * to the plugin (file)name it shows plugin type and description. The
 * dialog also has a control to enable/disable plugins.
 */
class PluginsListDlg : public CTrDialog
{
	DECLARE_DYNAMIC(PluginsListDlg)

public:
	explicit PluginsListDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~PluginsListDlg();

// Dialog Data
	enum { IDD = IDD_PLUGINS_LIST };

protected:
	void InitList();
	void SetPlugins(int sel);
	void AddPluginsToList(const wchar_t *pluginEvent);
	void RefreshList();
	void AddPlugin(unsigned id);
	void EditPlugin();
	void DuplicatePlugin();
	void RemovePlugin();
	PluginInfo *GetSelectedPluginInfo() const;
	internal_plugin::Info* GetSelectedInternalPluginInfo() const;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedPluginAdd();
	afx_msg void OnDropDownAdd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedPluginEdit();
	afx_msg void OnBnClickedPluginRemove();
	afx_msg void OnBnClickedPluginSettings();
	afx_msg void OnBnClickedFileFiltesDefaults();
	afx_msg void OnLVNItemChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLVNItemChanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDropDownPatterns();
	afx_msg void OnCloseUpPatterns();
	afx_msg void OnSelchangePluginType();
	afx_msg void OnHelp();
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_comboPatterns;
	CComboBox m_comboType;
	CListCtrl m_list; /**< A list control for plugins. */	
	prdlg::CMoveConstraint m_constraint; /**< Resizes dialog controls when dialog resized */
};
