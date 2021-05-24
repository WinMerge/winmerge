/* Support routines for GNU DIFF.
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

#include <windows.h>
#include "diff.h"

/* Queue up one-line messages to be printed at the end,
   when -l is specified.  Each message is recorded with a `struct msg'.  */

struct msg
{
  struct msg *next;
  char const *format;
  char const *arg1;
  char const *arg2;
  char const *arg3;
  char const *arg4;
};

/* Head of the chain of queues messages.  */

static struct msg *msg_chain;

/* Tail of the chain of queues messages.  */

static struct msg **msg_chain_end = &msg_chain;

/* Use when a system call returns non-zero status.
   TEXT should normally be the file name.  */

void
perror_with_name (char const *text)
{
  int e = errno;
  fprintf (stderr, "%s: ", program);
  errno = e;
  perror (text);
}

/* Use when a system call returns non-zero status and that is fatal.  */

void
pfatal_with_name (char const *text)
{
  int e = errno;
  print_message_queue ();
  fprintf (stderr, "%s: ", program);
  errno = e;
  perror (text);
  //exit (2);
  RaiseException(STATUS_ACCESS_VIOLATION, 0, 0, NULL);
}

/* Print an error message from the format-string FORMAT
   with args ARG1 and ARG2.  */

void
error (char const *format, char const *arg, char const *arg1)
{
  fprintf (stderr, "%s: ", program);
  fprintf (stderr, format, arg, arg1);
  fprintf (stderr, "\n");
}

/* Print an error message containing the string TEXT, then exit.  */

void
fatal (char const *m)
{
  print_message_queue ();
  error ("%s", m, 0);
  //exit (2);
  RaiseException(STATUS_ACCESS_VIOLATION, 0, 0, NULL);
}

/* Like printf, except if -l in effect then save the message and print later.
   This is used for things like "binary files differ" and "Only in ...".  */

void
message (char const *format, char const *arg1, char const *arg2)
{
  message5 (format, arg1, arg2, 0, 0);
}

void
message5 (char const *format, char const *arg1, char const *arg2, char const *arg3, char const *arg4)
{
  if (paginate_flag)
    {
      struct msg *new = (struct msg *) xmalloc (sizeof (struct msg));
      new->format = format;
      new->arg1 = concat (arg1, "", "");
      new->arg2 = concat (arg2, "", "");
      new->arg3 = arg3 ? concat (arg3, "", "") : 0;
      new->arg4 = arg4 ? concat (arg4, "", "") : 0;
      new->next = NULL;
      *msg_chain_end = new;
      msg_chain_end = &new->next;
    }
  else
    {
      if (sdiff_help_sdiff)
	putchar (' ');
      printf (format, arg1, arg2, arg3, arg4);
    }
}

/* Output all the messages that were saved up by calls to `message'.  */

void
print_message_queue ()
{
  struct msg *m;

  for (m = msg_chain; m; m = m->next)
    printf (m->format, m->arg1, m->arg2, m->arg3, m->arg4);
}

/* Call before outputting the results of comparing files NAME0 and NAME1
   to set up OUTFILE, the stdio stream for the output to go to.

   Usually, OUTFILE is just stdout.  But when -l was specified
   we fork off a `pr' and make OUTFILE a pipe to it.
   `pr' then outputs to our stdout.  */

static char const *current_name0;
static char const *current_name1;
static int current_depth;

void
setup_output (char const *name0, char const *name1, int depth)
{
  current_name0 = name0;
  current_name1 = name1;
  current_depth = depth;
  outfile = NULL;
}

static pid_t pr_pid;

