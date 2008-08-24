/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  CCPrompt.cpp
 *
 * @brief Implementation file for ClearCase dialog
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "CCPrompt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCCPrompt::CCCPrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CCCPrompt::IDD, pParent)
	, m_bMultiCheckouts(FALSE)
	, m_bCheckin(FALSE)
{
}

void CCCPrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCCPrompt)
	DDX_Text(pDX, IDC_COMMENTS, m_comments);
	DDX_Check(pDX, IDC_MULTI_CHECKOUT, m_bMultiCheckouts);
	DDX_Check(pDX, IDC_CHECKIN, m_bCheckin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCCPrompt, CDialog)
	//{{AFX_MSG_MAP(CCCPrompt)
	ON_BN_CLICKED(IDC_SAVE_AS, OnSaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @brief Handler for WM_INITDIALOG; conventional location to initialize
 * controls. At this point dialog and control windows exist.
 */
BOOL CCCPrompt::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCCPrompt::OnSaveas() 
{
	EndDialog(IDC_SAVE_AS);
}
