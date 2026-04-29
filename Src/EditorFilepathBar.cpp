/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  EditorFilepathBar.cpp
 *
 * @brief Implementation file for CEditorFilepathBar class
 */

#include "stdafx.h"
#include "EditorFilepathBar.h"
#include "RoundedRectWithShadow.h"
#include "cecolor.h"
#include "DarkModeLib.h"
#include "TempFile.h"
#include "paths.h"
#include "I18n.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr int RR_RADIUS = 3;
constexpr int RR_PADDING = 3;
constexpr int RR_SHADOWWIDTH = 3;
constexpr unsigned MAX_HISTORY_ITEMS = 15;

BEGIN_MESSAGE_MAP(CEditorFilePathBar, CDialogBar)
	ON_NOTIFY_EX (TTN_NEEDTEXT, 0, OnToolTipNotify)
	ON_CONTROL_RANGE (EN_SETFOCUS, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnSetFocusEdit)
	ON_CONTROL_RANGE (EN_KILLFOCUS, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnKillFocusEdit)
	ON_CONTROL_RANGE (EN_USER_CAPTION_CHANGED, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnChangeEdit)
	ON_CONTROL_RANGE (EN_USER_FILE_SELECTED, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnSelectEdit)
	ON_NOTIFY_RANGE (EN_USER_CUSTOMIZE_CONTEXT_MENU, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnCustomizeContextMenu)
	ON_NOTIFY_RANGE (EN_USER_MENU_ITEM_SELECTED, IDC_STATIC_TITLE_PANE0, IDC_STATIC_TITLE_PANE2, OnMenuItemSelected)
END_MESSAGE_MAP()


/**
 * @brief Constructor.
 */
CEditorFilePathBar::CEditorFilePathBar()
: m_nPanes(2)
{
}

/**
 * @brief Destructor.
 */
CEditorFilePathBar::~CEditorFilePathBar()
{
}

/**
 * @brief Create the window.
 * This function subclasses the edit controls.
 * @param [in] pParentWnd Parent window for edit controls.
 * @return TRUE if succeeded, FALSE otherwise.
 */
BOOL CEditorFilePathBar::Create(CWnd* pParentWnd)
{
	if (! __super::Create(pParentWnd, CEditorFilePathBar::IDD, 
			CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, AFX_IDW_CONTROLBAR_FIRST+29))
		return FALSE;

	NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
	if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0))
	{
		ncm.lfStatusFont.lfWeight = FW_BOLD;
		m_font.CreateFontIndirect(&ncm.lfStatusFont);
	}

	// subclass the two custom edit boxes
	for (int pane = 0; pane < static_cast<int>(std::size(m_Edit)); pane++)
	{
		m_Edit[pane].SubClassEdit(IDC_STATIC_TITLE_PANE0 + pane, this);
		m_Edit[pane].SetFont(&m_font);
		m_Edit[pane].SetMargins(0, std::abs(ncm.lfStatusFont.lfHeight));
	}
	return TRUE;
};

CSize CEditorFilePathBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont *pOldFont = dc.SelectObject(&m_font);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	int cy = pointToPixel(3 + RR_SHADOWWIDTH + RR_PADDING);
	return CSize(SHRT_MAX, 1 + tm.tmHeight + cy);
}

/** 
 * @brief Resize both controls to an equal size.
 */
void CEditorFilePathBar::Resize()
{
	if (m_hWnd == nullptr)
		return;

	WINDOWPLACEMENT infoBar = {};
	GetWindowPlacement(&infoBar);

	int widths[3] = {};
	for (int pane = 0; pane < m_nPanes; pane++)
		widths[pane] = (infoBar.rcNormalPosition.right / m_nPanes) - ((pane == 0) ? 7 : 5);
	Resize(widths);
}

/** 
 * @brief Set widths.
 * This function resizes both controls to given size. The width is usually
 * same as the splitter view width.
 * @param [in] leftWidth Left-side control width.
 * @param [in] rightWidth Right-side control width.
 */
void CEditorFilePathBar::Resize(int widths[])
{
	if (m_hWnd == nullptr)
		return;

	// resize left filename
	CRect rc;
	int x = 0;
	GetClientRect(&rc);
	bool resized = false;
	for (int pane = 0; pane < m_nPanes; pane++)
	{
		CRect rcOld;
		m_Edit[pane].GetClientRect(&rcOld);
		rc.left = x;
		rc.right = x + widths[pane] + (pane == 0 ? 5 : 7);
		x = rc.right;
		if (rcOld.Width() != rc.Width())
		{
			CClientDC dc(this);
			const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
			auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
			const int sw = pointToPixel(RR_SHADOWWIDTH);
			CRect rc2 = rc;
			rc2.DeflateRect(sw + sw, sw);
			m_Edit[pane].MoveWindow(&rc2);
			m_Edit[pane].RefreshDisplayText();
			resized = true;
		}
	}
	if (resized)
		InvalidateRect(nullptr, false);
}

