/** @file MakeDirs.cpp 
 *
 *  @brief Implementation of MakeDirs class
 */ 

#include "stdafx.h"
#include "MakeDirs.h"
#include "resource.h"
#include "exc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/** @brief Entry point */
CString
MakeDirs::DoIt(const CString & rootDir, MakeDirFilter * pifilter)
{
	MakeDirs md(rootDir, pifilter);
	return md.Worker();
}

/** @brief Main code outline */
CString
MakeDirs::Worker()
{
	// normalize root dir
	m_rootDir.Replace('/', '\\');
	if (m_rootDir[m_rootDir.GetLength()-1] == '\\')
		m_rootDir = m_rootDir.Left(m_rootDir.GetLength()-1);

	LoadFiles(m_rootDir + _T("\\original_all"), &m_originalFiles);
	LoadFiles(m_rootDir + _T("\\applied"), &m_appliedFiles);

	RemoveDirIfExists(m_rootDir + _T("\\original"));
	RemoveDirIfExists(m_rootDir + _T("\\altered"));

	DiffArray diffs;
	SplitFiles(m_originalFiles, m_appliedFiles, & diffs);

	for (int i=0; i<diffs.Right.GetSize(); ++i)
	{
		CString src = m_rootDir + _T("\\applied\\") + diffs.Right[i];
		CString dest = m_rootDir + _T("\\altered\\") + diffs.Right[i];
		MyCopyFile(src, dest);
	}

	int newfiles = diffs.Right.GetSize();
	int changedfiles = 0;
	for (i=0; i<diffs.Shared.GetSize(); ++i)
	{
		CString oldf = m_rootDir + _T("\\original_all\\") + diffs.Shared[i];
		CString newf = m_rootDir + _T("\\applied\\") + diffs.Shared[i];
		if (FilesDiffer(oldf, newf))
		{
			CString src = newf;
			CString dest = m_rootDir + _T("\\altered\\") + diffs.Shared[i];
			MyCopyFile(src, dest);
			src = oldf;
			dest = m_rootDir + _T("\\original\\") + diffs.Shared[i];
			MyCopyFile(src, dest);
			++changedfiles;
		}
	}
	CString summary = MakeStr(LoadResString(IDS_SUMMARY2), changedfiles, newfiles);
	return summary;
}


/** @brief Split files into diff groups */
void
MakeDirs::SplitFiles(const CStringArray & left, const CStringArray & right, DiffArray * diffs)
{
	// caller must have sorted left & right !
	int l=0, r=0;
	while (1)
	{
		if (l<left.GetSize())
		{
			if (r<right.GetSize())
			{
				int n = left[l].CollateNoCase(right[r]);
				if (n < 0)
				{
					diffs->Left.Add(left[l]);
					++l;
				}
				else if (n > 0)
				{
					diffs->Right.Add(right[r]);
					++r;
				}
				else
				{
					diffs->Shared.Add(left[l]);
					++l;
					++r;
				}
			}
			else
			{
				diffs->Left.Add(left[l]);
				++l;
			}
		}
		else
		{
			if (r<right.GetSize())
			{
				diffs->Right.Add(right[r]);
				++r;
			}
			else
			{
				return;
			}
		}
	}
}

/** @brief Load files recursively into array & sort */
void
MakeDirs::LoadFiles(const CString & path, CStringArray * array)
{
	CheckDirectory(path);
	CString subdir;
	LoadFilesWorker(path, subdir, array);
	SortArray(array);
}

/** @brief Send array contents to debug window */
void
MakeDirs::TraceArray(const CStringArray & array)
{
#ifdef _DEBUG
	for (int i=0; i<array.GetSize(); ++i)
		TRACE(_T("%d: %s\n"), i, array[i]);
#endif
}

/** @brief Throw exception if directory not found */
void
MakeDirs::CheckDirectory(const CString & path)
{
	if (!DoesDirExist(path))
		Fail(MakeStr(LoadResString(IDS_DIR_NOT_FOUND1), path));
}

/** @brief Return true if directory exists */
bool
MakeDirs::DoesDirExist(const CString & path)
{
	CFileFind finder;
	return !!finder.FindFile(path + _T("\\*.*"));
}

/** @brief Throw an exception */
void
MakeDirs::Fail(LPCTSTR sz)
{
	perry::exc e;
	e.reportAndThrowError(sz);
}

