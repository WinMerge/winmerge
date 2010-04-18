/**
 *  @file DirScan.cpp
 *
 *  @brief Implementation of DirScan (q.v.) and helper functions
 */ 
// ID line follows -- this is updated by SVN
// $Id: DirScan.cpp 6909 2009-07-11 13:03:42Z kimmov $

#include "StdAfx.h"
#include "UnicodeString.h"
#include "Merge.h"
#include "LogFile.h"
#include "DirScan.h"
#include "CompareStats.h"
#include "DiffThread.h"
#include "FolderCmp.h"
#include "FileFilterHelper.h"
#include "codepage.h"
#include "IAbortable.h"
#include "FolderCmp.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Static functions (ie, functions only used locally)
void CompareDiffItem(DIFFITEM &di, CDiffContext * pCtxt);
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const FolderCmp * pCmpData);
static DIFFITEM *AddToList(LPCTSTR sLeftDir, LPCTSTR sRightDir, const DirItem * lent, const DirItem * rent,
	UINT code, DiffFuncStruct *myStruct, DIFFITEM *parent);
static DIFFITEM *AddToList(LPCTSTR sLeftDir, LPCTSTR sMiddleDir, LPCTSTR sRightDir, const DirItem * lent, const DirItem * ment, const DirItem * rent,
	UINT code, DiffFuncStruct *myStruct, DIFFITEM *parent);
static void UpdateDiffItem(DIFFITEM & di, BOOL & bExists, CDiffContext *pCtxt);

/**
 * @brief Help minimize memory footprint by sharing CStringData if possible.
 * 
 * Use OPTIMIZE_SHARE_CSTRINGDATA to conditionally include code that is merely
 * intended to minimize memory footprint by having two CStrings share one
 * CStringData if possible. The rule is that semantics must be identical
 * regardless of whether OPTIMIZE_SHARE_CSTRINGDATA(X) expands to X or to
 * nothing. If you suspect some bug to be related to this kind of optimization,
 * then you can simply change OPTIMIZE_SHARE_CSTRINGDATA to expand to nothing,
 * recompile, and see if bug disappears.
 */
#define OPTIMIZE_SHARE_CSTRINGDATA(X) X

/**
 * @brief Collect file- and folder-names to list.
 * This function walks given folders and adds found subfolders and files into
 * lists. There are two modes, determined by the @p depth:
 * - in non-recursive mode we walk only given folders, and add files.
 *   contained. Subfolders are added as folder items, not walked into.
 * - in recursive mode we walk all subfolders and add the files they
 *   contain into list.
 *
 * Items are tested against file filters in this function.
 * 
 * @param [in] paths Root paths of compare
 * @param [in] leftsubdir Left side subdirectory under root path
 * @param [in] bLeftUniq Is left-side folder unique folder?
 * @param [in] rightsubdir Right side subdirectory under root path
 * @param [in] bRightUniq Is right-side folder unique folder?
 * @param [in] myStruct Compare-related data, like context etc.
 * @param [in] casesensitive Is filename compare casesensitive?
 * @param [in] depth Levels of subdirectories to scan, -1 scans all
 * @param [in] parent Folder diff item to be scanned
 * @param [in] bUniques If true, walk into unique folders.
 * @return 1 normally, -1 if compare was aborted
 */
