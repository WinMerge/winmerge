/* cmp -- compare two files.
   Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Torbjorn Granlund and David MacKenzie. */

#include "system.h"
#include <stdio.h>
#include "getopt.h"
#include "cmpbuf.h"

extern char const version_string[];

#if __STDC__ && defined (HAVE_VPRINTF)
void error (int, int, char const *, ...);
#else
void error ();
#endif
VOID *xmalloc PARAMS((size_t));

static int cmp PARAMS((void));
static off_t file_position PARAMS((int));
static size_t block_compare PARAMS((char const *, char const *));
static size_t block_compare_and_count PARAMS((char const *, char const *, long *));
static size_t block_read PARAMS((int, char *, size_t));
static void printc PARAMS((int, unsigned));
static void usage PARAMS((char const *));

/* Name under which this program was invoked.  */
char const *program_name;

/* Filenames of the compared files.  */
static char const *file[2];

/* File descriptors of the files.  */
static int file_desc[2];

/* Read buffers for the files.  */
static char *buffer[2];

/* Optimal block size for the files.  */
static size_t buf_size;

/* Initial prefix to ignore for each file.  */
static off_t ignore_initial;

/* Output format:
   type_first_diff
     to print the offset and line number of the first differing bytes
   type_all_diffs
     to print the (decimal) offsets and (octal) values of all differing bytes
   type_status
     to only return an exit status indicating whether the files differ */
static enum
  {
    type_first_diff, type_all_diffs, type_status
  } comparison_type;

/* If nonzero, print values of bytes quoted like cat -t does. */
static int opt_print_chars;

static struct option const long_options[] =
{
  {"print-chars", 0, 0, 'c'},
  {"ignore-initial", 1, 0, 'i'},
  {"verbose", 0, 0, 'l'},
  {"silent", 0, 0, 's'},
  {"quiet", 0, 0, 's'},
  {"version", 0, 0, 'v'},
  {0, 0, 0, 0}
};

static void
usage (reason)
     char const *reason;
{
  if (reason)
    fprintf (stderr, "%s: %s\n", program_name, reason);

  fprintf (stderr, "\
Usage: %s [-clsv] [-i chars] [--ignore-initial=bytes]\n\
	[--print-chars] [--quiet] [--silent] [--verbose] [--version]\n\
	from-file [to-file]\n", program_name);

  exit (2);
}

int
main (argc, argv)
     int argc;
     char *argv[];
{
  int c, i, exit_status;
  struct stat stat_buf[2];

  program_name = argv[0];

  /* Parse command line options.  */

  while ((c = getopt_long (argc, argv, "ci:ls", long_options, 0))
	 != EOF)
    switch (c)
      {
      case 'c':
	opt_print_chars = 1;
	break;

      case 'i':
	ignore_initial = 0;
	while (*optarg)
	  {
	    /* Don't use `atol', because `off_t' may be longer than `long'.  */
	    unsigned digit = *optarg++ - '0';
	    if (9 < digit)
	      usage ("--ignore-initial value must be a nonnegative integer");
	    ignore_initial = 10 * ignore_initial + digit;
	  }
	break;

      case 'l':
	comparison_type = type_all_diffs;
	break;

      case 's':
	comparison_type = type_status;
	break;

      case 'v':
	fprintf (stderr, "GNU cmp version %s\n", version_string);
	break;

      default:
	usage (0);
      }

  if (optind == argc)
    usage (0);

  file[0] = argv[optind++];
  file[1] = optind < argc ? argv[optind++] : "-";

  if (optind < argc)
    usage ("extra arguments");

  for (i = 0; i < 2; i++)
    {
      /* If file[1] is "-", treat it first; this avoids a misdiagnostic if
	 stdin is closed and opening file[0] yields file descriptor 0.  */
      int i1 = i ^ (strcmp (file[1], "-") == 0);

      /* Two files with the same name are identical.
	 But wait until we open the file once, for proper diagnostics.  */
      if (i && strcmp (file[0], file[1]) == 0)
	exit (0);

      if (strcmp (file[i1], "-") == 0)
	file_desc[i1] = STDIN_FILENO;
      else
	{
	  file_desc[i1] = open (file[i1], O_RDONLY);
	  if (file_desc[i1] < 0)
	    {
	      if (comparison_type == type_status)
		exit (2);
	      else
		error (2, errno, "%s", file[i1]);
	    }
	}
      if (fstat (file_desc[i1], &stat_buf[i1]) != 0)
	error (2, errno, "%s", file[i1]);
    }

  /* If the files are links to the same inode and have the same file position,
     they are identical.  */

  if (stat_buf[0].st_dev == stat_buf[1].st_dev
      && stat_buf[0].st_ino == stat_buf[1].st_ino
      && file_position (0) == file_position (1))
    exit (0);

  /* If output is redirected to "/dev/null", we may assume `-s'.  */

  if (comparison_type != type_status)
    {
      struct stat outstat, nullstat;

      if (fstat (STDOUT_FILENO, &outstat) == 0
	  && stat ("/dev/null", &nullstat) == 0
	  && outstat.st_dev == nullstat.st_dev
	  && outstat.st_ino == nullstat.st_ino)
	comparison_type = type_status;
    }

  /* If only a return code is needed,
     and if both input descriptors are associated with plain files,
     conclude that the files differ if they have different sizes.  */

  if (comparison_type == type_status
      && S_ISREG (stat_buf[0].st_mode)
      && S_ISREG (stat_buf[1].st_mode))
    {
      off_t s0 = stat_buf[0].st_size - file_position (0);
      off_t s1 = stat_buf[1].st_size - file_position (1);

      if (max (0, s0) != max (0, s1))
	exit (1);
    }

  /* Get the optimal block size of the files.  */

  buf_size = buffer_lcm (STAT_BLOCKSIZE (stat_buf[0]),
			 STAT_BLOCKSIZE (stat_buf[1]));

  /* Allocate buffers, with space for sentinels at the end.  */

  for (i = 0; i < 2; i++)
    buffer[i] = xmalloc (buf_size + sizeof (long));

  exit_status = cmp ();

  for (i = 0; i < 2; i++)
    if (close (file_desc[i]) != 0)
      error (2, errno, "%s", file[i]);
  if (comparison_type != type_status)
    {
      if (ferror (stdout))
	error (2, 0, "write error");
      else if (fclose (stdout) != 0)
	error (2, errno, "write error");
    }
  exit (exit_status);
  return exit_status;
}

