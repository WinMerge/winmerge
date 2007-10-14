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
class CDiffContext;

struct FileHandle
{
	FileHandle() : m_fp(0) { }
	void Assign(FILE * fp) { Close(); m_fp = fp; }
	void Close() { if (m_fp) { fclose(m_fp); m_fp = 0; } }
	~FileHandle() { Close(); }
	FILE * m_fp;
};

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

	bool Filepath_Transform(FileLocation & fpenc, const String & filepath, String & filepathTransformed,
		LPCTSTR filteredFilenames, PrediffingInfo * infoPrediffer, int fd);
	void GuessEncoding_from_buffer_in_DiffContext(int side, CDiffContext * pCtxt);
	static void GuessEncoding_from_buffer(FileLocation & fpenc, const char **data, int count);
	void GuessEncoding_from_FileLocation(FileLocation & fpenc);

// Data (public)
	file_data * m_inf;
	bool m_used; // whether m_inf has real data
	FileLocation m_FileLocation[2];
	FileTextStats m_textStats0, m_textStats1;

	CString m_sDisplayFilepath[2];
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
