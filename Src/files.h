/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  files.h
 *
 * @brief Declaration file for file routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _FILES_H_INCLUDED
#define _FILES_H_INCLUDED

/**
 * @brief File-operation return-statuses
 */
enum
{
	FRESULT_ERROR = 0,
	FRESULT_OK,
	FRESULT_BINARY,
	FRESULT_ERROR_UNPACK
};

/**
 * @brief Memory-mapped file information
 * When memory-mapped file is created, related information is
 * stored to this structure.
 */
struct MAPPEDFILEDATA
{
	TCHAR fileName[_MAX_PATH];
	BOOL bWritable;
	DWORD dwOpenFlags;		// CreateFile()'s dwCreationDisposition
	DWORD dwSize;
	HANDLE hFile;
	HANDLE hMapping;
	LPVOID pMapBase;
};

struct textline
{
	int start; // byte offset
	int end; // byte offset
	CString sline;
	typedef enum { EOL_NONE, EOL_CR, EOL_LF, EOL_CRLF } EOLTYPE;
	EOLTYPE eoltype;
	textline() : start(-1), end(-1), eoltype(EOL_NONE) { }
};
struct ParsedTextFile
{
	int crs; /* not including crlfs */
	int lfs; /* not including crlfs */
	int crlfs;
	int codeset;
	int charsize;
	bool lossy; /* was codeset conversion reversible ? */
	CArray<textline, textline> lines;

	ParsedTextFile()
		: crs(0), lfs(0), crlfs(0), codeset(0), charsize(1), lossy(false)
		{}
};

BOOL files_openFileMapped(MAPPEDFILEDATA *fileData);
BOOL files_closeFileMapped(MAPPEDFILEDATA *fileData, DWORD newSize, BOOL flush);
int files_loadLines(MAPPEDFILEDATA *fileData, ParsedTextFile * parsedTextFile);
BOOL files_isFileReadOnly(CString file, BOOL *fileExists = NULL);

#endif // _FILES_H
