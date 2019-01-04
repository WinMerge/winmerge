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

#include "stdafx.h"
#include "CompareStatisticsDlg.h"
#include "CompareStats.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg dialog

IMPLEMENT_DYNAMIC(CompareStatisticsDlg, CTrDialog)

CompareStatisticsDlg::CompareStatisticsDlg(const CompareStats* pStats, CWnd* pParent /*= nullptr*/) :
	m_pCompareStats(pStats),
	CTrDialog(pStats->GetCompareDirs() < 3 ? IDD_COMPARE_STATISTICS : IDD_COMPARE_STATISTICS3, pParent)
{
	//{{AFX_DATA_INIT(CompareStatisticsDlg)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CompareStatisticsDlg, CTrDialog)
	//{{AFX_MSG_MAP(SaveClosingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg message handlers

/**
 * @brief Initialize the dialog, set statistics and load icons.
 */
BOOL CompareStatisticsDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();
	int totalFiles = 0;
	int totalFolders = 0;
	const int iconCX = []() {
		const int cx = GetSystemMetrics(SM_CXSMICON);
		if (cx < 24)
			return 16;
		if (cx < 32)
			return 24;
		if (cx < 48)
			return 32;
		return 48;
	}();
	const int iconCY = iconCX;

	static const struct { int ctlID; CompareStats::RESULT resultType; bool isDir; } ctlResultTypeMap[] =
	{
		{ IDC_STAT_IDENTICFOLDER,  CompareStats::RESULT_DIRSAME,     true },
		{ IDC_STAT_IDENTICFILE,    CompareStats::RESULT_SAME,        false },
		{ IDC_STAT_IDENTICBINARY,  CompareStats::RESULT_BINSAME,     false },
		{ IDC_STAT_DIFFFILE,       CompareStats::RESULT_DIFF,        false },
		{ IDC_STAT_DIFFBINARY,     CompareStats::RESULT_BINDIFF,     false },
		{ IDC_STAT_LUNIQFOLDER,    CompareStats::RESULT_LDIRUNIQUE,  true },
		{ IDC_STAT_LUNIQFILE,      CompareStats::RESULT_LUNIQUE,     false },
		{ IDC_STAT_MUNIQFOLDER,    CompareStats::RESULT_MDIRUNIQUE,  true },
		{ IDC_STAT_MUNIQFILE,      CompareStats::RESULT_MUNIQUE,     false },
		{ IDC_STAT_RUNIQFOLDER,    CompareStats::RESULT_RDIRUNIQUE,  true },
		{ IDC_STAT_RUNIQFILE,      CompareStats::RESULT_RUNIQUE,     false },
		{ IDC_STAT_LMISSINGFOLDER, CompareStats::RESULT_LDIRMISSING, true },
		{ IDC_STAT_LMISSINGFILE,   CompareStats::RESULT_LMISSING,    false },
		{ IDC_STAT_MMISSINGFOLDER, CompareStats::RESULT_MDIRMISSING, true },
		{ IDC_STAT_MMISSINGFILE,   CompareStats::RESULT_MMISSING,    false },
		{ IDC_STAT_RMISSINGFOLDER, CompareStats::RESULT_RDIRMISSING, true },
		{ IDC_STAT_RMISSINGFILE,   CompareStats::RESULT_RMISSING,    false },
	};
	for (auto&& map : ctlResultTypeMap)
	{
		int count = m_pCompareStats->GetCount(map.resultType);
		if (!map.isDir)
			totalFiles += count;
		else
			totalFolders += count;
		SetDlgItemInt(map.ctlID, count);
	}

	// Total
	SetDlgItemInt(IDC_STAT_TOTALFOLDER, totalFolders);
	SetDlgItemInt(IDC_STAT_TOTALFILE, totalFiles);

	// Load small folder icons
	static const struct { int ctlID; int iconID; } ctlIconMap[] =
	{
		{ IDC_STAT_ILUNIQFOLDER,    IDI_LFOLDER },
		{ IDC_STAT_IMUNIQFOLDER,    IDI_MFOLDER },
		{ IDC_STAT_IRUNIQFOLDER,    IDI_RFOLDER },
		{ IDC_STAT_ILMISSINGFOLDER, IDI_MRFOLDER },
		{ IDC_STAT_IMMISSINGFOLDER, IDI_LRFOLDER },
		{ IDC_STAT_IRMISSINGFOLDER, IDI_LMFOLDER },
		{ IDC_STAT_INOTEQUAL,       IDI_NOTEQUALFILE },
		{ IDC_STAT_IDIFFBINFILE,    IDI_BINARYDIFF },
		{ IDC_STAT_ILUNIQFILE,      IDI_LFILE },
		{ IDC_STAT_IMUNIQFILE,      IDI_MFILE },
		{ IDC_STAT_IRUNIQFILE,      IDI_RFILE },
		{ IDC_STAT_ILMISSINGFILE,   IDI_MRFILE },
		{ IDC_STAT_IMMISSINGFILE,   IDI_LRFILE },
		{ IDC_STAT_IRMISSINGFILE,   IDI_LMFILE },
		{ IDC_STAT_IEQUALFILE,      IDI_EQUALFILE },
		{ IDC_STAT_IEQUALBINFILE,   IDI_EQUALBINARY },
		{ IDC_STAT_IIDENTICFOLDER,  IDI_EQUALFOLDER },
	};
	
	for (auto&& map : ctlIconMap)
	{
		if (GetDlgItem(map.ctlID))
		{
			HICON hIcon = (HICON)LoadImage(AfxGetInstanceHandle(),
				MAKEINTRESOURCE(map.iconID), IMAGE_ICON, iconCX, iconCY, LR_SHARED);
			SendDlgItemMessage(map.ctlID, STM_SETICON, (WPARAM)hIcon, 0L);
		}
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
