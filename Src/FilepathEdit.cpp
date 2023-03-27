/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  FilePathEdit.cpp
 *
 * @brief Implementation of the CFilepathEdit class.
 */

#include "stdafx.h"
#include <Shlwapi.h>
#include "FilepathEdit.h"
#include "Merge.h"
#include "ClipBoard.h"
#include "FileOrFolderSelect.h"
#include "Win_VersionHelper.h"
#include "paths.h"
#include "cecolor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int FormatFilePathForDisplayWidth(CDC * pDC, int maxWidth, String & sFilepath);

BEGIN_MESSAGE_MAP(CFilepathEdit, CEdit)
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND_RANGE(ID_EDITOR_COPY_PATH, ID_EDITOR_SELECT_FILE, OnContextMenuSelected)
END_MESSAGE_MAP()


/** 
 * @brief Format the path for display in header control. 
 *
 * Formats path so it fits to given length, tries to end lines after
 * slash characters.
 *
 * @param [in] pDC Pointer to draw context.
 * @param [in] maxWidth Maximum width of the string in the GUI.
 * @param [in,out] sFilepath:
 * - in: string to format
 * - out: formatted string
 * @return Number of lines path is splitted to.
 */
static int FormatFilePathForDisplayWidth(CDC * pDC, int maxWidth, String & sFilepath)
{
	size_t iBegin = 0;
	int nLines = 1;
	
	while (true)
	{
		String line;

		// find the next truncation point
		size_t iEndMin = 0;
		size_t iEndMax = sFilepath.length() - iBegin + 1;
		while(1)
		{
			size_t iEnd = (iEndMin + iEndMax) / 2;
			if (iEnd == iEndMin)
				break;
			line = sFilepath.substr(iBegin, iEnd);
			int width = (pDC->GetTextExtent(line.c_str())).cx;
			if (width > maxWidth)
				iEndMax = iEnd;
			else
				iEndMin = iEnd;
		};
		ASSERT(iEndMax == iEndMin+1);

		// here iEndMin is the last character displayed in maxWidth

		// exit the loop if we can display the remaining characters with no truncation
		if (iBegin + iEndMin == sFilepath.length())
			break;

		// truncate the text to the previous "\" if possible
		line = sFilepath.substr(iBegin, iEndMin);
		size_t lastSlash = line.rfind('\\');
		if (lastSlash != String::npos)
			iEndMin = lastSlash + 1;

		sFilepath.insert(iBegin + iEndMin, _T("\n"));
		iBegin += iEndMin + 2;
		nLines ++;
	}

	return nLines;
}

/**
 * @brief Constructor.
 * Set text color to black and background white by default.
 */
CFilepathEdit::CFilepathEdit()
 : m_crBackGnd(RGB(255, 255, 255))
 , m_crText(RGB(0,0,0))
 , m_bActive(false)
 , m_bInEditing(false)
 , m_bEnabledFileSelection(false)
 , m_bEnabledFolderSelection(false)
{
}

/**
 * @brief Subclass the control.
 * @param [in] nID ID of the control to subclass.
 * @param [in] pParent Parent control of the control to subclass.
 * @return `true` if succeeded, `false` otherwise.
 */
bool CFilepathEdit::SubClassEdit(UINT nID, CWnd* pParent)
{
	m_bActive = false;
	return SubclassDlgItem(nID, pParent);
};

/**
 * @brief Set the text to show in the control.
 * This function sets the text (original text) to show in the control.
 * The control may modify the text for displaying in the GUI.
 */
void CFilepathEdit::SetOriginalText(const String& sString)
{
	if (m_sOriginalText.compare(sString) == 0)
		return;

	m_sOriginalText = sString;

	RefreshDisplayText();
}

/**
 * @brief Re-format the displayed text and update GUI.
 * This method formats the visible text from original text.
 */
