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
#include "IListCtrl.h"

struct DiffFuncStruct;

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
	void SetReportType(REPORT_TYPE nReportType) { m_nReportType = nReportType;  }
	REPORT_TYPE GetReportType() const { return m_nReportType;  }
	void SetReportFile(const String& sReportFile) { m_sReportFile = sReportFile; }
	String GetReportFile() const { return m_sReportFile; }
	void SetColumns(int columns);
	void SetFileCmpReport(IFileCmpReport *pFileCmpReport);
	void SetCopyToClipboard(bool bCopyToClipbard) { m_bCopyToClipboard = bCopyToClipbard;  }
	bool GetCopyToClipboard() const { return m_bCopyToClipboard;  }
	void SetIncludeFileCmpReport(bool bIncludeFileCmpReport) { m_bIncludeFileCmpReport = bIncludeFileCmpReport; }
	bool GetIncludeFileCmpReport() const { return m_bIncludeFileCmpReport; }
	void SetDiffFuncStruct(DiffFuncStruct* myStruct) { m_myStruct = myStruct; }
	bool GenerateReport(String &errStr);

protected:
	void GenerateReport(REPORT_TYPE nReportType);
	void WriteString(const String&);
	void WriteStringEntityAware(const String& sText);
	void GenerateHeader();
	void GenerateContent();
	void GenerateHTMLHeader();
	void GenerateHTMLHeaderBodyPortion();
	void GenerateXmlHeader();
	void GenerateXmlHtmlContent(bool xml);
	void GenerateHTMLFooter();
	void GenerateXmlFooter();

private:
	std::unique_ptr<IListCtrl> m_pList; /**< Pointer to UI-list */
	PathContext m_rootPaths; /**< Root paths, printed to report */
	String m_sTitle; /**< Report title, built from root paths */
	String m_sReportFile;
	int m_nColumns; /**< Columns in UI */
	String m_sSeparator; /**< Column separator for report */
	CFile *m_pFile; /**< File to write report to */
	std::vector<String> m_colRegKeys; /**< Key names for currently displayed columns */
	std::unique_ptr<IFileCmpReport> m_pFileCmpReport;
	bool m_bIncludeFileCmpReport; /**< Do we include file compare report in folder compare report? */
	bool m_bOutputUTF8;
	REPORT_TYPE m_nReportType; /**< Report type integer */
	bool m_bCopyToClipboard; /**< Do we copy report to clipboard? */
	DiffFuncStruct* m_myStruct;
};
