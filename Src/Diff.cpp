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
#include "DirDoc.h"
#include "logfile.h"

// reduce some noise produced with the MSVC compiler
#if defined (_AFXDLL)
#pragma warning(disable : 4706)
#endif


extern int diff_dirs (CDiffContext*, int);


#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 130
#endif

#ifndef GUTTER_WIDTH_MINIMUM
#define GUTTER_WIDTH_MINIMUM 3
#endif


extern CLogFile gLog;
extern bool gWriteLog;

int compare_files (LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, CDiffContext*, int);
int excluded_filename (char const *f);

/* Nonzero for -r: if comparing two directories,
   compare their common subdirectories recursively.  */

int recursive;

/* For debugging: don't do discard_confusing_lines.  */

static char const **exclude;
static int exclude_alloc, exclude_count;

int
excluded_filename (char const *f)
{
  int i;
  for (i = 0;  i < exclude_count;  i++)
    if (fnmatch (exclude[i], f, 0) == 0)
      return 1;
  return 0;
}

static char const *
filetype (struct stat const *st)
{
  /* See Posix.2 section 4.17.6.1.1 and Table 5-1 for these formats.
     To keep diagnostics grammatical, the returned string must start
     with a consonant.  */

  if (S_ISREG (st->st_mode))
    {
      if (st->st_size == 0)
	return "regular empty file";
      /* Posix.2 section 5.14.2 seems to suggest that we must read the file
	 and guess whether it's C, Fortran, etc., but this is somewhat useless
	 and doesn't reflect historical practice.  We're allowed to guess
	 wrong, so we don't bother to read the file.  */
      return "regular file";
    }
  if (S_ISDIR (st->st_mode)) return "directory";

  /* other Posix.1 file types */
#ifdef S_ISBLK
  if (S_ISBLK (st->st_mode)) return "block special file";
#endif
#ifdef S_ISCHR
  if (S_ISCHR (st->st_mode)) return "character special file";
#endif
#ifdef S_ISFIFO
  if (S_ISFIFO (st->st_mode)) return "fifo";
#endif

  /* other popular file types */
  /* S_ISLNK is impossible with `stat'.  */
#ifdef S_ISSOCK
  if (S_ISSOCK (st->st_mode)) return "socket";
#endif

  return "weird file";
}



int FileIsBinary(int fd)
{
    int bResult=0;
    int cnt;
    char buf[40];
    long prevpos = tell(fd);
	lseek(fd, 0L, SEEK_SET);
	cnt=read(fd, buf, 40);
    if (cnt>0)
    {
		for (register int i=0; i < cnt; i++)
		{
			if (!isprint(buf[i])
			&& !isspace(buf[i])
			&& buf[i] != NULL)
			{
			bResult=1;
			break;
			}
		}
    }
    lseek(fd, prevpos, SEEK_SET);
    return bResult;
}


/* Compare two files (or dirs) with specified names
   DIR0/NAME0 and DIR1/NAME1, at level DEPTH in directory recursion.
   (if DIR0 is 0, then the name is just NAME0, etc.)
   This is self-contained; it opens the files and closes them.

   Value is 0 if files are the same, 1 if different,
   2 if there is a problem opening them.  */