void CEditorFilePathBar::DoPaint(CDC* pDC)
{
	const int lpx = pDC->GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int r = pointToPixel(RR_RADIUS);
	const int sw = pointToPixel(RR_SHADOWWIDTH);
	CRect rcBar;
	GetWindowRect(&rcBar);
	const COLORREF clrBarBackcolor = GetSysColor(COLOR_3DFACE);
	for (int pane = 0; pane < m_nPanes; pane++)
	{
		CRect rc;
		m_Edit[pane].GetWindowRect(&rc);
		const COLORREF clrBackcolor = m_Edit[pane].GetBackColor();
		const COLORREF clrShadow =
			CEColor::GetIntermediateColor(clrBarBackcolor, GetSysColor(COLOR_3DSHADOW), m_Edit[pane].GetActive() ? 0.5f : 0.8f);
		rc.OffsetRect(-rcBar.left, -rcBar.top);
		DrawRoundedRectWithShadow(pDC->m_hDC, rc.left - sw, rc.top, rc.right - rc.left + 2 * sw, rc.bottom - rc.top, r, sw,
			clrBackcolor, clrShadow, clrBarBackcolor);
		if (pane == m_nPanes - 1)
		{
			CRect rc2{ rc.right + sw + sw, 0, rcBar.Width(), rcBar.Height() };
			pDC->FillSolidRect(&rc2, clrBarBackcolor);
		}
	}
	__super::DoPaint(pDC);
}

/**
 * @brief Called when tooltip is about to be shown.
 * In this function we set the tooltip text shown.
 */
BOOL CEditorFilePathBar::OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult)
{
	if (m_hWnd == nullptr)
		return FALSE;

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the CEdit 
		int nID = ::GetDlgCtrlID((HWND)pTTTStruct->idFrom);
		if(nID == IDC_STATIC_TITLE_PANE0 || nID == IDC_STATIC_TITLE_PANE1 || nID == IDC_STATIC_TITLE_PANE2)
		{
			// compute max width : 97% of application width or 80% or full screen width
			CRect rect;
			GetWindowRect(rect);
			int maxWidth = (int)(rect.Width() * .97);
			CRect rectScreen; 
			SystemParametersInfo(SPI_GETWORKAREA, 0, rectScreen, 0);
			if (rectScreen.Width() * .8 > maxWidth)
				maxWidth = (int)(rectScreen.Width() * .8);

			// use the tooltip font
			HANDLE hFont = (HANDLE) ::SendMessage(pTTTStruct->hwndFrom, WM_GETFONT, 0, 0);
			CClientDC tempDC(this);
			HANDLE hOldFont = ::SelectObject(tempDC.GetSafeHdc(),hFont);

			// fill in the returned structure
			CFilepathEdit * pItem = static_cast<CFilepathEdit*>(GetDlgItem(nID));
			pTTT->lpszText = const_cast<tchar_t *>(pItem->GetUpdatedTipText(&tempDC, maxWidth).c_str());

			// set old font back
			if (hOldFont != nullptr)
				::SelectObject(tempDC.GetSafeHdc(),hOldFont);

			// we must set TTM_SETMAXTIPWIDTH to use \n in tooltips
			// just to do the first time, but how to access the tooltip during init ?
			::SendMessage(pTTTStruct->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 5000);
			SetToolTipsFirstTime(pTTTStruct->hwndFrom);
			return TRUE;
		}
	}
	return FALSE;
}

void CEditorFilePathBar::OnSetFocusEdit(UINT id)
{
	const int pane = id - IDC_STATIC_TITLE_PANE0;
	if (pane < 0 || pane >= m_nPanes)
		return;
	InvalidateRect(nullptr, false);
	if (m_setFocusCallbackfunc)
		m_setFocusCallbackfunc(pane);
}

void CEditorFilePathBar::OnKillFocusEdit(UINT id)
{
	InvalidateRect(nullptr, false);
}

void CEditorFilePathBar::OnChangeEdit(UINT id)
{
	const int pane = id - IDC_STATIC_TITLE_PANE0;
	if (pane < 0 || pane >= m_nPanes)
		return;
	InvalidateRect(nullptr, false);
	if (m_captionChangedCallbackfunc)
	{
		CString text;
		m_Edit[pane].GetWindowText(text);
		m_captionChangedCallbackfunc(pane, (const tchar_t*)text);
	}
}

