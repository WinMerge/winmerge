/** 
 * @file  DirCmpReport.h
 *
 * @brief Declaration file for DirCmpReport.
 *
 */
#pragma once

#include <vector>
#include "UnicodeString.h"
#include "PathContext.h"
#include "DirReportTypes.h"

struct IListCtrl;

/**
 * @brief This class creates directory compare reports.
 *
 * This class creates a directory compare report. To make things easier
 * we read data from view's listview. Reading from listview avoids
 * re-formatting data and complex GUI for selecting what info to show in
 * reports. Downside is we only have data that is visible in GUI.
 *
 * @todo We should read DIFFITEMs from CDirDoc and format data to better
 * fit for reporting. Duplicating formatting and sorting code should be
 * avoided.
 */

struct IFileCmpReport
{
	virtual bool operator()(REPORT_TYPE nReportType, IListCtrl *pList, int nIndex, const String &sDestDir, String &sLinkPath) = 0;
};

class DirCmpReport
{
public:

	explicit DirCmpReport(const std::vector<String>& colRegKeys);
	void SetList(IListCtrl *pList);
	void SetRootPaths(const PathContext &paths);
	void SetReportFile(const String& sReportFile) { m_sReportFile = sReportFile; }
	void SetColumns(int columns);
	void SetFileCmpReport(IFileCmpReport *pFileCmpReport);
	bool GenerateReport(String &errStr);

protected:
	void GenerateReport(REPORT_TYPE nReportType);
	void WriteString(const String&);
	void GenerateHeader();
	void GenerateContent();
	void GenerateHTMLHeader();
	void GenerateHTMLHeaderBodyPortion();
	void GenerateXmlHeader();
	void GenerateXmlHtmlContent(bool xml);
	void GenerateHTMLFooter();
	void GenerateXmlFooter();

private:
	IListCtrl * m_pList; /**< Pointer to UI-list */
	PathContext m_rootPaths; /**< Root paths, printed to report */
	String m_sTitle; /**< Report title, built from root paths */
	String m_sReportFile;
	int m_nColumns; /**< Columns in UI */
	String m_sSeparator; /**< Column separator for report */
	CFile *m_pFile; /**< File to write report to */
	const std::vector<String>& m_colRegKeys; /**< Key names for currently displayed columns */
	IFileCmpReport *m_pFileCmpReport;
	bool m_bIncludeFileCmpReport;
	bool m_bOutputUTF8;
};
