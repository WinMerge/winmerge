/** 
 * @file  DirCmpReport.cpp
 *
 * @brief Implementation file for DirCmpReport
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
//

#include "stdafx.h"
#include "DirCmpReport.h"
#include "DirCmpReportDlg.h"
#include <time.h>
#include "coretools.h"
#include "WaitStatusCursor.h"

/**
 * @brief EOL bytes for reports.
 * Since we are in Windows we want to use DOS-EOL.
 * @note HTML report doesn't use this.
 */
static const TCHAR ReportEOL[] = _T("\r\n");

/**
 * @brief Constructor.
 */
DirCmpReport::DirCmpReport()
: m_pList(NULL)
, m_nColumns(0)
{
	m_sSeparator = _T(",");
}

/**
 * @brief Set UI-list pointer.
 */
void DirCmpReport::SetList(CListCtrl *pList)
{
	m_pList = pList;
}

/**
 * @brief Set root-paths of current compare so we can add them to report.
 */
void DirCmpReport::SetRootPaths(const PathContext &paths)
{
	m_rootPaths.SetLeft(paths.GetLeft());
	m_rootPaths.SetRight(paths.GetRight());
}

/**
 * @brief Set column-count.
 */
void DirCmpReport::SetColumns(int columns)
{
	m_nColumns = columns;
}

/**
 * @brief Generate report and save it to file.
 * @param [out] errStr Empty if succeeded, otherwise contains error message.
 * @return TRUE if report was created, FALSE if user canceled report.
 */
BOOL DirCmpReport::GenerateReport(CString &errStr)
{
	ASSERT(m_pList != NULL);
	BOOL bRet = FALSE;

	DirCmpReportDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_CREATEREPORT));
		m_sReportFile = dlg.m_sReportFile;
		if (dlg.m_nReportType == REPORT_SIMPLEHTML)
		{
			GenerateHTMLHeader();
			GenerateHTMLContent();
			GenerateHTMLFooter();
		}
		else
		{
			if (dlg.m_nReportType == REPORT_COMMALIST)
				m_sSeparator = _T(",");
			else if (dlg.m_nReportType == REPORT_TABLIST)
				m_sSeparator = _T("\t");
				
			GenerateHeader();
			GenerateContent();
		}
		bRet = SaveToFile(errStr);
	}
	return bRet;
}

/**
 * @brief Generate header-data for report.
 */
void DirCmpReport::GenerateHeader()
{
	long nTime;
	_int64 nTime64;
	time(&nTime);
	nTime64 = nTime;
	CString sCurTime = TimeString(&nTime64);

	AfxFormatString2(m_sReport, IDS_DIRECTORY_REPORT_TITLE,
			m_rootPaths.GetLeft(), m_rootPaths.GetRight());
	m_sReport += ReportEOL;
	m_sReport += sCurTime + ReportEOL;

	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		TCHAR columnName[160]; // Assuming max col header will never be > 160
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = countof(columnName);
		if (m_pList->GetColumn(currCol, &lvc))
			m_sReport += lvc.pszText;
		// Add col-separator, but not after last column
		if (currCol < m_nColumns - 1)
			m_sReport += m_sSeparator;
	}
}

/**
 * @brief Generate report content (compared items).
 */
void DirCmpReport::GenerateContent()
{
	int nRows = m_pList->GetItemCount();

	// Report:Detail. All currently displayed columns will be added
	for (int currRow = 0; currRow < nRows; currRow++)
	{
		m_sReport += ReportEOL;
		for (int currCol = 0; currCol < m_nColumns; currCol++)
		{
			m_sReport += m_pList->GetItemText(currRow, currCol);

			// Add col-separator, but not after last column
			if (currCol < m_nColumns - 1)
				m_sReport += m_sSeparator;
		}
	}

}

/**
 * @brief Generate simple html report header.
 */
void DirCmpReport::GenerateHTMLHeader()
{
	long nTime;
	_int64 nTime64;
	time(&nTime);
	nTime64 = nTime;
	CString sCurTime = TimeString(&nTime64);

	m_sReport = _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n");
	m_sReport += _T("\t\"http://www.w3.org/TR/html4/loose.dtd\">\n");
	m_sReport += _T("<html>\n<head>\n\t<title>");
	CString title;
	AfxFormatString2(title, IDS_DIRECTORY_REPORT_TITLE,
			m_rootPaths.GetLeft(), m_rootPaths.GetRight());
	m_sReport += title;
	m_sReport += _T("</title>\n</head>\n<body>\n<h2>");
	m_sReport += title;
	m_sReport += _T("</h2>\n<p>");
	m_sReport += sCurTime + _T("</p>\n");
	m_sReport += _T("<table border=\"1\">\n<tr>\n");

	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		TCHAR columnName[160]; // Assuming max col header will never be > 160
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = countof(columnName);
		if (m_pList->GetColumn(currCol, &lvc))
		{
			m_sReport += _T("<th>");
			m_sReport += lvc.pszText;
			m_sReport += _T("</th>");
		}
	}
	m_sReport += _T("</tr>\n");
}

/**
 * @brief Generate simple html report content.
 */
void DirCmpReport::GenerateHTMLContent()
{
	int nRows = m_pList->GetItemCount();

	// Report:Detail. All currently displayed columns will be added
	for (int currRow = 0; currRow < nRows; currRow++)
	{
		m_sReport += _T("<tr>");
		for (int currCol = 0; currCol < m_nColumns; currCol++)
		{
			m_sReport += _T("<td>");
			m_sReport += m_pList->GetItemText(currRow, currCol);
			m_sReport += _T("</td>");
		}
		m_sReport += _T("</tr>\n");
	}
	m_sReport += _T("</table>\n");
}

/**
 * @brief Generate simple html report footer.
 */
void DirCmpReport::GenerateHTMLFooter()
{
	m_sReport += _T("</body>\n</html>\n");
}

/**
 * @brief Save generated report to file.
 * @param [out] sError Possible error message.
 */
BOOL DirCmpReport::SaveToFile(CString &sError)
{
	const UINT flags = CFile::modeCreate | CFile::modeWrite;
	CFile file;
	CFileException e;
	
	if (!file.Open(m_sReportFile, flags, &e))
	{
		TCHAR szError[1024];
		e.GetErrorMessage(szError, 1024);
		sError = szError;
		
		return FALSE;
	}

	// Convert to ANSI since most programs don't handle unicode files
	int reportLen = m_sReport.GetLength();
	int pos = 0;
	const int blockSize = 4095;
	char buf[blockSize + 1] = {0};
	
	USES_CONVERSION;
	while (pos < reportLen)
	{
		int curBlockSize = min(reportLen - pos, blockSize);
		strncpy(buf, T2CA((LPCTSTR)m_sReport.Mid(pos, curBlockSize)), curBlockSize);
		file.Write(buf, curBlockSize);
		pos += curBlockSize;
	}
	
	file.Close();

	return TRUE;
}
