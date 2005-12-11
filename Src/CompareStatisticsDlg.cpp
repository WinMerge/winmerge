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

void CompareStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CompareStatisticsDlg)
	DDX_Text(pDX, IDC_STAT_IDENTICFOLDER, m_sIdenticalFolders);
	DDX_Text(pDX, IDC_STAT_IDENTICFILE, m_sIdenticalFiles);
	DDX_Text(pDX, IDC_STAT_IDENTICBINARY, m_sIdenticalBinaries);
	DDX_Text(pDX, IDC_STAT_DIFFFILE, m_sDifferentFiles);
	DDX_Text(pDX, IDC_STAT_DIFFBINARY, m_sDifferentBinaries);
	DDX_Text(pDX, IDC_STAT_LUNIQFOLDER, m_sLUniqueFolders);
	DDX_Text(pDX, IDC_STAT_LUNIQFILE, m_sLUniqueFiles);
	DDX_Text(pDX, IDC_STAT_RUNIQFOLDER, m_sRUniqueFolders);
	DDX_Text(pDX, IDC_STAT_RUNIQFILE, m_sRUniqueFiles);
	DDX_Text(pDX, IDC_STAT_TOTALFOLDER, m_sTotalFolders);
	DDX_Text(pDX, IDC_STAT_TOTALFILE, m_sTotalFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CompareStatisticsDlg, CDialog)
	//{{AFX_MSG_MAP(SaveClosingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg message handlers

BOOL CompareStatisticsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int totalFiles = 0;
	int totalFolders = 0;

	// Identicals
	int count = m_pCompareStats->GetCount(CompareStats::RESULT_DIR);
	totalFolders += count;
	m_sIdenticalFolders.Format(_T("%d"), count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_SAME);
	totalFiles += count;
	m_sIdenticalFiles.Format(_T("%d"), count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_BINSAME);
	totalFiles += count;
	m_sIdenticalBinaries.Format(_T("%d"), count);

	// Different
	count = m_pCompareStats->GetCount(CompareStats::RESULT_DIFF);
	totalFiles += count;
	m_sDifferentFiles.Format(_T("%d"), count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_BINDIFF);
	totalFiles += count;
	m_sDifferentBinaries.Format(_T("%d"), count);

	// Unique
	count = m_pCompareStats->GetCount(CompareStats::RESULT_LDIRUNIQUE);
	totalFolders += count;
	m_sLUniqueFolders.Format(_T("%d"), count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_LUNIQUE);
	totalFiles += count;
	m_sLUniqueFiles.Format(_T("%d"), count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_RDIRUNIQUE);
	totalFolders += count;
	m_sRUniqueFolders.Format(_T("%d"), count);
	count = m_pCompareStats->GetCount(CompareStats::RESULT_RUNIQUE);
	totalFiles += count;
	m_sRUniqueFiles.Format(_T("%d"), count);

	// Total
	m_sTotalFolders.Format(_T("%d"), totalFolders);
	m_sTotalFiles.Format(_T("%d"), totalFiles);

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

	UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CompareStatisticsDlg::SetCompareStats(const CompareStats * pStats)
{
	m_pCompareStats = const_cast<CompareStats*>(pStats);
}