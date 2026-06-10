/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeStatusBar.cpp
 *
 * @brief Implementation file for CMergeStatusBar class
 */

#include "stdafx.h"
#include "MergeStatusBar.h"
#include "charsets.h"
#include "unicoder.h"
#include "SyntaxColors.h"
#include "Merge.h"
#include "RoundedRectWithShadow.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** @brief RO status panel width (point) */
static const UINT RO_PANEL_WIDTH = 20;
/** @brief Encoding status panel width (point) */
static const UINT ENCODING_PANEL_WIDTH = 90;
/** @brief EOL type status panel width (point) */
static const UINT EOL_PANEL_WIDTH = 30;

/**
 * @brief Statusbar pane indexes
 */
enum
{
	PANE_PANE0_INFO = 0,
	PANE_PANE0_ENCODING,
	PANE_PANE0_EOL,
	PANE_PANE0_RO,
	PANE_PANE1_INFO,
	PANE_PANE1_ENCODING,
	PANE_PANE1_EOL,
	PANE_PANE1_RO,
	PANE_PANE2_INFO,
	PANE_PANE2_ENCODING,
	PANE_PANE2_EOL,
	PANE_PANE2_RO,
};

const int nColumnsPerPane = PANE_PANE1_INFO - PANE_PANE0_INFO;

/**
 * @brief Bottom statusbar panels and indicators
 */
static UINT indicatorsBottom[] =
{
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
};

BEGIN_MESSAGE_MAP(CMergeStatusBar, CBasicFlatStatusBar)
    ON_WM_PAINT()
END_MESSAGE_MAP()

/**
 * @brief Constructor.
 */
CMergeStatusBar::CMergeStatusBar() : m_nPanes(2), m_bDiff{}
{
	for (int pane = 0; pane < sizeof(m_status) / sizeof(m_status[0]); pane++)
	{
		m_status[pane].m_pWndStatusBar = this;
		m_status[pane].m_base = PANE_PANE0_INFO + pane * nColumnsPerPane;
	}
	Options::DiffColors::Load(GetOptionsMgr(), m_cachedColors);
}

/**
 * @brief Destructor.
 */
CMergeStatusBar::~CMergeStatusBar()
{
}

BOOL CMergeStatusBar::Create(CWnd* pParentWnd)
{
	if (! __super::Create(pParentWnd, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, AFX_IDW_CONTROLBAR_FIRST+28))
		return FALSE;

	SetIndicators(indicatorsBottom, sizeof(indicatorsBottom) / sizeof(UINT));

	// Set text to read-only info panes
	// Text is hidden if file is writable
	String sText = _("RO");
	for (auto&& p : { PANE_PANE0_RO, PANE_PANE1_RO, PANE_PANE2_RO })
		SetPaneText(p, sText.c_str(), TRUE);

	for (int pane = 0; pane < 3; pane++)
	{
		SetPaneStyle(PANE_PANE0_INFO     + pane * nColumnsPerPane, SBPS_CLICKABLE);
		SetPaneStyle(PANE_PANE0_ENCODING + pane * nColumnsPerPane, SBPS_CLICKABLE);
		SetPaneStyle(PANE_PANE0_EOL      + pane * nColumnsPerPane, SBPS_CLICKABLE);
		SetPaneStyle(PANE_PANE0_RO       + pane * nColumnsPerPane, SBPS_CLICKABLE);
	}

	return TRUE;
};

