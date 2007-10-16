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
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _FILES_H_INCLUDED
#define _FILES_H_INCLUDED

/**
 * @brief File-operation return-statuses
 * Note that FileLoadResult class has no instance data or methods.
 * It is only a namespace for static methods & constants.
 * Everything is public.
 */
class FileLoadResult
{
public:
// Checking results
	// main results
	static bool IsError(DWORD flr) { return Main(flr) == FRESULT_ERROR; }
	static bool IsOk(DWORD flr) { return Main(flr) == FRESULT_OK; }
	static bool IsOkImpure(DWORD flr) { return Main(flr) == FRESULT_OK_IMPURE; }
	static bool IsBinary(DWORD flr) { return Main(flr) == FRESULT_BINARY; }
	static bool IsErrorUnpack(DWORD flr) { return Main(flr) == FRESULT_ERROR_UNPACK; }
	// modifiers
	static bool IsLossy(DWORD flr) { return IsModifier(flr, FRESULT_LOSSY); }

// Assigning results
	// main results
	static void SetMainOk(DWORD & flr) { SetMain(flr, FRESULT_OK); }
	// modifiers
	static void AddModifier(DWORD & flr, DWORD modifier) { flr = (flr | modifier); }

	// bit manipulations
	static void SetMain(DWORD & flr, DWORD newmain) { flr = flr & ~FRESULT_MAIN_MASK; flr = flr | newmain; }
	static DWORD Main(DWORD flr) { return flr & FRESULT_MAIN_MASK; }
	static bool IsModifier(DWORD flr, DWORD modifier) { return !!(flr & modifier); }

	/** @brief Return values for functions. */
	enum
	{
		FRESULT_MAIN_MASK = 0xF,
		// main results
		FRESULT_ERROR = 0x0,
		FRESULT_OK = 0x1,
		FRESULT_OK_IMPURE = 0x2,
		FRESULT_BINARY = 0x3,
		FRESULT_ERROR_UNPACK = 0x4,
		// modifiers
		FRESULT_LOSSY = 0x10000,
	};
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
BOOL files_isFileReadOnly(const CString &file, BOOL *fileExists = NULL);

#endif // _FILES_H