int
compare_files (LPCTSTR dir0, LPCTSTR name0, 
	       LPCTSTR dir1, LPCTSTR name1, 
	       CDiffContext *pCtx, int depth)
{
  struct file_data inf[2];
  register int i;
  int val=0;
  int same_files;
  int failed = 0;
  LPTSTR free0 = 0, free1 = 0;
  bool lunique=false,runique=false;

  memset(&inf[0], 0, sizeof(struct file_data));
  memset(&inf[1], 0, sizeof(struct file_data));

  /* If this is directory comparison, perhaps we have a file
     that exists only in one of the directories.  */

  if (gWriteLog) gLog.Write(_T("Comparing: n0=%s, n1=%s, d0=%s, d1=%s"), name0, name1, dir0, dir1);

  if (! ((name0 != 0 && name1 != 0)
	 || (unidirectional_new_file_flag && name1 != 0)
	 || entire_new_file_flag))
    {
	  // flag the unique status for later processing
	  lunique=(name0 != 0);
	  runique=(name0 == 0);
    }

  /* Mark any nonexistent file with -1 in the desc field.  */
  /* Mark unopened files (e.g. directories) with -2. */

  inf[0].desc = (name0 == 0 ? -1 : -2);
  inf[1].desc = (name1 == 0 ? -1 : -2);

  /* Now record the full name of each file, including nonexistent ones.  */

  if (name0 == 0)
    name0 = name1;
  if (name1 == 0)
    name1 = name0;

  inf[0].name = dir0 == 0 ? name0 : (free0 = dir_file_pathname (dir0, name0));
  inf[1].name = dir1 == 0 ? name1 : (free1 = dir_file_pathname (dir1, name1));

  /* Stat the files.  Record whether they are directories.  */

  for (i = 0; i <= 1; i++)
    {
      bzero (&inf[i].stat, sizeof (struct stat));
      inf[i].dir_p = 0;

      if (inf[i].desc != -1)
	{
	  int stat_result;

	  if (i && stricmp (inf[i].name, inf[0].name) == 0)
	    {
	      inf[i].stat = inf[0].stat;
	      stat_result = 0;
	    }
	  else if (strcmp (inf[i].name, "-") == 0)
	    {
	      inf[i].desc = STDIN_FILENO;
	      stat_result = fstat (STDIN_FILENO, &inf[i].stat);
	      if (stat_result == 0 && S_ISREG (inf[i].stat.st_mode))
		{
		  off_t pos = lseek (STDIN_FILENO, (off_t) 0, SEEK_CUR);
		  if (pos == -1)
		    stat_result = -1;
		  else
		    {
		      if (pos <= inf[i].stat.st_size)
			inf[i].stat.st_size -= pos;
		      else
			inf[i].stat.st_size = 0;
		      /* Posix.2 4.17.6.1.4 requires current time for stdin.  */
		      time (&inf[i].stat.st_mtime);
		    }
		}
	    }
	  else
	    stat_result = stat (inf[i].name, &inf[i].stat);

	  if (stat_result != 0)
	    {
	      perror_with_name (inf[i].name);
	      failed = 1;
	    }
	  else
	    {
	      inf[i].dir_p = S_ISDIR (inf[i].stat.st_mode) && inf[i].desc != 0;
	      if (inf[1 - i].desc == -1)
		{
		  inf[1 - i].dir_p = inf[i].dir_p;
		  inf[1 - i].stat.st_mode = inf[i].stat.st_mode;
		}
	    }
	}
    }

  // delayed unique determination so we can see if file is a directory or not
  if (!failed && (lunique || runique))
  {
	  BYTE code=0;
      LPCTSTR name=0;
	  if (lunique)
	  {
		  if (inf[0].dir_p != 0)
			  code = FILE_LDIRUNIQUE;
		  else
			  code = FILE_LUNIQUE;
		  name = name0;
	  }
	  else
	  {
		  if (inf[1].dir_p != 0)
			  code = FILE_RDIRUNIQUE;
		  else
			  code = FILE_RUNIQUE;
		  name = name1;
	  }
	  pCtx->AddDiff(name, dir0, dir1, code);
      if (gWriteLog) gLog.Write(_T("\tUnique\r\n"));
	  if (free0)
		  free (free0);
	  if (free1)
		  free (free1);
      return 1;
  }

  if (! failed && depth == 0 && inf[0].dir_p != inf[1].dir_p)
    {
      /* If one is a directory, and it was specified in the command line,
	 use the file in that dir with the other file's basename.  */

      int fnm_arg = inf[0].dir_p;
      int dir_arg = 1 - fnm_arg;
      char const *fnm = inf[fnm_arg].name;
      char const *dir = inf[dir_arg].name;
      char const *p = strrchr (fnm, '/');
      char const *filename = inf[dir_arg].name
	= dir_file_pathname (dir, p ? p + 1 : fnm);

      if (strcmp (fnm, "-") == 0)
	fatal ("can't compare - to a directory");

      if (stat (filename, &inf[dir_arg].stat) != 0)
	{
	  perror_with_name (filename);
	  failed = 1;
	}
      else
	inf[dir_arg].dir_p = S_ISDIR (inf[dir_arg].stat.st_mode);
    }

  if (failed)
    {

      /* If either file should exist but does not, return 2.  */

      val = 2;
      pCtx->AddDiff(name0, dir0, dir1, FILE_ERROR);
    }
#if defined(__MSDOS__) || defined(__NT__) || defined(WIN32)
  else if (same_files = 0) /* yes, only ONE equal sign intended! hmo11apr93 */
    ;
#else
  else if ((same_files =    inf[0].stat.st_ino == inf[1].stat.st_ino
			 && inf[0].stat.st_dev == inf[1].stat.st_dev
			 && inf[0].stat.st_size == inf[1].stat.st_size
			 && inf[0].desc != -1
			 && inf[1].desc != -1)
	   && no_diff_means_no_output)
    {
      /* The two named files are actually the same physical file.
	 We know they are identical without actually reading them.  */

      val = 0;
      pCtx->AddDiff(name0, dir0, dir0, FILE_SAME);
    }
#endif /*__MSDOS__||__NT__*/
  else if (inf[0].dir_p & inf[1].dir_p)
    {
      if (output_style == OUTPUT_IFDEF)
	fatal ("-D option not supported with directories");

      /* If both are directories, compare the files in them.  */

      if (depth > 0 && !recursive)
	{
	  /* But don't compare dir contents one level down
	     unless -r was specified.  */
	  message ("Common subdirectories: %s and %s\n",
		   inf[0].name, inf[1].name);
	  val = 0;
	}
      else
	{
	  if (gWriteLog) gLog.Write(_T("\tDirs found.\r\n"));

	  CDiffContext ctx(inf[0].name, inf[1].name, *pCtx);
	  val = diff_dirs (&ctx, depth);
	}

    }
  else if ((inf[0].dir_p | inf[1].dir_p)
	   || (depth > 0
	       && (! S_ISREG (inf[0].stat.st_mode)
		   || ! S_ISREG (inf[1].stat.st_mode))))
    {
      /* Perhaps we have a subdirectory that exists only in one directory.
	 If so, just print a message to that effect.  */

      if (inf[0].desc == -1 || inf[1].desc == -1)
	{
	  if ((inf[0].dir_p | inf[1].dir_p)
	      && recursive
	      && (entire_new_file_flag
		  || (unidirectional_new_file_flag && inf[0].desc == -1)))
	  {
		  if (gWriteLog) gLog.Write(_T("\tDirs found.\r\n"));
		  CDiffContext ctx(inf[0].name, inf[1].name, *pCtx);
		  val = diff_dirs (&ctx, depth);
	  }
	  else
	    {
	      char const *dir = (inf[0].desc == -1) ? dir1 : dir0;
	      /* See Posix.2 section 4.17.6.1.1 for this format.  */
	      message ("Only in %s: %s\n", dir, name0);
	      val = 1;
	    }
	}
      else
	{
	  /* We have two files that are not to be compared.  */

	  /* See Posix.2 section 4.17.6.1.1 for this format.  */
	  message5 ("File %s is a %s while file %s is a %s\n",
		    inf[0].name, filetype (&inf[0].stat),
		    inf[1].name, filetype (&inf[1].stat));

	  /* This is a difference.  */
	  val = 1;
	  pCtx->AddDiff(name0, dir0, dir1, FILE_DIFF);
	}
    }
  else if ((no_details_flag & ~ignore_some_changes)
	   && inf[0].stat.st_size != inf[1].stat.st_size
	   && (inf[0].desc == -1 || S_ISREG (inf[0].stat.st_mode))
	   && (inf[1].desc == -1 || S_ISREG (inf[1].stat.st_mode)))
    {
      message ("Files %s and %s differ\n", inf[0].name, inf[1].name);
      val = 1;
      pCtx->AddDiff(name0, dir0, dir1, FILE_DIFF);
    }
  else
    {
      /* Both exist and neither is a directory.  */
      int o_binary = always_text_flag ? O_BINARY : 0;
      /* Open the files and record their descriptors.  */

      if (inf[0].desc == -2)
	if ((inf[0].desc = open (inf[0].name, O_RDONLY|o_binary, 0)) < 0)
	  {
	    perror_with_name (inf[0].name);
	    failed = 1;
	  }
      if (inf[1].desc == -2)
	if (same_files)
	  inf[1].desc = inf[0].desc;
	else if ((inf[1].desc = open (inf[1].name, O_RDONLY|o_binary, 0)) < 0)
	  {
	    perror_with_name (inf[1].name);
	    failed = 1;
	}
	
	bool bBinary = ((inf[0].desc>0 && FileIsBinary(inf[0].desc))
		|| (inf[1].desc>0 && FileIsBinary(inf[1].desc)));
	if (bBinary)
	{
		struct change *p,*e,*script=NULL;
		int diff_flag=0;
	    
	    script = diff_2_files (inf, depth, &diff_flag);

		if (script != NULL)
		{
			for (e = script; e; e = p)
			{
			p = e->link;
			free (e);
			}
		}
		cleanup_file_buffers(inf);
		// close open file handles
	    if (inf[0].desc >= 0 && close (inf[0].desc) != 0)
	    {
			perror_with_name (inf[0].name);
			val = 2;
	    }
	    if (inf[1].desc >= 0 && inf[0].desc != inf[1].desc
		&& close (inf[1].desc) != 0)
	    {
			perror_with_name (inf[1].name);
			val = 2;
	    }

		if(val==2)
	    {
			pCtx->AddDiff(name0, dir0, dir1, FILE_ERROR);
			if (gWriteLog) gLog.Write(_T("\t%s.\r\n"), val==2? "error":"different");
	    }
		else if (diff_flag)
	    {
			val = 1;
			pCtx->AddDiff(name0, dir0, dir1, FILE_BINDIFF);
				if (gWriteLog) gLog.Write(_T("\tbinary.\r\n"));
	    }
	    else 
	    {
			val = 0;
			//pCtx->AddDiff(name0, dir0, dir1, FILE_SAME);
			//	if (gWriteLog) gLog.Write(_T("\tidentical.\r\n"));
	    }
	}
	else
	{
	    /* Compare the files, if no error was found.  */
	    struct change *e, *p;
	    struct change *script=NULL;
	    
	    script = diff_2_files (inf, depth, NULL);
	    val = script? 1 : 0;
	    /* Close the file descriptors.  */
	    // cleanup the script
	    for (e = script; e; e = p)
	    {
		p = e->link;
		free (e);
	    }
		cleanup_file_buffers(inf);
	    
	    if (inf[0].desc >= 0 && close (inf[0].desc) != 0)
	    {
		perror_with_name (inf[0].name);
		val = 2;
	    }
	    if (inf[1].desc >= 0 && inf[0].desc != inf[1].desc
		&& close (inf[1].desc) != 0)
	    {
		perror_with_name (inf[1].name);
		val = 2;
	    }
	    if (val==2 || val == 1)
	    {
		pCtx->AddDiff(name0, dir0, dir1, (BYTE)(val==2? FILE_ERROR:FILE_DIFF));
		if (gWriteLog) gLog.Write(_T("\t%s.\r\n"), val==2? "error":"different");
	    }
	}
    }

  /* Now the comparison has been done, if no error prevented it,
     and VAL is the value this function will return.  */

  if (val == 0 && !inf[0].dir_p)
    {
      if (print_file_same_flag)
	message ("Files %s and %s are identical\n",
		 inf[0].name, inf[1].name);
      pCtx->AddDiff(name0, dir0, dir1, FILE_SAME);
      if (gWriteLog) gLog.Write(_T("\tidentical.\r\n"));
   }
  else
  {
    fflush (stdout);
  }

  if (free0)
    free (free0);
  if (free1)
    free (free1);


  return val;
}
