/** 
 * @file  DirCmpReport.cpp
 *
 * @brief Implementation file for DirCmpReport
 *
 */

#include "pch.h"
#include <ctime>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <Poco/Base64Encoder.h>
#include "locality.h"
#include "DirCmpReport.h"
#include "paths.h"
#include "unicoder.h"
#include "markdown.h"
#include "CompareStats.h"
#include "DiffItem.h"
#include "DiffThread.h"
#include "IAbortable.h"
#include "UniFile.h"
#include "TempFile.h"
#include "I18n.h"

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
		return strutils::format(_T("<%s>"), elName);
	else
		return strutils::format(_T("<%s %s>"), elName, attr);
}

/**
 * @brief Format string as ending tag.
 * @param [in] elName String to format as ending tag.
 * @return String formatted as ending tag.
 */
static String EndEl(const String& elName)
{
	return strutils::format(_T("</%s>"), elName);
}

/**
 * @brief Constructor.
 */
DirCmpReport::DirCmpReport(const std::vector<String> & colRegKeys)
: m_pList(nullptr)
, m_pFile(nullptr)
, m_nColumns(0)
, m_colRegKeys(colRegKeys)
, m_sSeparator(_T(","))
, m_pFileCmpReport(nullptr)
, m_bIncludeFileCmpReport(false)
, m_bOutputUTF8(false)
, m_myStruct(nullptr)
, m_bCopyToClipboard(false)
, m_nReportType(REPORT_TYPE_COMMALIST)
{
}

/**
 * @brief Set UI-list pointer.
 */
void DirCmpReport::SetList(IListCtrl *pList)
{
	m_pList.reset(pList);
}

/**
 * @brief Set root-paths of current compare so we can add them to report.
 * @param [in] paths Root path information for the directory for which the report is generated.
 */
void DirCmpReport::SetRootPaths(const PathContext &paths)
{
	if (paths.GetSize() < 3)
	{
		m_rootPaths.SetLeft(paths.GetLeft());
		m_rootPaths.SetRight(paths.GetRight());
		m_sTitle = strutils::format_string2(_("Compare %1 with %2"),
			m_rootPaths.GetLeft(), m_rootPaths.GetRight());
	}
	else {
		m_rootPaths.SetLeft(paths.GetLeft());
		m_rootPaths.SetMiddle(paths.GetMiddle());
		m_rootPaths.SetRight(paths.GetRight());
		m_sTitle = strutils::format_string3(_("Compare %1 with %2 and %3"),
			m_rootPaths.GetLeft(), m_rootPaths.GetMiddle(), m_rootPaths.GetRight());
	}
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
	m_pFileCmpReport.reset(pFileCmpReport);
}

/**
 * @brief Copy string content to clipboard as Unicode text.
 * @param [in] content String to copy to clipboard.
 * @return true if successful, false otherwise.
 */
static bool CopyTextToClipboard(const String& content)
{
	size_t len = content.length();
	HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, (len + 1) * sizeof(wchar_t));
	if (!hMem)
		return false;

	wchar_t* pMem = static_cast<wchar_t*>(GlobalLock(hMem));
	if (!pMem)
	{
		GlobalFree(hMem);
		return false;
	}

	wcscpy_s(pMem, len + 1, content.c_str());
	GlobalUnlock(hMem);
	SetClipboardData(CF_UNICODETEXT, hMem);
	return true;
}

/**
 * @brief Generate CF_HTML format data and copy to clipboard.
 * @param [in] htmlContent HTML content string to convert to CF_HTML format.
 */
void DirCmpReport::GenerateCF_HTML(const String& htmlContent)
{
	// CF_HTML format constants
	static const char header[] =
		"Version:0.9\n"
		"StartHTML:%09d\n"
		"EndHTML:%09d\n"
		"StartFragment:%09d\n"
		"EndFragment:%09d\n";
	static const char start[] = "<html><body>\n<!--StartFragment -->";
	static const char end[] = "\n<!--EndFragment -->\n</body>\n</html>\n";

	// Convert to UTF-8 for CF_HTML
	std::string htmlUtf8 = ucr::toUTF8(htmlContent);
	std::vector<char> htmlBuffer(htmlUtf8.begin(), htmlUtf8.end());

	// Rewrite CF_HTML header with valid offsets
	char headerBuf[MAX_PATH_FULL];
	int cbHeader = wsprintfA(headerBuf, header, 0, 0, 0, 0);
	int size = static_cast<int>(htmlBuffer.size());
	wsprintfA(headerBuf, header, cbHeader,
		size - 1,
		cbHeader + sizeof start - 1,
		size - sizeof end + 1);
	memcpy(htmlBuffer.data(), headerBuf, cbHeader);

	HGLOBAL hMemHTML = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, size);
	if (hMemHTML)
	{
		memcpy(GlobalLock(hMemHTML), htmlBuffer.data(), size);
		GlobalUnlock(hMemHTML);
		SetClipboardData(CF_HTML, hMemHTML);
	}
}

