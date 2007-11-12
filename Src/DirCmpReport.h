/** 
 * @file  DirCmpReport.h
 *
 * @brief Declaration file for DirCmpReport.
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _DIRCMPREPORT_H_
#define _DIRCMPREPORT_H_

#ifndef _PATHCONTEXT_H_
#include "PathContext.h"
#endif

#include "DirReportTypes.h"

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
class DirCmpReport
{
public:

	DirCmpReport(const CStringArray & colRegKeys);
	void SetList(CListCtrl *pList);
	void SetRootPaths(const PathContext &paths);
	void SetColumns(int columns);
	BOOL GenerateReport(String &errStr);

protected:
	void GenerateReport(REPORT_TYPE nReportType);
	void WriteString(LPCTSTR);
	void GenerateHeader();
	void GenerateContent();
	void GenerateHTMLHeader();
	void GenerateHTMLHeaderBodyPortion();
	void GenerateXmlHeader();
	void GenerateXmlHtmlContent(bool xml);
	void GenerateHTMLFooter();
	void GenerateXmlFooter();

private:
	CListCtrl * m_pList; /**< Pointer to UI-list */
	PathContext m_rootPaths; /**< Root paths, printed to report */
	CString m_sTitle; /**< Report title, built from root paths */
	int m_nColumns; /**< Columns in UI */
	CString m_sSeparator; /**< Column separator for report */
	CFile *m_pFile; /**< File to write report to */
	const CStringArray & m_colRegKeys; /**< Key names for currently displayed columns */
};

#endif // _DIRCMPREPORT_H_
