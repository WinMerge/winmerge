/**
 *  @file DirScan.cpp
 *
 *  @brief Implementation of DirScan (q.v.) and helper functions
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include <shlwapi.h>
#include "DirScan.h"
#include "common/unicoder.h"
#include "DiffContext.h"
#include "DiffWrapper.h"
#include "logfile.h"
#include "paths.h"
#include "FileTransform.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Static types (ie, types only used locally)
/**
 * @brief directory or file info for one row in diff result
 */
struct fentry
{
	CString name;
	// storing __time_t if MSVC6 (__MSC_VER<1300)
	// storing __time64_t if MSVC7 (VC.NET)
	__int64 mtime;
	__int64 ctime;
	_int64 size;
	int attrs;
};
typedef CArray<fentry, fentry&> fentryArray;

// Static functions (ie, functions only used locally)
static void LoadFiles(const CString & sDir, fentryArray * dirs, fentryArray * files);
void LoadAndSortFiles(const CString & sDir, fentryArray * dirs, fentryArray * files, bool casesensitive);
static void Sort(fentryArray * dirs, bool casesensitive);;
static int collstr(const CString & s1, const CString & s2, bool casesensitive);
static void StoreDiffResult(const CString & sDir, const fentry * lent, const fentry *rent, 
			    int code, CDiffContext *, const DiffFileData * = 0);

/** @brief cmpmth is a typedef for a pointer to a method */
typedef int (CString::*cmpmth)(LPCTSTR sz) const;
/** @brief CALL_MEMBER_FN calls a method through a pointer to a method */
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))


/**
 * @brief Compare two directories and store results (via the context)
 *
 * Compare two directories & output all results found via calls to StoreDiffResult
 * base directories to compare are in the CDiffContext
 * and this call is responsible for diff'ing just the subdir specified
 * (ie, if subdir is empty, this is the base call)
 * return 1 normally, return -1 if aborting
 */