/**
 * @brief Generate report to clipboard.
 * @param [out] errStr Empty if succeeded, otherwise contains error message.
 * @return true if successful, false otherwise.
 */
bool DirCmpReport::GenerateReportToClipboard(String &errStr)
{
	if (!OpenClipboard(NULL))
		return false;
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return false;
	}

	// Generate report to temporary file
	TempFile tempFile;
	String tempFilePath = tempFile.Create(_T("winmerge_report_"), _T(".txt"));
	bool savedIncludeFileCmpReport = m_bIncludeFileCmpReport;
	m_bIncludeFileCmpReport = false;

	{
		UniStdioFile file;
		if (!file.OpenCreateUtf8(tempFilePath))
		{
			CloseClipboard();
			errStr = _("Failed to create temporary file.");
			return false;
		}
		file.SetBom(true);
		file.WriteBom();
		m_pFile = &file;
		GenerateReport(m_nReportType);
		file.Close();
		m_pFile = nullptr;
	}

	// Read temporary file and copy to clipboard
	UniMemFile file;
	if (file.OpenReadOnly(tempFilePath))
	{
		file.ReadBom();
		String content;
		file.ReadStringAll(content);
		file.Close();

		// Copy to clipboard as Unicode text
		if (!CopyTextToClipboard(content))
		{
			errStr = _("Failed to copy to clipboard.");
			CloseClipboard();
			m_bIncludeFileCmpReport = savedIncludeFileCmpReport;
			return false;
		}

		// If report type is HTML, render CF_HTML format as well
		if (m_nReportType == REPORT_TYPE_SIMPLEHTML)
		{
			GenerateCF_HTML(content);  // Pass the content directly
		}
	}
	CloseClipboard();
	m_bIncludeFileCmpReport = savedIncludeFileCmpReport;
	// TempFile destructor will automatically delete the temporary file
	return true;
}

/**
 * @brief Generate report to file.
 * @param [out] errStr Empty if succeeded, otherwise contains error message.
 * @return true if successful, false otherwise.
 */
bool DirCmpReport::GenerateReportToFile(String &errStr)
{
	String path;
	paths::SplitFilename(m_sReportFile, &path, nullptr, nullptr);
	if (!paths::CreateIfNeeded(path))
	{
		errStr = _("Folder does not exist.");
		return false;
	}

	UniStdioFile file;
	if (!file.OpenCreateUtf8(m_sReportFile))
	{
		errStr = _("Failed to create report file.");
		return false;
	}

	m_pFile = &file;
	GenerateReport(m_nReportType);
	file.Close();
	m_pFile = nullptr;
	return true;
}

/**
 * @brief Generate report and save it to file.
 * @param [out] errStr Empty if succeeded, otherwise contains error message.
 * @return `true` if report was created, `false` if user canceled report.
 */
bool DirCmpReport::GenerateReport(String &errStr)
{
	assert(m_pList != nullptr);
	assert(m_pFile == nullptr);

	if (m_bCopyToClipboard)
	{
		if (!GenerateReportToClipboard(errStr))
			return false;
	}
	if (!m_sReportFile.empty())
	{
		if (!GenerateReportToFile(errStr))
			return false;
	}
	return true;
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
	m_pFile->WriteString(sText);
}

/**
 * @brief Write text to report file while turning special chars to entities.
 * @param [in] sText Text to write to report file.
 */
