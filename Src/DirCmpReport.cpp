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
#include <time.h>

#include "DirCmpReport.h"
#include "DirCmpReportDlg.h"
#include "coretools.h"
#include "WaitStatusCursor.h"
#include "paths.h"
#include "UniFile.h"
#include "DirReportTypes.h"

/**
 * @brief Return current time as string.
 * @return Current time as CString.
 */
static CString GetCurrentTimeString()
{
	time_t nTime=0;
	time(&nTime);
	_int64 nTime64 = nTime;
	CString str = TimeString(&nTime64);
	return str;
}

/**
 * @brief Format string as beginning tag.
 * @param [in] elName String to format as beginning tag.
 * @return String formatted as beginning tag.
 */
static CString BeginEl(LPCTSTR elName)
{
	CString tag;
	tag.Format(_T("<%s>"), elName);
	return tag;
}

/**
 * @brief Format string as ending tag.
 * @param [in] elName String to format as ending tag.
 * @return String formatted as ending tag.
 */
static CString EndEl(LPCTSTR elName)
{
	CString tag;
	tag.Format(_T("</%s>"), elName);
	return tag;
}

/**
 * @brief Constructor.
 */
DirCmpReport::DirCmpReport(const CStringArray & colRegKeys)
: m_pList(NULL)
, m_nColumns(0)
, m_colRegKeys(colRegKeys)
, m_sSeparator(_T(","))
{
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
		CString path;
		SplitFilename(m_sReportFile, &path, NULL, NULL);
		if (!paths_CreateIfNeeded(path))
		{
			VERIFY(errStr.LoadString(IDS_FOLDER_NOTEXIST));
			return FALSE;
		}

		// Preallocate large CString buffer to speed up building the string
		// (because CString::Append is not smart enough to preallocate exponentially)
		m_sReport.GetBufferSetLength(m_pList->GetItemCount() * 512);

		if (dlg.m_nReportType == REPORT_TYPE_SIMPLEHTML)
		{
			bool xml = false;
			GenerateHTMLHeader();
			GenerateXmlHtmlContent(xml);
			GenerateHTMLFooter();
		}
		else if (dlg.m_nReportType == REPORT_TYPE_SIMPLEXML)
		{
			bool xml = true;
			GenerateXmlHeader();
			GenerateXmlHtmlContent(xml);
			GenerateXmlFooter();
		}
		else
		{
			if (dlg.m_nReportType == REPORT_TYPE_COMMALIST)
				m_sSeparator = _T(",");
			else if (dlg.m_nReportType == REPORT_TYPE_TABLIST)
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
	time_t nTime=0;
	time(&nTime);
	_int64 nTime64 = nTime;
	CString sCurTime = TimeString(&nTime64);

	AfxFormatString2(m_sReport, IDS_DIRECTORY_REPORT_TITLE,
			m_rootPaths.GetLeft(), m_rootPaths.GetRight());
	m_sReport += _T("\n");
	m_sReport += sCurTime + _T("\n");

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
		m_sReport += _T("\n");
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
	m_sReport += GetCurrentTimeString() + _T("</p>\n");
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
 * @brief Generate simple xml report header.
 */
void DirCmpReport::GenerateXmlHeader()
{
	m_sReport = _T(""); // @todo xml declaration
	m_sReport += _T("<WinMergeDiffReport version=\"1\">\n");
	m_sReport += Fmt(_T("<left>%s</left>\n"), m_rootPaths.GetLeft());
	m_sReport += Fmt(_T("<right>%s</right>\n"), m_rootPaths.GetRight());
	m_sReport += Fmt(_T("<time>%s</time>\n"), GetCurrentTimeString());

	// Add column headers
	const CString rowEl = _T("column_name");
	m_sReport += BeginEl(rowEl);
	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		TCHAR columnName[160]; // Assuming max col header will never be > 160
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = countof(columnName);
		
		const CString colEl = m_colRegKeys[currCol];
		if (m_pList->GetColumn(currCol, &lvc))
		{
			m_sReport += BeginEl(colEl);
			m_sReport += lvc.pszText;
			m_sReport += EndEl(colEl);
		}
	}
	m_sReport += EndEl(rowEl) + _T("\n");
}

/**
 * @brief Generate simple html or xml report content.
 */
void DirCmpReport::GenerateXmlHtmlContent(bool xml)
{
	int nRows = m_pList->GetItemCount();

	// Report:Detail. All currently displayed columns will be added
	for (int currRow = 0; currRow < nRows; currRow++)
	{
		CString rowEl = _T("tr");
		if (xml)
			rowEl = _T("filediff");
		m_sReport += BeginEl(rowEl);
		for (int currCol = 0; currCol < m_nColumns; currCol++)
		{
			CString colEl = _T("td");
			if (xml)
				colEl = m_colRegKeys[currCol];
			m_sReport += BeginEl(colEl);
			m_sReport += m_pList->GetItemText(currRow, currCol);
			m_sReport += EndEl(colEl);
		}
		m_sReport += EndEl(rowEl) + _T("\n");
	}
	if (!xml)
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
 * @brief Generate simple xml report header.
 */
void DirCmpReport::GenerateXmlFooter()
{
	m_sReport += _T("</WinMergeDiffReport>\n");
}

/**
 * @brief Save generated report to file.
 * @param [out] sError Possible error message.
 */
BOOL DirCmpReport::SaveToFile(CString &sError)
{
	UniStdioFile file;

	// @todo
	// We could support Windows or Unix or Mac style here of course
	// Right now we're always doing Windows style lines

	m_sReport.Replace(_T("\n"), _T("\r\n"));
	
	if (!file.OpenCreate(m_sReportFile))
	{
		sError = GetSysError(GetLastError());		
		return FALSE;
	}

	// @todo
	// Should support Unicode output here

	file.SetCodepage(GetACP());
	file.WriteString(m_sReport);	
	file.Close();

	return TRUE;
}
