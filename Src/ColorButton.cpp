// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton( COLORREF clrFill ) : m_clrFill(clrFill)
{
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

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


void CColorButton::SetColor( COLORREF clrFill )
{
	m_clrFill = clrFill;
	Invalidate();
}


