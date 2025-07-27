/**
 *  @file ctextmarkerdlg.cpp
 *
 *  @brief Implementation of Marker-dialog.
 */

#include "StdAfx.h"
#include "resource.h"
#include "ctextmarkerdlg.h"
#include "ccrystaltextmarkers.h"
#include "DDXHelper.h"
#include "../ccrystaltextview.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextMarkerDlg dialog


CTextMarkerDlg::CTextMarkerDlg (CCrystalTextMarkers & markers, const CString& sText, findtext_flags_t dwFlags)
: CDialog (CTextMarkerDlg::IDD, nullptr)
, m_markers(markers)
, m_nCurItemIndex(0)
, m_tempMarkers(markers)
, m_nBgColorIndex(0)
, m_bMatchCase((dwFlags & FIND_MATCH_CASE) != 0)
, m_bWholeWord((dwFlags & FIND_WHOLE_WORD) != 0)
, m_bRegExp((dwFlags & FIND_REGEXP) != 0)
, m_bMarkersEnabled(markers.GetEnabled())
{
	if (!sText.IsEmpty())
	{
		m_tempMarkers.SetMarker(m_tempMarkers.MakeNewId(), sText, dwFlags, 
			static_cast<COLORINDEX>(static_cast<int>(COLORINDEX_MARKERBKGND1) + (m_tempMarkers.GetMarkers().size() % 3)));
	}
}

void CTextMarkerDlg::
DoDataExchange (CDataExchange * pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CTextMarkerDlg)
	DDX_Control(pDX, IDC_EDIT_MARKER_LIST, m_listMarkers);
	if (!pDX->m_bSaveAndValidate)
		UpdateDataListView(false);
	DDX_Control(pDX, IDC_EDIT_WHOLE_WORD, m_ctlWholeWord);
	DDX_Control(pDX, IDC_EDIT_MARKER_BGCOLORIDX, m_ctlBgColorIdx);
	DDX_Text(pDX, IDC_EDIT_FINDTEXT, m_sFindWhat);
	DDX_CBIndex(pDX, IDC_EDIT_MARKER_BGCOLORIDX, m_nBgColorIndex);
	DDX_Check(pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_EDIT_REGEXP, m_bRegExp);
	DDX_Check(pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
	DDX_Check(pDX, IDC_EDIT_MARKERS_ENABLED, m_bMarkersEnabled);
	if (pDX->m_bSaveAndValidate)
		UpdateDataListView(true);
	else
		m_ctlWholeWord.EnableWindow(m_bRegExp ? FALSE : TRUE);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CTextMarkerDlg, CDialog)
