/** 
 * @file DiffFileData.h
 *
 * @brief Declaration for DiffFileData class.
 *
 * @date  Created: 2003-08-22
 */
// ID line follows -- this is updated by SVN
// $Id: DiffFileData.h 5055 2008-02-19 18:35:40Z kimmov $

#ifndef _DIFFFILEDATA_H_
#define _DIFFFILEDATA_H_

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

	static void SetDefaultCodepage(int defcp); // set codepage to assume for all unknown files


// instance interface

	DiffFileData();
	~DiffFileData();

	bool OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2);
	void Reset();
	void Close() { Reset(); }
	void SetDisplayFilepaths(LPCTSTR szTrueFilepath1, LPCTSTR szTrueFilepath2);

	bool Filepath_Transform(BOOL bForceUTF8, const FileTextEncoding & encoding, const String & filepath, String & filepathTransformed,
		LPCTSTR filteredFilenames, PrediffingInfo * infoPrediffer);

// Data (public)
	file_data * m_inf;
	bool m_used; // whether m_inf has real data
	FileLocation m_FileLocation[2];
	FileTextStats m_textStats[3];

	CString m_sDisplayFilepath[2];
	struct UniFileBom // detect unicode file and quess encoding
	{
		UniFileBom(int); // initialize from file descriptor
		int size;
		int unicoding;
		bool bom; /**< Are BOM bytes present in file? */
	};

private:
	bool DoOpenFiles();
};

#endif // _DIFFFILEDATA_H_
