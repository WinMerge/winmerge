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
#pragma once

#include "DiffWrapper.h"

class CPatchDlg;

/** 
 * @brief Files used for patch creating.
 * Stores paths of two files used to create a patch. Left side file
 * is considered as "original" file and right side file as "changed" file.
 * Times are for printing filetimes to patch file.
 */
struct PATCHFILES
{
	String lfile; /**< Left file */
	String pathLeft; /**< Left path added to patch file */
	String rfile; /**< Right file */
	String pathRight; /**< Right path added to patch file */
	time_t ltime; /**< Left time */
	time_t rtime; /**< Right time */
	PATCHFILES() : ltime(0), rtime(0) {};
	/**
	 * @brief Swap diff sides.
	 */
	void swap_sides()
	{
		std::swap(lfile, rfile);
		std::swap(pathLeft, pathRight);
		std::swap(ltime, rtime);
	}
};

/** 
 * @brief A class which creates patch files.
 * This class is used to create patch files. The files to patch can be added
 * to list before calling CreatePatch(). Or user can select files in the
 * the dialog that CreatePatch() shows.
 */
class CPatchTool
{
public:
	CPatchTool();
	~CPatchTool();

	void AddFiles(const String &file1, const String &file2);
	void AddFiles(const String &file1, const String &altPath1,
		const String &file2, const String &altPath2);
	int CreatePatch();

protected:
	bool ShowDialog(CPatchDlg *pDlgPatch);

private:
    std::vector<PATCHFILES> m_fileList; /**< List of files to patch. */
	CDiffWrapper m_diffWrapper; /**< DiffWrapper instance we use to create patch. */
	String m_sPatchFile; /**< Patch file path and filename. */
	bool m_bOpenToEditor; /**< Is patch file opened to external editor? */
};