int DirScan_GetItems(const PathContext &paths, LPCTSTR subdir[],
		DiffFuncStruct *myStruct,
		bool casesensitive, int depth, DIFFITEM *parent,
		bool bUniques)
{
	static const TCHAR backslash[] = _T("\\");
	int nDirs = paths.GetSize();
	CDiffContext *pCtxt = myStruct->context;
	String sDir[3];
	String subprefix[3];

	int nIndex;
	for (nIndex = 0; nIndex < nDirs; nIndex++)
		sDir[nIndex] = paths.GetPath(nIndex);

	if (subdir[0][0])
	{
		for (nIndex = 0; nIndex < paths.GetSize(); nIndex++)
			sDir[nIndex] = paths_ConcatPath(sDir[nIndex], subdir[nIndex]);
		subprefix[0] = subdir[0];
		subprefix[0] += backslash;
		for (nIndex = 1; nIndex < paths.GetSize(); nIndex++)
		{
			// minimize memory footprint by having left/rightsubprefix share CStringData if possible
			subprefix[nIndex] = OPTIMIZE_SHARE_CSTRINGDATA
			(
				_tcsicmp(subdir[0], subdir[nIndex]) == 0 ? subprefix[0] : 
			) String(subdir[nIndex]) + backslash;
		}
	}

	DirItemArray dirs[3], files[3];
	for (nIndex = 0; nIndex < nDirs; nIndex++)
		LoadAndSortFiles(sDir[nIndex].c_str(), &dirs[nIndex], &files[nIndex], casesensitive);

	// Allow user to abort scanning
	if (pCtxt->ShouldAbort())
		return -1;

	// Handle directories
	// i points to current directory in left list (leftDirs)
	// j points to current directory in right list (rightDirs)

	// If there is only one directory on each side, and no files
	// then pretend the directories have the same name
	bool bTreatDirAsEqual;
	if (nDirs < 3)
	{
		bTreatDirAsEqual = 
		  (dirs[0].size() == 1)
		&& (dirs[1].size() == 1)
		&& (files[0].size() == 0)
		&& (files[1].size() == 0)
		;
	}
	else
	{
		bTreatDirAsEqual = 
		  (dirs[0].size() == 1)
		&& (dirs[1].size() == 1)
		&& (dirs[2].size() == 1)
		&& (files[0].size() == 0)
		&& (files[1].size() == 0)
		&& (files[2].size() == 0)
		;
	}

	for (nIndex = 0; nIndex < nDirs; nIndex++)
		if (dirs[nIndex].size() != 0 || files[nIndex].size() != 0) break;
	if (nIndex == nDirs)
		return 0;

	DirItemArray::size_type i=0, j=0, k=0;
	UINT nDiffCode;
	while (1)
	{
		if (pCtxt->ShouldAbort())
			return -1;

		if (i >= dirs[0].size() && j >= dirs[1].size() && (nDirs < 3 || k >= dirs[2].size()))
			break;

		// Comparing directories leftDirs[i].name to rightDirs[j].name
TCHAR buf[1024];
if (nDirs == 2)
	wsprintf(buf, _T("%s %s\n"), (i < dirs[0].size()) ? dirs[0][i].filename.c_str() : _T(""), (j < dirs[1].size()) ? dirs[1][j].filename.c_str() : _T(""));
else
	wsprintf(buf, _T("%s %s %s\n"), (i < dirs[0].size()) ? dirs[0][i].filename.c_str() : _T(""), (j < dirs[1].size()) ?  dirs[1][j].filename.c_str() : _T(""), (k < dirs[2].size()) ? dirs[2][k].filename.c_str() : _T(""));
OutputDebugString(buf);

		if (!bTreatDirAsEqual)
		{
			if (i<dirs[0].size() && (j==dirs[1].size() || collstr(dirs[0][i].filename, dirs[1][j].filename, casesensitive)<0)
				&& (nDirs < 3 ||      (k==dirs[2].size() || collstr(dirs[0][i].filename, dirs[2][k].filename, casesensitive)<0) ))
			{
				nDiffCode = DIFFCODE::FIRST | DIFFCODE::DIR;
				if (!bUniques)
				{
					if (nDirs < 3)
						AddToList(subdir[0], subdir[1], &dirs[0][i], 0, nDiffCode, myStruct, parent);
					else
						AddToList(subdir[0], subdir[1], subdir[2], &dirs[0][i], 0, 0, nDiffCode, myStruct, parent);
					// Advance left pointer over left-only entry, and then retest with new pointers
					++i;
					continue;
				}
			}
			else if (j<dirs[1].size() && (i==dirs[0].size() || collstr(dirs[1][j].filename, dirs[0][i].filename, casesensitive)<0)
				&& (nDirs < 3 ||      (k==dirs[2].size() || collstr(dirs[1][j].filename, dirs[2][k].filename, casesensitive)<0) ))
			{
				nDiffCode = DIFFCODE::SECOND | DIFFCODE::DIR;
				if (!bUniques)
				{
					if (nDirs < 3)
						AddToList(subdir[0], subdir[1], 0, &dirs[1][j], nDiffCode, myStruct, parent);
					else
						AddToList(subdir[0], subdir[1], subdir[2], 0, &dirs[1][j], 0, nDiffCode, myStruct, parent);
					// Advance right pointer over right-only entry, and then retest with new pointers
					++j;
					continue;
				}
			}
			else if (nDirs < 3)
			{
				nDiffCode = DIFFCODE::BOTH | DIFFCODE::DIR;
			}
			else
			{
				if (k<dirs[2].size() && (i==dirs[0].size() || collstr(dirs[2][k].filename, dirs[0][i].filename, casesensitive)<0)
					&&                     (j==dirs[1].size() || collstr(dirs[2][k].filename, dirs[1][j].filename, casesensitive)<0) )
				{
					nDiffCode = DIFFCODE::THIRD | DIFFCODE::DIR;
					if (!bUniques)
					{
						AddToList(subdir[0], subdir[1], subdir[2], 0, 0, &dirs[2][k], nDiffCode, myStruct, parent);
						++k;
						// Advance right pointer over right-only entry, and then retest with new pointers
						continue;
	
					}	
	
				}
				else if ((i<dirs[0].size() && j<dirs[1].size() && collstr(dirs[0][i].filename, dirs[1][j].filename, casesensitive) == 0)
				    && (k==dirs[2].size() || collstr(dirs[2][k].filename, dirs[0][i].filename, casesensitive) != 0))
				{
					nDiffCode = DIFFCODE::FIRST | DIFFCODE::SECOND | DIFFCODE::DIR;
					if (!bUniques)
					{
						AddToList(subdir[0], subdir[1], subdir[2], &dirs[0][i], &dirs[1][j], 0, nDiffCode, myStruct, parent);
						++i;
						++j;
						continue;	
					}
				}
				else if ((i<dirs[0].size() && k<dirs[2].size() && collstr(dirs[0][i].filename, dirs[2][k].filename, casesensitive) == 0)
				    && (j==dirs[1].size() || collstr(dirs[1][j].filename, dirs[2][k].filename, casesensitive) != 0))
				{
					nDiffCode = DIFFCODE::FIRST | DIFFCODE::THIRD | DIFFCODE::DIR;
					if (!bUniques)
					{
						AddToList(subdir[0], subdir[1], subdir[2], &dirs[0][i], 0, &dirs[2][k], nDiffCode, myStruct, parent);
						++i;
						++k;
						continue;
					}
				}
				else if ((j<dirs[1].size() && k<dirs[2].size() && collstr(dirs[1][j].filename, dirs[2][k].filename, casesensitive) == 0)
				    && (i==dirs[0].size() || collstr(dirs[0][i].filename, dirs[1][j].filename, casesensitive) != 0))
				{
					nDiffCode = DIFFCODE::SECOND | DIFFCODE::THIRD | DIFFCODE::DIR;
					if (!bUniques)
					{
						AddToList(subdir[0], subdir[1], subdir[2], 0, &dirs[1][j], &dirs[2][k], nDiffCode, myStruct, parent);
						++j;
						++k;
						continue;
					}
				}
				else
				{
					nDiffCode = DIFFCODE::ALL | DIFFCODE::DIR;
				}
			}
		}
		else
		{
			if (nDirs < 3)
				nDiffCode = DIFFCODE::BOTH | DIFFCODE::DIR;
			else
				nDiffCode = DIFFCODE::ALL | DIFFCODE::DIR;
		}

		// add to list
		if (!depth)
		{
			if (nDirs < 3)
				AddToList(subdir[0], subdir[1], 
					nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL, 
					nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
					nDiffCode, myStruct, parent);
			else
				AddToList(subdir[0], subdir[1], subdir[2], 
					nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL,
					nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
					nDiffCode & DIFFCODE::THIRD  ? &dirs[2][k] : NULL,
					nDiffCode, myStruct, parent);
		}
		else
		{
			// Recursive compare
			String leftnewsub;
			String rightnewsub;
			String middlenewsub;
			if (nDirs < 3)
			{
				leftnewsub  = (nDiffCode & DIFFCODE::FIRST)  ? subprefix[0] + dirs[0][i].filename.c_str() : subprefix[0] + dirs[1][j].filename.c_str();
				rightnewsub = (nDiffCode & DIFFCODE::SECOND) ? subprefix[1] + dirs[1][j].filename.c_str() : subprefix[1] + dirs[0][i].filename.c_str();
			}
			else
			{
				leftnewsub   = subprefix[0];
				if (nDiffCode & DIFFCODE::FIRST)       leftnewsub += dirs[0][i].filename.c_str();
				else if (nDiffCode & DIFFCODE::SECOND) leftnewsub += dirs[1][j].filename.c_str();
				else if (nDiffCode & DIFFCODE::THIRD)  leftnewsub += dirs[2][k].filename.c_str();
				middlenewsub = subprefix[1];
				if (nDiffCode & DIFFCODE::SECOND)      middlenewsub += dirs[1][j].filename.c_str();
				else if (nDiffCode & DIFFCODE::FIRST)  middlenewsub += dirs[0][i].filename.c_str();
				else if (nDiffCode & DIFFCODE::THIRD)  middlenewsub += dirs[2][k].filename.c_str();
				rightnewsub  = subprefix[2];
				if (nDiffCode & DIFFCODE::THIRD)       rightnewsub += dirs[2][k].filename.c_str();
				else if (nDiffCode & DIFFCODE::FIRST)  rightnewsub += dirs[0][i].filename.c_str();
				else if (nDiffCode & DIFFCODE::SECOND) rightnewsub += dirs[1][j].filename.c_str();
			}
			if (nDirs < 3)
			{
				// Test against filter so we don't include contents of filtered out directories
				// Also this is only place we can test for both-sides directories in recursive compare
				if (!pCtxt->m_piFilterGlobal->includeDir(leftnewsub.c_str(), rightnewsub.c_str()))
				{
					nDiffCode |= DIFFCODE::SKIPPED;
					AddToList(subdir[0], subdir[1], 
						nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL, 
						nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
						nDiffCode, myStruct, parent);
				}
				else
				{
					DIFFITEM *me = AddToList(subdir[0], subdir[1], 
						nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL, 
						nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
						nDiffCode, myStruct, parent);
					// Scan recursively all subdirectories too, we are not adding folders
					LPCTSTR newsubdir[3];
					newsubdir[0] = leftnewsub.c_str();
					newsubdir[1] = rightnewsub.c_str();
					int result = DirScan_GetItems(paths, newsubdir, myStruct, casesensitive,
							depth - 1, me, bUniques);
					if (result == -1)
						return -1;
/*
					if (result == 0)
					{
						if (!(nDiffCode & DIFFCODE::FIRST) || !(nDiffCode & DIFFCODE::SECOND))
						{
							AddToList(subdir[0], subdir[1], 
								nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL, 
								nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
								nDiffCode, myStruct, parent);
						}
					}
*/
				}
			}
			else
			{
				// Test against filter so we don't include contents of filtered out directories
				// Also this is only place we can test for both-sides directories in recursive compare
				if (!pCtxt->m_piFilterGlobal->includeDir(leftnewsub.c_str(), middlenewsub.c_str(), rightnewsub.c_str()))
				{
					nDiffCode |= DIFFCODE::SKIPPED;
					AddToList(subdir[0], subdir[1], subdir[2], 
						nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL,
						nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
						nDiffCode & DIFFCODE::THIRD  ? &dirs[2][k] : NULL,
						nDiffCode, myStruct, parent);
				}
				else
				{
					DIFFITEM *me = AddToList(subdir[0], subdir[1], subdir[2], 
						nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL,
						nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
						nDiffCode & DIFFCODE::THIRD  ? &dirs[2][k] : NULL,
						nDiffCode, myStruct, parent);
					// Scan recursively all subdirectories too, we are not adding folders
					LPCTSTR newsubdir[3];
					newsubdir[0] = leftnewsub.c_str();
					newsubdir[1] = middlenewsub.c_str();
					newsubdir[2] = rightnewsub.c_str();
					int result = DirScan_GetItems(paths, newsubdir, myStruct, casesensitive,
							depth - 1, me, bUniques);
					if (result == -1)
						return -1;
/*
					if (result == 0)
					{
						if (!(nDiffCode & DIFFCODE::FIRST) || !(nDiffCode & DIFFCODE::SECOND) || !(nDiffCode & DIFFCODE::THIRD))
						{
							AddToList(subdir[0], subdir[1], subdir[2], 
								nDiffCode & DIFFCODE::FIRST  ? &dirs[0][i] : NULL,
								nDiffCode & DIFFCODE::SECOND ? &dirs[1][j] : NULL,
								nDiffCode & DIFFCODE::THIRD  ? &dirs[2][k] : NULL,
								nDiffCode, myStruct, parent);
						}
					}
*/
				}
			}
		}
		if (nDiffCode & DIFFCODE::FIRST)
			i++;
		if (nDiffCode & DIFFCODE::SECOND)
			j++;
		if (nDiffCode & DIFFCODE::THIRD)
			k++;
	}
	// Handle files
	// i points to current file in left list (files[0])
	// j points to current file in right list (files[1])
	i=0, j=0, k=0;
	while (1)
	{
		if (pCtxt->ShouldAbort())
			return -1;


		// Comparing file files[0][i].name to files[1][j].name
TCHAR buf[1024];
if (nDirs == 2)
	wsprintf(buf, _T("%s %s\n"), (i < files[0].size()) ? files[0][i].filename.c_str() : _T(""), (j < files[1].size()) ? files[1][j].filename.c_str() : _T(""));
else
	wsprintf(buf, _T("%s %s %s\n"), (i < files[0].size()) ? files[0][i].filename.c_str() : _T(""), (j < files[1].size()) ?  files[1][j].filename.c_str() : _T(""), 
(k < files[2].size()) ? files[2][k].filename.c_str() : _T(""));
OutputDebugString(buf);
		
		if (i<files[0].size() && (j==files[1].size() ||
				collstr(files[0][i].filename, files[1][j].filename, casesensitive) < 0)
			&& (nDirs < 3 || 
				(k==files[2].size() || collstr(files[0][i].filename, files[2][k].filename, casesensitive)<0) ))
		{
			if (nDirs < 3)
			{
				const UINT nDiffCode = DIFFCODE::FIRST | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], &files[0][i], 0, nDiffCode, myStruct, parent);
			}
			else
			{
				const UINT nDiffCode = DIFFCODE::FIRST | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], 0, 0, nDiffCode, myStruct, parent);
			}
			// Advance left pointer over left-only entry, and then retest with new pointers
			++i;
			continue;
		}
		if (j<files[1].size() && (i==files[0].size() ||
				collstr(files[0][i].filename, files[1][j].filename, casesensitive) > 0)
			&& (nDirs < 3 ||
				(k==files[2].size() || collstr(files[1][j].filename, files[2][k].filename, casesensitive)<0) ))
		{
			const UINT nDiffCode = DIFFCODE::SECOND | DIFFCODE::FILE;
			if (nDirs < 3)
				AddToList(subdir[0], subdir[1], 0, &files[1][j], nDiffCode, myStruct, parent);
			else
				AddToList(subdir[0], subdir[1], subdir[2], 0, &files[1][j], 0, nDiffCode, myStruct, parent);
			// Advance right pointer over right-only entry, and then retest with new pointers
			++j;
			continue;
		}
		if (nDirs == 3)
		{
			if (k<files[2].size() && (i==files[0].size() ||
					collstr(files[2][k].filename, files[0][i].filename, casesensitive)<0)
				&& (j==files[1].size() || collstr(files[2][k].filename, files[1][j].filename, casesensitive)<0) )
			{
				int nDiffCode = DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], 0, 0, &files[2][k], nDiffCode, myStruct, parent);
				++k;
				// Advance right pointer over right-only entry, and then retest with new pointers
				continue;
			}

			if ((i<files[0].size() && j<files[1].size() && collstr(files[0][i].filename, files[1][j].filename, casesensitive) == 0)
			    && (k==files[2].size() || collstr(files[0][i].filename, files[2][k].filename, casesensitive) != 0))
			{
				int nDiffCode = DIFFCODE::FIRST | DIFFCODE::SECOND | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], &files[1][j], 0, nDiffCode, myStruct, parent);
				++i;
				++j;
				continue;
			}
			else if ((i<files[0].size() && k<files[2].size() && collstr(files[0][i].filename, files[2][k].filename, casesensitive) == 0)
			    && (j==files[1].size() || collstr(files[1][j].filename, files[2][k].filename, casesensitive) != 0))
			{
				int nDiffCode = DIFFCODE::FIRST | DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], 0, &files[2][k], nDiffCode, myStruct, parent);
				++i;
				++k;
				continue;
			}
			else if ((j<files[1].size() && k<files[2].size() && collstr(files[1][j].filename, files[2][k].filename, casesensitive) == 0)
			    && (i==files[0].size() || collstr(files[0][i].filename, files[1][j].filename, casesensitive) != 0))
			{
				int nDiffCode = DIFFCODE::SECOND | DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], 0, &files[1][j], &files[2][k], nDiffCode, myStruct, parent);
				++j;
				++k;
				continue;
			}
		}
		if (i<files[0].size())
		{
			if (nDirs < 3)
			{
				ASSERT(j<files[1].size());
				const int nDiffCode = DIFFCODE::BOTH | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], &files[0][i], &files[1][j], nDiffCode, myStruct, parent);
				++i;
				++j;
				continue;
			}
			else
			{
				ASSERT(j<files[1].size());
				ASSERT(k<files[2].size());
				const int nDiffCode = DIFFCODE::ALL | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], &files[1][j], &files[2][k], nDiffCode, myStruct, parent);
				++i;
				++j;
				++k;
				continue;
			}
		}
		break;
	}
	return 1;
}