void CFilepathEdit::RefreshDisplayText()
{
	String line = m_sOriginalText;

	// we want to keep the first and the last path component, and in between,
	// as much characters as possible from the right
	// PathCompactPath keeps, in between, as much characters as possible from the left
	// so we reverse everything between the first and the last component before calling PathCompactPath
	size_t iBeginLast = line.rfind('\\');
	size_t iEndIntro = line.find('\\');
	if (iBeginLast != String::npos && iEndIntro != iBeginLast)
	{
		String textToReverse = line.substr(iEndIntro + 1, iBeginLast -
				(iEndIntro + 1));
		std::reverse(textToReverse.begin(), textToReverse.end());
		line = line.substr(0, iEndIntro + 1) + textToReverse + line.substr(iBeginLast);
	}

	// get a device context object
	CClientDC lDC(this);
	// and use the correct font
	CFont *pFontOld = lDC.SelectObject(GetFont());	

	// compact the path
	CRect rect;
	GetRect(rect);
	std::vector<tchar_t> tmp((std::max)(MAX_PATH, line.length() + 1));
	std::copy(line.begin(), line.end(), tmp.begin());
	PathCompactPath(lDC.GetSafeHdc(), &tmp[0],	rect.Width());
	line = &tmp[0];
	
	// set old font back
	lDC.SelectObject(pFontOld);

	// we reverse back everything between the first and the last component
	// it works OK as "..." reversed = "..." again
	iBeginLast = line.rfind('\\');
	iEndIntro = line.find('\\');
	if (iBeginLast != String::npos && iEndIntro != iBeginLast)
	{
		String textToReverse = line.substr(iEndIntro + 1, iBeginLast -
				(iEndIntro+1));
		std::reverse(textToReverse.begin(), textToReverse.end());
		line = line.substr(0, iEndIntro + 1) + textToReverse + line.substr(iBeginLast);
	}

	SetWindowText(line.c_str());
}

/**
 * @brief Updates and returns the tooltip for this edit box
 */
const String& CFilepathEdit::GetUpdatedTipText(CDC * pDC, int maxWidth)
{
	GetOriginalText(m_sToolTipString);
	FormatFilePathForDisplayWidth(pDC, maxWidth, m_sToolTipString);
	return m_sToolTipString;
}

/**
 * @brief retrieve text from the OriginalText
 *
 * @note The standard Copy function works with the (compacted) windowText 
 */
void CFilepathEdit::CustomCopy(size_t iBegin, size_t iEnd /*=-1*/)
{
	if (iEnd == String::npos)
		iEnd = m_sOriginalText.length();

	PutToClipboard(m_sOriginalText.substr(iBegin, iEnd - iBegin), m_hWnd);
}

/**
 * @brief Format the context menu.
 */
void CFilepathEdit::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (m_bInEditing)
	{
		__super::OnContextMenu(pWnd, point);
	}
	else
	{
		if (!m_bActive)
			SetFocus();

		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP_EDITOR_HEADERBAR));
		theApp.TranslateMenu(menu.m_hMenu);

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != nullptr);

		DWORD sel = GetSel();
		if (HIWORD(sel) == LOWORD(sel))
			pPopup->EnableMenuItem(ID_EDITOR_COPY, MF_GRAYED);
		if (paths::EndsWithSlash(m_sOriginalText))
			// no filename, we have to disable the unwanted menu entry
			pPopup->EnableMenuItem(ID_EDITOR_COPY_FILENAME, MF_GRAYED);
		if (!m_bEnabledFileSelection && !m_bEnabledFolderSelection)
			pPopup->EnableMenuItem(ID_EDITOR_SELECT_FILE, MF_GRAYED);

		// invoke context menu
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

static COLORREF MakeBackColor(bool bActive, bool bInEditing)
{
	if (bActive)
		return CEColor::GetIntermediateColor(::GetSysColor(bInEditing ? COLOR_WINDOW : COLOR_ACTIVECAPTION), ::GetSysColor(COLOR_3DFACE), 0.5f);
	else
		return CEColor::GetIntermediateColor(::GetSysColor(bInEditing ? COLOR_WINDOW : COLOR_INACTIVECAPTION), ::GetSysColor(COLOR_3DFACE), 0.5f);
}

