// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  PatchTool.cpp
 *
 * @brief Code file routines
 */

#include "StdAfx.h"
#include "PatchTool.h"
#include "UnicodeString.h"
#include "DiffWrapper.h"
#include "PathContext.h"
#include "PatchDlg.h"
#include "paths.h"
#include "Merge.h"
#include "DirTravel.h"
#include "OptionsDiffOptions.h"
#include "UniFile.h"
#include "codepage_detect.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "ClipBoard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Default constructor.
 */
CPatchTool::CPatchTool() : m_bOpenToEditor(false), m_bCopyToClipbard(false)
{
}

/**
 * @brief Default destructor.
 */
CPatchTool::~CPatchTool() = default;

/** 
 * @brief Adds files to list for patching.
 * @param [in] file1 First file to add.
 * @param [in] file2 Second file to add.
 */
void CPatchTool::AddFiles(const String &file1, const String &file2)
{
	PATCHFILES tFiles;
	tFiles.lfile = file1;
	tFiles.rfile = file2;

	// TODO: Read and add file's timestamps
	m_fileList.push_back(tFiles);
}

/**
 * @brief Add files with alternative paths.
 * This function adds files with alternative paths. Alternative path is the
 * one that is added to the patch file. So while @p file1 and @p file2 are
 * paths in disk (can be temp file names), @p altPath1 and @p altPath2 are
 * "visible " paths printed to the patch file.
 * @param [in] file1 First path in disk.
 * @param [in] altPath1 First path as printed to the patch file.
 * @param [in] file2 Second path in disk.
 * @param [in] altPath2 Second path as printed to the patch file.
 */
void CPatchTool::AddFiles(const String &file1, const String &altPath1,
		const String &file2, const String &altPath2)
{
	PATCHFILES tFiles;
	tFiles.lfile = file1;
	tFiles.rfile = file2;
	tFiles.pathLeft = altPath1;
	tFiles.pathRight = altPath2;

	// TODO: Read and add file's timestamps
	m_fileList.push_back(tFiles);
}

/** 
 * @brief Create a patch from files given.
 * @note Files can be given using AddFiles() or selecting using
 * CPatchDlg.
 */
