/**
 * @file  ccrystaltextmarkers.h
 *
 * @brief Declaration file for CCrystalTextMarkers.
 */
#pragma once

#include <vector>
#include <map>
#include "SyntaxColors.h"

class CCrystalTextView;

class CCrystalTextMarkers
{
public:
	struct Marker
	{
		CString sFindWhat;
		DWORD dwFlags;
		enum COLORINDEX nBgColorIndex;
	};

	CCrystalTextMarkers();
	~CCrystalTextMarkers();

	bool SetMarker(const TCHAR *pKey, const CString& sFindWhat, DWORD dwFlags, enum COLORINDEX nBgColorIndex);
	void DeleteMarker(const TCHAR *pKey);
	void DeleteAllMarker();
	void AddView(CCrystalTextView *pView);
	void DeleteView(CCrystalTextView *pView);
	void UpdateViews();
	void SetEnabled(bool enabled) { m_enabled = enabled; };
	bool GetEnabled() const { return m_enabled; };
	const std::map<const TCHAR *, Marker>& GetMarkers() const { return m_markers; }
	std::map<const TCHAR *, Marker>& GetMarkers() { return m_markers; }

private:
	std::map<const TCHAR *, Marker> m_markers;
	std::vector<CCrystalTextView *> m_views;
	bool m_enabled;
};