/**
 * @brief Compare DiffItems in list and add results to compare context.
 *
 * @param myStruct [in] A structure containing compare-related data.
 * @param parentdiffpos [in] Position of parent diff item 
 * @return >= 0 number of diff items, -1 if compare was aborted
 */
int DirScan_CompareItems(DiffFuncStruct *myStruct, UINT_PTR parentdiffpos)
{
	DWORD dwElapse = GetTickCount();
	CDiffContext *pCtxt = myStruct->context;
	int res = 0;
	if (!parentdiffpos)
		WaitForSingleObject(myStruct->hSemaphore, INFINITE);
	UINT_PTR pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	while (pos)
	{
		if (pCtxt->ShouldAbort())
		{
			res = -1;
			break;
		}
		if (GetTickCount() - dwElapse > 2000)
		{
			PostMessage(myStruct->hWindow, myStruct->msgUIUpdate, 1, myStruct->bOnlyRequested);
			dwElapse = GetTickCount();
		}
		WaitForSingleObject(myStruct->hSemaphore, INFINITE);
		UINT_PTR curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		bool existsalldirs = ((pCtxt->GetCompareDirs() == 2 && di.diffcode.isSideBoth()) || (pCtxt->GetCompareDirs() == 3 && di.diffcode.isSideAll()));
		if (di.diffcode.isDirectory() && pCtxt->m_bRecursive)
		{
			di.diffcode.diffcode &= ~(DIFFCODE::DIFF | DIFFCODE::SAME);
			int ndiff = DirScan_CompareItems(myStruct, curpos);
			if (ndiff > 0)
			{
				if (existsalldirs)
					di.diffcode.diffcode |= DIFFCODE::DIFF;
				res += ndiff;
			}
			else if (ndiff == 0)
			{
				if (existsalldirs)
					di.diffcode.diffcode |= DIFFCODE::SAME;
			}
		}
		CompareDiffItem(di, pCtxt);
		if (di.diffcode.isResultDiff() ||
			(!existsalldirs && !di.diffcode.isResultFiltered()))
			res++;
		pos = curpos;
		pCtxt->GetNextSiblingDiffRefPosition(pos);
	}
	return res;
}