//{{AFX_MSG_MAP(CTextMarkerDlg)
ON_BN_CLICKED(IDC_EDIT_REGEXP, OnBnclickedEditRegExp)
ON_BN_CLICKED(IDC_EDIT_MARKER_NEW, &CTextMarkerDlg::OnBnClickedEditMarkerNew)
ON_BN_CLICKED(IDC_EDIT_MARKER_DELETE, &CTextMarkerDlg::OnBnClickedEditMarkerDelete)
ON_BN_CLICKED(ID_APPLY_NOW, &CTextMarkerDlg::OnBnClickedApplyNow)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_EDIT_MARKER_LIST, &CTextMarkerDlg::OnItemchangedEditMarkerList)
ON_CBN_EDITCHANGE(IDC_EDIT_FINDTEXT, &CTextMarkerDlg::OnEditchangeEditFindtext)
ON_NOTIFY(LVN_ITEMCHANGING, IDC_EDIT_MARKER_LIST, &CTextMarkerDlg::OnItemchangingEditMarkerList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CTextMarkerDlg message handlers

BOOL CTextMarkerDlg::OnInitDialog ()
{
	LangTranslateDialog(m_hWnd);
	CDialog::OnInitDialog ();

	CString fmt{LoadResString(IDS_MARKER_COLOR_FMT).c_str()};
	for (int i = 0; i < 3; ++i)
	{
		CString str;
		str.Format(fmt, i + 1);
		m_ctlBgColorIdx.InsertString(-1, str);
	}

	m_listMarkers.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT);
	std::vector<CString> keys;
	for (const auto& marker : m_tempMarkers.GetMarkers())
		keys.push_back(marker.first);
	std::sort(keys.begin(), keys.end());
	int i = 0;
	for (const auto& key: keys)
	{
		const auto& marker = m_tempMarkers.GetMarkers()[key];
		if (marker.bUserDefined)
		{
			m_listMarkers.InsertItem(LVIF_TEXT | LVIF_PARAM,
				m_listMarkers.GetItemCount(), 
				marker.sFindWhat, 0, 0, 0,
				reinterpret_cast<LPARAM>((const tchar_t *)(m_tempMarkers.GetMarkers().find(key)->first)));
			m_listMarkers.SetCheck(i, marker.bVisible);
			++i;
		}
	}
	if (i > 0)
		m_listMarkers.SetItemState(static_cast<int>(i - 1), 
			LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

	UpdateData(FALSE);
	return true;
}

int CTextMarkerDlg::GetSelectedItemIndex() const
{
	for (int i = 0; i < m_listMarkers.GetItemCount(); ++i)
	{
		if ((m_listMarkers.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED) != 0)
			return i;
	}
	return -1;
}

void CTextMarkerDlg::UpdateDataListView(bool bSaveAndValidate)
{
	if (!bSaveAndValidate)
	{
		for (int i = 0; i < m_listMarkers.GetItemCount(); ++i)
		{
			const tchar_t *pKey = reinterpret_cast<tchar_t *>(m_listMarkers.GetItemData(i));
			CCrystalTextMarkers::Marker& marker = m_tempMarkers.GetMarkers().at(pKey);
			if (m_listMarkers.GetItemState(i, LVIS_SELECTED))
			{
				m_sFindWhat = marker.sFindWhat;
				m_nBgColorIndex = static_cast<int>(marker.nBgColorIndex) - COLORINDEX_MARKERBKGND1;
				if (m_nBgColorIndex < 0)
					m_nBgColorIndex = 0;
				m_bWholeWord = (marker.dwFlags & FIND_WHOLE_WORD) != 0;
				m_bMatchCase = (marker.dwFlags & FIND_MATCH_CASE) != 0;
				m_bRegExp = (marker.dwFlags & FIND_REGEXP) != 0;
				if (m_bRegExp)
					m_bWholeWord = false;
			}
		}
	}
	else
	{
		for (int i = 0; i < m_listMarkers.GetItemCount(); ++i)
		{
			const tchar_t *pKey = reinterpret_cast<tchar_t *>(m_listMarkers.GetItemData(i));
			CCrystalTextMarkers::Marker& marker = m_tempMarkers.GetMarkers().at(pKey);
			if (m_listMarkers.GetItemState(i, LVIS_SELECTED))
			{
				marker.sFindWhat = m_sFindWhat;
				marker.nBgColorIndex = static_cast<COLORINDEX>(m_nBgColorIndex + COLORINDEX_MARKERBKGND1);
				marker.dwFlags = GetLastSearchFlags();
			}
		}
	}
}

findtext_flags_t CTextMarkerDlg::GetLastSearchFlags() const
{
	return
		(m_bMatchCase ? FIND_MATCH_CASE : 0) |
		(m_bWholeWord ? FIND_WHOLE_WORD : 0) |
		(m_bRegExp ? FIND_REGEXP : 0);
}

void CTextMarkerDlg::OnBnclickedEditRegExp()
{
	UpdateData();
}

void CTextMarkerDlg::OnBnClickedEditMarkerNew()
{
	CString name = m_tempMarkers.MakeNewId();
	CString sFindWhat = LoadResString(IDS_MARKER_NEW_PATTERN).c_str();
	m_tempMarkers.SetMarker(name, sFindWhat, GetLastSearchFlags(), 
		static_cast<COLORINDEX>(static_cast<int>(COLORINDEX_MARKERBKGND1) + (m_tempMarkers.GetMarkers().size() % 3)));
	int i = GetSelectedItemIndex();
	if (i >= 0)
		m_listMarkers.SetItemState(i, 0, LVIS_SELECTED);
	m_listMarkers.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM,
		m_listMarkers.GetItemCount(), sFindWhat, LVIS_SELECTED, LVIS_SELECTED, 0,
		reinterpret_cast<LPARAM>((const tchar_t *)(m_tempMarkers.GetMarkers().find(name)->first)));
	m_listMarkers.SetCheck(m_listMarkers.GetItemCount() - 1, 1);
}

void CTextMarkerDlg::OnBnClickedEditMarkerDelete()
{
	int i = GetSelectedItemIndex();
	if (i < 0)
		return;
	const tchar_t *pKey = reinterpret_cast<tchar_t *>(m_listMarkers.GetItemData(i));
	m_listMarkers.DeleteItem(i);
	m_tempMarkers.GetMarkers().erase(pKey);
	if (i >= m_listMarkers.GetItemCount() - 1)
		i = m_listMarkers.GetItemCount() - 1;
	if (i >= 0)
		m_listMarkers.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void CTextMarkerDlg::OnBnClickedApplyNow()
{
	UpdateData(TRUE);
	for (int i = 0; i < m_listMarkers.GetItemCount(); ++i)
	{
		const tchar_t *pKey = reinterpret_cast<tchar_t *>(m_listMarkers.GetItemData(i));
		m_tempMarkers.GetMarkers()[pKey].bVisible = !!m_listMarkers.GetCheck(i);
	}
	m_tempMarkers.SetEnabled(m_bMarkersEnabled);
	m_markers = m_tempMarkers;
	m_markers.UpdateViews();
}

void CTextMarkerDlg::OnOK()
{
	OnBnClickedApplyNow();
	CDialog::OnOK();
}

void CTextMarkerDlg::OnEditchangeEditFindtext()
{
	int i = GetSelectedItemIndex();
	if (i < 0)
		return;
	CString sFindWhat;
	GetDlgItemText(IDC_EDIT_FINDTEXT, sFindWhat);
	m_listMarkers.SetItemText(i, 0, sFindWhat);
}

void CTextMarkerDlg::OnItemchangingEditMarkerList(NMHDR *, LRESULT *pResult)
{
	UpdateData(TRUE);
	*pResult = 0;
}

void CTextMarkerDlg::OnItemchangedEditMarkerList(NMHDR *, LRESULT *pResult)
{
	UpdateData(FALSE);
	*pResult = 0;
}
