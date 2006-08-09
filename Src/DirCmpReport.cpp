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
#include <afxadv.h>

UINT CF_HTML = RegisterClipboardFormat(_T("HTML Format"));

/**
 * @brief Return current time as string.
 * @return Current time as CString.
 */
static CString GetCurrentTimeString()
{
	time_t nTime = 0;
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
, m_pFile(NULL)
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
	AfxFormatString2(m_sTitle, IDS_DIRECTORY_REPORT_TITLE,
			m_rootPaths.GetLeft(), m_rootPaths.GetRight());
}

/**
 * @brief Set column-count.
 */
void DirCmpReport::SetColumns(int columns)
{
	m_nColumns = columns;
}

static ULONG GetLength32(CFile const &f)
{
	ULONGLONG length = f.GetLength();
	if (length > ULONG_MAX)
		length = ULONG_MAX;
	return static_cast<ULONG>(length);
}
/**
 * @brief Generate report and save it to file.
 * @param [out] errStr Empty if succeeded, otherwise contains error message.
 * @return TRUE if report was created, FALSE if user canceled report.
 */
BOOL DirCmpReport::GenerateReport(CString &errStr)
{
	ASSERT(m_pList != NULL);
	ASSERT(m_pFile == NULL);
	BOOL bRet = FALSE;

	DirCmpReportDlg dlg;
	if (dlg.DoModal() == IDOK) try
	{
		WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_CREATEREPORT));
		if (dlg.m_bCopyToClipboard)
		{
			if (!CWnd::GetSafeOwner()->OpenClipboard())
				return FALSE;
			if (!EmptyClipboard())
				return FALSE;
			CSharedFile file(GMEM_DDESHARE|GMEM_MOVEABLE|GMEM_ZEROINIT);
			m_pFile = &file;
			GenerateReport(dlg.m_nReportType);
			SetClipboardData(CF_TEXT, file.Detach());
			// If report type is HTML, render CF_HTML format as well
			if (dlg.m_nReportType == REPORT_TYPE_SIMPLEHTML)
			{
				// Reconstruct the CSharedFile object
				file.~CSharedFile();
				file.CSharedFile::CSharedFile(GMEM_DDESHARE|GMEM_MOVEABLE|GMEM_ZEROINIT);
				// Write preliminary CF_HTML header with all offsets zero
				static const char header[] =
					"Version:0.9\n"
					"StartHTML:%09d\n"
					"EndHTML:%09d\n"
					"StartFragment:%09d\n"
					"EndFragment:%09d\n";
				static const char start[] = "<html><body>\n<!--StartFragment -->";
				static const char end[] = "\n<!--EndFragment -->\n</body>\n</html>\n";
				char buffer[256];
				int cbHeader = wsprintfA(buffer, header, 0, 0, 0, 0);
				file.Write(buffer, cbHeader);
				file.Write(start, sizeof start - 1);
				GenerateHTMLHeaderBodyPortion();
				GenerateXmlHtmlContent(false);
				file.Write(end, sizeof end); // include terminating zero
				DWORD size = GetLength32(file);
				// Rewrite CF_HTML header with valid offsets
				file.SeekToBegin();
				wsprintfA(buffer, header, cbHeader, size - 1,
					cbHeader + sizeof start - 1, size - sizeof end + 1);
				file.Write(buffer, cbHeader);
				SetClipboardData(CF_HTML, GlobalReAlloc(file.Detach(), size, 0));
			}
			CloseClipboard();
		}
		if (!dlg.m_sReportFile.IsEmpty())
		{
			CString path;
			SplitFilename(dlg.m_sReportFile, &path, NULL, NULL);
			if (!paths_CreateIfNeeded(path))
			{
				VERIFY(errStr.LoadString(IDS_FOLDER_NOTEXIST));
				return FALSE;
			}
			CFile file(dlg.m_sReportFile,
				CFile::modeWrite|CFile::modeCreate|CFile::shareDenyWrite);
			m_pFile = &file;
			GenerateReport(dlg.m_nReportType);
		}
		bRet = TRUE;
	}
	catch (CException *e)
	{
		e->ReportError(MB_ICONSTOP);
		e->Delete();
	}
	m_pFile = NULL;
	return bRet;
}

/**
 * @brief Generate report of given type.
 * @param [in] nReportType Type of report.
 */
void DirCmpReport::GenerateReport(REPORT_TYPE nReportType)
{
	switch (nReportType)
	{
	case REPORT_TYPE_SIMPLEHTML:
		GenerateHTMLHeader();
		GenerateXmlHtmlContent(false);
		GenerateHTMLFooter();
		break;
	case REPORT_TYPE_SIMPLEXML:
		GenerateXmlHeader();
		GenerateXmlHtmlContent(true);
		GenerateXmlFooter();
		break;
	case REPORT_TYPE_COMMALIST:
		m_sSeparator = _T(",");
		GenerateHeader();
		GenerateContent();
		break;
	case REPORT_TYPE_TABLIST:
		m_sSeparator = _T("\t");
		GenerateHeader();
		GenerateContent();
		break;
	}
}