/**
 * @brief Compare DiffItems in context marked for rescan.
 *
 * @param myStruct [in,out] A structure containing compare-related data.
 * @param parentdiffpos [in] Position of parent diff item 
 * @return >= 0 number of diff items, -1 if compare was aborted
 */
int DirScan_CompareRequestedItems(DiffFuncStruct *myStruct, UINT_PTR parentdiffpos)
{
	CDiffContext *pCtxt = myStruct->context;
	int res = 0;
	UINT_PTR pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	
	while (pos != NULL)
	{
		if (pCtxt->ShouldAbort())
		{
			res = -1;
			break;
		}

		UINT_PTR curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		bool existsalldirs = ((pCtxt->GetCompareDirs() == 2 && di.diffcode.isSideBoth()) || (pCtxt->GetCompareDirs() == 3 && di.diffcode.isSideAll()));
		if (di.diffcode.isDirectory() && pCtxt->m_bRecursive)
		{
			di.diffcode.diffcode &= ~(DIFFCODE::DIFF | DIFFCODE::SAME);
			int ndiff = DirScan_CompareRequestedItems(myStruct, curpos);
			if (ndiff > 0)
			{
				if (existsalldirs)
					di.diffcode.diffcode |= DIFFCODE::DIFF;
				res += ndiff;
			}
			else if (ndiff == 0)
			{
				if (existsalldirs)
					di.diffcode.diffcode |= DIFFCODE::SAME;
			}		
		}
		else
		{
			if (di.diffcode.isScanNeeded())
			{
				BOOL bItemsExist = TRUE;
				UpdateDiffItem(di, bItemsExist, pCtxt);
				if (bItemsExist)
				{
					CompareDiffItem(di, pCtxt);
				}
			}
		}
		if (di.diffcode.isResultDiff() ||
			(!existsalldirs && !di.diffcode.isResultFiltered()))
			res++;
	}
	return res;
}

