/**
 * @file  MergeResultStatusBar.cpp
 *
 * @brief Implementation of CMergeResultStatusBar class
 */

#include "stdafx.h"
#include "MergeResultStatusBar.h"
#include "MergeDoc.h"
#include "MergeResultPane.h"
#include "MergeResultView.h"
#include "MergeResultContainer.h"
#include "charsets.h"
#include "unicoder.h"
#include "resource.h"
#include "I18nGUI.h"
#include "BCMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMergeResultStatusBar, CBasicFlatStatusBar)

BEGIN_MESSAGE_MAP(CMergeResultStatusBar, CBasicFlatStatusBar)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/** @brief Panel widths in points */
static const UINT RO_PANEL_WIDTH = 20;
static const UINT ENCODING_PANEL_WIDTH = 90;
static const UINT EOL_PANEL_WIDTH = 30;
static const UINT LINEINFO_PANEL_WIDTH = 160;
static const UINT CONFLICT_PANEL_WIDTH = 200;

static UINT indicatorsResult[] =
{
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
};

static String ResultEolString(const String& sEol)
{
	if (sEol == _T("\r\n"))
		return _("CRLF");
	if (sEol == _T("\n"))
		return _("LF");
	if (sEol == _T("\r"))
		return _("CR");
	if (sEol.empty())
		return _("None");
	if (sEol == _T("hidden"))
		return _T("");
	return _T("?");
}

CMergeResultStatusBar::CMergeResultStatusBar()
	: m_nConflicts(0)
	, m_nUnresolved(0)
	, m_nWhiteSpaceOnly(0)
	, m_nColumn(0)
	, m_nColumns(0)
	, m_nChar(0)
	, m_nChars(0)
	, m_nSelectedLines(0)
	, m_nSelectedChars(0)
	, m_nCodepage(-1)
	, m_bHasBom(false)
	, m_bReadOnly(false)
{
}

CMergeResultStatusBar::~CMergeResultStatusBar()
{
}

BOOL CMergeResultStatusBar::Create(CWnd* pParentWnd)
{
	if (!__super::Create(pParentWnd, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, AFX_IDW_CONTROLBAR_FIRST + 29))
		return FALSE;

	SetIndicators(indicatorsResult, sizeof(indicatorsResult) / sizeof(UINT));

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

	int roWidth = pointToPixel(RO_PANEL_WIDTH);
	int eolWidth = pointToPixel(EOL_PANEL_WIDTH);
	int encodingWidth = pointToPixel(ENCODING_PANEL_WIDTH);
	int lineInfoWidth = pointToPixel(LINEINFO_PANEL_WIDTH);
	int conflictWidth = pointToPixel(CONFLICT_PANEL_WIDTH);

	SetPaneInfo(PANE_CONFLICT, ID_SEPARATOR, SBPS_NOBORDERS, conflictWidth);
	SetPaneInfo(PANE_PATH, ID_SEPARATOR, SBPS_STRETCH | SBPS_NOBORDERS, 0);
	SetPaneInfo(PANE_LINEINFO, ID_SEPARATOR, SBPS_CLICKABLE, lineInfoWidth);
	SetPaneInfo(PANE_ENCODING, ID_SEPARATOR, SBPS_CLICKABLE, encodingWidth);
	SetPaneInfo(PANE_EOL,      ID_SEPARATOR, SBPS_CLICKABLE, eolWidth);
	SetPaneInfo(PANE_RO,       ID_SEPARATOR, SBPS_CLICKABLE, roWidth);

	UpdateConflictText();
	UpdatePathText();
	UpdateLineText();
	UpdateEncodingText();
	UpdateEolText();
	UpdateRoText();

	return TRUE;
}

void CMergeResultStatusBar::SetConflictInfo(int nConflicts, int nUnresolved, int nWhiteSpaceOnly)
{
	if (m_nConflicts != nConflicts || m_nUnresolved != nUnresolved || m_nWhiteSpaceOnly != nWhiteSpaceOnly)
	{
		m_nConflicts = nConflicts;
		m_nUnresolved = nUnresolved;
		m_nWhiteSpaceOnly = nWhiteSpaceOnly;
		UpdateConflictText();
	}
}

void CMergeResultStatusBar::SetPath(const String& sOutputPath)
{
	if (m_sOutputPath != sOutputPath)
	{
		m_sOutputPath = sOutputPath;
		UpdatePathText();
	}
}