/**
 * @brief Write text to report file.
 * @param [in] pszText Text to write to report file.
 */
void DirCmpReport::WriteString(LPCTSTR pszText)
{
	USES_CONVERSION;
	LPCSTR pchOctets = T2A((LPTSTR)pszText);
	size_t cchAhead = strlen(pchOctets);
	while (LPCSTR pchAhead = (LPCSTR)memchr(pchOctets, '\n', cchAhead))
	{
		int cchLine = pchAhead - pchOctets;
		m_pFile->Write(pchOctets, cchLine);
		static const char eol[] = { '\r', '\n' };
		m_pFile->Write(eol, sizeof eol);
		++cchLine;
		pchOctets += cchLine;
		cchAhead -= cchLine;
	}
	m_pFile->Write(pchOctets, cchAhead);
}

/**
 * @brief Generate header-data for report.
 */
void DirCmpReport::GenerateHeader()
{
	WriteString(m_sTitle);
	WriteString(_T("\n"));
	WriteString(GetCurrentTimeString());
	WriteString(_T("\n"));
	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		TCHAR columnName[160]; // Assuming max col header will never be > 160
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = countof(columnName);
		if (m_pList->GetColumn(currCol, &lvc))
			WriteString(lvc.pszText);
		// Add col-separator, but not after last column
		if (currCol < m_nColumns - 1)
			WriteString(m_sSeparator);
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
		WriteString(_T("\n"));
		for (int currCol = 0; currCol < m_nColumns; currCol++)
		{
			WriteString(m_pList->GetItemText(currRow, currCol));

			// Add col-separator, but not after last column
			if (currCol < m_nColumns - 1)
				WriteString(m_sSeparator);
		}
	}

}

/**
 * @brief Generate simple html report header.
 */
void DirCmpReport::GenerateHTMLHeader()
{
	WriteString(_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n")
		_T("\t\"http://www.w3.org/TR/html4/loose.dtd\">\n")
		_T("<html>\n<head>\n\t<title>"));
	WriteString(m_sTitle);
	WriteString(_T("</title>\n</head>\n<body>\n"));
	GenerateHTMLHeaderBodyPortion();
}

/**
 * @brief Generate body portion of simple html report header (w/o body tag).
 */
void DirCmpReport::GenerateHTMLHeaderBodyPortion()
{
	WriteString(_T("<h2>"));
	WriteString(m_sTitle);
	WriteString(_T("</h2>\n<p>"));
	WriteString(GetCurrentTimeString());
	WriteString(_T("</p>\n"));
	WriteString(_T("<table border=\"1\">\n<tr>\n"));

	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		TCHAR columnName[160]; // Assuming max col header will never be > 160
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = countof(columnName);
		if (m_pList->GetColumn(currCol, &lvc))
		{
			WriteString(_T("<th>"));
			WriteString(lvc.pszText);
			WriteString(_T("</th>"));
		}
	}
	WriteString(_T("</tr>\n"));
}

/**
 * @brief Generate simple xml report header.
 */
void DirCmpReport::GenerateXmlHeader()
{
	WriteString(_T("")); // @todo xml declaration
	WriteString(_T("<WinMergeDiffReport version=\"1\">\n"));
	WriteString(Fmt(_T("<left>%s</left>\n"), m_rootPaths.GetLeft()));
	WriteString(Fmt(_T("<right>%s</right>\n"), m_rootPaths.GetRight()));
	WriteString(Fmt(_T("<time>%s</time>\n"), GetCurrentTimeString()));

	// Add column headers
	const CString rowEl = _T("column_name");
	WriteString(BeginEl(rowEl));
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
			WriteString(BeginEl(colEl));
			WriteString(lvc.pszText);
			WriteString(EndEl(colEl));
		}
	}
	WriteString(EndEl(rowEl) + _T("\n"));
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
		WriteString(BeginEl(rowEl));
		for (int currCol = 0; currCol < m_nColumns; currCol++)
		{
			CString colEl = _T("td");
			if (xml)
				colEl = m_colRegKeys[currCol];
			WriteString(BeginEl(colEl));
			WriteString(m_pList->GetItemText(currRow, currCol));
			WriteString(EndEl(colEl));
		}
		WriteString(EndEl(rowEl) + _T("\n"));
	}
	if (!xml)
		WriteString(_T("</table>\n"));
}

/**
 * @brief Generate simple html report footer.
 */
void DirCmpReport::GenerateHTMLFooter()
{
	WriteString(_T("</body>\n</html>\n"));
}

/**
 * @brief Generate simple xml report header.
 */
void DirCmpReport::GenerateXmlFooter()
{
	WriteString(_T("</WinMergeDiffReport>\n"));
}

