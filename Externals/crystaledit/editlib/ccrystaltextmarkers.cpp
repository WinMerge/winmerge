/**
 * @file  ccrystaltextmarkers.cpp
 *
 * @brief Imprementation file for CCrystalTextMarkers.
 */
#pragma once

#include "stdafx.h"
#include "ccrystaltextmarkers.h"
#include "ccrystaltextview.h"

CCrystalTextMarkers::CCrystalTextMarkers() :
	m_enabled(true)
{
}

CCrystalTextMarkers::~CCrystalTextMarkers()
{
}

bool CCrystalTextMarkers::SetMarker(const TCHAR *pKey, const CString& sFindWhat, DWORD dwFlags, enum COLORINDEX nBgColorIndex)
{
	Marker marker = { sFindWhat, dwFlags, nBgColorIndex };
	m_markers.insert_or_assign(pKey, marker);
	return true;
}

void CCrystalTextMarkers::DeleteMarker(const TCHAR *pKey)
{
	m_markers.erase(pKey);
}

void CCrystalTextMarkers::DeleteAllMarker()
{
	m_markers.clear();
}

void CCrystalTextMarkers::AddView(CCrystalTextView *pView)
{
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
