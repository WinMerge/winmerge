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
 * @file  EditorFilepathBar.cpp
 *
 * @brief Implementation file for CEditorFilepathBar class
 */

#include "stdafx.h"
#include "MergeStatusBar.h"
#include "charsets.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** @brief RO status panel width */
static const UINT RO_PANEL_WIDTH = 40;
/** @brief Encoding status panel width */
static const UINT ENCODING_PANEL_WIDTH = 80;
/** @brief EOL type status panel width */
static const UINT EOL_PANEL_WIDTH = 60;

/**
 * @brief Statusbar pane indexes
 */
enum
{
	PANE_PANE0_INFO = 0,
	PANE_PANE0_RO,
	PANE_PANE0_EOL,
	PANE_PANE1_INFO,
	PANE_PANE1_RO,
	PANE_PANE1_EOL,
	PANE_PANE2_INFO,
	PANE_PANE2_RO,
	PANE_PANE2_EOL,
};

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
};

BEGIN_MESSAGE_MAP(CMergeStatusBar, CStatusBar)
END_MESSAGE_MAP()

/**
 * @brief Constructor.
 */
CMergeStatusBar::CMergeStatusBar() : m_nPanes(2)
{
	for (int pane = 0; pane < sizeof(m_status) / sizeof(m_status[0]); pane++)
	{
		m_status[pane].m_pWndStatusBar = this;
		m_status[pane].m_base = PANE_PANE0_INFO + pane * 3;
	}
}

/**
 * @brief Destructor.
 */
CMergeStatusBar::~CMergeStatusBar()
{
}

BOOL CMergeStatusBar::Create(CWnd* pParentWnd)
{
	if (! CStatusBar::Create(pParentWnd))
		return FALSE;

	SetIndicators(indicatorsBottom, sizeof(indicatorsBottom) / sizeof(UINT));

	// Set text to read-only info panes
	// Text is hidden if file is writable
	String sText = _("RO");
	SetPaneText(PANE_PANE0_RO, sText.c_str(), TRUE);
	SetPaneText(PANE_PANE1_RO, sText.c_str(), TRUE);
	SetPaneText(PANE_PANE2_RO, sText.c_str(), TRUE);

	return TRUE;
};

void CMergeStatusBar::Resize(int widths[])
{
	// Set bottom statusbar panel widths
	// Kimmo - I don't know why 4 seems to be right for me
	int borderWidth = 4; // GetSystemMetrics(SM_CXEDGE);
	for (int pane = 0; pane < m_nPanes; pane++)
	{
		int paneWidth = widths[pane] - (RO_PANEL_WIDTH + EOL_PANEL_WIDTH +
			(2 * borderWidth));
		if (paneWidth < borderWidth)
			paneWidth = borderWidth;

		SetPaneStyle(PANE_PANE0_INFO + pane * 3, SBPS_NORMAL);
		SetPaneInfo(PANE_PANE0_INFO + pane * 3, ID_STATUS_PANE0FILE_INFO + pane,
			SBPS_NORMAL, paneWidth);
		SetPaneStyle(PANE_PANE0_RO + pane * 3, SBPS_NORMAL);
		SetPaneInfo(PANE_PANE0_RO + pane * 3, ID_STATUS_PANE0FILE_RO + pane,
			SBPS_NORMAL, RO_PANEL_WIDTH - borderWidth);
		SetPaneStyle(PANE_PANE0_EOL + pane * 3, SBPS_NORMAL);
		SetPaneInfo(PANE_PANE0_EOL + pane * 3, ID_STATUS_PANE0FILE_EOL + pane,
			SBPS_NORMAL, EOL_PANEL_WIDTH - borderWidth);
	}
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeStatusBar::UpdateResources()
{
	for (int pane = 0; pane < sizeof(m_status) / sizeof(m_status[0]); pane++)
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
, m_pWndStatusBar(nullptr)
, m_base(0)
{
}

/// Send status line info (about one side of merge view) to screen
void CMergeStatusBar::MergeStatus::Update()
{
	if (IsWindow(m_pWndStatusBar->m_hWnd))
	{
		CString str;
		if (m_nChars == -1)
		{
			str.Format(_("Line: %s").c_str(),
				m_sLine.c_str());
		}
		else if (m_sEolDisplay.empty())
		{
			str.Format(_("Ln: %s  Col: %d/%d  Ch: %d/%d  Cp: %d(%s)").c_str(),
				m_sLine.c_str(), m_nColumn, m_nColumns, m_nChar, m_nChars, m_nCodepage, m_sCodepageName.c_str());
		}
		else
		{
			str.Format(_("Ln: %s  Col: %d/%d  Ch: %d/%d  EOL: %s  Cp: %d(%s)").c_str(),
				m_sLine.c_str(), m_nColumn, m_nColumns, m_nChar, m_nChars, m_sEolDisplay.c_str(), m_nCodepage, m_sCodepageName.c_str());
		}

		m_pWndStatusBar->SetPaneText(m_base, str);
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
void CMergeStatusBar::MergeStatus::SetLineInfo(LPCTSTR szLine, int nColumn,
		int nColumns, int nChar, int nChars, LPCTSTR szEol, int nCodepage, bool bHasBom)
{
	if (m_sLine != szLine || m_nColumn != nColumn || m_nColumns != nColumns ||
		m_nChar != nChar || m_nChars != nChars || m_sEol != szEol != 0 || m_nCodepage != nCodepage || m_bHasBom != bHasBom)
	{
		USES_CONVERSION;
		m_sLine = szLine;
		m_nColumn = nColumn;
		m_nColumns = nColumns;
		m_nChar = nChar;
		m_nChars = nChars;
		m_sEol = szEol;
		m_sEolDisplay = EolString(m_sEol);
		if (m_nCodepage != nCodepage)
		{
			const char *pszCodepageName = GetEncodingNameFromCodePage(nCodepage);
			m_sCodepageName = pszCodepageName ? ucr::toTString(pszCodepageName) : _T("");
			if (bHasBom)
				m_sCodepageName += _T(" with BOM");
		}
		m_nCodepage = nCodepage;
		m_bHasBom = bHasBom;
		Update();
	}
}
