/**
 * @file  FileCmpReport.h
 *
 * @brief Declaration of CFileCmpReport class for HTML report generation
 */

#pragma once

#include "UnicodeString.h"
struct IMergeDoc;

class CFileCmpReport
{
public:
	struct Options
	{
		double fontSize = 10.0;
		bool includeAllImagePages = false;
		bool darkMode = false;
	};
	static bool GenerateDocumentReport(const std::vector<IMergeDoc*>& mergeDocuments, const String& sFileName, const Options& options, String& sError);
	static bool CopyToClipboard(const String& sFileName, String& sError);
};