void CMergeResultStatusBar::SetLineInfo(const tchar_t* szLine, int nColumn, int nColumns,
	int nChar, int nChars, int nSelectedLines, int nSelectedChars)
{
	if (m_sLine != szLine || m_nColumn != nColumn || m_nColumns != nColumns ||
		m_nChar != nChar || m_nChars != nChars ||
		m_nSelectedLines != nSelectedLines || m_nSelectedChars != nSelectedChars)
	{
		m_sLine = szLine;
		m_nColumn = nColumn;
		m_nColumns = nColumns;
		m_nChar = nChar;
		m_nChars = nChars;
		m_nSelectedLines = nSelectedLines;
		m_nSelectedChars = nSelectedChars;
		UpdateLineText();
	}
}

void CMergeResultStatusBar::SetEncodingAndEol(int nCodepage, bool bHasBom, const tchar_t* szEol)
{
	bool bEncodingChanged = (m_nCodepage != nCodepage || m_bHasBom != bHasBom);
	bool bEolChanged = (m_sEol != szEol);

	if (bEncodingChanged)
	{
		m_nCodepage = nCodepage;
		m_bHasBom = bHasBom;
		const char* pszCodepageName = GetEncodingNameFromCodePage(nCodepage);
		m_sCodepageName = pszCodepageName ? ucr::toTString(pszCodepageName) : _T("");
		if (bHasBom)
			m_sCodepageName += _T(" BOM");
		UpdateEncodingText();
	}

	if (bEolChanged)
	{
		m_sEol = szEol;
		m_sEolDisplay = ResultEolString(m_sEol);
		UpdateEolText();
	}
}

void CMergeResultStatusBar::SetReadOnly(bool bReadOnly)
{
	if (m_bReadOnly != bReadOnly)
	{
		m_bReadOnly = bReadOnly;
		UpdateRoText();
	}
}

void CMergeResultStatusBar::UpdateConflictText()
{
	if (!IsWindow(m_hWnd))
		return;

	String strInfo;
	if (m_nConflicts == 0)
	{
		strInfo = _("No Conflicts");
	}
	else if (m_nUnresolved == 0)
	{
		strInfo = strutils::format_string1(_("Conflicts: %1 (all resolved)"),
			strutils::to_str(m_nConflicts));
	}
	else if (m_nWhiteSpaceOnly == 0)
	{
		strInfo = strutils::format_string2(_("Conflicts: %1, Unresolved: %2"), 
			strutils::to_str(m_nConflicts), strutils::to_str(m_nUnresolved));
	}
	else
	{
		strInfo = strutils::format_string3(_("Conflicts: %1, Unresolved: %2 (%3 whitespace-only)"),
			strutils::to_str(m_nConflicts), strutils::to_str(m_nUnresolved),
			strutils::to_str(m_nWhiteSpaceOnly));
	}
	SetPaneText(PANE_CONFLICT, strInfo.c_str());
}

void CMergeResultStatusBar::UpdatePathText()
{
	if (!IsWindow(m_hWnd))
		return;

	SetPaneText(PANE_PATH, (m_sOutputPath.empty() ? _("<Untitled>") : m_sOutputPath).c_str());
}

void CMergeResultStatusBar::UpdateLineText()
{
	if (!IsWindow(m_hWnd))
		return;

	CString strInfo;
	if (!m_sLine.empty())
	{
		if (m_nChars == -1)
		{
			strInfo.Format(_("Line: %s").c_str(), m_sLine.c_str());
		}
		else
		{
			strInfo.Format(_("Ln: %s  Col: %d/%d  Ch: %d/%d").c_str(),
				m_sLine.c_str(), m_nColumn, m_nColumns, m_nChar, m_nChars);
		}
		if (m_nSelectedLines > 0)
		{
			CString strSelected;
			strSelected.Format(_("  Sel: %d | %d").c_str(), m_nSelectedLines, m_nSelectedChars);
			strInfo += strSelected;
		}
	}
	SetPaneText(PANE_LINEINFO, strInfo);
}

void CMergeResultStatusBar::UpdateEncodingText()
{
	if (!IsWindow(m_hWnd))
		return;

	CString strEncoding;
	if (m_nCodepage > 0)
		strEncoding = m_sCodepageName.c_str();
	SetPaneText(PANE_ENCODING, strEncoding);
}

void CMergeResultStatusBar::UpdateEolText()
{
	if (!IsWindow(m_hWnd))
		return;

	SetPaneText(PANE_EOL, m_sEolDisplay.c_str());
}

void CMergeResultStatusBar::UpdateRoText()
{
	if (!IsWindow(m_hWnd))
		return;

	SetPaneText(PANE_RO, m_bReadOnly ? _("RO").c_str() : _T(""));
}

void CMergeResultStatusBar::UpdateResources()
{
	m_sEolDisplay = ResultEolString(m_sEol);
	UpdateConflictText();
	UpdateLineText();
	UpdateEncodingText();
	UpdateEolText();
	UpdateRoText();
}

