/** 
 * @file  ColorButton.h
 *
 * @brief Declaration file for CColorButton class
 */
// ID line follows -- this is updated by SVN
// $Id$

#if !defined(AFX_COLORBUTTON_H__75623C05_2D2D_4FBE_B819_661F49DEAE94__INCLUDED_)
#define AFX_COLORBUTTON_H__75623C05_2D2D_4FBE_B819_661F49DEAE94__INCLUDED_

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
	CColorButton(COLORREF clrFill);

	void SetColor(COLORREF clrFill, BOOL bInvalidate = TRUE);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

#endif // !defined(AFX_COLORBUTTON_H__75623C05_2D2D_4FBE_B819_661F49DEAE94__INCLUDED_)