void
begin_output ()
{
  char *name;

  if (outfile != NULL)
    return;

  char *mySwitch = (switch_string != NULL ? switch_string : "");

  /* Construct the header of this piece of diff.  */
  const size_t nameSiz = strlen(current_name0) + strlen(current_name1)
	  + strlen(mySwitch) + 7;
  name = xmalloc (nameSiz);
  /* Posix.2 section 4.17.6.1.1 specifies this format.  But there are some
     bugs in the first printing (IEEE Std 1003.2-1992 p 251 l 3304):
     it says that we must print only the last component of the pathnames,
     and it requires two spaces after "diff" if there are no options.
     These requirements are silly and do not match historical practice.  */
  sprintf_s (name, nameSiz, "diff%s %s %s", mySwitch, current_name0, current_name1);

  if (paginate_flag)
    {
#if defined(__MSDOS__) || defined(__NT__) || defined(WIN32)
      char command[120];

      sprintf_s(command, sizeof(command), "%s -f -h \"%s\"", PR_FILE_NAME, name);
      if ((outfile = popen(command, "w")) == NULL)
        pfatal_with_name ("popen");
#else
      int pipes[2];

      /* Fork a `pr' and make OUTFILE a pipe to it.  */
      if (pipe (pipes) < 0)
	pfatal_with_name ("pipe");

      fflush (stdout);

      pr_pid = vfork ();
      if (pr_pid < 0)
	pfatal_with_name ("vfork");

      if (pr_pid == 0)
	{
	  close (pipes[1]);
	  if (pipes[0] != STDIN_FILENO)
	    {
	      if (dup2 (pipes[0], STDIN_FILENO) < 0)
		pfatal_with_name ("dup2");
	      close (pipes[0]);
	    }

	  execl (PR_FILE_NAME, PR_FILE_NAME, "-f", "-h", name, 0);
	  pfatal_with_name (PR_FILE_NAME);
	}
      else
	{
	  close (pipes[0]);
	  outfile = fdopen (pipes[1], "w");
	}
#endif /*__MSDOS__||__NT__*/
    }
  else
    {

      /* If -l was not specified, output the diff straight to `stdout'.  */

      outfile = stdout;

      /* If handling multiple files (because scanning a directory),
	 print which files the following output is about.  */
      if (current_depth > 0)
	printf ("%s\n", name);
    }

  free (name);

  /* A special header is needed at the beginning of context output.  */
  switch (output_style)
    {
    case OUTPUT_CONTEXT:
      print_context_header (files, 0);
      break;

    case OUTPUT_UNIFIED:
      print_context_header (files, 1);
      break;

    default:
      break;
    }
}

/* Call after the end of output of diffs for one file.
   Close OUTFILE and get rid of the `pr' subfork.  */

void
finish_output ()
{
  if (outfile != NULL && outfile != stdout)
    {
#if defined(__MSDOS__) || defined(__NT__) || defined(WIN32)
      if (pclose (outfile))
	pfatal_with_name ("write error");
#else
      int wstatus;
      if (ferror (outfile))
	fatal ("write error");
      if (fclose (outfile) != 0)
	pfatal_with_name ("write error");
#if HAVE_WAITPID
      if (waitpid (pr_pid, &wstatus, 0) < 0)
	pfatal_with_name ("waitpid");
#else
      for (;;) {
	pid_t w = wait (&wstatus);
	if (w < 0)
	  pfatal_with_name ("wait");
	if (w == pr_pid)
	  break;
      }
#endif
      if (! WIFEXITED (wstatus) || WEXITSTATUS (wstatus) != 0)
	fatal ("subsidiary pr failed");
#endif /*__MSDOS__||__NT__*/
    }

  outfile = NULL;
}


static int
ISWSPACE (char ch)
{
	return ch==' ' || ch=='\t';
}

/* Compare two lines (typically one from each input file)
   according to the command line options.
   Return 1 if the lines differ, like `memcmp'.  */

