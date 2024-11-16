// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  AccentColor.cpp
 *
 * @brief Implementation of the CAccentColor class
 */

#include "StdAfx.h"
#include "AccentColor.h"
#include "RegKey.h"

CAccentColor::CAccentColor()
	: m_accentColor(CLR_NONE)
	, m_accentColorInactive(CLR_NONE)
	, m_colorPrevalence(false)
{
	Reload();
}

CAccentColor& CAccentColor::Get()
{
	static CAccentColor s_accentColor;
	return s_accentColor;
}

void CAccentColor::Reload()
{
	CRegKeyEx reg;
	if (ERROR_SUCCESS != reg.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\Windows\\DWM")))
		return;
	m_accentColor = reg.ReadDword(_T("AccentColor"), CLR_NONE);
	if (m_accentColor != CLR_NONE)
		m_accentColor &= 0xffffff;
	m_accentColorInactive = reg.ReadDword(_T("AccentColorInactive"), CLR_NONE);
	if (m_accentColorInactive != CLR_NONE)
		m_accentColorInactive &= 0xffffff;
	m_colorPrevalence = reg.ReadDword(_T("ColorPrevalence"), false);
}