void CEditorFilePathBar::OnSelectEdit(UINT id)
{
	const int pane = id - IDC_STATIC_TITLE_PANE0;
	if (pane < 0 || pane >= m_nPanes)
		return;
	InvalidateRect(nullptr, false);
	String selectedPath = m_Edit[pane].GetSelectedPath();
	if (m_fileSelectedCallbackfunc)
		m_fileSelectedCallbackfunc(pane, selectedPath, _T(""));
	else if (m_folderSelectedCallbackfunc)
		m_folderSelectedCallbackfunc(pane, selectedPath);
}

/** 
 * @brief Get the path for one side
 *
 * @param [in] pane Index (0-based) of pane to update.
 */
String CEditorFilePathBar::GetCaption(int pane) const
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return _T("");

	CString str;
	m_Edit[pane].GetWindowText(str);
	return String(str);
}

/** 
 * @brief Set the path for one side
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] lpszString New text for pane.
 */
void CEditorFilePathBar::SetCaption(int pane, const String& sString)
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return;

	m_Edit[pane].SetOriginalText(sString);
}

String CEditorFilePathBar::GetPath(int pane) const
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return _T("");

	return m_Edit[pane].GetPath();
}

void CEditorFilePathBar::SetPath(int pane, const String& sString)
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return;

	m_Edit[pane].SetPath(sString);
}

int CEditorFilePathBar::GetActive() const
{
	for (int pane = 0; pane < m_nPanes; pane++)
	{
		if (m_Edit[pane].GetActive())
			return pane;
	}
	return -1;
}

/** 
 * @brief Set the active status for one status (change the appearance)
 *
 * @param [in] pane Index (0-based) of pane to update.
 * @param [in] bActive If `true` activates pane, `false` deactivates.
 */
void CEditorFilePathBar::SetActive(int pane, bool bActive)
{
	ASSERT (pane >= 0 && pane < static_cast<int>(std::size(m_Edit)));

	// Check for `nullptr` since window may be closing..
	if (m_hWnd == nullptr)
		return;

	if (bActive != m_Edit[pane].GetActive())
		InvalidateRect(nullptr, false);
	m_Edit[pane].SetActive(bActive);
}

void CEditorFilePathBar::SetToolTipsFirstTime(HWND hTip)
{
	if (m_Tips.find(hTip) == m_Tips.end())
	{
		m_Tips.insert(hTip);
		DarkMode::setDarkTooltips(hTip, static_cast<int>(DarkMode::ToolTipsType::tooltip));
	}
}

void CEditorFilePathBar::EditActivePanePath()
{
	const int pane = GetActive();
	if (pane >= 0)
		m_Edit[pane].PostMessage(WM_COMMAND, ID_EDITOR_EDIT_PATH, 0);
}

void CEditorFilePathBar::SetOnGetRecentItemsCallback(const std::function<std::vector<String>(int pane, unsigned maxCount, MruHelper::RecentItemType type)> callbackfunc)
{
	m_getRecentItemsCallbackfunc = callbackfunc;
}

void CEditorFilePathBar::SetOnGetClipboardHistoryCallback(const std::function<std::vector<ClipboardHistory::Item>(unsigned maxCount)> callbackfunc)
{
	m_getClipboardHistoryCallbackfunc = callbackfunc;
}

std::vector<String> CEditorFilePathBar::GetRecentItems(int pane, unsigned maxCount, MruHelper::RecentItemType type) const
{
	if (m_getRecentItemsCallbackfunc)
		return m_getRecentItemsCallbackfunc(pane, maxCount, type);
	return {};
}

std::vector<ClipboardHistory::Item> CEditorFilePathBar::GetClipboardHistory(unsigned maxCount) const
{
	if (m_getClipboardHistoryCallbackfunc)
		return m_getClipboardHistoryCallbackfunc(maxCount);
	return {};
}