int DirScan(const CString & subdir, CDiffContext * pCtxt, bool casesensitive,
	int depth, IAbortable * piAbortable)
{
	//int (WINAPI *collstr)(LPCTSTR, LPCTSTR) = casesensitive ? lstrcmp : lstrcmpi;
	static const TCHAR backslash[] = _T("\\");
	CString sLeftDir = pCtxt->m_strNormalizedLeft;
	CString sRightDir = pCtxt->m_strNormalizedRight;
	CString subprefix;
	if (!subdir.IsEmpty())
	{
		sLeftDir += backslash + subdir;
		sRightDir += backslash + subdir;
		subprefix = subdir + backslash;
	}

	fentryArray leftDirs, leftFiles, rightDirs, rightFiles;
	LoadAndSortFiles(sLeftDir, &leftDirs, &leftFiles, casesensitive);
	LoadAndSortFiles(sRightDir, &rightDirs, &rightFiles, casesensitive);

	if (piAbortable && piAbortable->ShouldAbort()) return -1;

	// Handle directories
	// i points to current directory in left list (leftDirs)
	// j points to current directory in right list (rightDirs)
	int i=0, j=0;
	while (1)
	{
		if (piAbortable && piAbortable->ShouldAbort()) return -1;

		// In debug mode, send current status to debug window
		if (i<leftDirs.GetSize())
			TRACE(_T("Candidate left: leftDirs[i]=%s\n"), (LPCTSTR)leftDirs[i].name);
		if (j<rightDirs.GetSize())
			TRACE(_T("Candidate right: rightDirs[j]=%s\n"), (LPCTSTR)rightDirs[j].name);
		if (i<leftDirs.GetSize() && (j==rightDirs.GetSize() || collstr(leftDirs[i].name, rightDirs[j].name, casesensitive)<0))
		{
			int nDiffCode = DIFFCODE::LEFT | DIFFCODE::DIR;

			// Test against filter
			CString newsub = subprefix + leftDirs[i].name;
			if (!pCtxt->m_piFilterUI->includeDir(newsub) || !pCtxt->m_piFilterGlobal->includeDir(newsub))
				nDiffCode = DIFFCODE::LEFT | DIFFCODE::DIR | DIFFCODE::SKIPPED;

			// Advance left pointer over left-only entry, and then retest with new pointers
			StoreDiffResult(subdir, &leftDirs[i], 0, nDiffCode, pCtxt);
			++i;
			continue;
		}
		if (j<rightDirs.GetSize() && (i==leftDirs.GetSize() || collstr(leftDirs[i].name, rightDirs[j].name, casesensitive)>0))
		{
			int nDiffCode = DIFFCODE::RIGHT | DIFFCODE::DIR;

			// Test against filter
			CString newsub = subprefix + rightDirs[j].name;
			if (!pCtxt->m_piFilterUI->includeDir(newsub) || !pCtxt->m_piFilterGlobal->includeDir(newsub))
				nDiffCode = DIFFCODE::RIGHT | DIFFCODE::DIR | DIFFCODE::SKIPPED;

			// Advance right pointer over right-only entry, and then retest with new pointers
			StoreDiffResult(subdir, 0, &rightDirs[j], nDiffCode, pCtxt);
			++j;
			continue;
		}
		if (i<leftDirs.GetSize())
		{
			ASSERT(j<rightDirs.GetSize());
			CString newsub = subprefix + leftDirs[i].name;
			if (!pCtxt->m_piFilterUI->includeDir(newsub) || !pCtxt->m_piFilterGlobal->includeDir(newsub))
			{
				int nDiffCode = DIFFCODE::BOTH | DIFFCODE::SKIPPED | DIFFCODE::DIR;
				StoreDiffResult(subdir, &leftDirs[i], &rightDirs[j], nDiffCode, pCtxt);
			}
			else
			{
				if (!depth)
				{
					// Non-recursive compare
					// We are only interested about list of subdirectories to show - user can open them
					// TODO: scan one level deeper to see if directories are identical/different
					int nDiffCode = DIFFCODE::BOTH | DIFFCODE::DIR;
					StoreDiffResult(subdir, &leftDirs[i], &rightDirs[j], nDiffCode, pCtxt);
				}
				else
				{
					// Recursive compare
					// Scan recursively all subdirectories too, we are not adding folders
					if (DirScan(newsub, pCtxt, casesensitive, depth-1, piAbortable) == -1)
						return -1;
				}
			}
			++i;
			++j;
			continue;
		}
		break;
	}
	// Handle files
	// i points to current file in left list (leftFiles)
	// j points to current file in right list (rightFiles)
	i=0, j=0;
	while (1)
	{
		if (piAbortable && piAbortable->ShouldAbort()) return -1;

		// In debug mode, send current status to debug window
		if (i<leftFiles.GetSize())
			TRACE(_T("Candidate left: leftFiles[i]=%s\n"), (LPCTSTR)leftFiles[i].name);
		if (j<rightFiles.GetSize())
			TRACE(_T("Candidate right: rightFiles[j]=%s\n"), (LPCTSTR)rightFiles[j].name);
		if (i<leftFiles.GetSize() && (j==rightFiles.GetSize() || collstr(leftFiles[i].name, rightFiles[j].name, casesensitive)<0))
		{
			// Test against filter
			CString newsubfile = subprefix + leftFiles[i].name;
			int nDiffCode = DIFFCODE::LEFT | DIFFCODE::FILE;
			if (!pCtxt->m_piFilterUI->includeFile(newsubfile) || !pCtxt->m_piFilterGlobal->includeFile(newsubfile))
			{
				nDiffCode |= DIFFCODE::SKIPPED;
				StoreDiffResult(subdir, &leftFiles[i], 0, nDiffCode, pCtxt);
			}
			else if (mf->m_nCompMethod != 1)
			{
				// Compare file to itself to detect encoding
				CString filepath = sLeftDir + backslash + leftFiles[i].name;
				DiffFileData diffdata;
				diffdata.prepAndCompareTwoFiles(pCtxt, filepath, filepath);
				StoreDiffResult(subdir, &leftFiles[i], 0, nDiffCode, pCtxt, &diffdata);
			}
			else
			{
				StoreDiffResult(subdir, &leftFiles[i], 0, nDiffCode, pCtxt);
			}
			// Advance left pointer over left-only entry, and then retest with new pointers
			++i;
			continue;
		}
		if (j<rightFiles.GetSize() && (i==leftFiles.GetSize() || collstr(leftFiles[i].name, rightFiles[j].name, casesensitive)>0))
		{
			// Test against filter
			CString newsubfile = subprefix + rightFiles[j].name;
			int nDiffCode = DIFFCODE::RIGHT | DIFFCODE::FILE;
			if (!pCtxt->m_piFilterUI->includeFile(newsubfile) || !pCtxt->m_piFilterGlobal->includeFile(newsubfile))
			{
				nDiffCode |= DIFFCODE::SKIPPED;
				StoreDiffResult(subdir, 0, &rightFiles[j], nDiffCode, pCtxt);
			}
			else if (mf->m_nCompMethod != 1)
			{
				// Compare file to itself to detect encoding
				CString filepath = sRightDir + backslash + rightFiles[j].name;
				DiffFileData diffdata;
				diffdata.prepAndCompareTwoFiles(pCtxt, filepath, filepath);
				StoreDiffResult(subdir, 0, &rightFiles[j], nDiffCode, pCtxt, &diffdata);
			}
			else
			{
				StoreDiffResult(subdir, 0, &rightFiles[j], nDiffCode, pCtxt);
			}
			// Advance right pointer over right-only entry, and then retest with new pointers
			++j;
			continue;
		}
		if (i<leftFiles.GetSize())
		{
			ASSERT(j<rightFiles.GetSize());
			CString newsubfile = subprefix + leftFiles[i].name;
			int nDiffCode = DIFFCODE::BOTH | DIFFCODE::FILE;
			if (!pCtxt->m_piFilterUI->includeFile(newsubfile) || !pCtxt->m_piFilterGlobal->includeFile(newsubfile))
			{
				nDiffCode |= DIFFCODE::SKIPPED;
				StoreDiffResult(subdir, &leftFiles[i], &rightFiles[j], nDiffCode, pCtxt);
				++i;
				++j;
				continue;
			}
			LPCTSTR leftname = leftFiles[i].name;
			LPCTSTR rightname = rightFiles[j].name;

			gLog.Write(_T("Comparing: n0=%s, n1=%s, d0=%s, d1=%s"), 
			  leftname, rightname, (LPCTSTR)sLeftDir, (LPCTSTR)sRightDir);

			if (mf->m_nCompMethod == 1)
			{
					// Compare only by modified date
				if (leftFiles[i].mtime == rightFiles[j].mtime)
					nDiffCode |= DIFFCODE::TEXT | DIFFCODE::SAME;
				else
					nDiffCode |= DIFFCODE::TEXT | DIFFCODE::DIFF;
				// report result back to caller
				StoreDiffResult(subdir, &leftFiles[i], &rightFiles[j], nDiffCode, pCtxt);
				++i;
				++j;
				continue;
			}
			// Files to compare
			CString filepath1 = sLeftDir + backslash + leftname;
			CString filepath2 = sRightDir + backslash + rightname;
					// Really compare
			DiffFileData diffdata;
			nDiffCode |= diffdata.prepAndCompareTwoFiles(pCtxt, filepath1, filepath2);
				// report result back to caller
			StoreDiffResult(subdir, &leftFiles[i], &rightFiles[j], nDiffCode, pCtxt, &diffdata);
			++i;
			++j;
			continue;
		}
		break;
	}
	return 1;
}