int
line_cmp (char const *s1, size_t len1, char const *s2, size_t len2)
{
  register unsigned char const *t1, *t2;
  register unsigned char end_char = line_end_char;

  /* Check first for exact identity.
     If that is true, return 0 immediately.
     This detects the common case of exact identity
     faster than complete comparison would.  */

  if (len1 == len2 && memcmp (s1, s2, len1) == 0)
    return 0;

  /* Not exactly identical, but perhaps they match anyway
     when case or white space is ignored.  */
  /* c1 is the current character value for the line s1, it is set to 0
     when the line has been entirely scanned.
     c2 is the equivalent of c1 for the line s2 */

  if (ignore_case_flag | ignore_space_change_flag | ignore_all_space_flag | ignore_eol_diff)
    {
      t1 = (unsigned char const *) s1;
      t2 = (unsigned char const *) s2;

      while (1)
	{
	  register unsigned char c1;
	  register unsigned char c2;
	  if (t1-(unsigned char *)s1<(int)len1)
	    c1 = *t1++;
	  else
	    c1 = 0;
	  if (t2-(unsigned char *)s2<(int)len2)
	    c2 = *t2++;
	  else
	    c2 = 0;

      /* Test for exact char equality first, since it's a common case.  */
      if (c1 != c2)
	{
	  /* Ignore horizontal white space if -b or -w is specified.  */

	  if (ignore_all_space_flag)
	    {
	      /* For -w, just skip past any white space.  */
	      while (ISWSPACE (c1))
		{
		  if (t1-(unsigned char *)s1<(int)len1)
		    {
		      c1 = *t1++;
		    }
		  else
		    {
		      c1 = 0;
		      break;
		    }
		}
	      while (ISWSPACE (c2))
		{
		  if (t2-(unsigned char *)s2<(int)len2)
		    {
		      c2 = *t2++;
		    }
		  else
		    {
		      c2 = 0;
		      break;
		    }
		}
	    }
	  else if (ignore_space_change_flag)
	    {
	      /* For -b, advance past any sequence of white space in line 1
		 and consider it just one Space, or nothing at all
		 if it is at the end of the line.  */
	      if (ISWSPACE (c1))
		{
		  /* Any whitespace sequence counts as one space */
		  c1 = ' ';
		  /* Skip to end of whitespace sequence */
		  while (t1-(unsigned char *)s1<(int)len1 && ISWSPACE(*t1))
		    ++t1;
		  /* if c1 is whitespace and c2 is end of line
		  we must advance c1 to next char, because c1
		  whitespace matches the nothing in c2 */
		  if (c2=='\r' || c2=='\n')
		    {
		      if (t1-(unsigned char *)s1<(int)len1)
			c1 = *t1++;
		      else
			c1 = 0;
		    }
		}

	      /* Likewise for line 2.  */
	      if (ISWSPACE (c2))
		{
		  /* Any whitespace sequence counts as one space */
		  c2 = ' ';
		  /* Skip to end of whitespace sequence */
		  while (t2-(unsigned char *)s2<(int)len2 && ISWSPACE(*t2))
		    ++t2;
		  /* if c2 is whitespace and c1 is end of line
		  we must advance c1 to next char, because c2
		  whitespace matches the nothing in c1 */
		  if (c1=='\r' || c1=='\n')
		    {
		      if (t2-(unsigned char *)s2<(int)len2)
			c2 = *t2++;
		      else
			c2 = 0;
		    }
		}

	      /*
	      Whitespace at end of line matches end of file
	      make them both say ' ' so they match, and the one
	      not at end will get advanced
	      */
	      if (c1 != c2)
	        {
		  if (c1==' ' && !c2)
		    c2 = ' ';
		  else if (c2==' ' && !c1)
		    c1 = ' ';
	        }

	      if (c1 != c2)
		{
		  /* backtracking necessary when matching "cat and" against "cat  and"
		     because the spaces got matched with equality, so now "a" and " "
		     are mismatches */

		  /* If we went too far when doing the simple test
		     for equality, go back to the first non-white-space
		     character in both sides and try again.  */
		  if (c2 == ' ' && c1 && c1 != '\n' && c1 != '\r'
		      && (unsigned char const *) s1 + 1 < t1
		      && isspace(t1[-2]))
		    {
		      --t1;
		      continue;
		    }
		  if (c1 == ' ' && c2 && c2 != '\n' && c2 != '\r'
		      && (unsigned char const *) s2 + 1 < t2
		      && isspace(t2[-2]))
		    {
		      --t2;
		      continue;
		    }
		}
	    }

	  /* Upcase all letters if -i is specified.  */

	  if (ignore_case_flag)
	    {
	      if (islower (c1))
		c1 = (unsigned char)toupper (c1);
	      if (islower (c2))
		c2 = (unsigned char)toupper (c2);
	    }

	  if (ignore_eol_diff)
	    {
	      if (c1 == '\r')
		c1 = 0;
	      else if (c2 == '\r')
		c2 = 0;
	    }

	  if (c1 != c2)
	    break;
	  }

	  /* If we got here, c1 == c2 */

	  if (!c1)
	    return 0;
	}
    }

  return 1;
}

