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
 * @file  WMGotoDlg.cpp
 *
 * @brief Implementation of the CWMGotoDlg class
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "WMGotoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg dialog


CWMGotoDlg::CWMGotoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWMGotoDlg::IDD, pParent)
	, m_nFile(-1)
	, m_nGotoWhat(-1)
{
}


void CWMGotoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWMGotoDlg)
	DDX_Text(pDX, IDC_WMGOTO_PARAM, m_strParam);
	DDX_Radio(pDX, IDC_WMGOTO_FILELEFT, m_nFile);
	DDX_Radio(pDX, IDC_WMGOTO_TOLINE, m_nGotoWhat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWMGotoDlg, CDialog)
	//{{AFX_MSG_MAP(CWMGotoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWMGotoDlg message handlers