void CFilepathEdit::OnNcPaint()
{
	COLORREF crBackGnd = m_bInEditing ? ::GetSysColor(COLOR_ACTIVEBORDER) : m_crBackGnd;
	CWindowDC dc(this);
	CRect rect;
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int margin = pointToPixel(3);

	GetWindowRect(rect);
	rect.OffsetRect(-rect.TopLeft());
	dc.FillSolidRect(CRect(rect.left, rect.top, rect.left + margin, rect.bottom), CEColor::GetDarkenColor(crBackGnd, 0.98f));
	dc.FillSolidRect(CRect(rect.left, rect.top, rect.left + 1, rect.bottom), CEColor::GetDarkenColor(crBackGnd, 0.96f));
	dc.FillSolidRect(CRect(rect.right - margin, rect.top, rect.right, rect.bottom), crBackGnd);
	dc.FillSolidRect(CRect(rect.left + 1, rect.top, rect.right, rect.top + margin), CEColor::GetDarkenColor(crBackGnd, 0.98f));
	dc.FillSolidRect(CRect(rect.left, rect.top, rect.right, rect.top + 1), CEColor::GetDarkenColor(crBackGnd, 0.96f));
	dc.FillSolidRect(CRect(rect.left + margin, rect.bottom - margin, rect.right, rect.bottom), crBackGnd);
}

void CFilepathEdit::OnPaint()
{
	__super::OnPaint();
	if (!m_bInEditing)
	{
		CClientDC dc(this);
		CFont *pFontOld = dc.SelectObject(GetFont());	
		int oldBkMode = dc.SetBkMode(TRANSPARENT);
		CRect rc = GetMenuCharRect(&dc);
		const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
		auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
		const int margin = pointToPixel(3);
		dc.TextOutW(rc.left + margin, 0, IsWin7_OrGreater() ? _T("\u2261") : _T("="));
		dc.SetBkMode(oldBkMode);
		dc.SelectObject(pFontOld);
	}
}

void CFilepathEdit::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);
	if (m_bInEditing)
	{
		m_bInEditing = false;
		SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		SetBackColor(MakeBackColor(false, false));
		RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
		SetReadOnly(true);
		SetWindowText(m_sOriginalText.c_str());
	}
}

CRect CFilepathEdit::GetMenuCharRect(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	int charWidth;
	pDC->GetCharWidth('=', '=', &charWidth);
	const int lpx = pDC->GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	rc.left = rc.right - charWidth - pointToPixel(3 * 2);
	return rc;
}

void CFilepathEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	CClientDC dc(this);
	CRect rc = GetMenuCharRect(&dc);
	if (PtInRect(&rc, point))
	{
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}
	else
	{
		__super::OnLButtonDown(nFlags, point);
	}
}

BOOL CFilepathEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	CClientDC dc(this);
	CRect rc = GetMenuCharRect(&dc);
	if (PtInRect(&rc, pt))
	{
		SetCursor(LoadCursor(nullptr, IDC_ARROW));
		return TRUE;
	}
	return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CFilepathEdit::OnEditCopy()
{
	int nStartChar, nEndChar;
	GetSel(nStartChar, nEndChar);
	if (nStartChar == nEndChar)
		SetSel(0, -1);
	Copy();
	if (nStartChar == nEndChar)
		SetSel(nStartChar, nEndChar);
}

void CFilepathEdit::OnEditPaste()
{
	Paste();
}

void CFilepathEdit::OnEditCut()
{
	Cut();
}

void CFilepathEdit::OnEditUndo()
{
	Undo();
}

void CFilepathEdit::OnEditSelectAll()
{
	SetSel(0, -1);
}

void CFilepathEdit::OnContextMenuSelected(UINT nID)
{
	// compute the beginning of the text to copy (in OriginalText)
	size_t iBegin = 0;
	switch (nID)
	{
	case ID_EDITOR_COPY:
		Copy();
		return;
	case ID_EDITOR_COPY_FILENAME:
		{
		size_t lastSlash = m_sOriginalText.rfind('\\');
		if (lastSlash == String::npos)
			lastSlash = m_sOriginalText.rfind('/');
		if (lastSlash != String::npos)
			iBegin = lastSlash+1;
		else
			iBegin = 0;
		}
		break;
	case ID_EDITOR_COPY_PATH:
		// pass the heading "*" for modified files
		if (m_sOriginalText.at(0) == '*')
			iBegin = 2;
		else
			iBegin = 0;
		break;
	case ID_EDITOR_EDIT_CAPTION:
		m_bInEditing = true;
		SetReadOnly(false);
		SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		SetBackColor(::GetSysColor(COLOR_WINDOW));
		RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
		SetWindowText((m_sOriginalText.at(0) == '*' ? m_sOriginalText.substr(2) : m_sOriginalText).c_str());
		SetSel(0, -1);
		SetFocus();
		return;
	case ID_EDITOR_SELECT_FILE:
	{
		CString text;
		GetWindowText(text);
		if (!text.IsEmpty() && text[0] == '*')
			text = text.Right(text.GetLength() - 2);
		String dir = paths::GetParentPath(static_cast<const tchar_t*>(text));
		bool selected = false;
		if (m_bEnabledFileSelection)
			selected = SelectFile(m_hWnd, m_sFilepath, true, dir.c_str());
		else
			selected = SelectFolder(m_sFilepath, dir.c_str(), _T(""), GetSafeHwnd());
		if (selected)
			GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_USER_FILE_SELECTED), (LPARAM)m_hWnd);
		return;
	}
	default:
		return;
	}
	
	CustomCopy(iBegin);
}

