/**
 *  @file DirScan.cpp
 *
 *  @brief Implementation of DirScan (q.v.) and helper functions
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "DirScan.h"
#include "DiffContext.h"
#include "logfile.h"
#include "paths.h"
#include "FileTransform.h"
#include "mainfrm.h"

extern bool just_compare_files (LPCTSTR filepath1, LPCTSTR filepath2, int depth, bool * diff, bool * bin, int * ndiffs, int *ntrivialdiffs);
extern CLogFile gLog;

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
			    int code, CDiffContext * pCtxt, int ndiffs=-1, int ntrivialdiffs=-1);
static int prepAndCompareTwoFiles(const CString & filepath1, const CString & filepath2, int * ndiffs, int * ntrivialdiffs);

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
	CString sLeftDir = pCtxt->m_strNormalizedLeft;
	CString sRightDir = pCtxt->m_strNormalizedRight;
	CString subprefix;
	if (!subdir.IsEmpty())
	{
		sLeftDir += _T("\\") + subdir;
		sRightDir += _T("\\") + subdir;
		subprefix = subdir + _T("\\");
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
			TRACE(_T("Candidate left: leftDirs[i]=%s\n"), leftDirs[i]);
		if (j<rightDirs.GetSize())
			TRACE(_T("Candidate right: rightDirs[j]=%s\n"), rightDirs[j]);
		if (i<leftDirs.GetSize() && (j==rightDirs.GetSize() || collstr(leftDirs[i].name, rightDirs[j].name, casesensitive)<0))
		{
			int nDiffCode = DIFFCODE::LEFT + DIFFCODE::DIR;

			// Test against filter
			CString newsub = subprefix + leftDirs[i].name;
			if (!pCtxt->m_piFilterUI->includeDir(newsub) || !pCtxt->m_piFilterGlobal->includeDir(newsub))
				nDiffCode |= DIFFCODE::SKIPPED;

			// Advance left pointer over left-only entry, and then retest with new pointers
			StoreDiffResult(subdir, &leftDirs[i], 0, nDiffCode, pCtxt);
			++i;
			continue;
		}
		if (j<rightDirs.GetSize() && (i==leftDirs.GetSize() || collstr(leftDirs[i].name, rightDirs[j].name, casesensitive)>0))
		{
			int nDiffCode = DIFFCODE::RIGHT + DIFFCODE::DIR;

			// Test against filter
			CString newsub = subprefix + rightDirs[j].name;
			if (!pCtxt->m_piFilterUI->includeDir(newsub) || !pCtxt->m_piFilterGlobal->includeDir(newsub))
				nDiffCode |= DIFFCODE::SKIPPED;

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
				StoreDiffResult(subdir, &leftDirs[i], &rightDirs[j], DIFFCODE::SKIPPED+DIFFCODE::DIR, pCtxt);
			}
			else
			{
				// If non-recursive compare, add folders appearing both sides
				if (!depth)
					StoreDiffResult(subdir, &leftDirs[i], &rightDirs[j], DIFFCODE::DIR, pCtxt);

				if (DirScan(newsub, pCtxt, casesensitive, depth-1, piAbortable) == -1)
					return -1;
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
			TRACE(_T("Candidate left: leftFiles[i]=%s\n"), leftFiles[i]);
		if (j<rightFiles.GetSize())
			TRACE(_T("Candidate right: rightFiles[j]=%s\n"), rightFiles[j]);
		if (i<leftFiles.GetSize() && (j==rightFiles.GetSize() || collstr(leftFiles[i].name, rightFiles[j].name, casesensitive)<0))
		{
			int nDiffCode = DIFFCODE::LEFT + DIFFCODE::FILE;

			// Test against filter
			CString newsubfile = subprefix + leftFiles[i].name;
			if (!pCtxt->m_piFilterUI->includeFile(newsubfile) || !pCtxt->m_piFilterGlobal->includeFile(newsubfile))
				nDiffCode |= DIFFCODE::SKIPPED;

			// Advance left pointer over left-only entry, and then retest with new pointers
			StoreDiffResult(subdir, &leftFiles[i], 0, nDiffCode, pCtxt);
			++i;
			continue;
		}
		if (j<rightFiles.GetSize() && (i==leftFiles.GetSize() || collstr(leftFiles[i].name, rightFiles[j].name, casesensitive)>0))
		{
			int nDiffCode = DIFFCODE::RIGHT + DIFFCODE::FILE;

			// Test against filter
			CString newsubfile = subprefix + rightFiles[j].name;
			if (!pCtxt->m_piFilterUI->includeFile(newsubfile) || !pCtxt->m_piFilterGlobal->includeFile(newsubfile))
				nDiffCode |= DIFFCODE::SKIPPED;

			// Advance right pointer over right-only entry, and then retest with new pointers
			StoreDiffResult(subdir, 0, &rightFiles[j], nDiffCode, pCtxt);
			++j;
			continue;
		}
		if (i<leftFiles.GetSize())
		{
			ASSERT(j<rightFiles.GetSize());
			CString newsubfile = subprefix + leftFiles[i].name;
			if (!pCtxt->m_piFilterUI->includeFile(newsubfile) || !pCtxt->m_piFilterGlobal->includeFile(newsubfile))
			{
				StoreDiffResult(subdir, &leftFiles[i], &rightFiles[j], DIFFCODE::SKIPPED+DIFFCODE::FILE, pCtxt);
			}
			else
			{
				CString leftname = leftFiles[i].name;
				CString rightname = rightFiles[j].name;

				gLog.Write(_T("Comparing: n0=%s, n1=%s, d0=%s, d1=%s")
					, leftname, rightname, sLeftDir, sRightDir);

				// Files to compare
				CString filepath1 = paths_ConcatPath(sLeftDir, leftname);
				CString filepath2 = paths_ConcatPath(sRightDir, rightname);


				int code = 0;
				int ndiffs=0, ntrivialdiffs=0;
				if (mf->m_nCompMethod == 1)
				{
					// Compare only by modified date
					// TODO: Set ndiffs & ntrivialdiffs to -1 & handle in display
					if (leftFiles[i].mtime != rightFiles[j].mtime)
						code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::DIFF;
					else
						code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::SAME;
				}
				else
				{
					// Really compare
					code = prepAndCompareTwoFiles(filepath1, filepath2, &ndiffs, &ntrivialdiffs);
				}

				// report result back to caller
				StoreDiffResult(subdir, &leftFiles[i], &rightFiles[j], code, pCtxt, ndiffs, ntrivialdiffs);
			}
			++i;
			++j;
			continue;
		}
		break;
	}
	return 1;
}

/**
 * @brief Invoke appropriate plugins for unpacking and prediffing
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
static bool UnpackAndTransform(const CString & filepath, CString & filepathTransformed,
	const CString & filteredFilenames, PackingInfo * infoUnpacker, PackingInfo * infoPrediffer, bool toUtf8)
{
	BOOL bMayOverwrite = FALSE; // temp variable set each time it is used

	// first step : unpack (plugins)
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(filepathTransformed, filteredFilenames, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}
	else
	{
		if (!FileTransform_Unpacking(filepathTransformed, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}

	if (toUtf8)
	{
		// second step : normalize Unicode to OLECHAR (most of time, do nothing) (OLECHAR = UCS-2LE in Windows)
		bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
		if (!FileTransform_NormalizeUnicode(filepathTransformed, bMayOverwrite))
			return false;
	}

	// Note: filepathTransformed may be in UCS-2 (if toUtf8), or it may be raw encoding (if !Utf8)
	// prediff plugins must handle both

	// third step : prediff (plugins)
	bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
	if (!FileTransform_Prediffing(filepathTransformed, filteredFilenames, infoPrediffer, bMayOverwrite))
		return false;

	if (toUtf8)
	{
		// fourth step : prepare for diffing
		bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
		if (!FileTransform_UCS2ToUTF8(filepathTransformed, bMayOverwrite))
			return false;
	}

	return true;
}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode
 */
