/** 
 * @file DiffFileData.h
 *
 * @brief Declaration for DiffFileData class.
 *
 * @date  Created: 2003-08-22
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _DIFFFILEDATA_H_
#define _DIFFFILEDATA_H_

// forward declarations needed by DiffFileData
struct file_data;
class PrediffingInfo;
struct DIFFITEM;
class IAbortable;
class CDiffContext;

/**
 * @brief C++ container for the structure (file_data) used by diffutils' diff_2_files(...)
 */
struct DiffFileData
{
// class interface

	static void SetDefaultCodepage(int defcp); // set codepage to assume for all unknown files

// enums
	enum { DIFFS_UNKNOWN=-1, DIFFS_UNKNOWN_QUICKCOMPARE=-9 };

// instance interface

	DiffFileData();
	~DiffFileData();

	bool OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2);
	void Reset();
	void Close() { Reset(); }
	void SetDisplayFilepaths(LPCTSTR szTrueFilepath1, LPCTSTR szTrueFilepath2);

	int diffutils_compare_files(int depth);
	int byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable);
	int prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di);
	BOOL Diff2Files(struct change ** diffs, int depth,
		int * bin_status, BOOL bMovedBlocks, int * bin_file);
	bool Filepath_Transform(FileLocation & fpenc, const CString & filepath, CString & filepathTransformed,
		const CString & filteredFilenames, PrediffingInfo * infoPrediffer, int fd);
	void GuessEncoding_from_buffer_in_DiffContext(int side, CDiffContext * pCtxt);
	static void GuessEncoding_from_buffer(FileLocation & fpenc, const char **data, int count);
	void GuessEncoding_from_FileLocation(FileLocation & fpenc);

// Data (public)
	file_data * m_inf;
	bool m_used; // whether m_inf has real data
	FileLocation m_FileLocation[2];
	FileTextStats m_textStats0, m_textStats1;

	CString m_sDisplayFilepath[2];
	int m_ndiffs;
	int m_ntrivialdiffs;
	struct UniFileBom // detect unicode file and quess encoding
	{
		UniFileBom(int); // initialize from file descriptor
		int size;
		int unicoding;
		unsigned char buffer[4];
	};

private:
	bool DoOpenFiles();
};

#endif // _DIFFFILEDATA_H_