/**
 * @brief Update diffitem file/dir infos.
 *
 * Re-tests dirs/files if sides still exists, and updates infos for
 * existing sides. This assumes filenames, or paths are not changed.
 * Since in normal situations (I can think of) they cannot change
 * after first compare.
 *
 * @param [in,out] di DiffItem to update.
 * @param [out] bExists Set to
 *  - TRUE if one of items exists so diffitem is valid
 *  - FALSE if items were deleted, so diffitem is not valid
 * @param [in] pCtxt Compare context
 */
void UpdateDiffItem(DIFFITEM & di, BOOL & bExists, CDiffContext *pCtxt)
{
	// Clear side-info and file-infos
	di.diffFileInfo[0].ClearPartial();
	di.diffFileInfo[1].ClearPartial();
	di.diffcode.setSideBoth(); // FIXME: DIRTY HACK for UpdateInfoFromDiskHalf
	BOOL bLeftExists = pCtxt->UpdateInfoFromDiskHalf(di, 0);
	BOOL bRightExists = pCtxt->UpdateInfoFromDiskHalf(di, 1);
	bExists = bLeftExists || bRightExists;
	if (bLeftExists)
	{
		if (bRightExists)
			di.diffcode.setSideBoth();
		else
			di.diffcode.setSideFirstOnly();
	}
	else
	{
		if (bRightExists)
			di.diffcode.setSideSecondOnly();
		else
			di.diffcode.setSideNone();
	}
}

