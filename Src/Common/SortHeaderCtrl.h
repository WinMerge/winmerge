/**
 *  @file SortHeaderCtrl.h
 *
 *  @brief Declaration of CSortHeaderCtrl
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$
//////////////////////////////////////////////////////////////////////


#ifndef __SORTHEADERCTRL_H__
#define __SORTHEADERCTRL_H__


/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl

class CSortHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CSortHeaderCtrl)

// Construction
public:
	CSortHeaderCtrl();

// Attributes
protected:
	int 	m_nSortCol;
	BOOL	m_bSortAsc;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortHeaderCtrl)
	//}}AFX_VIRTUAL

	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual int 	SetSortImage(int nCol, BOOL bAsc);

// Implementation
public:
	virtual ~CSortHeaderCtrl();

// Generated message map functions
protected:
	//{{AFX_MSG(CSortHeaderCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif
// __SORTHEADERCTRL_H__
