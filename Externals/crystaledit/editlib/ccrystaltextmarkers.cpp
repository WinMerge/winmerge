/**
 * @file  ccrystaltextmarkers.cpp
 *
 * @brief Imprementation file for CCrystalTextMarkers.
 */
#pragma once

#include "stdafx.h"
#include "ccrystaltextmarkers.h"
#include "ccrystaltextview.h"
#include "editreg.h"
#include <algorithm>

CCrystalTextMarkers::CCrystalTextMarkers() :
	m_enabled(true)
{
}

CCrystalTextMarkers::~CCrystalTextMarkers()
{
}

bool CCrystalTextMarkers::SetMarker(const tchar_t *pKey, const CString& sFindWhat, findtext_flags_t dwFlags, enum COLORINDEX nBgColorIndex, bool bUserDefined, bool bVisible)
{
	Marker marker = { sFindWhat, dwFlags, nBgColorIndex, bUserDefined, bVisible };
	m_markers.insert_or_assign(pKey, marker);
	return true;
}

void CCrystalTextMarkers::DeleteMarker(const tchar_t *pKey)
{
	m_markers.erase(pKey);
}

void CCrystalTextMarkers::DeleteAllMarker()
{
	m_markers.clear();
}

CString CCrystalTextMarkers::MakeNewId() const
{
	int id_max = 0;
	for (const auto& marker : m_markers)
	{
		if (marker.first.GetLength() > 6)
		{
			int tmp = tc::ttoi(marker.first.Mid(6));
			if (id_max < tmp)
				id_max = tmp;
		}
	}
	CString newid;
	newid.Format(_T("MARKER%4d"), id_max + 1);
	return newid;
}

void CCrystalTextMarkers::AddView(CCrystalTextView *pView)
{
	auto it = std::find(m_views.begin(), m_views.end(), pView);
	if (it == m_views.end())
		m_views.push_back(pView);
}

void CCrystalTextMarkers::DeleteView(CCrystalTextView *pView)
{
	auto it = std::find(m_views.begin(), m_views.end(), pView);
	if (it != m_views.end())
		m_views.erase(it);
}

void CCrystalTextMarkers::UpdateViews()
{
	for (auto& pView : m_views)
		pView->UpdateView(nullptr, nullptr, 0, -1);
}

CString CCrystalTextMarkers::Serialize() const
{
	CString text;
	text += m_enabled ? _T("Enabled") : _T("Disabled");
	text += _T("\n");
	for (const auto& marker : m_markers)
	{
		if (marker.second.bUserDefined)
		{
			text.AppendFormat(_T("%s:%s\t%d\t%d\t%d\t%d\n"),
				(const tchar_t*)marker.first, (const tchar_t*)marker.second.sFindWhat, 
				marker.second.dwFlags, marker.second.nBgColorIndex,
				marker.second.bUserDefined, marker.second.bVisible);
		}
	}
	return text;
}

bool CCrystalTextMarkers::Deserialize(const CString& value)
{
	for (const auto& marker : m_markers)
	{
		if (marker.second.bUserDefined)
			m_markers.erase(marker.first);
	}

	int pos = 0;
	int pos_delim = value.Find(_T("\n"), pos);
	if (pos_delim == -1)
		return false;
	m_enabled = (value.Mid(pos, pos_delim - pos) == _T("Enabled"));
	pos = pos_delim + 1;

	while (pos < value.GetLength())
	{
		pos_delim = value.Find(_T(":"), pos);
		if (pos_delim == -1)
			return false;
		CString key = value.Mid(pos, pos_delim - pos);
		pos = pos_delim + 1;

		std::vector<CString> ary;
		for (int i = 0; i < 4; ++i)
		{
			pos_delim = value.Find(_T("\t"), pos);
			if (pos_delim == -1)
				return false;
			ary.push_back (value.Mid(pos, pos_delim - pos));
			pos = pos_delim + 1;
		}

		Marker marker;
		marker.sFindWhat = ary[0];
		marker.dwFlags = tc::ttoi(ary[1]);
		marker.nBgColorIndex = static_cast<COLORINDEX>(tc::ttoi(ary[2]));
		marker.bVisible = tc::ttoi(ary[3]) != 0;
		marker.bUserDefined = true;

		pos_delim = value.Find(_T("\n"), pos);
		if (pos_delim == -1)
			return false;
		pos = pos_delim + 1;

		m_markers.insert_or_assign(key, marker);
	}
	return true;
}

bool CCrystalTextMarkers::SaveToRegistry() const
{
    return AfxGetApp()->WriteProfileString (EDITPAD_SECTION, _T ("Marker") + m_sGroupName, Serialize());
}

bool CCrystalTextMarkers::LoadFromRegistry()
{
	auto value = AfxGetApp()->GetProfileString (EDITPAD_SECTION, _T ("Marker") + m_sGroupName, _T(""));
	return Deserialize(value);
}