void CMergeStatusBar::OnPaint()
{
	CStatusBarCtrl& ctrl = GetStatusBarCtrl();
	int parts[32];
	const int nParts = ctrl.GetParts(32, parts);

	bool bDiffNew[4]{};
	std::vector<CString> textary(m_nPanes);
	for (int pane = 0; pane < m_nPanes; ++pane)
		textary[pane] = GetPaneText(PANE_PANE0_ENCODING + pane * nColumnsPerPane);
	bDiffNew[PANE_PANE0_ENCODING] = !std::equal(textary.begin() + 1, textary.end(), textary.begin());
	for (int pane = 0; pane < m_nPanes; ++pane)
		textary[pane] = GetPaneText(PANE_PANE0_EOL + pane * nColumnsPerPane);
	bDiffNew[PANE_PANE0_EOL] = !std::equal(textary.begin() + 1, textary.end(), textary.begin());
	for (int i = 0; i < nParts; i++)
	{
		CRect rcPart;
		ctrl.GetRect(i, &rcPart);
		if (m_bDiff[i % nColumnsPerPane] != bDiffNew[i % nColumnsPerPane])
			InvalidateRect(&rcPart);
	}
	m_bDiff[PANE_PANE0_ENCODING] = bDiffNew[PANE_PANE0_ENCODING];
	m_bDiff[PANE_PANE0_EOL] = bDiffNew[PANE_PANE0_EOL];

	const COLORREF clr3DFace = GetSysColor(COLOR_3DFACE);
	const COLORREF clr3DFaceLight = LightenColor(clr3DFace, 0.5);
	const COLORREF clrWordDiffLight = LightenColor(m_cachedColors.clrWordDiff, 0.5);
	const COLORREF clrBtnText = GetSysColor(COLOR_BTNTEXT);

	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	memDC.FillSolidRect(&rect, GetSysColor(COLOR_3DFACE));
	memDC.SetBkMode(TRANSPARENT);
	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? memDC.SelectObject(pFont) : nullptr;
	const int radius = MulDiv (3, memDC.GetDeviceCaps (LOGPIXELSY), 72);
	for (int i = 0; i < nParts; i++)
	{
		const unsigned style = GetPaneStyle(i);
		CRect rcPart;
		ctrl.GetRect(i, &rcPart);
		const bool lighten = (m_bMouseTracking && (style & SBPS_CLICKABLE) != 0 && i == m_nTrackingPane);
		if (lighten)
			DrawRoundedRect(memDC.m_hDC, rcPart.left, rcPart.top, rcPart.Width(), rcPart.Height(), radius, clr3DFaceLight, clr3DFace);
		const bool disabled = (style & SBPS_DISABLED) != 0;
		if (!disabled)
		{
			if (m_bDiff[i % nColumnsPerPane])
			{
				memDC.SetTextColor(m_cachedColors.clrWordDiffText == -1 ?
					theApp.GetMainSyntaxColors()->GetColor(COLORINDEX_NORMALTEXT) : m_cachedColors.clrWordDiffText);
				DrawRoundedRect(memDC.m_hDC, rcPart.left, rcPart.top, rcPart.Width(), rcPart.Height(), radius, lighten ? clrWordDiffLight : m_cachedColors.clrWordDiff, clr3DFace);
			}
			else
			{
				memDC.SetTextColor(clrBtnText);
			}
			CRect rcText = rcPart;
			rcText.left += radius;
			memDC.DrawText(ctrl.GetText(i), &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
	}
	if (pOldFont)
		memDC.SelectObject(pOldFont);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOldBitmap);
}

void CMergeStatusBar::Resize(int widths[])
{
	// Set bottom statusbar panel widths
	// Kimmo - I don't know why 4 seems to be right for me
	int borderWidth = 4; // GetSystemMetrics(SM_CXEDGE);
	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	for (int pane = 0; pane < m_nPanes; pane++)
	{
		int fixedPaneWidth = pointToPixel(RO_PANEL_WIDTH + ENCODING_PANEL_WIDTH + EOL_PANEL_WIDTH) +
			(3 * borderWidth);
		int paneWidth = widths[pane] - fixedPaneWidth;
		int encodingWidth = pointToPixel(ENCODING_PANEL_WIDTH) - borderWidth;
		int roWidth = pointToPixel(RO_PANEL_WIDTH) - borderWidth;
		int eolWidth = pointToPixel(EOL_PANEL_WIDTH) - borderWidth;
		if (paneWidth < 0)
		{
			paneWidth = 0;
			int restWidth = widths[pane] - paneWidth - borderWidth;
			if (restWidth < 0) restWidth = 0;
			roWidth = (roWidth + borderWidth) * restWidth / fixedPaneWidth - borderWidth;
			if (roWidth < 0) roWidth = 0;
			eolWidth = (eolWidth + borderWidth) * restWidth / fixedPaneWidth - borderWidth;
			if (eolWidth < 0) eolWidth = 0;
			encodingWidth = widths[pane] - (paneWidth + roWidth + eolWidth + 6 * borderWidth);
			if (encodingWidth < 0) encodingWidth = 0;
		}

		SetPaneInfo(PANE_PANE0_INFO + pane * nColumnsPerPane, ID_STATUS_PANE0FILE_INFO + pane,
			SBPS_CLICKABLE, paneWidth);
		SetPaneInfo(PANE_PANE0_ENCODING + pane * nColumnsPerPane, ID_STATUS_PANE0FILE_ENCODING + pane,
			SBPS_CLICKABLE, encodingWidth);
		SetPaneInfo(PANE_PANE0_RO + pane * nColumnsPerPane, ID_STATUS_PANE0FILE_RO + pane,
			SBPS_CLICKABLE, roWidth);
		SetPaneInfo(PANE_PANE0_EOL + pane * nColumnsPerPane, ID_STATUS_PANE0FILE_EOL + pane,
			SBPS_CLICKABLE, eolWidth);
	}
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeStatusBar::UpdateResources()
{
	for (int pane = 0; pane < m_nPanes; pane++)
		m_status[pane].UpdateResources();
}

/// Bridge class which implements the interface from crystal editor to frame status line display
CMergeStatusBar::MergeStatus::MergeStatus()
: m_nColumn(0)
, m_nColumns(0)
, m_nChar(0)
, m_nChars(0)
, m_nCodepage(-1)
, m_bHasBom(false)
, m_nSelectedLines(0)
, m_nSelectedChars(0)
, m_pWndStatusBar(nullptr)
, m_base(0)
{
}

/// Send status line info (about one side of merge view) to screen
void CMergeStatusBar::MergeStatus::Update()
{
	if (IsWindow(m_pWndStatusBar->m_hWnd))
	{
		CString strInfo, strEncoding;
		if (m_nChars == -1)
		{
			strInfo.Format(_("Line: %s").c_str(),
				m_sLine.c_str());
		}
		else if (m_sEolDisplay.empty())
		{
			strInfo.Format(_("Ln: %s  Col: %d/%d  Ch: %d/%d").c_str(),
				m_sLine.c_str(), m_nColumn, m_nColumns, m_nChar, m_nChars, m_nCodepage, m_sCodepageName.c_str());
		}
		else
		{
			strInfo.Format(_("Ln: %s  Col: %d/%d  Ch: %d/%d  EOL: %s").c_str(),
				m_sLine.c_str(), m_nColumn, m_nColumns, m_nChar, m_nChars, m_sEolDisplay.c_str(), m_nCodepage, m_sCodepageName.c_str());
		}
		if (m_nSelectedLines > 0)
		{
			CString strSelected;
			strSelected.Format(_("  Sel: %d | %d").c_str(), m_nSelectedLines, m_nSelectedChars);
			strInfo += strSelected;
		}

		if (m_nCodepage > 0)
			strEncoding.Format(_("%s").c_str(), m_sCodepageName.c_str());
		m_pWndStatusBar->SetPaneText(m_base, strInfo);
		m_pWndStatusBar->SetPaneText(m_base + 1, strEncoding);
	}
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeStatusBar::MergeStatus::UpdateResources()
{
	Update();
}

/// Visible representation of eol
static String EolString(const String & sEol)
{
	if (sEol == _T("\r\n"))
	{
		return _("CRLF");
	}
	if (sEol == _T("\n"))
	{
		return _("LF");
	}
	if (sEol == _T("\r"))
	{
		return _("CR");
	}
	if (sEol.empty())
	{
		return _("None");
	}
	if (sEol == _T("hidden"))
		return _T("");
	return _T("?");
}

/// Receive status line info from crystal window and display
void CMergeStatusBar::MergeStatus::SetLineInfo(const tchar_t* szLine, int nColumn,
		int nColumns, int nChar, int nChars, int nSelectedLines, int nSelectedChars, const tchar_t* szEol, int nCodepage, bool bHasBom)
{
	if (m_sLine != szLine || m_nColumn != nColumn || m_nColumns != nColumns ||
		m_nChar != nChar || m_nChars != nChars || 
		m_nSelectedLines != nSelectedLines || m_nSelectedChars != nSelectedChars ||
		m_sEol != szEol != 0 || m_nCodepage != nCodepage || m_bHasBom != bHasBom)
	{
		m_sLine = szLine;
		m_nColumn = nColumn;
		m_nColumns = nColumns;
		m_nChar = nChar;
		m_nChars = nChars;
		m_nSelectedLines = nSelectedLines;
		m_nSelectedChars = nSelectedChars;
		m_sEol = szEol;
		m_sEolDisplay = EolString(m_sEol);
		if (m_nCodepage != nCodepage || m_bHasBom != bHasBom)
		{
			const char *pszCodepageName = GetEncodingNameFromCodePage(nCodepage);
			m_sCodepageName = pszCodepageName ? ucr::toTString(pszCodepageName) : _T("");
			if (bHasBom)
				m_sCodepageName += _T(" BOM");
		}
		m_nCodepage = nCodepage;
		m_bHasBom = bHasBom;
		Update();
	}
}
