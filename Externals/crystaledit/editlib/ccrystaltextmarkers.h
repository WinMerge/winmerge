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
		bool bUserDefined;
		bool bVisible;
	};

	CCrystalTextMarkers();
	~CCrystalTextMarkers();

	void SetGroupName(const TCHAR *name) { m_sGroupName = name; }
	CString GetGroupName() const { return m_sGroupName; }
	bool SetMarker(const TCHAR *pKey, const CString& sFindWhat, DWORD dwFlags, enum COLORINDEX nBgColorIndex, bool bUserDefined = true, bool bVisible = true);
	void DeleteMarker(const TCHAR *pKey);
	void DeleteAllMarker();
	void AddView(CCrystalTextView *pView);
	void DeleteView(CCrystalTextView *pView);
	void UpdateViews();
	void SetEnabled(bool enabled) { m_enabled = enabled; };
	bool GetEnabled() const { return m_enabled; };
	CString MakeNewId() const;
	const std::map<const CString, Marker>& GetMarkers() const { return m_markers; }
	std::map<const CString, Marker>& GetMarkers() { return m_markers; }
	CString Serialize() const;
	bool Deserialize(const CString& value);
	bool SaveToRegistry() const;
	bool LoadFromRegistry();

private:
	CString m_sGroupName;
	std::map<const CString, Marker> m_markers;
	std::vector<CCrystalTextView *> m_views;
	bool m_enabled;
};
