// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  AccentColor.h
 *
 * @brief Declaration file for CAccentColor class
 */

#pragma once

class CAccentColor
{
public:
	CAccentColor();
	COLORREF GetAccentColor() const { return m_accentColor; };
	COLORREF GetAccentColorInactive() const { return m_accentColorInactive; }
	bool GetColorPrevalence() const { return m_colorPrevalence; }
	void Reload();
	static CAccentColor& Get();
private:
	COLORREF m_accentColor;
	COLORREF m_accentColorInactive;
	bool m_colorPrevalence;
};