/* Find the consecutive changes at the start of the script START.
   Return the last link before the first gap.  */

struct change *
find_change (struct change *start)
{
  return start;
}

struct change *
find_reverse_change (struct change *start)
{
  return start;
}

/* Divide SCRIPT into pieces by calling HUNKFUN and
   print each piece with PRINTFUN.
   Both functions take one arg, an edit script.

   HUNKFUN is called with the tail of the script
   and returns the last link that belongs together with the start
   of the tail.

   PRINTFUN takes a subscript which belongs together (with a null
   link at the end) and prints it.  */

void
print_script (struct change *script, 
				struct change *(*hunkfun) (struct change *), 
				void (*printfun) (struct change *) )
{
  struct change *next = script;

  while (next)
    {
      struct change *this, *end;

      /* Find a set of changes that belong together.  */
      this = next;
      end = (*hunkfun) (next);

      /* Disconnect them from the rest of the changes,
	 making them a hunk, and remember the rest for next iteration.  */
      next = end->link;
      end->link = NULL;
#ifdef DEBUG
      debug_script (this);
#endif

      /* Print this hunk.  */
      (*printfun) (this);

      /* Reconnect the script so it will all be freed properly.  */
      end->link = next;
    }
}

/* Print the text of a single line LINE,
   flagging it with the characters in LINE_FLAG (which say whether
   the line is inserted, deleted, changed, etc.).  */

void
print_1_line (char const *line_flag, char const * const *line)
{
  char const HUGE *text = line[0], HUGE *limit = line[1]; /* Help the compiler.  */
  FILE *out = outfile; /* Help the compiler some more.  */
  char const *flag_format = NULL;

  /* If -T was specified, use a Tab between the line-flag and the text.
     Otherwise use a Space (as Unix diff does).
     Print neither space nor tab if line-flags are empty.  */

  if (line_flag != NULL && *line_flag != 0)
    {
      flag_format = tab_align_flag ? "%s\t" : "%s ";
      fprintf (out, flag_format, line_flag);
    }

  output_1_line (text, limit, flag_format, line_flag);

  if ((line_flag == NULL || line_flag[0]) && limit[-1] != '\n' && limit[-1] != '\r'
      && line_end_char == '\n')
    fprintf (out, "\n\\ No newline at end of file\n");
}

/*
A version of fwrite which converts any embedded \r or \n or \r\n to \n
before passing it to fwrite. This is meant to be used with mixed eol mode input
being written to a text mode stream.
*/
static size_t
fwrite_textify( const void *buffer, size_t size, size_t count, FILE *stream )
{
	/*
	\r = carriage return
	\n = line feed
	We have to handle the carriage returns (\r) specially
	because some of them may be the first half of a \r\n pair
	We output \r\n for any solo \r or solo \n, but also for any \r\n pair
	so the tricky part is just avoiding outputing \r\n\r\n for the \r\n pair.
	*/

	size_t bytes=0;
	unsigned int i;
	const char * text = buffer;
	int cr = 0;
	char ch;
	i = 0;
	while (1)
	{
		// first handle any pending carriage returns
		// before even checking if we've finished file
		if (cr)
		{
			// currently handling a carriage return
			// we always finish the line for carriage returns
			bytes += fwrite("\n", 1, 1, stream);
			if (i==size*count)
			{
				// we're done
				return bytes;
			}
			// now check to see if we need to swallow the trailing line feed
			// of a carriage return/line feed pair (\r\n)
			if (text[i] == '\n')
				++i;
			// finished the pending carriage return
			cr = 0;
		}
		// check if we finished
		if (i==size*count)
			return bytes;
		ch = text[i];
		// first check if new character is a carriage return
		if (ch == '\r')
		{
			// activate our special mode flag, and go to next character
			cr = 1;
			++i;
			continue;
		}
		// (any bare \n characters are ok, stream will convert them)
		bytes += fwrite(&text[i], 1, 1, stream);
		++i;
	}
}