/**
 * @brief Load arrays with all directories & files in specified dir
 */
void LoadAndSortFiles(const CString & sDir, fentryArray * dirs, fentryArray * files, bool casesensitive)
{
	LoadFiles(sDir, dirs, files);
	Sort(dirs, casesensitive);
	Sort(files, casesensitive);
}

/**
 * @brief Load arrays with all directories & files in specified dir
 */
void LoadFiles(const CString & sDir, fentryArray * dirs, fentryArray * files)
{
	CString sPattern = sDir;
	sPattern.TrimRight(_T("\\"));
	sPattern += _T("\\*.*");

	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(sPattern, &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			DWORD dwIsDirectory = ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if (dwIsDirectory && StrStr(_T(".."), ff.cFileName))
			continue;
		fentry ent;
			ent.ctime = CTime(ff.ftCreationTime).GetTime();
			ent.mtime = CTime(ff.ftLastWriteTime).GetTime();
			ent.size = ff.nFileSizeLow + (ff.nFileSizeHigh << 32);
			ent.name = ff.cFileName;
			ent.attrs = ff.dwFileAttributes;
			(dwIsDirectory ? dirs : files) -> Add(ent);
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}
}

/**
 * @brief case-sensitive collate function for qsorting an array
 */
static int __cdecl cmpstring(const void *elem1, const void *elem2)
{
	const fentry * s1 = static_cast<const fentry *>(elem1);
	const fentry * s2 = static_cast<const fentry *>(elem2);
	return s1->name.Collate(s2->name);
}