/* Compare the two files already open on `file_desc[0]' and `file_desc[1]',
   using `buffer[0]' and `buffer[1]'.
   Return 0 if identical, 1 if different, >1 if error. */

static int
cmp ()
{
  long line_number = 1;		/* Line number (1...) of first difference. */
  long char_number = ignore_initial + 1;
				/* Offset (1...) in files of 1st difference. */
  size_t read0, read1;		/* Number of chars read from each file. */
  size_t first_diff;		/* Offset (0...) in buffers of 1st diff. */
  size_t smaller;		/* The lesser of `read0' and `read1'. */
  char *buf0 = buffer[0];
  char *buf1 = buffer[1];
  int ret = 0;
  int i;

  if (ignore_initial)
    for (i = 0; i < 2; i++)
      if (file_position (i) == -1)
	{
	  /* lseek failed; read and discard the ignored initial prefix.  */
	  off_t ig = ignore_initial;
	  do
	    {
	      size_t r = read (file_desc[i], buf0, (size_t) min (ig, buf_size));
	      if (!r)
		break;
	      if (r == -1)
		error (2, errno, "%s", file[i]);
	      ig -= r;
	    }
	  while (ig);
	}

  do
    {
      read0 = block_read (file_desc[0], buf0, buf_size);
      if (read0 == -1)
	error (2, errno, "%s", file[0]);
      read1 = block_read (file_desc[1], buf1, buf_size);
      if (read1 == -1)
	error (2, errno, "%s", file[1]);

      /* Insert sentinels for the block compare.  */

      buf0[read0] = ~buf1[read0];
      buf1[read1] = ~buf0[read1];

      /* If the line number should be written for differing files,
	 compare the blocks and count the number of newlines
	 simultaneously.  */
      first_diff = (comparison_type == type_first_diff
		    ? block_compare_and_count (buf0, buf1, &line_number)
		    : block_compare (buf0, buf1));

      char_number += first_diff;
      smaller = min (read0, read1);

      if (first_diff < smaller)
	{
	  switch (comparison_type)
	    {
	    case type_first_diff:
	      /* See Posix.2 section 4.10.6.1 for this format.  */
	      printf ("%s %s differ: char %ld, line %ld",
		      file[0], file[1], char_number, line_number);
	      if (opt_print_chars)
		{
		  unsigned char c0 = buf0[first_diff];
		  unsigned char c1 = buf1[first_diff];
		  printf (" is %3o ", c0);
		  printc (0, c0);
		  printf (" %3o ", c1);
		  printc (0, c1);
		}
	      putchar ('\n');
	      /* Fall through. */
	    case type_status:
	      return 1;

	    case type_all_diffs:
	      do
		{
		  unsigned char c0 = buf0[first_diff];
		  unsigned char c1 = buf1[first_diff];
		  if (c0 != c1)
		    {
		      if (opt_print_chars)
			{
			  printf ("%6lu %3o ", char_number, c0);
			  printc (4, c0);
			  printf (" %3o ", c1);
			  printc (0, c1);
			  putchar ('\n');
			}
		      else
			/* See Posix.2 section 4.10.6.1 for this format.  */
			printf ("%6ld %3o %3o\n", char_number, c0, c1);
		    }
		  char_number++;
		  first_diff++;
		}
	      while (first_diff < smaller);
	      ret = 1;
	      break;
	    }
	}

      if (read0 != read1)
	{
	  if (comparison_type != type_status)
	    /* See Posix.2 section 4.10.6.2 for this format.  */
	    fprintf (stderr, "cmp: EOF on %s\n", file[read1 < read0]);

	  return 1;
	}
    }
  while (read0 == buf_size);
  return ret;
}