/* Output a line from TEXT up to LIMIT.  Without -t, output verbatim.
   With -t, expand white space characters to spaces, and if FLAG_FORMAT
   is nonzero, output it with argument LINE_FLAG after every
   internal carriage return, so that tab stops continue to line up.  */

void
output_1_line (char const *text, char const *limit, char const *flag_format, char const *line_flag)
{
  char * pos = NULL;
  if (!tab_expand_flag)
    fwrite_textify (text, sizeof (char), limit - text, outfile);
  else
    {
      register FILE *out = outfile;
      register unsigned char c;
      register char const HUGE *t = text;
      register unsigned column = 0;

      while (t < limit)
	switch ((c = *t++))
	  {
	  case '\t':
	    {
	      unsigned spaces = TAB_WIDTH - column % TAB_WIDTH;
	      column += spaces;
	      do
		putc (' ', out);
	      while (--spaces);
	    }
	    break;

	  case '\r':
	    putc (c, out);
	    if (flag_format && t < limit && *t != '\n')
	      fprintf (out, flag_format, line_flag);
	    column = 0;
	    break;

	  case '\b':
	    if (column == 0)
	      continue;
	    column--;
	    putc (c, out);
	    break;

	  default:
	    if (isprint (c))
	      column++;
	    putc (c, out);
	    break;
	  }
    }
}

int
change_letter (int inserts, int deletes)
{
  if (!inserts)
    return 'd';
  else if (!deletes)
    return 'a';
  else
    return 'c';
}

/* Translate an internal line number (an index into diff's table of lines)
   into an actual line number in the input file.
   The internal line number is LNUM.  FILE points to the data on the file.

   Internal line numbers count from 0 starting after the prefix.
   Actual line numbers count from 1 within the entire file.  */

int
translate_line_number (struct file_data const *file, int lnum)
{
  return lnum + file->prefix_lines + 1;
}

void
translate_range (struct file_data const *file, int a, int b, int *aptr, int *bptr)
{
  *aptr = translate_line_number (file, a - 1) + 1;
  *bptr = translate_line_number (file, b + 1) - 1;
}

/* Print a pair of line numbers with SEPCHAR, translated for file FILE.
   If the two numbers are identical, print just one number.

   Args A and B are internal line numbers.
   We print the translated (real) line numbers.  */

void
print_number_range (int sepchar, struct file_data *file, int a, int b)
{
  int trans_a, trans_b;
  translate_range (file, a, b, &trans_a, &trans_b);

  /* Note: we can have B < A in the case of a range of no lines.
     In this case, we should print the line number before the range,
     which is B.  */
  if (trans_b > trans_a)
    fprintf (outfile, "%d%c%d", trans_a, sepchar, trans_b);
  else
    fprintf (outfile, "%d", trans_b);
}

int iseolch (char ch)
{
  return ch=='\n' || ch=='\r';
}

int is_blank_line (char const *pch, char const *limit)
{
  while (pch < limit)
    {
      if ((*pch) == '\n' || (*pch) == '\r')
        break;
      if ((*pch) != ' ' && (*pch) != '\t')
        return 0;
      pch++;
    }
  return 1;
}

/* Look at a hunk of edit script and report the range of lines in each file
   that it applies to.  HUNK is the start of the hunk, which is a chain
   of `struct change'.  The first and last line numbers of file 0 are stored in
   *FIRST0 and *LAST0, and likewise for file 1 in *FIRST1 and *LAST1.
   Note that these are internal line numbers that count from 0.

   If no lines from file 0 are deleted, then FIRST0 is LAST0+1.

   Also set *DELETES nonzero if any lines of file 0 are deleted
   and set *INSERTS nonzero if any lines of file 1 are inserted.
   If only ignorable lines are inserted or deleted, both are
   set to 0.  */

