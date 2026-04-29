/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  EditorFilePathBar.h
 *
 * @brief Interface of the CEditorFilePathBar class.
 *
 */
#pragma once

#include "FilepathEdit.h"
#include "MruHelper.h"
#include "ClipboardHistory.h"
#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

class TempFile;

/**
 * Interface to update the header data.
 */
class IHeaderBar
{
public:

	// Type aliases for convenience
	using RecentItemType = MruHelper::RecentItemType;
	using ClipboardItem = ClipboardHistory::Item;

	virtual String GetCaption(int pane) const = 0;
	virtual void SetCaption(int pane, const String& sCaption) = 0;
	virtual String GetPath(int pane) const = 0;
	virtual void SetPath(int pane, const String& sPath) = 0;
	virtual int GetActive() const = 0;
	virtual void SetActive(int pane, bool bActive) = 0;
	virtual void SetPaneCount(int nPanes) = 0;
	virtual void Resize() = 0;
	virtual void SetOnSetFocusCallback(const std::function<void(int)> callbackfunc) = 0;
	virtual void SetOnCaptionChangedCallback(const std::function<void(int, const String& sText)> callbackfunc) = 0;
	virtual void SetOnFileSelectedCallback(const std::function<void(int, const String& sFilepath, const String& sDescription)> callbackfunc) = 0;
	virtual void SetOnFolderSelectedCallback(const std::function<void(int, const String& sFolderpath)> callbackfunc) = 0;
	virtual void EditActivePanePath() = 0;
	virtual void SetOnGetRecentItemsCallback(const std::function<std::vector<String>(int pane, unsigned maxCount, RecentItemType type)> callbackfunc) = 0;
	virtual void SetOnGetClipboardHistoryCallback(const std::function<std::vector<ClipboardItem>(unsigned maxCount)> callbackfunc) = 0;
	virtual void SetDefaultHistoryCallbacks() = 0;
};


/**
 * @brief A dialog bar with two controls for left/right path.
 * This class is a dialog bar for the both files path in the editor. 
 * The bar looks like a statusBar (font, height). The control
 * displays a tip for each path (as a tooltip). 
 */
class CEditorFilePathBar : public CDialogBar, public IHeaderBar
{
public : 
	CEditorFilePathBar();
	~CEditorFilePathBar();

	BOOL Create( CWnd* pParentWnd);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual void DoPaint(CDC* pDC);

// Dialog Data
	enum { IDD = IDD_EDITOR_HEADERBAR };
	
	void Resize(int widths[]);

	// Implement IFilepathHeaders
	void SetCaption(int pane, const String& sString) override;
	String GetCaption(int pane) const override;
	void SetPath(int pane, const String& sString) override;
	String GetPath(int pane) const override;
	int GetActive() const override;
	void SetActive(int pane, bool bActive) override;
	void SetPaneCount(int nPanes) override;
	void Resize() override;
	void SetOnSetFocusCallback(const std::function<void(int)> callbackfunc) override;
	void SetOnCaptionChangedCallback(const std::function<void(int, const String& sText)> callbackfunc) override;
	void SetOnFileSelectedCallback(const std::function<void(int, const String& sFilepath, const String& sDescription)> callbackfunc) override;
	void SetOnFolderSelectedCallback(const std::function<void(int, const String& sFolderpath)> callbackfunc) override;
	void SetOnGetRecentItemsCallback(const std::function<std::vector<String>(int pane, unsigned maxCount, MruHelper::RecentItemType type)> callbackfunc) override;
	void SetOnGetClipboardHistoryCallback(const std::function<std::vector<ClipboardHistory::Item>(unsigned maxCount)> callbackfunc) override;
	void EditActivePanePath() override;
	void SetDefaultHistoryCallbacks() override
	{
		SetOnGetRecentItemsCallback([](int pane, unsigned maxCount, MruHelper::RecentItemType type) {
			return MruHelper::GetRecentFiles(pane, maxCount, type);
			});
		SetOnGetClipboardHistoryCallback([](unsigned maxCount) {
			return ClipboardHistory::GetItems(maxCount, 1);
			});
	}

protected:
	//{{AFX_MSG(CEditorFilePathBar)
	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg void OnSetFocusEdit(UINT id);
	afx_msg void OnKillFocusEdit(UINT id);
	afx_msg void OnChangeEdit(UINT id);
	afx_msg void OnSelectEdit(UINT id);
	afx_msg void OnMenuItemSelected(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomizeContextMenu(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

private:
	// Internal helper methods
	std::vector<String> GetRecentItems(int pane, unsigned maxCount, MruHelper::RecentItemType type = MruHelper::RecentItemType::All) const;
	std::vector<ClipboardHistory::Item> GetClipboardHistory(unsigned maxCount) const;
	void OnRecentItemSelected(int pane, const String& path);
	void OnClipboardItemSelected(int pane, int itemIndex);
	void SetToolTipsFirstTime(HWND hTip);

	// this dialog uses custom edit boxes
	CFilepathEdit m_Edit[3]; /**< Edit controls. */
	std::unordered_set<HWND> m_Tips;
	CFont m_font; /**< Font for editcontrols */
	int m_nPanes;
	std::function<void(int)> m_setFocusCallbackfunc;
	std::function<void(int, const String& sText)> m_captionChangedCallbackfunc;
	std::function<void(int, const String& sFilepath, const String& sDescription)> m_fileSelectedCallbackfunc;
	std::function<void(int, const String& sFolderpath)> m_folderSelectedCallbackfunc;
	std::function<std::vector<String>(int pane, unsigned maxCount, MruHelper::RecentItemType type)> m_getRecentItemsCallbackfunc;
	std::function<std::vector<ClipboardHistory::Item>(unsigned maxCount)> m_getClipboardHistoryCallbackfunc;
	std::vector<std::shared_ptr<TempFile>> m_tempFiles; /**< Temp files from clipboard history */
	std::vector<ClipboardHistory::Item> m_cachedClipboardItems; /**< Cached clipboard items for menu consistency */
};

inline void CEditorFilePathBar::SetPaneCount(int nPanes)
{
	m_nPanes = nPanes;
}

/** 
 * @brief Set callback function on EN_SETFOCUS notification
 */
inline void CEditorFilePathBar::SetOnSetFocusCallback(const std::function<void(int)> callbackfunc)
{
	m_setFocusCallbackfunc = callbackfunc;
}

inline void CEditorFilePathBar::SetOnCaptionChangedCallback(const std::function<void(int, const String& sText)> callbackfunc)
{
	m_captionChangedCallbackfunc = callbackfunc;
}

inline void CEditorFilePathBar::SetOnFileSelectedCallback(const std::function<void(int, const String& sFilepath, const String& sDescription)> callbackfunc)
{
	m_fileSelectedCallbackfunc = callbackfunc;
	for (int pane = 0; pane < m_nPanes; ++pane)
		m_Edit[pane].EnableFileSelection(true);
}

inline void CEditorFilePathBar::SetOnFolderSelectedCallback(const std::function<void(int, const String& sFolderpath)> callbackfunc)
{
	m_folderSelectedCallbackfunc = callbackfunc;
	for (int pane = 0; pane < m_nPanes; ++pane)
		m_Edit[pane].EnableFolderSelection(true);
}