/* Compare two blocks of memory P0 and P1 until they differ,
   and count the number of '\n' occurrences in the common
   part of P0 and P1.
   Assumes that P0 and P1 are aligned at long addresses!
   If the blocks are not guaranteed to be different, put sentinels at the ends
   of the blocks before calling this function.

   Return the offset of the first byte that differs.
   Increment *COUNT by the count of '\n' occurrences.  */

static size_t
block_compare_and_count (p0, p1, count)
     char const *p0, *p1;
     long *count;
{
  long l;		/* One word from first buffer. */
  long const *l0, *l1;	/* Pointers into each buffer. */
  char const *c0, *c1;	/* Pointers for finding exact address. */
  long cnt = 0;		/* Number of '\n' occurrences. */
  long nnnn;		/* Newline, sizeof (long) times.  */
  int i;

  l0 = (long const *) p0;
  l1 = (long const *) p1;

  nnnn = 0;
  for (i = 0; i < sizeof (long); i++)
    nnnn = (nnnn << CHAR_BIT) | '\n';

  /* Find the rough position of the first difference by reading long ints,
     not bytes.  */

  while ((l = *l0++) == *l1++)
    {
      l ^= nnnn;
      for (i = 0; i < sizeof (long); i++)
	{
	  cnt += ! (unsigned char) l;
	  l >>= CHAR_BIT;
	}
    }

  /* Find the exact differing position (endianness independent).  */

  c0 = (char const *) (l0 - 1);
  c1 = (char const *) (l1 - 1);
  while (*c0 == *c1)
    {
      cnt += *c0 == '\n';
      c0++;
      c1++;
    }

  *count += cnt;
  return c0 - p0;
}

/* Compare two blocks of memory P0 and P1 until they differ.
   Assumes that P0 and P1 are aligned at long addresses!
   If the blocks are not guaranteed to be different, put sentinels at the ends
   of the blocks before calling this function.

   Return the offset of the first byte that differs.  */

static size_t
block_compare (p0, p1)
     char const *p0, *p1;
{
  long const *l0, *l1;
  char const *c0, *c1;

  l0 = (long const *) p0;
  l1 = (long const *) p1;

  /* Find the rough position of the first difference by reading long ints,
     not bytes.  */

  while (*l0++ == *l1++)
    ;

  /* Find the exact differing position (endianness independent).  */

  c0 = (char const *) (l0 - 1);
  c1 = (char const *) (l1 - 1);
  while (*c0 == *c1)
    {
      c0++;
      c1++;
    }

  return c0 - p0;
}

/* Read NCHARS bytes from descriptor FD into BUF.
   Return the number of characters successfully read.
   The number returned is always NCHARS unless end-of-file or error.  */

static size_t
block_read (fd, buf, nchars)
     int fd;
     char *buf;
     size_t nchars;
{
  char *bp = buf;

  do
    {
      size_t nread = read (fd, bp, nchars);
      if (nread == -1)
	return -1;
      if (nread == 0)
	break;
      bp += nread;
      nchars -= nread;
    }
  while (nchars != 0);

  return bp - buf;
}

/* Print character C, making nonvisible characters
   visible by quoting like cat -t does.
   Pad with spaces on the right to WIDTH characters.  */

static void
printc (width, c)
     int width;
     unsigned c;
{
  register FILE *fs = stdout;

  /* Handle non-printable chars */
  if (! isprint (c))
    {
    if (c >= 128)
      {
        putc ('M', fs);
        putc ('-', fs);
        c -= 128;
        width -= 2;
      }
    if (c < 32)
      {
        putc ('^', fs);
        c += 64;
        --width;
      }
    else if (c == 127)
      {
        putc ('^', fs);
        c = '?';
        --width;
      }
    }

  putc (c, fs);
  while (--width > 0)
    putc (' ', fs);
}

/* Position file I to `ignore_initial' bytes from its initial position,
   and yield its new position.  Don't try more than once.  */

static off_t
file_position (i)
     int i;
{
  static int positioned[2];
  static off_t position[2];

  if (! positioned[i])
    {
      positioned[i] = 1;
      position[i] = lseek (file_desc[i], ignore_initial, SEEK_CUR);
    }
  return position[i];
}
