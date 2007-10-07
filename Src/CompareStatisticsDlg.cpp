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
 * @file  CompareStatisticsDlg.cpp
 *
 * @brief Implementation file for CompareStatisticsDlg dialog
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "CompareStatisticsDlg.h"
#include "CompareStats.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Icon size
static const int IconCX = 16;
static const int IconCY = 16;

/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg dialog

IMPLEMENT_DYNAMIC(CompareStatisticsDlg, CDialog)

CompareStatisticsDlg::CompareStatisticsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CompareStatisticsDlg::IDD, pParent)
, m_pCompareStats(NULL)
{
	//{{AFX_DATA_INIT(CompareStatisticsDlg)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CompareStatisticsDlg, CDialog)
	//{{AFX_MSG_MAP(SaveClosingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg message handlers

BOOL CompareStatisticsDlg::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();
	int totalFiles = 0;
	int totalFolders = 0;

	// Identicals
	int count = m_pCompareStats->GetCount(CompareStats::RESULT_DIR);
	totalFolders += count;
	SetDlgItemInt(IDC_STAT_IDENTICFOLDER, count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_SAME);
	totalFiles += count;
	SetDlgItemInt(IDC_STAT_IDENTICFILE, count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_BINSAME);
	totalFiles += count;
	SetDlgItemInt(IDC_STAT_IDENTICBINARY, count);

	// Different
	count = m_pCompareStats->GetCount(CompareStats::RESULT_DIFF);
	totalFiles += count;
	SetDlgItemInt(IDC_STAT_DIFFFILE, count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_BINDIFF);
	totalFiles += count;
	SetDlgItemInt(IDC_STAT_DIFFBINARY, count);

	// Unique
	count = m_pCompareStats->GetCount(CompareStats::RESULT_LDIRUNIQUE);
	totalFolders += count;
	SetDlgItemInt(IDC_STAT_LUNIQFOLDER, count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_LUNIQUE);
	totalFiles += count;
	SetDlgItemInt(IDC_STAT_LUNIQFILE, count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_RDIRUNIQUE);
	totalFolders += count;
	SetDlgItemInt(IDC_STAT_RUNIQFOLDER, count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_RUNIQUE);
	totalFiles += count;
	SetDlgItemInt(IDC_STAT_RUNIQFILE, count);

	// Total
	SetDlgItemInt(IDC_STAT_TOTALFOLDER, totalFolders);
	SetDlgItemInt(IDC_STAT_TOTALFILE, totalFiles);

	// Load small folder icons
	CStatic * pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_ILUNIQFOLDER);
	HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_LFOLDER), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_IRUNIQFOLDER);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_RFOLDER), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);
	
	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_IIDENTICFOLDER);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);
	
	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_INOTEQUAL);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_NOTEQUALFILE), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_IDIFFBINFILE);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_BINARYDIFF), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_ILUNIQFILE);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_LFILE), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_IRUNIQFILE);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_RFILE), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_IEQUALFILE);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_EQUALFILE), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_IEQUALBINFILE);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_EQUALBINARY), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_ITOTALFOLDERS);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_SIGMA), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	pBitmapCtrl = (CStatic *) GetDlgItem(IDC_STAT_ITOTALFILES);
	hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_SIGMA), IMAGE_ICON, IconCX, IconCY, LR_SHARED);
	pBitmapCtrl->SetIcon(hIcon);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CompareStatisticsDlg::SetCompareStats(const CompareStats * pStats)
{
	m_pCompareStats = pStats;
}