void CEditorFilePathBar::OnRecentItemSelected(int pane, const String& path)
{
	bool isFolder = false;
	if (!paths::IsURLorCLSID(path))
	{
		// Check if the path exists
		paths::PATH_EXISTENCE pathExists = paths::DoesPathExist(path);

		if (pathExists == paths::DOES_NOT_EXIST)
		{
			// Show error message in caption
			String errorMsg = strutils::format_string1(m_fileSelectedCallbackfunc ? _("File not found: %1") : _("Folder not found: %1"), path);
			SetCaption(pane, errorMsg);
			return;
		}
		 isFolder = paths::EndsWithSlash(path);
	}

	if (isFolder && m_folderSelectedCallbackfunc)
	{
		m_folderSelectedCallbackfunc(pane, path);
	}
	else if (!isFolder && m_fileSelectedCallbackfunc)
	{
		m_fileSelectedCallbackfunc(pane, path, _T(""));
	}
	else if (m_fileSelectedCallbackfunc)
	{
		// Fallback to file callback if folder callback is not set
		m_fileSelectedCallbackfunc(pane, path, _T(""));
	}
}

void CEditorFilePathBar::OnClipboardItemSelected(int pane, int itemIndex)
{
	// Use cached clipboard items to ensure consistency with displayed menu
	if (itemIndex < 0 || itemIndex >= static_cast<int>(m_cachedClipboardItems.size()))
		return;

	const auto& clipItem = m_cachedClipboardItems[itemIndex];

	// Determine which temp file to use - bitmap takes precedence over text
	String clipboardPath;
	std::shared_ptr<TempFile> tempFile;

	if (clipItem.pBitmapTempFile)
	{
		// Use bitmap temp file
		clipboardPath = clipItem.pBitmapTempFile->GetPath();
		tempFile = clipItem.pBitmapTempFile;
	}
	else if (clipItem.pTextTempFile)
	{
		// Use text temp file
		clipboardPath = clipItem.pTextTempFile->GetPath();
		tempFile = clipItem.pTextTempFile;
	}
	else
	{
		// No valid content
		return;
	}

	// Check if the clipboard content file exists
	paths::PATH_EXISTENCE pathExists = paths::DoesPathExist(clipboardPath);
	if (pathExists == paths::DOES_NOT_EXIST)
		return;

	// Save the temp file to prevent deletion
	// Limit the number of cached temp files to prevent memory leak
	constexpr size_t MAX_TEMP_FILES = 50;
	if (m_tempFiles.size() >= MAX_TEMP_FILES)
	{
		// Remove oldest temp files
		m_tempFiles.erase(m_tempFiles.begin(), m_tempFiles.begin() + (m_tempFiles.size() - MAX_TEMP_FILES + 1));
	}
	m_tempFiles.push_back(tempFile);

	// Use file callback to notify about the clipboard content (always treated as file)
	// Pass the description so the caller can set the caption
	if (m_fileSelectedCallbackfunc)
	{
		m_fileSelectedCallbackfunc(pane, clipboardPath, clipItem.description);
	}
	else if (m_folderSelectedCallbackfunc)
	{
		String path = strutils::trim_ws(clipItem.previewText);
		if (!path.empty() && path[0] == '"')
		{
			// Remove surrounding quotes if present
			path = path.substr(1, path.length() - 2);
		}
		paths::PATH_EXISTENCE pathExists2 = paths::DoesPathExist(path);
		if (pathExists2 == paths::DOES_NOT_EXIST || pathExists2 == paths::IS_EXISTING_FILE)
		{
			// Show error message in caption
			String errorMsg = strutils::format_string1(_("Folder not found: %1"), path);
			SetCaption(pane, errorMsg);
			return;
		}
		m_folderSelectedCallbackfunc(pane, path);
	}
}

void CEditorFilePathBar::OnMenuItemSelected(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	NMMENUITEMSELECTED* pNM = reinterpret_cast<NMMENUITEMSELECTED*>(pNMHDR);
	const int pane = id - IDC_STATIC_TITLE_PANE0;

	if (pane < 0 || pane >= m_nPanes)
	{
		*pResult = 0;
		return;
	}

	UINT menuId = pNM->menuId;

	// Handle recent item selection
	if (menuId >= ID_EDITOR_RECENT_FIRST && menuId <= ID_EDITOR_RECENT_LAST)
	{
		int index = menuId - ID_EDITOR_RECENT_FIRST;

		// Get the actual path from the stored recent items
		MruHelper::RecentItemType itemType = MruHelper::RecentItemType::All;
		if (m_Edit[pane].IsFileSelectionEnabled() && !m_Edit[pane].IsFolderSelectionEnabled())
			itemType = MruHelper::RecentItemType::FilesOnly;
		else if (m_Edit[pane].IsFolderSelectionEnabled() && !m_Edit[pane].IsFileSelectionEnabled())
			itemType = MruHelper::RecentItemType::FoldersOnly;

		auto recentPaths = GetRecentItems(pane, MAX_HISTORY_ITEMS, itemType);
		if (index < static_cast<int>(recentPaths.size()))
		{
			OnRecentItemSelected(pane, recentPaths[index]);
		}
	}
	// Handle clipboard history selection
	else if (menuId >= ID_EDITOR_CLIPBOARD_FIRST && menuId <= ID_EDITOR_CLIPBOARD_LAST)
	{
		int index = menuId - ID_EDITOR_CLIPBOARD_FIRST;
		OnClipboardItemSelected(pane, index);
	}
	// Handle "Open Clipboard" menu item - opens the current clipboard content
	else if (menuId == ID_EDITOR_OPEN_CLIPBOARD)
	{
		// Open the most recent clipboard item (index 0)
		OnClipboardItemSelected(pane, 0);
	}

	*pResult = 0;
}

