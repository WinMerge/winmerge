// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  PatchTool.h
 *
 * @brief Declaration file for PatchTool class
 */
#pragma once

#include "DiffWrapper.h"
#include "DiffItem.h"

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
	void AddFilesToList(const String& sDir1, const String& sDir2, const DirItem* ent1, const DirItem* ent2, std::vector<PATCHFILES>* fileList);
	int GetItemsForPatchList(const PathContext& paths, const String subdir[], std::vector<PATCHFILES>* fileList);

private:
    std::vector<PATCHFILES> m_fileList; /**< List of files to patch. */
	CDiffWrapper m_diffWrapper; /**< DiffWrapper instance we use to create patch. */
	String m_sPatchFile; /**< Patch file path and filename. */
	bool m_bOpenToEditor; /**< Is patch file opened to external editor? */
};