/**
 * @brief case-insensitive collate function for qsorting an array
 */
static int __cdecl cmpistring(const void *elem1, const void *elem2)
{
	const fentry * s1 = static_cast<const fentry *>(elem1);
	const fentry * s2 = static_cast<const fentry *>(elem2);
	return s1->name.CollateNoCase(s2->name);
}

/**
 * @brief sort specified array
 */
void Sort(fentryArray * dirs, bool casesensitive)
{
	fentry * data = dirs->GetData();
	if (!data) return;
	int (__cdecl *comparefnc)(const void *elem1, const void *elem2) = (casesensitive ? cmpstring : cmpistring);
	qsort(data, dirs->GetSize(), sizeof(dirs->GetAt(0)), comparefnc);
}

/**
 * @brief  Compare (NLS aware) two strings, either case-sensitive or case-insensitive as caller specifies
 */
static int collstr(const CString & s1, const CString & s2, bool casesensitive)
{
	if (casesensitive)
		return s1.Collate(s2);
	else
		return s1.CollateNoCase(s2);
}

/**
 * @brief Send one file or directory result back through the diff context
 */
static void StoreDiffResult(const CString & sDir, const fentry * lent, const fentry * rent,
	int code, CDiffContext * pCtxt, const DiffFileData * pDiffFileData)
{
	// We must store both paths - we cannot get paths later
	// and we need unique item paths for example when items
	// change to identical

	DIFFITEM di;
	di.sSubdir = sDir;

	if (pDiffFileData)
	{
		di.nsdiffs = pDiffFileData->m_ndiffs - pDiffFileData->m_ntrivialdiffs;
		di.ndiffs = pDiffFileData->m_ndiffs;
		di.left.unicoding = pDiffFileData->m_sFilepath[0].unicoding;
		di.left.codepage = pDiffFileData->m_sFilepath[0].codepage;
		di.right.unicoding = pDiffFileData->m_sFilepath[1].unicoding;
		di.right.codepage = pDiffFileData->m_sFilepath[1].codepage;
	}

	if (lent)
	{
		di.sfilename = lent->name;
		//di.left.spath = pCtxt->m_strNormalizedLeft; //paths_ConcatPath(pCtxt->m_strNormalizedLeft, sDir);
		di.left.mtime = lent->mtime;
		di.left.ctime = lent->ctime;
		di.left.size = lent->size;
		di.left.flags.flags = lent->attrs;
	}
	else
	{
		di.left.unicoding = 0;
		di.left.codepage = 0;
	}

	if (rent)
	{
		di.sfilename = rent->name;
		//di.right.spath = pCtxt->m_strNormalizedRight; //paths_ConcatPath(pCtxt->m_strNormalizedRight, sDir);
		di.right.mtime = rent->mtime;
		di.right.ctime = rent->ctime;
		di.right.size = rent->size;
		di.right.flags.flags = rent->attrs;
	}
	else
	{
		di.right.unicoding = 0;
		di.right.codepage = 0;
	}

	di.diffcode = code;

	gLog.Write
	(
		LOGLEVEL::LCOMPAREDATA, _T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d"),
		(LPCTSTR)di.sfilename, (LPCTSTR)_T("di.left.spath"), (LPCTSTR)_T("di.right.spath"), code
	);
	pCtxt->AddDiff(di);
}
