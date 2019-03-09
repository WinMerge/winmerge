/* HTML-format output routines for GNU DIFF.
   Copyright (C) 1988, 89, 91, 92, 93 Free Software Foundation, Inc.

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

#include "pch.h"
#include "PatchHTML.h"
#include "diff.h"
#include <algorithm>

static struct change *find_hunk(struct change *);
static void mark_ignorable(struct change *);
static void pr_unidiff_hunk(struct change *);
static void print_1_escapedhtml(const char **line);
static void output_1_escapedhtml(const char *text, const char *limit);

/* Print a header for a context diff, with the file names and dates.  */

void
print_html_header (void)
{
  fprintf (outfile, 
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/loose.dtd\">\n"
   "<html>\n"
   "<head>\n"
   "<title>WinMerge File Compare Report</title>\n"
   "</head>\n"
   "<style type=\"text/css\">\n"
   "<!--\n"
   "  HTML, BODY {\n"
   "    color: #000000;\n"
   "    background-color: #ffffff;\n"
   "  }\n"
   "  \n"
   "  table {\n"
   "    width: 100%%;\n"
   "    margin: 0; \n"
   "    border: none;\n"
   "  }\n"
   "  A:link    { color: #0000ff; }\n"
   "  A:visited { color: #880088; }\n"
   "  A:active  { color: #0000ff; }\n"
   "  \n"
   "  \n"
   "  /** Navigation Headers ***/\n"
   "  .vc_navheader {\n"
   "    background-color: #8888ff;\n"
   "  }\n"
   "  \n"
   "  \n"
   "  /*** Table Headers ***/\n"
   "  .vc_header {\n"
   "    text-align: left;\n"
   "    background-color: #cccccc;\n"
   "  }\n"
   "  .vc_header_sort {\n"
   "    text-align: left;\n"
   "    background-color: #88ff88;\n"
   "  }\n"
   "  \n"
   "  \n"
   "  /*** Table Rows ***/\n"
   "  .vc_row_even {\n"
   "    background-color: #ffffff;\n"
   "  }\n"
   "  .vc_row_odd {\n"
   "    background-color: #ccccee;\n"
   "  }\n"
   "  \n"
   "  \n"
   "  /*** Markup Summary Header ***/\n"
   "  .vc_summary {\n"
   "    background-color: #eeeeee;\n"
   "  }\n"
   "  \n"
   "  \n"
   "  /*** Colour Diff Styles ***/\n"
   "  .vc_diff_header {\n"
   "    background-color: #ffffff;\n"
   "  }\n"
   "  .vc_diff_chunk_header {\n"
   "    background-color: #99cccc;\n"
   "  }\n"
   "  .vc_diff_chunk_extra {\n"
   "    font-size: smaller;\n"
   "  }\n"
   "  .vc_diff_empty {\n"
   "    background-color: #cccccc;\n"
   "    font-family: monospace;\n"
   "    font-size: smaller;\n"
   "  }\n"
   "  .vc_diff_add {\n"
   "    background-color: #aaffaa;\n"
   "    font-family: monospace;\n"
   "    font-size: smaller;\n"
   "  }\n"
   "  .vc_diff_remove {\n"
   "    background-color: #ffaaaa;\n"
   "    font-family: monospace;\n"
   "    font-size: smaller;\n"
   "  }\n"
   "  .vc_diff_change {\n"
   "    background-color: #ffff77;\n"
   "    font-family: monospace;\n"
   "    font-size: smaller;\n"
   "  }\n"
   "  .vc_diff_change_empty {\n"
   "    background-color: #eeee77;\n"
   "    font-family: monospace;\n"
   "    font-size: smaller;\n"
   "  }\n"
   "  .vc_diff_nochange {\n"
   "    font-family: monospace;\n"
   "    font-size: smaller;\n"
   "  }\n"
   "  \n"
   "  \n"
   "  /*** Query Form ***/\n"
   "  .vc_query_form {\n"
   "    background-color: #e6e6e6;\n"
   "  }\n"
   "  \n"
   "  \n"
   "  -->\n"
   "</style>\n"
   "\n"
   "<body>\n");
}

void
print_html_terminator (void)
{
  fprintf (outfile, 
    "</body>\n"
    "</html>\n");
}

