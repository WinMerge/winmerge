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

/**
 * @brief This class creates directory compare reports.
 *
 * This class creates a directory compare report. To make things easier
 * we read data from view's listview. Reading from listview avoids
 * re-formatting data and complex GUI for selecting what info to show in
 * reports. Downside is we only have data that is visible in GUI.
 *
 * @todo We should read DIFFITEMs from CDirDoc and format data to better
 * fit for reporting. Dublicating formatting and sorting code should be
 * avoided.
 */
class DirCmpReport
{
public:
	/**
	 * @brief Report types in selection list.
	 */
	enum REPORT_TYPE
	{
		REPORT_COMMALIST = 0, /**< Comma-separated list */
		REPORT_TABLIST, /**< Tab-separated list */
		REPORT_SIMPLEHTML, /**< Simple html table */
	};

	DirCmpReport();
	void SetList(CListCtrl *pList);
	void SetRootPaths(const PathContext &paths);
	void SetColumns(int columns);
	BOOL GenerateReport(CString &errStr);

protected:
	void GenerateHeader();
	void GenerateContent();
	void GenerateHTMLHeader();
	void GenerateHTMLContent();
	void GenerateHTMLFooter();
	BOOL SaveToFile(CString &sError);

private:
	CListCtrl * m_pList; /**< Pointer to UI-list */
	PathContext m_rootPaths; /**< Root paths, printed to report */
	int m_nColumns; /**< Columns in UI */
	CString m_sSeparator; /**< Column separator for report */
	CString m_sReport; /**< Report as string */
	CString m_sReportFile; /**< Filename for report */
};

#endif // _DIRCMPREPORT_H_
