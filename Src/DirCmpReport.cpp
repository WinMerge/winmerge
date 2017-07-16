/** 
 * @file  DirCmpReport.cpp
 *
 * @brief Implementation file for DirCmpReport
 *
 */

#include "stdafx.h"
#include <ctime>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <Poco/Base64Encoder.h>
#include "locality.h"
#include "DirCmpReport.h"
#include "DirCmpReportDlg.h"
#include "paths.h"
#include "unicoder.h"
#include "IListCtrl.h"

UINT CF_HTML = RegisterClipboardFormat(_T("HTML Format"));

/**
 * @brief Return current time as string.
 * @return Current time as String.
 */
static String GetCurrentTimeString()
{
	time_t nTime = 0;
	time(&nTime);
	_int64 nTime64 = nTime;
	String str = locality::TimeString(&nTime64);
	return str;
}

/**
 * @brief Format string as beginning tag.
 * @param [in] elName String to format as beginning tag.
 * @return String formatted as beginning tag.
 */
static String BeginEl(const String& elName, const String& attr = _T(""))
{
	if (attr.empty())
		return strutils::format(_T("<%s>"), elName.c_str());
	else
		return strutils::format(_T("<%s %s>"), elName.c_str(), attr.c_str());
}

/**
 * @brief Format string as ending tag.
 * @param [in] elName String to format as ending tag.
 * @return String formatted as ending tag.
 */
static String EndEl(const String& elName)
{
	return strutils::format(_T("</%s>"), elName.c_str());
}

/**
 * @brief Constructor.
 */
DirCmpReport::DirCmpReport(const std::vector<String> & colRegKeys)
: m_pList(NULL)
, m_pFile(NULL)
, m_nColumns(0)
, m_colRegKeys(colRegKeys)
, m_sSeparator(_T(","))
, m_pFileCmpReport(NULL)
, m_bIncludeFileCmpReport(false)
, m_bOutputUTF8(false)
{
}

/**
 * @brief Set UI-list pointer.
 */
void DirCmpReport::SetList(IListCtrl *pList)
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
	m_sTitle = strutils::format_string2(_("Compare %1 with %2"),
		m_rootPaths.GetLeft(), m_rootPaths.GetRight());
}

/**
 * @brief Set column-count.
 */
void DirCmpReport::SetColumns(int columns)
{
	m_nColumns = columns;
}

/**
 * @brief Set file compare reporter functor
 */