void
print_html_diff_header (struct file_data inf[])
{
  fprintf (outfile, "Left: %s<br />Right: %s<br /><br />", inf[0].name, inf[1].name);
  fprintf (outfile, 
    "<table cellspacing=\"0\" cellpadding=\"0\">\n"
    "    <tr class=\"vc_diff_header\">\n");
  
  char ctimeBuffer[26];
  ctime_s(ctimeBuffer, sizeof(ctimeBuffer), &inf[0].stat.st_mtime);
  fprintf (outfile, 
    "    <th style=\"width:50%%; vertical-align:top;\">Left: %s</th>\n", ctimeBuffer);
  ctime_s(ctimeBuffer, sizeof(ctimeBuffer), &inf[1].stat.st_mtime);
  fprintf (outfile, 
    "    <th style=\"width:50%%; vertical-align:top;\">Right: %s</th>\n", ctimeBuffer);
  fprintf (outfile, 
    "    </tr>\n");
}

void
print_html_diff_terminator (void)
{
  fprintf (outfile, "</table>\n");
}

/* Print an edit script in context format.  */

void
print_html_script (struct change *script/*, int src_codepage*/)
{
  if (ignore_blank_lines_flag)
    mark_ignorable (script);
  else
    {
      struct change *e;
      for (e = script; e; e = e->link)
	e->ignore = 0;
    }

  print_script (script, find_hunk, pr_unidiff_hunk);
}

/* Print a portion of an edit script in unidiff format.
   HUNK is the beginning of the portion to be printed.
   The end is marked by a `link' that has been nulled out.

   Prints out lines from both files, and precedes each
   line with the appropriate flag-character.  */

static void
pr_unidiff_hunk (struct change *hunk)
{
  int first0, last0, first1, last1, show_from, show_to, i, j, k0, k1;
  int trans_a, trans_b;
  struct change *next;
  FILE *out;

  /* Determine range of line numbers involved in each file.  */

  analyze_hunk (hunk, &first0, &last0, &first1, &last1, &show_from, &show_to, files);

  if (!show_from && !show_to)
    return;

  /* Include a context's width before and after.  */

  i = - files[0].prefix_lines;
  first0 = (std::max) (first0 - context, i);
  first1 = (std::max) (first1 - context, i);
  last0 = (std::min) (last0 + context, files[0].valid_lines - 1);
  last1 = (std::min) (last1 + context, files[1].valid_lines - 1);

  out = outfile;

  fprintf (out, "  <tr class=\"vc_diff_chunk_header\">\n");
  fprintf (out, "    <td style=\"width:50%%;\">\n");
  translate_range (&files[0], first0, last0, &trans_a, &trans_b);
  fprintf (out, "      <strong>Line %d</strong>&nbsp;\n", trans_a);
  fprintf (out, "      <span class=\"vc_diff_chunk_extra\"></span>\n");
  fprintf (out, "    </td>\n");
  fprintf (out, "    <td style=\"width:50%%;\">\n");
  translate_range (&files[1], first1, last1, &trans_a, &trans_b);
  fprintf (out, "      <strong>Line %d</strong>&nbsp;\n", trans_a);
  fprintf (out, "      <span class=\"vc_diff_chunk_extra\"></span>\n");
  fprintf (out, "    </td>\n");
  fprintf (out, "  </tr>\n");

  next = hunk;
  i = first0;
  j = first1;

  while (i <= last0 || j <= last1)
    {

      /* If the line isn't a difference, output the context from file 0. */

      if (next == nullptr || i < next->line0)
	{
	  fprintf (out, "  <tr>\n");
	  fprintf (out, "    <td class=\"vc_diff_nochange\">&nbsp;");
	  print_1_escapedhtml(&files[0].linbuf[i++]);
	  fprintf (out, "</td>\n");
	  fprintf (out, "    <td class=\"vc_diff_nochange\">&nbsp;");
	  print_1_escapedhtml(&files[1].linbuf[j++]);
	  fprintf (out, "</td>\n");
	  fprintf (out, "  </tr>\n");
	}
      else
	{
	  k0 = next->deleted;
	  k1 = next->inserted;
	  if (next->inserted > 0 && next->deleted > 0)
	    {
	      while (k0 > 0 || k1 > 0)
	        {
	          fprintf (out, "  <tr>\n");
	          if (k0 > 0)
	            {
	              fprintf (out, "    <td class=\"vc_diff_change\">&nbsp;");
	               print_1_escapedhtml(&files[0].linbuf[i++]);
	              fprintf (out, "</td>\n");
	            }
		  else
	            {
	              fprintf (out, "    <td class=\"vc_diff_empty\">&nbsp;</td>");
	            }
	          if (k1 > 0)
	            {
	              fprintf (out, "    <td class=\"vc_diff_change\">&nbsp;");
	              print_1_escapedhtml(&files[1].linbuf[j++]);
	              fprintf (out, "</td>\n");
	            }
		  else
	            {
	              fprintf (out, "    <td class=\"vc_diff_empty\">&nbsp;</td>");
	            }
	          fprintf (out, "  </tr>\n");
	          if (k0 > 0) k0--;
	          if (k1 > 0) k1--;
	        }
	    }
	  else if (next->deleted > 0 )
	    {
	      while (k0--)
	        {
	          fprintf (out, "  <tr>\n");
	          fprintf (out, "    <td class=\"vc_diff_remove\">&nbsp;");
	          print_1_escapedhtml(&files[0].linbuf[i++]);
	          fprintf (out, "</td>\n");
	          fprintf (out, "    <td class=\"vc_diff_empty\">&nbsp;</td>");
	          fprintf (out, "  </tr>\n");
	        }
	    }
	   else
	    {
	      while (k1--)
	        {
	          fprintf (out, "  <tr>\n");
	          fprintf (out, "    <td class=\"vc_diff_empty\">&nbsp;</td>");
	          fprintf (out, "    <td class=\"vc_diff_add\">&nbsp;");
	          print_1_escapedhtml(&files[1].linbuf[j++]);
	          fprintf (out, "</td>\n");
	          fprintf (out, "  </tr>\n");
	        }
	    }
	  /* We're done with this hunk, so on to the next! */

	  next = next->link;
	}
    }
}

