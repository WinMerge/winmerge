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

#include "PatchDlg.h"

/** 
 * @brief Provides patch creation functionality
 */
class CPatchTool
{
public:
	void AddFiles(CString file1, CString file2);
	int CreatePatch();
	CString GetPatchFile() const;
	BOOL GetOpenToEditor() const;

protected:
	BOOL ShowDialog();

private:
	CList<PATCHFILES,PATCHFILES> m_fileList;
	CDiffWrapper m_diffWrapper;
	CPatchDlg m_dlgPatch;
	CString m_sPatchFile;
	BOOL m_bOpenToEditor;
};

#endif	// _PATCHTOOL_H_