void DirCmpReport::SetFileCmpReport(IFileCmpReport *pFileCmpReport)
{
	m_pFileCmpReport = pFileCmpReport;
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
bool DirCmpReport::GenerateReport(String &errStr)
{
	assert(m_pList != NULL);
	assert(m_pFile == NULL);
	bool bRet = false;

	DirCmpReportDlg dlg;
	dlg.LoadSettings();
	dlg.m_sReportFile = m_sReportFile;

	if (!m_sReportFile.empty() || dlg.DoModal() == IDOK) try
	{
		CWaitCursor waitstatus;
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
				char buffer[_MAX_PATH];
				int cbHeader = wsprintfA(buffer, header, 0, 0, 0, 0);
				file.Write(buffer, cbHeader);
				file.Write(start, sizeof start - 1);
				GenerateHTMLHeaderBodyPortion();
				GenerateXmlHtmlContent(false);
				file.Write(end, sizeof end); // include terminating zero
				DWORD size = GetLength32(file);
				// Rewrite CF_HTML header with valid offsets
				file.SeekToBegin();
				wsprintfA(buffer, header, cbHeader, 
					static_cast<int>(size - 1),
					static_cast<int>(cbHeader + sizeof start - 1),
					static_cast<int>(size - sizeof end + 1));
				file.Write(buffer, cbHeader);
				SetClipboardData(CF_HTML, GlobalReAlloc(file.Detach(), size, 0));
			}
			CloseClipboard();
		}
		if (!dlg.m_sReportFile.empty())
		{
			String path;
			paths::SplitFilename(dlg.m_sReportFile, &path, NULL, NULL);
			if (!paths::CreateIfNeeded(path))
			{
				errStr = _("Folder does not exist.");
				return FALSE;
			}
			CFile file(dlg.m_sReportFile.c_str(),
				CFile::modeWrite|CFile::modeCreate|CFile::shareDenyWrite);
			m_pFile = &file;
			m_bIncludeFileCmpReport = !!dlg.m_bIncludeFileCmpReport;
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
		m_bOutputUTF8 = true;
		GenerateHTMLHeader();
		GenerateXmlHtmlContent(false);
		GenerateHTMLFooter();
		break;
	case REPORT_TYPE_SIMPLEXML:
		m_bOutputUTF8 = true;
		GenerateXmlHeader();
		GenerateXmlHtmlContent(true);
		GenerateXmlFooter();
		break;
	case REPORT_TYPE_COMMALIST:
		m_bOutputUTF8 = false;
		m_sSeparator = _T(",");
		GenerateHeader();
		GenerateContent();
		break;
	case REPORT_TYPE_TABLIST:
		m_bOutputUTF8 = false;
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
void DirCmpReport::WriteString(const String& sText)
{
	std::string sOctets(m_bOutputUTF8 ? ucr::toUTF8(sText) : ucr::toThreadCP(sText));
	const char *pchOctets = sOctets.c_str();
	void *pvOctets = const_cast<char *>(pchOctets);
	size_t cchAhead = sOctets.length();
	while (const char *pchAhead = (const char *)memchr(pchOctets, '\n', cchAhead))
	{
		size_t cchLine = pchAhead - pchOctets;
		m_pFile->Write(pchOctets, static_cast<unsigned>(cchLine));
		static const char eol[] = { '\r', '\n' };
		m_pFile->Write(eol, sizeof eol);
		++cchLine;
		pchOctets += cchLine;
		cchAhead -= cchLine;
	}
	m_pFile->Write(pchOctets, static_cast<unsigned>(cchAhead));

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
		WriteString(m_pList->GetColumnName(currCol));
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
	int nRows = m_pList->GetRowCount();

	// Report:Detail. All currently displayed columns will be added
	for (int currRow = 0; currRow < nRows; currRow++)
	{
		WriteString(_T("\n"));
		for (int currCol = 0; currCol < m_nColumns; currCol++)
		{
			String value = m_pList->GetItemText(currRow, currCol);
			if (value.find(m_sSeparator) != String::npos) {
				WriteString(_T("\""));
				WriteString(value);
				WriteString(_T("\""));
			}
			else
				WriteString(value);

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
		_T("<html>\n<head>\n")
		_T("\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n")
		_T("\t<title>"));
	WriteString(m_sTitle);
	WriteString(_T("</title>\n"));
	WriteString(_T("\t<style type=\"text/css\">\n\t<!--\n"));
	WriteString(_T("\t\tbody {\n"));
	WriteString(_T("\t\t\tfont-family: sans-serif;\n"));
	WriteString(_T("\t\t\tfont-size: smaller;\n"));
	WriteString(_T("\t\t}\n"));
	WriteString(_T("\t\ttable {\n"));
	WriteString(_T("\t\t\tborder-collapse: collapse;\n"));
	WriteString(_T("\t\t\tborder: 1px solid gray;\n"));
	WriteString(_T("\t\t}\n"));
	WriteString(_T("\t\tth,td {\n"));
	WriteString(_T("\t\t\tpadding: 3px;\n"));
	WriteString(_T("\t\t\ttext-align: left;\n"));
	WriteString(_T("\t\t\tvertical-align: middle;\n"));
	WriteString(_T("\t\t\tborder: 1px solid gray;\n"));
	WriteString(_T("\t\t}\n"));
	WriteString(_T("\t\tth {\n"));
	WriteString(_T("\t\t\tcolor: white;\n"));
	WriteString(_T("\t\t\tbackground: blue;\n"));
	WriteString(_T("\t\t\tpadding: 4px 4px;\n"));
	WriteString(_T("\t\t\tbackground: linear-gradient(mediumblue, darkblue);\n"));
	WriteString(_T("\t\t}\n"));
	WriteString(_T("\t\t.border {\n"));
	WriteString(_T("\t\t\tdisplay: table;\n"));
	WriteString(_T("\t\t\tborder-radius: 3px;\n"));
	WriteString(_T("\t\t\tborder: 1px #a0a0a0 solid;\n"));
	WriteString(_T("\t\t\tbox-shadow: 1px 1px 2px rgba(0, 0, 0, 0.15)\n"));
	WriteString(_T("\t\t\toverflow: hidden;\n"));
	WriteString(_T("\t\t}\n"));

	std::vector<bool> usedIcon(m_pList->GetIconCount());
	int maxIndent = 0;
	for (int i = 0; i < m_pList->GetRowCount(); ++i)
	{
		usedIcon[m_pList->GetIconIndex(i)] = true;
		maxIndent = (std::max)(m_pList->GetIndent(i), maxIndent);
	}
	for (int i = 0; i < m_pList->GetIconCount(); ++i)
	{
		if (usedIcon[i])
		{
			std::ostringstream stream;
			Poco::Base64Encoder enc(stream);
			enc.rdbuf()->setLineLength(0);
			enc << m_pList->GetIconPNGData(i);
			enc.close();
			WriteString(strutils::format(_T("\t\t.icon%d { background-image: url('data:image/png;base64,%s'); background-repeat: no-repeat; background-size: 16px 16px; }\n"), i, ucr::toTString(stream.str()).c_str()));
		}
	}
	for (int i = 0; i < maxIndent + 1; ++i)
		WriteString(strutils::format(_T("\t\t.indent%d { padding-left: %dpx; background-position: %dpx center; }\n"), i, 2 * 2 + 16 + 8 * i, 2 + 8 * i));
	WriteString(_T("\t-->\n\t</style>\n"));
	WriteString(_T("</head>\n<body>\n"));
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
	WriteString(_T("<div class=\"border\">\n<table border=\"1\">\n<tr>\n"));

	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		WriteString(_T("<th>"));
		WriteString(m_pList->GetColumnName(currCol));
		WriteString(_T("</th>"));
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
	WriteString(strutils::format(_T("<left>%s</left>\n"), m_rootPaths.GetLeft().c_str()));
	WriteString(strutils::format(_T("<right>%s</right>\n"), m_rootPaths.GetRight().c_str()));
	WriteString(strutils::format(_T("<time>%s</time>\n"), GetCurrentTimeString().c_str()));

	// Add column headers
	const String rowEl = _T("column_name");
	WriteString(BeginEl(rowEl));
	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		const String colEl = m_colRegKeys[currCol];
		WriteString(BeginEl(colEl));
		WriteString(m_pList->GetColumnName(currCol));
		WriteString(EndEl(colEl));
	}
	WriteString(EndEl(rowEl) + _T("\n"));
}

/**
 * @brief Generate simple html or xml report content.
 */
void DirCmpReport::GenerateXmlHtmlContent(bool xml)
{
	String sFileName, sParentDir;
	paths::SplitFilename((const TCHAR *)m_pFile->GetFilePath(), &sParentDir, &sFileName, NULL);
	String sRelDestDir = sFileName.substr(0, sFileName.find_last_of(_T("."))) + _T(".files");
	String sDestDir = paths::ConcatPath(sParentDir, sRelDestDir);
	if (!xml && m_bIncludeFileCmpReport && m_pFileCmpReport)
		paths::CreateIfNeeded(sDestDir);

	int nRows = m_pList->GetRowCount();

	// Report:Detail. All currently displayed columns will be added
	for (int currRow = 0; currRow < nRows; currRow++)
	{
		String sLinkPath;
		if (!xml && m_bIncludeFileCmpReport && m_pFileCmpReport)
			(*m_pFileCmpReport)(REPORT_TYPE_SIMPLEHTML, m_pList, currRow, sDestDir, sLinkPath);

		String rowEl = _T("tr");
		if (xml)
		{
			rowEl = _T("filediff");
			WriteString(BeginEl(rowEl));
		}
		else
		{
			COLORREF color = m_pList->GetBackColor(currRow);
			String attr = strutils::format(_T("style='background-color: #%02x%02x%02x'"),
				GetRValue(color), GetGValue(color), GetBValue(color));
			WriteString(BeginEl(rowEl, attr));
		}
		for (int currCol = 0; currCol < m_nColumns; currCol++)
		{
			String colEl = _T("td");
			if (xml)
			{
				colEl = m_colRegKeys[currCol];
				WriteString(BeginEl(colEl));
			}
			else
			{
				if (currCol == 0)
					WriteString(BeginEl(colEl, strutils::format(_T("class=\"icon%d indent%d\""), m_pList->GetIconIndex(currRow), m_pList->GetIndent(currRow))));
				else
					WriteString(BeginEl(colEl));
			}
			if (currCol == 0 && !sLinkPath.empty())
			{
				WriteString(_T("<a href=\""));
				WriteString(sRelDestDir);
				WriteString(_T("/"));
				WriteString(sLinkPath);
				WriteString(_T("\">"));
				WriteString(m_pList->GetItemText(currRow, currCol));
				WriteString(_T("</a>"));
			}
			else
			{
				WriteString(m_pList->GetItemText(currRow, currCol));
			}
			WriteString(EndEl(colEl));
		}
		WriteString(EndEl(rowEl) + _T("\n"));
	}
	if (!xml)
		WriteString(_T("</table>\n</div>\n"));
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