void DirCmpReport::WriteStringEntityAware(const String& sText)
{
	WriteString(ucr::toTString(CMarkdown::Entities(ucr::toUTF8(sText))));
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
		if (m_myStruct && m_myStruct->context->GetAbortable()->ShouldAbort())
			break;
		WriteString(_T("\n"));
		DIFFITEM* pdi = reinterpret_cast<DIFFITEM*>(m_pList->GetItemData(currRow));
		if (reinterpret_cast<uintptr_t>(pdi) == -1)
			continue;
		if (m_myStruct)
			m_myStruct->context->m_pCompareStats->BeginCompare(pdi, 0);
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
		if (m_myStruct)
			m_myStruct->context->m_pCompareStats->AddItem(-1);
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
	WriteStringEntityAware(m_sTitle);
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
	WriteString(_T("\t\t\tposition: sticky; top: 0;\n"));
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
			WriteString(strutils::format(_T("\t\t.icon%d { background-image: url('data:image/png;base64,%s'); background-repeat: no-repeat; background-size: 16px 16px; }\n"), i, ucr::toTString(stream.str())));
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
	WriteString(_T("<table border=\"1\">\n<tr>\n"));

	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		WriteString(_T("<th>"));
		WriteStringEntityAware(m_pList->GetColumnName(currCol));
		WriteString(_T("</th>"));
	}
	WriteString(_T("</tr>\n"));
}

/**
 * @brief Generate simple xml report header.
 */
void DirCmpReport::GenerateXmlHeader()
{
	WriteString(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
				_T("<WinMergeDiffReport version=\"2\">\n")
				_T("<left>"));
	WriteStringEntityAware(m_rootPaths.GetLeft());
	WriteString(_T("</left>\n"));
	if (m_rootPaths.GetSize() == 3)
	{
		WriteString(_T("<middle>"));
		WriteStringEntityAware(m_rootPaths.GetMiddle());
		WriteString(_T("</middle>\n"));
	}
	WriteString(_T("<right>"));
	WriteStringEntityAware(m_rootPaths.GetRight());
	WriteString(_T("</right>\n")
				_T("<time>"));
	WriteStringEntityAware(GetCurrentTimeString());
	WriteString(_T("</time>\n"));

	// Add column headers
	const String rowEl = _T("column_name");
	WriteString(BeginEl(rowEl));
	for (int currCol = 0; currCol < m_nColumns; currCol++)
	{
		const String colEl = m_colRegKeys[currCol];
		WriteString(BeginEl(colEl));
		WriteStringEntityAware(m_pList->GetColumnName(currCol));
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
	paths::SplitFilename(m_pFile->GetFullyQualifiedPath(), &sParentDir, &sFileName, nullptr);
	String sRelDestDir = sFileName.substr(0, sFileName.find_last_of(_T('.'))) + _T(".files");
	String sDestDir = paths::ConcatPath(sParentDir, sRelDestDir);
	if (!xml && m_bIncludeFileCmpReport && m_pFileCmpReport != nullptr)
		paths::CreateIfNeeded(sDestDir);

	int nRows = m_pList->GetRowCount();

	// Report:Detail. All currently displayed columns will be added
	for (int currRow = 0; currRow < nRows; currRow++)
	{
		if (m_myStruct && m_myStruct->context->GetAbortable()->ShouldAbort())
			break;
		DIFFITEM* pdi = reinterpret_cast<DIFFITEM*>(m_pList->GetItemData(currRow));
		if (reinterpret_cast<uintptr_t>(pdi) == -1)
			continue;
		String sLinkPath;
		if (m_myStruct)
			m_myStruct->context->m_pCompareStats->BeginCompare(pdi, 0);
		if (!xml && m_bIncludeFileCmpReport && m_pFileCmpReport != nullptr)
			(*m_pFileCmpReport.get())(REPORT_TYPE_SIMPLEHTML, m_pList.get(), currRow, sDestDir, sLinkPath);

		String rowEl = _T("tr");
		if (xml)
		{
			rowEl = _T("filediff");
			WriteString(BeginEl(rowEl));
		}
		else
		{
			COLORREF backcolor = m_pList->GetBackColor(currRow);
			COLORREF textcolor = m_pList->GetTextColor(currRow);
			String attr = strutils::format(_T("style='%sbackground-color: #%02x%02x%02x'"),
				textcolor == 0 ? _T("") : strutils::format(_T("color: #%02x%02x%02x; "),
						GetRValue(textcolor), GetGValue(textcolor), GetBValue(textcolor)).c_str(),
				GetRValue(backcolor), GetGValue(backcolor), GetBValue(backcolor));
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
				WriteString(paths::urlEncodeFileName(sLinkPath));
				WriteString(_T("\">"));
				WriteStringEntityAware(m_pList->GetItemText(currRow, currCol));
				WriteString(_T("</a>"));
			}
			else
			{
				WriteStringEntityAware(m_pList->GetItemText(currRow, currCol));
			}
			WriteString(EndEl(colEl));
		}
		WriteString(EndEl(rowEl) + _T("\n"));
		if (m_myStruct)
			m_myStruct->context->m_pCompareStats->AddItem(-1);
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

