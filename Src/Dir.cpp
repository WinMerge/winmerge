/* Read, sort and compare two directories.  Used for GNU DIFF.
Copyright (C) 1988, 1989, 1992, 1993 Free Software Foundation, Inc.

  This file is part of GNU DIFF.
  
	GNU DIFF is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.
	
	  GNU DIFF is distributed in the hope that it will be useful,
	  but WITHOUT ANY WARRANTY; without even the implied warranty of
	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	  GNU General Public License for more details.
	  
		You should have received a copy of the GNU General Public License
		along with GNU DIFF; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "stdafx.h"
#include "diff.h"
#include "RegExp.h"
#include "direct.h"
#include "DiffContext.h"

/* Read the directory named by DIR and store into DIRDATA a sorted vector
of filenames for its contents.  DIR->desc == -1 means this directory is
known to be nonexistent, so set DIRDATA to an empty vector.
Return -1 (setting errno) if error, 0 otherwise.  */

int compare_files (LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, CDiffContext*, int);

static int compare_names (void const *, void const *);
static int dir_sort (LPCTSTR pszDir, struct dirdata *dirdata, CRegExp& rgx);


static int
dir_sort (LPCTSTR pszDir, struct dirdata *dirdata, CRegExp& rgx)
{
	register int i;
	WIN32_FIND_DATA fd;
	HANDLE hff;
	CString s;
	
	/* Address of block containing the files that are described.  */
	char const **names;
	
	/* Number of files in directory.  */
	size_t nnames;
	
	/* Allocated and used storage for file name data.  */
	char *data;
	size_t data_alloc, data_used;
	
	dirdata->names = 0;
	dirdata->data = 0;
	nnames = 0;
	data = 0;
	
	/* Open the directory and check for errors.  */
	//register DIR *reading = opendir (pszDir);
	//if (!reading)
	//	return -1;
	s.Format(_T("%s\\*.*"), pszDir);
	if ((hff=FindFirstFile(s, &fd)) == INVALID_HANDLE_VALUE)
		return -1;
	
	/* Initialize the table of filenames.  */
	struct stat sb;
	stat(pszDir, &sb);
	
	data_alloc = max (1, (size_t) sb.st_size);
	data_used = 0;
	dirdata->data = data = (char*)xmalloc (data_alloc);
	
	/* Read the directory entries, and insert the subfiles
	into the `data' table.  */
	
	do
	{
		char *d_name = fd.cFileName;
		size_t d_size;
		
		/* Ignore the files `.' and `..' */
		if (d_name[0] == '.'
			&& (d_name[1] == 0 || (d_name[1] == '.' && d_name[2] == 0)))
			continue;
		
		if (excluded_filename (d_name)
			|| (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && rgx.RegFind(d_name)==-1))
			continue;
		
		d_size = strlen (d_name) + 1;
		while (data_alloc < data_used + d_size)
			dirdata->data = data = (char*)xrealloc (data, data_alloc *= 2);
		memcpy (data + data_used, d_name, d_size);
		data_used += d_size;
		nnames++;

	} while (FindNextFile(hff, &fd));

	FindClose(hff);

	
	/* Create the `names' table from the `data' table.  */
	dirdata->names = names = (char const **) xmalloc (sizeof (char *)
		* (nnames + 1));
	for (i = 0;  i < nnames;  i++)
    {
		names[i] = data;
		data += strlen (data) + 1;
    }
	names[nnames] = 0;
	
	/* Sort the table.  */
	qsort (names, nnames, sizeof (char *), compare_names);
	
	return 0;
}

/* Sort the files now in the table.  */

static int
compare_names (void const *file1, void const *file2)
{
	return stricmp (* (char const *const *) file1, * (char const *const *) file2);
}

/* Compare the contents of two directories named in FILEVEC[0] and FILEVEC[1].
This is a top-level routine; it does everything necessary for diff
on two directories.

  FILEVEC[0].desc == -1 says directory FILEVEC[0] doesn't exist,
  but pretend it is empty.  Likewise for FILEVEC[1].
  
	HANDLE_FILE is a caller-provided subroutine called to handle each file.
	It gets five operands: dir and name (rel to original working dir) of file
	in dir 0, dir and name pathname of file in dir 1, and the recursion depth.
	
	  For a file that appears in only one of the dirs, one of the name-args
	  to HANDLE_FILE is zero.
	  
		DEPTH is the current depth in recursion, used for skipping top-level
		files by the -S option.
		
		  Returns the maximum of all the values returned by HANDLE_FILE,
or 2 if trouble is encountered in opening files.  */

int diff_dirs (CDiffContext *pCtx, int depth)
{
	int val = 0;			/* Return value.  */

	/* Get sorted contents of both dirs.  */
	if (dir_sort (pCtx->m_strLeft, &pCtx->ddLeft, pCtx->m_rgx) != 0)
	{
		val = 2;
	}
	if (dir_sort (pCtx->m_strRight, &pCtx->ddRight, pCtx->m_rgx) != 0)
	{
		val = 2;
	}
		
	if (val == 0)
	{
		char const * const *names0 = pCtx->ddLeft.names;
		char const * const *names1 = pCtx->ddRight.names;
		char const *name0 = pCtx->m_strLeft;
		char const *name1 = pCtx->m_strRight;
		
		/* If `-S name' was given, and this is the topmost level of comparison,
		ignore all file names less than the specified starting name.  */
		
		if (dir_start_file && depth == 0)
		{
			while (*names0 && stricmp (*names0, dir_start_file) < 0)
				names0++;
			while (*names1 && stricmp (*names1, dir_start_file) < 0)
				names1++;
		}
		
		/* Loop while files remain in one or both dirs.  */
		while (*names0 || *names1)
		{
		/* Compare next name in dir 0 with next name in dir 1.
		At the end of a dir,
			pretend the "next name" in that dir is very large.  */
			int nameorder = (!*names0 ? 1 : !*names1 ? -1
				: stricmp (*names0, *names1));
			int v1 = compare_files(name0, 0 < nameorder ? 0 : *names0++,
				name1, nameorder < 0 ? 0 : *names1++,
				pCtx, depth+1);
			if (v1 > val)
				val = v1;
		}
	}
	
	if (pCtx->ddLeft.names)
		free (pCtx->ddLeft.names);
	if (pCtx->ddLeft.data)
		free (pCtx->ddLeft.data);
	if (pCtx->ddRight.names)
		free (pCtx->ddRight.names);
	if (pCtx->ddRight.data)
		free (pCtx->ddRight.data);

	return val;
}