int CPatchTool::CreatePatch()
{
	DIFFSTATUS status;
	int retVal = 0;

	CPatchDlg dlgPatch;

	// If files already inserted, add them to dialog
    for(std::vector<PATCHFILES>::iterator iter = m_fileList.begin(); iter != m_fileList.end(); ++iter)
    {
        dlgPatch.AddItem(*iter);
	}

	if (ShowDialog(&dlgPatch))
	{
		bool bResult = true;

		if (!paths::CreateIfNeeded(paths::GetPathOnly(dlgPatch.m_fileResult)))
		{
			LangMessageBox(IDS_FOLDER_NOTEXIST, MB_OK | MB_ICONSTOP);
			return 0;
		}

		// Select patch create -mode
		m_diffWrapper.SetCreatePatchFile(dlgPatch.m_fileResult);
		m_diffWrapper.SetAppendFiles(dlgPatch.m_appendFile);
		m_diffWrapper.SetPrediffer(nullptr);

		size_t fileCount = dlgPatch.GetItemCount();

		std::vector<PATCHFILES> fileList;
		for (size_t index = 0; index < fileCount; index++)
		{
			const PATCHFILES& tFiles = dlgPatch.GetItemAt(index);
			if (paths::DoesPathExist(tFiles.lfile) == paths::IS_EXISTING_DIR && paths::DoesPathExist(tFiles.rfile) == paths::IS_EXISTING_DIR)
			{
				// Walk given folders recursively and adds found files into patch list
				String lfile = tFiles.lfile;
				String rfile = tFiles.rfile;
				paths::normalize(lfile);
				paths::normalize(rfile);
				PathContext paths((tFiles.pathLeft.empty() ? _T("") : paths::GetParentPath(lfile)),
								  (tFiles.pathRight.empty() ? _T("") : paths::GetParentPath(rfile)));
				String subdir[2] = { (tFiles.pathLeft.empty() ? lfile : tFiles.pathLeft),
									 (tFiles.pathRight.empty() ? rfile : tFiles.pathRight) };
				GetItemsForPatchList(paths, subdir, &fileList);
			}
			else {
				fileList.push_back(tFiles);
			}
		}
		fileCount = fileList.size();

		m_diffWrapper.WritePatchFileHeader(dlgPatch.m_outputStyle, dlgPatch.m_appendFile);
		m_diffWrapper.SetAppendFiles(true);

		bool bShowedBinaryMessage = false;
		int writeFileCount = 0;

		for (size_t index = 0; index < fileCount; index++)
		{
			const PATCHFILES& tFiles = fileList[index];
			String filename1 = tFiles.lfile.length() == 0 ? _T("NUL") : tFiles.lfile;
			String filename2 = tFiles.rfile.length() == 0 ? _T("NUL") : tFiles.rfile;
			
			// Set up DiffWrapper
			m_diffWrapper.SetPaths(PathContext(filename1, filename2), false);
			m_diffWrapper.SetAlternativePaths(PathContext(tFiles.pathLeft, tFiles.pathRight));
			m_diffWrapper.SetCompareFiles(PathContext(tFiles.lfile, tFiles.rfile));
			bool bDiffSuccess = m_diffWrapper.RunFileDiff();
			m_diffWrapper.GetDiffStatus(&status);

			if (!bDiffSuccess)
			{
				LangMessageBox(IDS_FILEERROR, MB_ICONSTOP);
				bResult = false;
				break;
			}
			else if (status.bBinaries)
			{
				if (!bShowedBinaryMessage)
				{
					LangMessageBox(IDS_CANNOT_CREATE_BINARYPATCH, MB_ICONWARNING);
					bShowedBinaryMessage = true;
				}
			}
			else if (status.bPatchFileFailed)
			{
				String errMsg = strutils::format_string1(_("Could not write to file %1."), dlgPatch.m_fileResult);
				AfxMessageBox(errMsg.c_str(), MB_ICONSTOP);
				bResult = false;
				break;
			}
			else
			{
				writeFileCount++;
			}

		}
		
		m_diffWrapper.WritePatchFileTerminator(dlgPatch.m_outputStyle);

		if (bResult && writeFileCount > 0)
		{
			LangMessageBox(IDS_DIFF_SUCCEEDED, MB_ICONINFORMATION|MB_DONT_DISPLAY_AGAIN,
				            IDS_DIFF_SUCCEEDED);
			
			m_sPatchFile = dlgPatch.m_fileResult;
			m_bOpenToEditor = dlgPatch.m_openToEditor;
			m_bCopyToClipbard = dlgPatch.m_copyToClipboard;
			retVal = 1;
		}
	}
	dlgPatch.ClearItems();
	if (retVal)
	{
		if (m_bOpenToEditor)
			CMergeApp::OpenFileToExternalEditor(m_sPatchFile);
		if (m_bCopyToClipbard)
		{
			UniMemFile file;
			if (file.OpenReadOnly(m_sPatchFile))
			{
				int iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
				FileTextEncoding encoding = codepage_detect::Guess(m_sPatchFile, iGuessEncodingType);
				file.SetUnicoding(encoding.m_unicoding);
				file.SetCodepage(encoding.m_codepage);
				file.SetBom(encoding.m_bom);
				if (encoding.m_bom)
					file.ReadBom();
				String lines;
				file.ReadStringAll(lines);
				file.Close();
				PutToClipboard(lines, AfxGetMainWnd()->m_hWnd);
			}
		}
	}
	return retVal;
}

/** 
 * @brief Show patch options dialog and check options selected.
 * @return `true` if user wants to create a patch (didn't cancel dialog).
 */
bool CPatchTool::ShowDialog(CPatchDlg *pDlgPatch)
{
	DIFFOPTIONS diffOptions = {0};
	PATCHOPTIONS patchOptions;
	bool bRetVal = true;

	if (pDlgPatch->DoModal() == IDOK)
	{
		// There must be one filepair
		if (pDlgPatch->GetItemCount() < 1)
			bRetVal = false;

		// These two are from dropdown list - can't be wrong
		patchOptions.outputStyle = pDlgPatch->m_outputStyle;
		patchOptions.nContext = pDlgPatch->m_contextLines;

		// Checkbox - can't be wrong
		patchOptions.bAddCommandline = pDlgPatch->m_includeCmdLine;
		m_diffWrapper.SetPatchOptions(&patchOptions);

		// These are from checkboxes and radiobuttons - can't be wrong
		m_diffWrapper.SetAppendFiles(pDlgPatch->m_appendFile);
		Options::DiffOptions::Load(GetOptionsMgr(), diffOptions);
		m_diffWrapper.SetOptions(&diffOptions);
	}
	else
		return false;

	return bRetVal;
}

/**
 * @brief Add one compare item to patch list.
 * @param [in] sDir1 Left subdirectory.
 * @param [in] sDir2 Right subdirectory.
 * @param [in] ent1 Left item data to add.
 * @param [in] ent2 Right item data to add.
 * @param [out] fileList Patch files list.
 */
