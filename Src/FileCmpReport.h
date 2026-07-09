/**
 * @file  FileCmpReport.h
 *
 * @brief Declaration of CFileCmpReport class for HTML report generation
 */

#pragma once

#include "UniFile.h"
#include "MergeDoc.h"

/**
 * @brief HTML report generator for file comparisons
 */
class CFileCmpReport
{
public:
	/**
	 * @brief Generate a unified HTML report from multiple documents
	 * @param [in] mergeDocuments Vector of merge documents to include
	 * @param [in] sFileName Output HTML file path
	 * @return true if report was generated successfully
	 */
	static bool GenerateDocumentReport(const std::vector<IMergeDoc*>& mergeDocuments, 
											const String& sFileName);

private:
	/**
	 * @brief Write HTML header to file
	 * @param [in] mergeDocuments Vector of merge documents to include
	 * @param [in,out] file UniStdioFile to write header to
	 */
	static void WriteHeader(const std::vector<IMergeDoc*>& mergeDocuments, UniStdioFile& file);

	/**
	 * @brief Write HTML footer to file
	 * @param [in,out] file UniStdioFile to write footer to
	 */
	static void WriteFooter(UniStdioFile& file);
};