/** @brief Load files recursively into array */
void
MakeDirs::LoadFilesWorker(const CString & path, const CString & subdir, CStringArray * array)
{
	CFileFind finder;
	CString findpath = path + _T("\\");
	if (subdir.IsEmpty())
		findpath += _T("*.*");
	else
		findpath += subdir + _T("\\*.*");
	BOOL bWorking = finder.FindFile(findpath);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		CString subfilepath = finder.GetFilePath().Mid(path.GetLength()+1);
		CString filename = finder.GetFileName();
		CString filebase = filename;
		CString ext;
		int ind = filename.ReverseFind('.');
		if (ind >= 0)
		{
			ext = filename.Mid(ind+1);
			filebase = filename.Left(ind);
		}
		bool dir = !!finder.IsDirectory();
		subfilepath = subfilepath.Left(subfilepath.GetLength() - filename.GetLength());
		if (!m_pifilter->handle(dir, subfilepath, filebase, ext))
			continue;
		if (finder.IsDirectory())
		{
			CString newsubdir;
			if (!subfilepath.IsEmpty())
				newsubdir = subfilepath + (CString)_T("\\");
			newsubdir += finder.GetFileName();
			LoadFilesWorker(path, newsubdir, array);
		}
		else
		{
			if (subdir.IsEmpty())
				continue;
			array->Add(subfilepath+filename);
		}
	}
}

static int __cdecl cmpstrarrents(const void * el1, const void * el2)
{
	const CString * str1 = reinterpret_cast<const CString *>(el1);
	const CString * str2 = reinterpret_cast<const CString *>(el2);
	return str1->CollateNoCase(*str2);
}

/** @brief Sort contents of array */
void
MakeDirs::SortArray(CStringArray * array)
{
	qsort(array->GetData(), array->GetSize(), sizeof(array->GetAt(0)), &cmpstrarrents);
}

/** @brief copy file, creating directories as needed */
void
MakeDirs::MyCopyFile(CString src, CString dest)
{
	int i = m_rootDir.GetLength()+1;
	while (1)
	{
		int n = dest.Find('\\', i+1);
		if (n<0)
			break;
		CString subdir = dest.Left(n);
		if (!DoesDirExist(subdir))
		{
			if (!CreateDirectory(subdir, 0))
				Fail(MakeStr(LoadResString(IDS_CREATE_DIR_FAILED1), subdir));
		}
		i = n+1;
	}
	if (!CopyFile(src, dest, TRUE))
		Fail(MakeStr(LoadResString(IDS_COPY_FILE_FAILED2), src, dest));
}

/** @brief Has file been changed (just use modification time) ? */
bool
MakeDirs::FilesDiffer(CString oldf, CString newf)
{
	CFileStatus olds, news;
	if (!CFile::GetStatus(oldf, olds))
		Fail(MakeStr(LoadResString(IDS_FILE_STATUS_ERROR1), oldf));
	if (!CFile::GetStatus(newf, news))
		Fail(MakeStr(LoadResString(IDS_FILE_STATUS_ERROR1), newf));
	return (olds.m_size != news.m_size || olds.m_mtime != news.m_mtime);
}

/** @brief Recursively delete directory (throw exception if error) */
void
MakeDirs::MyDeleteDir(LPCTSTR szDir)
{
	CFileFind finder;
	CString sSpec = szDir;
	sSpec += _T("\\*.*");
	if (finder.FindFile(sSpec))
	{
		BOOL done=FALSE;
		while (!done)
		{
			done = !finder.FindNextFile();
			if (finder.IsDots()) continue;
			if (finder.IsDirectory())
			{
				MyDeleteDir(finder.GetFilePath());
			}
			else
			{
				MyDeleteFile(finder.GetFilePath());
			}
		}
	}
	finder.Close(); // must close the handle or RemoveDirectory will fail
	if (!RemoveDirectory(szDir))
		Fail(MakeStr(LoadResString(IDS_REMOVEDIRECTORY_FAILED1), szDir));
}

/** @brief Delete file (throw exception if error) */
void
MakeDirs::MyDeleteFile(LPCTSTR szFile)
{
	if (!DeleteFile(szFile))
		Fail(MakeStr(LoadResString(IDS_DELETEFILE_FAILED1), szFile));
}

/** @brief Remove directory if it exists */
void
MakeDirs::RemoveDirIfExists(CString path)
{
	if (DoesDirExist(path))
		MyDeleteDir(path);
}