void CPatchTool::AddFilesToList(const String& sDir1, const String& sDir2, const DirItem* ent1, const DirItem* ent2, std::vector<PATCHFILES>* fileList)
{
	if ((ent1 == nullptr && ent2 == nullptr) || fileList == nullptr)
		return;

	static const tchar_t backslash[] = _T("\\");

	PATCHFILES tFiles;

	if (ent1 != nullptr)
	{
		tFiles.lfile = ent1->path.get() + backslash + ent1->filename.get();

		String pathLeft = _T("");
		if (!sDir1.empty())
			pathLeft = sDir1 + backslash;
		pathLeft += ent1->filename.get();
		tFiles.pathLeft = std::move(pathLeft);
	}

	if (ent2 != nullptr)
	{
		tFiles.rfile = ent2->path.get() + backslash + ent2->filename.get();

		String pathRight = _T("");
		if (!sDir2.empty())
			pathRight = sDir2 + backslash;
		pathRight += ent2->filename.get();

		tFiles.pathRight = std::move(pathRight);
	}

	fileList->push_back(tFiles);
}

/**
 * @brief This function walks given folders and adds found files into patch list.
 * We walk all subfolders and add the files they contain into list.
 *
 * @param [in] paths Root paths of compare
 * @param [in] subdir Subdirectories under root path
 * @param [out] fileList Patch files list
 * @return 1 normally, 0 if no directories and files exist.
 * @remark This function was written based on DirScan_GetItems() in DirScan.cpp
 */
int CPatchTool::GetItemsForPatchList(const PathContext& paths, const String subdir[], std::vector<PATCHFILES>* fileList)
{
	static const tchar_t backslash[] = _T("\\");
	int nDirs = paths.GetSize();

	String sDir[2];
	String subprefix[2];

	std::copy(paths.begin(), paths.end(), sDir);

	if (!subdir[0].empty())
	{
		for (int nIndex = 0; nIndex < paths.GetSize(); nIndex++)
		{
			sDir[nIndex] = paths::ConcatPath(sDir[nIndex], subdir[nIndex]);
			subprefix[nIndex] = subdir[nIndex] + backslash;
		}
	}

	DirItemArray dirs[2], aFiles[2];
	for (int nIndex = 0; nIndex < nDirs; nIndex++)
		LoadAndSortFiles(sDir[nIndex], &dirs[nIndex], &aFiles[nIndex], false);

	{
		int nIndex;
		for (nIndex = 0; nIndex < nDirs; nIndex++)
			if (dirs[nIndex].size() != 0 || aFiles[nIndex].size() != 0) break;
		if (nIndex == nDirs)
			return 0;
	}

	// Handle directories
	// i points to current directory in left list (dirs[0])
	// j points to current directory in right list (dirs[1])
	DirItemArray::size_type i = 0, j = 0;
	while (true)
	{
		if (i >= dirs[0].size() && j >= dirs[1].size())
			break;

		unsigned nDiffCode = DIFFCODE::DIR;
		// Comparing directories leftDirs[i].name to rightDirs[j].name
		if (i < dirs[0].size() && (j == dirs[1].size() || collstr(dirs[0][i].filename, dirs[1][j].filename, false) < 0))
		{
			nDiffCode |= DIFFCODE::FIRST;
		}
		else if (j < dirs[1].size() && (i == dirs[0].size() || collstr(dirs[1][j].filename, dirs[0][i].filename, false) < 0))
		{
			nDiffCode |= DIFFCODE::SECOND;
		}
		else
		{
			nDiffCode |= DIFFCODE::BOTH;
		}

		String leftnewsub = (nDiffCode & DIFFCODE::FIRST) ? subprefix[0] + dirs[0][i].filename.get() : subprefix[0] + dirs[1][j].filename.get();
		String rightnewsub = (nDiffCode & DIFFCODE::SECOND) ? subprefix[1] + dirs[1][j].filename.get() : subprefix[1] + dirs[0][i].filename.get();

		// Scan recursively all subdirectories too
		String newsubdir[2] = { leftnewsub, rightnewsub };
		GetItemsForPatchList(paths, newsubdir, fileList);

		if (nDiffCode & DIFFCODE::FIRST)
			i++;
		if (nDiffCode & DIFFCODE::SECOND)
			j++;
	}

	// Handle files
	// i points to current file in left list (aFiles[0])
	// j points to current file in right list (aFiles[1])
	i = 0, j = 0;
	while (true)
	{
		// Comparing file aFiles[0][i].name to aFiles[1][j].name
		if (i < aFiles[0].size() && (j == aFiles[1].size() || collstr(aFiles[0][i].filename, aFiles[1][j].filename, false) < 0))
		{
			AddFilesToList(subdir[0], subdir[1], &aFiles[0][i], nullptr, fileList);
			++i;
			continue;
		}
		if (j < aFiles[1].size() && (i == aFiles[0].size() || collstr(aFiles[0][i].filename, aFiles[1][j].filename, false) > 0))
		{
			AddFilesToList(subdir[0], subdir[1], nullptr, &aFiles[1][j], fileList);
			++j;
			continue;
		}
		if (i < aFiles[0].size())
		{
			assert(j < aFiles[1].size());

			AddFilesToList(subdir[0], subdir[1], &aFiles[0][i], &aFiles[1][j], fileList);
			++i;
			++j;
			continue;
		}
		break;
	}

	return 1;
}