void CEditorFilePathBar::OnCustomizeContextMenu(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	NMHEADERBARCONTEXTMENU* pNM = reinterpret_cast<NMHEADERBARCONTEXTMENU*>(pNMHDR);
	CMenu* pPopup = pNM->pMenu;
	const int pane = id - IDC_STATIC_TITLE_PANE0;

	if (pane < 0 || pane >= m_nPanes)
	{
		*pResult = 0;
		return;
	}

	// Determine which type of items to show based on enabled callbacks
	MruHelper::RecentItemType itemType = MruHelper::RecentItemType::All;
	if (m_Edit[pane].IsFileSelectionEnabled() && !m_Edit[pane].IsFolderSelectionEnabled())
		itemType = MruHelper::RecentItemType::FilesOnly;
	else if (m_Edit[pane].IsFolderSelectionEnabled() && !m_Edit[pane].IsFileSelectionEnabled())
		itemType = MruHelper::RecentItemType::FoldersOnly;
	// Add Recent Files/Folders submenu
	auto recentPaths = GetRecentItems(pane, MAX_HISTORY_ITEMS, itemType);
	if (!recentPaths.empty())
	{
		pPopup->AppendMenu(MF_SEPARATOR);

		CMenu recentMenu;
		recentMenu.CreatePopupMenu();
		int ID = ID_EDITOR_RECENT_FIRST;
		for (size_t i = 0; i < recentPaths.size() && ID <= ID_EDITOR_RECENT_LAST; ++i, ++ID)
		{
			// Extract filename or folder name for display
			String displayName;
			if (paths::EndsWithSlash(recentPaths[i]))
			{
				// For folders, get the last directory name
				String pathWithoutSlash = recentPaths[i].substr(0, recentPaths[i].length() - 1);
				displayName = paths::FindFileName(pathWithoutSlash);
			}
			else
			{
				// For files, get the filename
				displayName = paths::FindFileName(recentPaths[i]);
			}

			String menuText = strutils::format(_T("&%c %s"), "123456789abcdef"[i], displayName.c_str());
			recentMenu.AppendMenu(MF_STRING, ID, menuText.c_str());
		}
		pPopup->AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(recentMenu.m_hMenu), _("Recent F&iles or Folders").c_str());
		recentMenu.Detach();
	}

	// Add Clipboard History submenu
	// Cache clipboard items for consistency between menu display and selection
	m_cachedClipboardItems = GetClipboardHistory(MAX_HISTORY_ITEMS);
	if (m_cachedClipboardItems.size() > 1)
	{
		CMenu clipboardMenu;
		clipboardMenu.CreatePopupMenu();
		int ID = ID_EDITOR_CLIPBOARD_FIRST;
		for (size_t i = 0; i < m_cachedClipboardItems.size() && ID <= ID_EDITOR_CLIPBOARD_LAST; ++i, ++ID)
		{
			String prefix;
			if (m_cachedClipboardItems[i].pBitmapTempFile)
				prefix = _T("[") + _("Image") + _T("] ");
			String displayName = m_cachedClipboardItems[i].previewText;
			if (displayName.length() > 60)
				displayName = displayName.substr(0, 57) + _T("...");
			// Replace newlines with spaces for menu display
			std::replace(displayName.begin(), displayName.end(), '\n', ' ');
			std::replace(displayName.begin(), displayName.end(), '\r', ' ');

			String menuText = strutils::format(_T("&%c %s"), "123456789abcdef"[i], displayName.c_str());
			clipboardMenu.AppendMenu(MF_STRING, ID, menuText.c_str());
		}
		pPopup->AppendMenu(MF_POPUP, reinterpret_cast<UINT_PTR>(clipboardMenu.m_hMenu), _("Clipboard &History").c_str());
		clipboardMenu.Detach();
	}

	*pResult = 0;
}