CMergeDoc* CMergeResultStatusBar::GetDocument() const
{
	CWnd* pParent = GetParent();
	if (pParent != nullptr)
	{
		CMergeResultContainer* pContainer = dynamic_cast<CMergeResultContainer*>(pParent);
		if (pContainer != nullptr)
			return pContainer->GetDocument();
	}
	return nullptr;
}

void CMergeResultStatusBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	const int i = GetIndexFromPoint(point);
	CMergeDoc* pDoc = GetDocument();

	if (pDoc == nullptr)
	{
		__super::OnLButtonDown(nFlags, point);
		return;
	}

	CMergeResultTextBuffer* pBuf = pDoc->GetMergeResultBuffer();

	switch (i)
	{
	case PANE_LINEINFO:
		if (auto* pResultView = pDoc->GetMergeResultView())
		{
			pResultView->TakeFocus();
			pResultView->PostMessage(WM_COMMAND, ID_EDIT_WMGOTO);
		}
		break;

	case PANE_ENCODING:
		if (pBuf != nullptr && pBuf->IsInitialized())
		{
			// Show encoding choice menu
			CPoint ptScreen;
			::GetCursorPos(&ptScreen);

			BCMenu menu;
			menu.CreatePopupMenu();

			const int curCodepage = pBuf->getCodepage();
			const bool curBom = pBuf->getHasBom();

			struct EncodingItem
			{
				UINT nID;
				int codepage;
				bool bom;
				const tchar_t* label;
			};
			static const EncodingItem items[] =
			{
				{ 1, ucr::CP_UTF_8, false, _T("UTF-8") },
				{ 2, ucr::CP_UTF_8, true,  _T("UTF-8 with BOM") },
				{ 3, ucr::CP_UCS2LE, true, _T("UTF-16 LE") },
				{ 4, ucr::CP_UCS2BE, true, _T("UTF-16 BE") },
				{ 5, 0, false,              _T("ANSI / System default") },
			};

			for (const auto& item : items)
			{
				UINT flags = MF_STRING;
				int itemCP = item.codepage == 0 ? static_cast<int>(::GetACP()) : item.codepage;
				if (curCodepage == itemCP && curBom == item.bom)
					flags |= MF_CHECKED;
				menu.AppendMenu(flags, item.nID, item.label);
			}

			int cmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
				ptScreen.x, ptScreen.y, this);

			if (cmd >= 1 && cmd <= static_cast<int>(_countof(items)))
			{
				const auto& selected = items[cmd - 1];
				int targetCP = selected.codepage == 0 ? static_cast<int>(::GetACP()) : selected.codepage;
				pBuf->setCodepage(targetCP);
				pBuf->setHasBom(selected.bom);
				pBuf->SetModified(true);
				SetEncodingAndEol(targetCP, selected.bom, m_sEol.c_str());
			}
		}
		break;

	case PANE_EOL:
		if (pBuf != nullptr && pBuf->IsInitialized())
		{
			CPoint ptScreen;
			::GetCursorPos(&ptScreen);

			CMenu menu;
			VERIFY(menu.LoadMenu(IDR_POPUP_MERGEEDITFRAME_STATUSBAR_EOL));
			I18n::TranslateMenu(menu.m_hMenu);

			int cmd = menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD,
				ptScreen.x, ptScreen.y, this);

			CRLFSTYLE style = CRLFSTYLE::AUTOMATIC;
			switch (cmd)
			{
			case ID_EOL_TO_DOS:  style = CRLFSTYLE::DOS; break;
			case ID_EOL_TO_UNIX: style = CRLFSTYLE::UNIX; break;
			case ID_EOL_TO_MAC:  style = CRLFSTYLE::MAC; break;
			default: break;
			}

			if (cmd != 0)
			{
				pBuf->SetCRLFMode(style);
				if (pBuf->applyEOLMode())
					pBuf->SetModified(true);

				const tchar_t* pszEol = _T("");
				switch (style)
				{
				case CRLFSTYLE::DOS:  pszEol = _T("\r\n"); break;
				case CRLFSTYLE::UNIX: pszEol = _T("\n"); break;
				case CRLFSTYLE::MAC:  pszEol = _T("\r"); break;
				default: break;
				}
				SetEncodingAndEol(m_nCodepage, m_bHasBom, pszEol);
			}
		}
		break;

	case PANE_RO:
		if (pBuf != nullptr && pBuf->IsInitialized())
		{
			bool bNewRO = !pBuf->GetReadOnly();
			pBuf->SetReadOnly(bNewRO);
			SetReadOnly(bNewRO);
		}
		break;

	default:
		__super::OnLButtonDown(nFlags, point);
		break;
	}
}
