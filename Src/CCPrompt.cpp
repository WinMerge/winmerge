// CCPrompt.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "CCPrompt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCCPrompt dialog


CCCPrompt::CCCPrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CCCPrompt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCCPrompt)
	m_comments = _T("");
	//}}AFX_DATA_INIT
}


void CCCPrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCCPrompt)
	DDX_Text(pDX, IDC_COMMENTS, m_comments);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCCPrompt, CDialog)
	//{{AFX_MSG_MAP(CCCPrompt)
	ON_BN_CLICKED(IDSAVEAS, OnSaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCCPrompt message handlers

void CCCPrompt::OnSaveas() 
{
	EndDialog(IDSAVEAS);
}

