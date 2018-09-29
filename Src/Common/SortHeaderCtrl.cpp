/**
 *  @file SortHeaderCtrl.cpp
 *
 *  @brief Implementation of CSortHeaderCtrl
 */ 

#include "StdAfx.h"
#include "SortHeaderCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl

IMPLEMENT_DYNAMIC(CSortHeaderCtrl, CHeaderCtrl)

BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortHeaderCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSortHeaderCtrl::CSortHeaderCtrl() : m_bSortAsc(true), m_nSortCol(-1)
{
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
}

int CSortHeaderCtrl::SetSortImage(int nCol, bool bAsc)
{
	int nPrevCol = m_nSortCol;

	m_nSortCol = nCol;
	m_bSortAsc = bAsc;

	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;

	// Clear HDF_SORTDOWN and HDF_SORTUP flag in all columns.
	int i;
	for (i = 0; i < this->GetItemCount(); i++)
	{
		GetItem( i, &hditem );
		if (hditem.fmt & (HDF_SORTDOWN | HDF_SORTUP))
		{
			hditem.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
			SetItem( i, &hditem );
		}
	}

	GetItem( nCol, &hditem );

	// We can use HDF_SORTUP and HDF_SORTDOWN flag to draw a sort arrow on the header.
	hditem.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
	hditem.fmt |= bAsc ? HDF_SORTUP : HDF_SORTDOWN;
	SetItem( nCol, &hditem );

	// Invalidate header control so that it gets redrawn
	Invalidate();

	return nPrevCol;
}