BOOL CFilepathEdit::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (::TranslateAccelerator (m_hWnd, static_cast<CFrameWnd *>(AfxGetMainWnd())->GetDefaultAccelerator(), pMsg))
			return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			m_bInEditing = false;
			SetTextColor(::GetSysColor(COLOR_CAPTIONTEXT));
			SetBackColor(MakeBackColor(true, false));
			RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
			SetReadOnly();
			CString sText;
			GetWindowText(sText);
			if (m_sOriginalText.at(2) == '*')
				sText = _T("* ") + sText;
			SetWindowText(sText);
			GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_USER_CAPTION_CHANGED), (LPARAM)m_hWnd);
			return TRUE;
		}
		if (pMsg->wParam == VK_ESCAPE)
		{
			m_bInEditing = false;
			SetTextColor(GetSysColor(COLOR_CAPTIONTEXT));
			SetBackColor(GetSysColor(COLOR_INACTIVECAPTION));
			RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
			SetReadOnly();
			SetWindowText(m_sOriginalText.c_str());
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

/**
 * @brief Set the control to look active/inactive.
 * This function sets control to look like an active control. We don't
 * have real focus on this control, but editor pane below it. However
 * for user this active look informs which editor pane is active.
 * @param [in] bActive If `true` set control look like active control.
 */
void CFilepathEdit::SetActive(bool bActive)
{
	m_bActive = bActive;

	if (m_hWnd == nullptr)
		return;

	CRect rcWnd;
	GetWindowRect(&rcWnd);

	if (bActive)
	{
		SetTextColor(::GetSysColor(m_bInEditing ? COLOR_WINDOWTEXT : COLOR_CAPTIONTEXT));
	}
	else
	{
		SetTextColor(::GetSysColor(m_bInEditing ? COLOR_WINDOWTEXT : COLOR_INACTIVECAPTIONTEXT));
	}
	SetBackColor(MakeBackColor(bActive, m_bInEditing));
	RedrawWindow(nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
}

/**
 * @brief Set control's colors.
 * @param [in] pDC pointer to device context.
 * @param [in] nCtlColor Control color to set.
 * @note Parameter @p nCtlColor is not used but must be present as this method
 * is called by framework.
 * @return Brush for background.
 */
HBRUSH CFilepathEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	UNUSED_ALWAYS(nCtlColor);
	// Return a non-`nullptr` brush if the parent's 
	//handler should not be called

	//set text color
	pDC->SetTextColor(m_crText);

	//set the text's background color
	pDC->SetBkColor(m_crBackGnd);

	//return the brush used for background this sets control background
	return m_brBackGnd;
}

/**
 * @brief Set control's bacground color.
 * @param [in] rgb Color to set as background color.
 */
void CFilepathEdit::SetBackColor(COLORREF rgb)
{
	//set background color ref (used for text's background)
	m_crBackGnd = rgb;
	
	//free brush
	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
	//set brush to new color
	m_brBackGnd.CreateSolidBrush(rgb);
	
	//redraw
	Invalidate(TRUE);
}

/**
 * @brief Set control's text color.
 * @param [in] Color to set as text color.
 */
void CFilepathEdit::SetTextColor(COLORREF rgb)
{
	//set text color ref
	m_crText = rgb;

	//redraw
	Invalidate(TRUE);
}
