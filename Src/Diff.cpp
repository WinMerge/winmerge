/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm

/* GNU DIFF main routine.
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

/* GNU DIFF was written by Mike Haertel, David Hayes,
   Richard Stallman, Len Tower, and Paul Eggert.  */

#include "stdafx.h"
#define GDIFF_MAIN
#include "diff.h" 
#include "getopt.h"
#include "fnmatch.h"
#include "io.h"

// reduce some noise produced with the MSVC compiler
#if defined (_AFXDLL)
#pragma warning(disable : 4706)
#endif



#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 130
#endif

#ifndef GUTTER_WIDTH_MINIMUM
#define GUTTER_WIDTH_MINIMUM 3
#endif



/* Nonzero for -r: if comparing two directories,
   compare their common subdirectories recursively.  */

int recursive;


/* Compare two specified files
   This is self-contained; it opens the files and closes them.

   Value is 0 if files are the same, 1 if different,
   2 if there is a problem opening them.  */

int
just_compare_files (LPCTSTR filepath1, LPCTSTR filepath2, int depth)
{
	struct file_data inf[2];

	ZeroMemory(&inf[0], sizeof(inf[0]));
	ZeroMemory(&inf[1], sizeof(inf[1]));

	inf[0].name = filepath1;
	inf[1].name = filepath2;

	inf[0].desc = open(inf[0].name, O_RDONLY|O_BINARY, 0);
	if (inf[0].desc < 0)
	{
		return 2;
	}
	inf[1].desc = open(inf[1].name, O_RDONLY|O_BINARY, 0);
	if (inf[1].desc < 0)
	{
		close(inf[0].desc);
		return 2;
	}

	int diff_flag=0;

	// Do the actual comparison (generating a change script)
	int val = 0;
	struct change *script = diff_2_files (inf, depth, &diff_flag);

	// Free change script (which we don't want)
	if (script != NULL)
	{
		struct change *p,*e;
		for (e = script; e; e = p)
		{
			p = e->link;
			free (e);
		}
		val = 1; /* different */
	}

	// Tell diff code to cleanup
	cleanup_file_buffers(inf);

	// close open file handles
	if (close(inf[0].desc) != 0)
	{
		val = 2;
	}
	if (close(inf[1].desc) != 0)
	{
		val = 2;
	}

	return val;
}