/**
 * @brief Compare two diffitems and add results to difflist in context.
 *
 * This function does the actual compare for previously gathered list of
 * items. Basically we:
 * - ignore items matching filefilters
 * - add non-ignored directories (no compare for directory items)
 * - add  unique files
 * - compare files
 *
 * @param [in] di DiffItem to compare
 * @param [in,out] pCtxt Compare context: contains difflist, encoding info etc.
 * @todo For date compare, maybe we should use creation date if modification
 * date is missing?
 */
void CompareDiffItem(DIFFITEM &di, CDiffContext * pCtxt)
{
	int nDirs = pCtxt->GetCompareDirs();
	// Clear rescan-request flag (not set by all codepaths)
	di.diffcode.diffcode &= ~DIFFCODE::NEEDSCAN;
	// Is it a directory?
	if (di.diffcode.isDirectory())
	{
		// 1. Test against filters
		if (
			(nDirs == 2 && pCtxt->m_piFilterGlobal->includeDir(di.diffFileInfo[0].filename.c_str(), di.diffFileInfo[1].filename.c_str())) ||
			(nDirs == 3 && pCtxt->m_piFilterGlobal->includeDir(di.diffFileInfo[0].filename.c_str(), di.diffFileInfo[1].filename.c_str(), di.diffFileInfo[2].filename.c_str()))
			)
			di.diffcode.diffcode |= DIFFCODE::INCLUDED;
		else
			di.diffcode.diffcode |= DIFFCODE::SKIPPED;
		// We don't actually 'compare' directories, just add non-ignored
		// directories to list.
		StoreDiffData(di, pCtxt, NULL);
	}
	else
	{
		// 1. Test against filters
		if (
			(nDirs == 2 && pCtxt->m_piFilterGlobal->includeFile(di.diffFileInfo[0].filename.c_str(), di.diffFileInfo[1].filename.c_str())) ||
			(nDirs == 3 && pCtxt->m_piFilterGlobal->includeFile(di.diffFileInfo[0].filename.c_str(), di.diffFileInfo[1].filename.c_str(), di.diffFileInfo[2].filename.c_str()))
			)
		{
			di.diffcode.diffcode |= DIFFCODE::INCLUDED;
			// 2. Add unique files
			// We must compare unique files to itself to detect encoding
			if (di.diffcode.isSideFirstOnly() || di.diffcode.isSideSecondOnly() || (nDirs > 2 && di.diffcode.isSideThirdOnly()))
			{
				if (pCtxt->m_nCurrentCompMethod != CMP_DATE &&
					pCtxt->m_nCurrentCompMethod != CMP_DATE_SIZE &&
					pCtxt->m_nCurrentCompMethod != CMP_SIZE)
				{
					FolderCmp folderCmp;
					UINT diffCode = folderCmp.prepAndCompareFiles(pCtxt, di);
					
					// Add possible binary flag for unique items
					if (diffCode & DIFFCODE::BIN)
						di.diffcode.diffcode |= DIFFCODE::BIN;
					StoreDiffData(di, pCtxt, &folderCmp);
				}
				else
				{
					StoreDiffData(di, pCtxt, NULL);
				}
			}
			// 3. Compare two files
			else
			{
				// Really compare
				FolderCmp folderCmp;
				di.diffcode.diffcode |= folderCmp.prepAndCompareFiles(pCtxt, di);
				StoreDiffData(di, pCtxt, &folderCmp);
			}
		}
		else
		{
			di.diffcode.diffcode |= DIFFCODE::SKIPPED;
			StoreDiffData(di, pCtxt, NULL);
		}
	}
}

