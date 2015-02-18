/** 
 * @file  ColorButton.cpp
 *
 * @brief Implementation file for CColorButton
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** 
 * @brief Default constructor setting color to black.
 */
CColorButton::CColorButton()
: m_clrFill(RGB(0, 0, 0))
{
}

/** 
 * @brief Constructor setting initial color
 * @param [in] clfFill Initial color to set.
 */
CColorButton::CColorButton(COLORREF clrFill)
: m_clrFill(clrFill)
{
}

/** 
 * @brief Draws color button
 */
void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CBrush brush;
	brush.CreateSolidBrush(m_clrFill);
	::FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, brush);
}

/** 
 * @brief Sets new button color
 * @param [in] crlFill New color
 * @param [in] bInvalidate If TRUE button is invalidated (causing redraw)
 */
void CColorButton::SetColor(COLORREF clrFill, BOOL bInvalidate /* = TRUE*/)
{
	m_clrFill = clrFill;
	if (bInvalidate)
		Invalidate();
}