void
analyze_hunk (struct change *hunk, 
    int *first0, int *last0, 
    int *first1, int *last1, 
    int *deletes, int *inserts, const struct file_data fd[])
{
  int l0, l1, show_from, show_to;
  int i;
  int trivial = ignore_blank_lines_flag;
  struct change *next;

  show_from = show_to = 0;

  *first0 = hunk->line0;
  *first1 = hunk->line1;

  next = hunk;
  do
    {
      l0 = next->line0 + next->deleted - 1;
      l1 = next->line1 + next->inserted - 1;
      show_from += next->deleted;
      show_to += next->inserted;

      for (i = next->line0; i <= l0 && trivial; i++)
        {
          if (!ignore_blank_lines_flag)
            {
              trivial = 0;
            }
          else if (ignore_all_space_flag | ignore_space_change_flag)
            {
              if (!is_blank_line(fd[0].linbuf[i], fd[0].linbuf[i + 1]))
                trivial = 0;
            }
          else if (!iseolch(fd[0].linbuf[i][0]) && fd[0].linbuf[i][0] != 0)
            {
              trivial = 0;
            }
        }
      for (i = next->line1; i <= l1 && trivial; i++)
        {
          if (!ignore_blank_lines_flag)
            {
              trivial = 0;
            }
          else if (ignore_all_space_flag | ignore_space_change_flag)
            {
              if (!is_blank_line(fd[1].linbuf[i], fd[1].linbuf[i + 1]))
                trivial = 0;
            }
          else if (!iseolch(fd[1].linbuf[i][0]) && fd[1].linbuf[i][0] != 0)
            {
              trivial = 0;
            }
        }
    }
  while ((next = next->link) != NULL);

  *last0 = l0;
  *last1 = l1;

  /* If all inserted or deleted lines are ignorable,
     tell the caller to ignore this hunk.  */
  if (trivial)
    show_from = show_to = 0;

  /* WinMerge editor needs to know if there were trivial changes though,
     so stash that off in the trivial field */
  if (trivial)
    hunk->trivial = 1;
  else
    hunk->trivial = 0;

  *deletes = show_from;
  *inserts = show_to;
}

/* malloc a block of memory, with fatal error message if we can't do it. */

VOID *
xmalloc (size_t size)
{
  register VOID *value;

  if (size == 0)
    size = 1;

  value = (VOID *) malloc (size);

  if (!value)
#ifdef __MSDOS__
    fatal ("real memory exhausted");
#else
    fatal ("virtual memory exhausted");
#endif
  return value;
}

/* realloc a block of memory, with fatal error message if we can't do it. */

VOID *
xrealloc (VOID *old, size_t size)
{
  register VOID *value;

  if (size == 0)
    size = 1;

  value = (VOID *) realloc (old, size);

  if (!value)
#ifdef __MSDOS__
    fatal ("real memory exhausted");
#else
    fatal ("virtual memory exhausted");
#endif
  return value;
}

/* Concatenate three strings, returning a newly malloc'd string.  */

char *
concat (char const *s1, char const *s2, char const *s3)
{
  size_t len = strlen (s1) + strlen (s2) + strlen (s3);
  char *new = xmalloc (len + 1);
  sprintf_s (new, len+1, "%s%s%s", s1, s2, s3);
  return new;
}

/* Yield the newly malloc'd pathname
   of the file in DIR whose filename is FILE.  */

char *
dir_file_pathname (char const *dir, char const *file)
{
#if defined(__MSDOS__) || defined(__NT__) || defined(WIN32)
  char sep = dir[strlen(dir) - 1];
  return concat (dir, "\\" + (*dir && ((sep == '/') || (sep == '\\'))), file);
#else
  return concat (dir, "/" + (*dir && dir[strlen (dir) - 1] == '/'), file);
#endif /*__MSDOS__||__NT__*/
}

void
debug_script (struct change *sp)
{
  fflush (stdout);
  for (; sp!=NULL; sp = sp->link)
    fprintf (stderr, "%3d %3d delete %d insert %d\n",
	     sp->line0, sp->line1, sp->deleted, sp->inserted);
  fflush (stderr);
}

#if !HAVE_MEMCHR
char *
memchr (s, c, n)
     char const *s;
     int c;
     size_t n;
{
  unsigned char const *p = (unsigned char const *) s, *lim = p + n;
  for (;  p < lim;  p++)
    if (*p == c)
      return (char *) p;
  return NULL;
}
#endif