static int
prepAndCompareTwoFiles(const CString & filepath1, const CString & filepath2, int * ndiffs, int * ntrivialdiffs)
{
	// compareok equals true as long as everything is fine
	BOOL compareok = TRUE;

	// these hold results of comparison, for constructing return code
	bool diff=false, bin=false;

	// For user chosen plugins, define bAutomaticUnpacker as false and use the chosen infoHandler
	// but how can we receive the infoHandler ? DirScan actually only 
	// returns info, but can not use file dependent information.

	// Transformation happens here
	// text used for automatic mode : plugin filter must match it
	CString filteredFilenames = filepath1 + "|" + filepath2;
	// Use temporary plugins info
	PackingInfo infoUnpacker(m_bUnpackerMode);
	PackingInfo infoPrediffer(m_bPredifferMode);

	// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
	CString filepathTransformed1 = filepath1;
	CString filepathTransformed2 = filepath2;

	// TODO: 2004-02-03
	// Call DiffContext::GuessEncoding to get encodings of both files
	// and skip the UTF-8 part of file transformations if
	// - encodings are the same and not unicode
	// or
	// - encodings are the same and are UTF-8
	bool toUtf8 = true; // We will adjust this after we call GuessEncoding & get encoding info

	// Invoke unpacking & prediff'ing plugins
	if (!UnpackAndTransform(filepath1, filepathTransformed1, filteredFilenames, &infoUnpacker, &infoPrediffer, toUtf8))
		goto exitPrepAndCompare;

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(infoUnpacker.bToBeScanned == FALSE);
	ASSERT(infoPrediffer.bToBeScanned == FALSE);

	// Invoke unpacking & prediff'ing plugins
	if (!UnpackAndTransform(filepath2, filepathTransformed2, filteredFilenames, &infoUnpacker, &infoPrediffer, toUtf8))
		goto exitPrepAndCompare;

	// If options are binary equivalent, we could check for filesize
	// difference here, and bail out if files are clearly different
	// But, then we don't know if file is ascii or binary, and this
	// affects behavior (also, we don't have an icon for unknown type)

	// Actually compare the files
	// just_compare_files is a fairly thin front-end to diffutils
	compareok = just_compare_files(filepathTransformed1, filepathTransformed2, 
		0, &diff, &bin, ndiffs, ntrivialdiffs);

exitPrepAndCompare:
	// delete the temp files after comparison
	if (filepathTransformed1 != filepath1)
		::DeleteFile(filepathTransformed1);
	if (filepathTransformed2 != filepath2)
		::DeleteFile(filepathTransformed2);

	// assemble bit flags for result code
	int code = DIFFCODE::FILE;
	if (!compareok)
	{
		code |= DIFFCODE::CMPERR;
	}
	else
	{
		code |= (diff ? DIFFCODE::DIFF : DIFFCODE::SAME);
		code |= (bin ? DIFFCODE::BIN : DIFFCODE::TEXT);
	}
	return code;
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
	if (!sPattern.IsEmpty() && sPattern[sPattern.GetLength()-1]!='\\')
		sPattern += '\\';
	sPattern += _T("\\*.*");
	CFileFind finder;
	BOOL bWorking = finder.FindFile(sPattern);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		fentry ent;
		CTime mtim;
		finder.GetLastWriteTime(mtim);
		CTime ctim;
		finder.GetCreationTime(ctim);
		ent.mtime = mtim.GetTime();
		ent.ctime = ctim.GetTime();
#if _MSC_VER < 1300
		// MSVC6
		ent.size = finder.GetLength64(); // __int64
#else
		// MSVC7 (VC.NET)
		ent.size = finder.GetLength(); // ULONGLONG
#endif
		ent.name = finder.GetFileName();
		ent.attrs = 0;
		if (finder.IsDirectory())
			dirs->Add(ent);
		else
			files->Add(ent);
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
			    int code, CDiffContext * pCtxt, int ndiffs, int ntrivialdiffs)
{
	CString name, leftdir, rightdir;
	_int64 rmtime=0, lmtime=0, rctime=0, lctime=0;
	_int64 lsize=0, rsize=0;
	int lattrs=0, rattrs=0;

	// We must store both paths - we cannot get paths later
	// and we need unique item paths for example when items
	// change to identical
	leftdir = paths_ConcatPath(pCtxt->m_strNormalizedLeft, sDir);
	rightdir = paths_ConcatPath(pCtxt->m_strNormalizedRight, sDir);

	if (lent)
	{
		lmtime = lent->mtime;
		lctime = lent->ctime;
		lsize = lent->size;
		name = lent->name;
		lattrs = lent->attrs;
	}
	if (rent)
	{
		rmtime = rent->mtime;
		rctime = rent->ctime;
		rsize = rent->size;
		name = rent->name;
		rattrs = rent->attrs;
	}
	gLog.Write(LOGLEVEL::LCOMPAREDATA,_T("name=<%s>, leftdir=<%s>, rightdir=<%s>, code=%d")
		, (LPCTSTR)name, (LPCTSTR)leftdir, (LPCTSTR)rightdir, code);
	pCtxt->AddDiff(name, sDir, leftdir, rightdir
		, lmtime, rmtime, lctime, rctime, lsize, rsize, code, lattrs, rattrs
		, ndiffs, ntrivialdiffs);
}
