/** 
 * @file DiffFileData.h
 *
 * @brief Declaration for DiffFileData class.
 *
 * @date  Created: 2003-08-22
 */
#pragma once

#include "FileLocation.h"
#include "FileTextStats.h"

// forward declarations needed by DiffFileData
struct file_data;
class PrediffingInfo;
class CDiffContext;

/**
 * @brief C++ container for the structure (file_data) used by diffutils' diff_2_files(...)
 */
struct DiffFileData
{
// class interface

// instance interface

	DiffFileData();
	DiffFileData(const DiffFileData& other) = delete;
	~DiffFileData();

	bool OpenFiles(const String& szFilepath1, const String& szFilepath2);
	void Reset();
	void Close() { Reset(); }
	void SetDisplayFilepaths(const String& szTrueFilepath1, const String& szTrueFilepath2);

	bool Filepath_Transform(bool bForceUTF8, const FileTextEncoding & encoding, const String & filepath, String & filepathTransformed,
		const String& filteredFilenames, PrediffingInfo * infoPrediffer);

// Data (public)
	file_data * m_inf;
	bool m_used; // whether m_inf has real data
	FileLocation m_FileLocation[3];
	FileTextStats m_textStats[3];

	String m_sDisplayFilepath[2];

private:
	bool DoOpenFiles();
};
