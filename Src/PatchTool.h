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
 * @file  PatchTool.h
 *
 * @brief Declaration file for PatchTool class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _PATCHTOOL_H_
#define _PATCHTOOL_H_


/** 
 * @brief Filepair to create patch
 */
typedef struct tagPATCHFILES
{
	CString lfile;
	CString rfile;
	time_t ltime, rtime;
} PATCHFILES;

/** 
 * @brief Provides patch creation functionality
 */
class CPatchTool
{
public:
	void AddFiles(CString file1, CString file2);
	int CreatePatch();

private:
	CList<PATCHFILES,PATCHFILES> m_fileList;
	CString m_sPatchFile;
	CDiffWrapper m_diffWrapper;
};

#endif	// _PATCHTOOL_H_