/* Scan a (forward-ordered) edit script for the first place that more than
   2*CONTEXT unchanged lines appear, and return a pointer
   to the `struct change' for the last change before those lines.  */

static struct change *
find_hunk (struct change *start)
{
  struct change *prev;
  int top0;
  int thresh;

  do
    {
      /* Compute number of first line in each file beyond this changed.  */
      top0 = start->line0 + start->deleted;
      int top1 = start->line1 + start->inserted;
      prev = start;
      start = start->link;
      /* Threshold distance is 2*CONTEXT between two non-ignorable changes,
	 but only CONTEXT if one is ignorable.  */
      thresh = ((prev->ignore || (start && start->ignore))
		? context
		: 2 * context + 1);
      /* It is not supposed to matter which file we check in the end-test.
	 If it would matter, crash.  */
      if (start && start->line0 - top0 != start->line1 - top1)
	abort ();
    } while (start
	     /* Keep going if less than THRESH lines
		elapse before the affected line.  */
	     && start->line0 < top0 + thresh);

  return prev;
}

/* Set the `ignore' flag properly in each change in SCRIPT.
   It should be 1 if all the lines inserted or deleted in that change
   are ignorable lines.  */

static void
mark_ignorable (struct change *script)
{
  while (script != nullptr)
    {
      struct change *next = script->link;
      int first0, last0, first1, last1, deletes, inserts;

      /* Turn this change into a hunk: detach it from the others.  */
      script->link = nullptr;

      /* Determine whether this change is ignorable.  */
      analyze_hunk (script, &first0, &last0, &first1, &last1, &deletes, &inserts, files);
      /* Reconnect the chain as before.  */
      script->link = next;

      /* If the change is ignorable, mark it.  */
      script->ignore = (char)(!deletes && !inserts);

      /* Advance to the following change.  */
      script = next;
    }
}

static void
print_1_escapedhtml(const char **line)
{
  output_1_escapedhtml(line[0], line[1]);
}

static void
output_1_escapedhtml(const char *text, const char *limit)
{
  FILE *out = outfile;
  const char *t = text;
  int column = 0;
  int spcolumn = -2;

  while (t < limit)
    switch (unsigned char c = *t++)
      {
      case '&':
	fprintf (out, "&amp;");
	column++;
	break;

      case '>':
	fprintf (out, "&gt;");
	column++;
	break;

      case '<':
	fprintf (out, "&lt;");
	column++;
	break;

      case ' ':
	if (spcolumn + 1 < column)
	  putc (' ', out);
	else
	  fprintf (out, "&nbsp;");
	spcolumn = column;
	column++;
	break;

      case '\"':
	fprintf (out, "&quot;");
	break;

      case '\t':
	{
	  unsigned spaces = TAB_WIDTH - column % TAB_WIDTH;
	  column += spaces;
	  if (spaces > 0)
	    {
	      putc (' ', out);
	      spaces--;
	    }
	  if (spaces == 0)
	    break;
	  do
	    fprintf (out, "&nbsp;");
	  while (--spaces);
	}
	break;

      case '\r':
      case '\n':
	putc (c, out);
	column = 0;
	break;

      case '\b':
	if (column == 0)
	  continue;
	column--;
	putc (c, out);
	break;

      default:
	column++;
	putc (c, out);
	break;
      }
}
