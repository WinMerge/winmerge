/** 
 * @file  ColorButton.h
 *
 * @brief Declaration file for CColorButton class
 */
#pragma once

/** 
 * @brief Button showing currently selected color.
 * This class creates a button that shows currently selected color in its
 * client area. When the button is selected it opens color selection
 * dialog for selecting new color.
 */
class CColorButton : public CButton
{
private:
	COLORREF m_clrFill;

public:
	CColorButton();
	explicit CColorButton(COLORREF clrFill);

	void SetColor(COLORREF clrFill, bool bInvalidate = true);
	COLORREF GetColor() const { return m_clrFill; };
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};