/**
 * @brief Send one file or directory result back through the diff context.
 * @param [in] di Data to store.
 * @param [in] pCtxt Compare context.
 * @param [in] pCmpData Folder compare data.
 */
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const FolderCmp * pCmpData)
{
	if (pCmpData)
	{
		// Set text statistics
		if (di.diffcode.isSideLeftOnlyOrBoth())
			di.diffFileInfo[0].m_textStats = pCmpData->m_diffFileData.m_textStats[0];
		if (di.diffcode.isSideRightOnlyOrBoth())
			di.diffFileInfo[1].m_textStats = pCmpData->m_diffFileData.m_textStats[1];

		di.nsdiffs = pCmpData->m_ndiffs;
		di.nidiffs = pCmpData->m_ntrivialdiffs;

		if (!di.diffcode.isSideFirstOnly())
		{
			di.diffFileInfo[1].encoding = pCmpData->m_diffFileData.m_FileLocation[1].encoding;
		}
		
		if (!di.diffcode.isSideSecondOnly())
		{
			di.diffFileInfo[0].encoding = pCmpData->m_diffFileData.m_FileLocation[0].encoding;
		}
	}

	GetLog()->Write
	(
		CLogFile::LCOMPAREDATA, _T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d"),
		di.diffFileInfo[0].filename.c_str(), (LPCTSTR)_T("di.diffFileInfo[0].spath"), (LPCTSTR)_T("di.diffFileInfo[1].spath"), di.diffcode
	);
	pCtxt->m_pCompareStats->AddItem(di.diffcode.diffcode);
	//pCtxt->AddDiff(di);
}

