/** 
 * @file  ColorButton.cpp
 *
 * @brief Implementation file for CColorButton
 *
 */
// RCS ID line follows -- this is updated by CVS
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
{
	m_clrFill = RGB(0, 0, 0);
}

/** 
 * @brief Constructor setting initial color
 */
CColorButton::CColorButton( COLORREF clrFill ) : m_clrFill(clrFill)
{
}

/** 
 * @brief Destructor
 */
CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

/** 
 * @brief Draws color button
 */
void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CBrush	brush;

	brush.CreateSolidBrush( m_clrFill );

	::FillRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, brush );

#if 0
	::FrameRect( lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
			(HBRUSH)GetStockObject(BLACK_BRUSH) );
#endif
}

/** 
 * @brief Sets color
 * @param [in] crlFill New color
 * @param [in] bInvalidate If TRUE button is invalidated (causing redraw)
 */
void CColorButton::SetColor(COLORREF clrFill, BOOL bInvalidate /* = TRUE*/)
{
	m_clrFill = clrFill;
	if (bInvalidate)
		Invalidate();
}