/**
 * @brief Add one compare item to list.
 * @param [in] sLeftDir Left subdirectory.
 * @param [in] sRightDir Right subdirectory.
 * @param [in] lent Left item data to add.
 * @param [in] rent Right item data to add.
 * @param [in] pCtxt Compare context.
 * @param [in] parent Parent of item to be added
 */
static DIFFITEM *AddToList(const LPCTSTR sLeftDir, const LPCTSTR sRightDir,
	const DirItem * lent, const DirItem * rent,
	UINT code, DiffFuncStruct *myStruct, DIFFITEM *parent)
{
	return AddToList(sLeftDir, sRightDir, sLeftDir, lent, rent, 0, code, myStruct, parent);
}

/**
 * @brief Add one compare item to list.
 */
static DIFFITEM *AddToList(LPCTSTR sLeftDir, LPCTSTR sMiddleDir, LPCTSTR sRightDir,
	const DirItem * lent, const DirItem * ment, const DirItem * rent,
	UINT code, DiffFuncStruct *myStruct, DIFFITEM *parent)
{
	// We must store both paths - we cannot get paths later
	// and we need unique item paths for example when items
	// change to identical
	DIFFITEM *di = myStruct->context->AddDiff(parent);

	di->parent = parent;
	di->diffFileInfo[0].path = sLeftDir;
	di->diffFileInfo[1].path = sMiddleDir;
	di->diffFileInfo[2].path = sRightDir;

	if (lent)
	{
		di->diffFileInfo[0].filename = lent->filename.c_str();
		di->diffFileInfo[0].mtime = lent->mtime;
		di->diffFileInfo[0].ctime = lent->ctime;
		di->diffFileInfo[0].size = lent->size;
		di->diffFileInfo[0].flags.attributes = lent->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyRightToLeft()
		if (rent)
			di->diffFileInfo[0].filename = rent->filename.c_str();
		else
			di->diffFileInfo[0].filename = ment->filename.c_str();
	}

	if (ment)
	{
		di->diffFileInfo[1].filename = OPTIMIZE_SHARE_CSTRINGDATA
		(
			ment && di->diffFileInfo[0].filename == ment->filename.c_str() ? di->diffFileInfo[0].filename :
		) ment->filename.c_str();
		di->diffFileInfo[1].mtime = ment->mtime;
		di->diffFileInfo[1].ctime = ment->ctime;
		di->diffFileInfo[1].size = ment->size;
		di->diffFileInfo[1].flags.attributes = ment->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyLeftToRight()
		if (lent)
			di->diffFileInfo[1].filename = lent->filename.c_str();
		else
			di->diffFileInfo[1].filename = rent->filename.c_str();
	}

	if (rent)
	{
		di->diffFileInfo[2].filename = OPTIMIZE_SHARE_CSTRINGDATA
		(
			lent && di->diffFileInfo[0].filename == lent->filename.c_str() ? di->diffFileInfo[0].filename :
		) rent->filename.c_str();
		di->diffFileInfo[2].mtime = rent->mtime;
		di->diffFileInfo[2].ctime = rent->ctime;
		di->diffFileInfo[2].size = rent->size;
		di->diffFileInfo[2].flags.attributes = rent->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyLeftToRight()
		if (lent)
			di->diffFileInfo[2].filename = lent->filename.c_str();
		else
			di->diffFileInfo[2].filename = ment->filename.c_str();
	}

	di->diffcode = code;

	GetLog()->Write
	(
		CLogFile::LCOMPAREDATA, _T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d"),
		di->diffFileInfo[0].filename.c_str(), (LPCTSTR)_T("di->diffFileInfo[0].spath"), (LPCTSTR)_T("di->diffFileInfo[1].spath"), code
	);
	myStruct->context->m_pCompareStats->IncreaseTotalItems();
	ReleaseSemaphore(myStruct->hSemaphore, 1, 0);
	return di;
}

void // static
DirScan_InitializeDefaultCodepage()
{
	// Set thread default codepage
	// This is application-wide initialization
	// but neither MainFrame nor MergeApp included the needed headers
	DiffFileData::SetDefaultCodepage(getDefaultCodepage());
}

