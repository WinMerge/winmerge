/* Shared definitions for GNU DIFF
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

#pragma once

#include "system.h"
#include <ctype.h>
#include <stdio.h>

#ifdef NOMINMAX
#undef min
#undef max
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PR_FILE_NAME
#if defined(__MSDOS__) || defined(__NT__) || defined(WIN32)
#define PR_FILE_NAME "pr"
#else
#define PR_FILE_NAME "/bin/pr"
#endif
#endif

#ifdef _MSC_VER
#define DECL_TLS __declspec(thread)
#else
#define DECL_TLS __thread
#endif

#define TAB_WIDTH 8

/* Variables for command line options */

#ifndef GDIFF_MAIN
#define EXTERN extern DECL_TLS 
#else
#define EXTERN DECL_TLS
#endif

enum output_style {
	
  // NOTE: these values are stored in the user's Registry - don't change their value !!
  //   (see enum DiffOutputType in Src/CompareOptions.h)
  /* Default output style.  */
  OUTPUT_NORMAL = 0,
  /* Output the differences with lines of context before and after (-c).  */
  OUTPUT_CONTEXT = 1,
  /* Output the differences in a unified context diff format (-u). */
  OUTPUT_UNIFIED = 2,
  /* Output the differences as commands suitable for `ed' (-e).  */
#if 0
  OUTPUT_ED = 3,
  /* Output the diff as a forward ed script (-f).  */
  OUTPUT_FORWARD_ED = 4,
  /* Like -f, but output a count of changed lines in each "command" (-n). */
  OUTPUT_RCS = 5,
  /* Output merged #ifdef'd file (-D).  */
  OUTPUT_IFDEF = 6,
  /* Output sdiff style (-y).  */
  OUTPUT_SDIFF = 7,
#endif
  /* Output html style.  */
  OUTPUT_HTML = 8
};

/* True for output styles that are robust,
   i.e. can handle a file that ends in a non-newline.  */
#define ROBUST_OUTPUT_STYLE(S) ((S)>=0) // ((S) != OUTPUT_ED && (S) != OUTPUT_FORWARD_ED)

EXTERN int output_style;

/* Nonzero if output cannot be generated for identical files.  */
EXTERN int no_diff_means_no_output;

/* Number of lines of context to show in each set of diffs.
   This is zero when context is not to be shown.  */
EXTERN int      context;

/* Consider all files as text files (-a).
   Don't interpret codes over 0177 as implying a "binary file".  */
EXTERN int	always_text_flag;

/* Number of lines to keep in identical prefix and suffix.  */
EXTERN int      horizon_lines;

/* Ignore changes in horizontal white space (-b).  */
EXTERN int      ignore_space_change_flag;

/* Ignore end of line differences (at least between UNIX & DOS */
EXTERN int      ignore_eol_diff;

/* Ignore all horizontal white space (-w).  */
EXTERN int      ignore_all_space_flag;

/* Ignore changes that affect only blank lines (-B).  */
EXTERN int      ignore_blank_lines_flag;

/* WinMerge moved block code */
EXTERN int moved_blocks_flag;

/* 1 if lines may match even if their lengths are different.
   This depends on various options.  */
EXTERN int      length_varies;

/* 1 if files may match even if their contents are not byte-for-byte identical.
   This depends on various options.  */
EXTERN int      ignore_some_changes;

/* Ignore differences in case of letters (-i).  */
EXTERN int      ignore_case_flag;

/* Ignore differences in case of numbers.  */
EXTERN int      ignore_numbers_flag;

/* File labels for `-c' output headers (-L).  */
EXTERN char *file_label[2];

/* Say only whether files differ, not how (-q).  */
EXTERN int 	no_details_flag;

/* Report files compared that match (-s).
   Normally nothing is output when that happens.  */
EXTERN int      print_file_same_flag;

/* character that ends a line.  Currently this is always `\n'.  */
EXTERN char     line_end_char;

/* Output the differences with exactly 8 columns added to each line
   so that any tabs in the text line up properly (-T).  */
EXTERN int	tab_align_flag;

/* Expand tabs in the output so the text lines up properly
   despite the characters added to the front of each line (-t).  */
EXTERN int	tab_expand_flag;

/* In directory comparison, specify file to start with (-S).
   All file names less than this name are ignored.  */
EXTERN char	*dir_start_file;

/* If a file is new (appears in only one dir)
   include its entire contents (-N).
   Then `patch' would create the file with appropriate contents.  */
EXTERN int	entire_new_file_flag;

/* If a file is new (appears in only the second dir)
   include its entire contents (-P).
   Then `patch' would create the file with appropriate contents.  */
EXTERN int	unidirectional_new_file_flag;

/* Pipe each file's output through pr (-l).  */
EXTERN int	paginate_flag;

enum line_class {
  /* Lines taken from just the first file.  */
  OLD,
  /* Lines taken from just the second file.  */
  NEW,
  /* Lines common to both files.  */
  UNCHANGED,
  /* A hunk containing both old and new lines (line groups only).  */
  CHANGED
};

/* Line group formats for old, new, unchanged, and changed groups.  */
EXTERN char *group_format[CHANGED + 1];

/* Line formats for old, new, and unchanged lines.  */
EXTERN char *line_format[UNCHANGED + 1];

/* If using OUTPUT_SDIFF print extra information to help the sdiff filter. */
EXTERN int sdiff_help_sdiff;

/* Tell OUTPUT_SDIFF to show only the left version of common lines. */
EXTERN int sdiff_left_only;

/* Tell OUTPUT_SDIFF to not show common lines. */
EXTERN int sdiff_skip_common_lines;

/* The half line width and column 2 offset for OUTPUT_SDIFF.  */
EXTERN unsigned sdiff_half_width;
EXTERN unsigned sdiff_column2_offset;

/* String containing all the command options diff received,
   with spaces between and at the beginning but none at the end.
   If there were no options given, this string is empty.  */
EXTERN char *	switch_string;

/* Nonzero means use heuristics for better speed.  */
EXTERN int	heuristic;

/* Name of program the user invoked (for error messages).  */
EXTERN char *	program;

/* The result of comparison is an "edit script": a chain of `struct change'.
   Each `struct change' represents one place where some lines are deleted
   and some are inserted.

   LINE0 and LINE1 are the first affected lines in the two files (origin 0).
   DELETED is the number of lines deleted here from file 0.
   INSERTED is the number of lines inserted here in file 1.

   If DELETED is 0 then LINE0 is the number of the line before
   which the insertion was done; vice versa for INSERTED and LINE1.  */

struct change
{
  struct change *link;		/* Previous or next edit command  */
  int inserted;			/* # lines of file 1 changed here.  */
  int deleted;			/* # lines of file 0 changed here.  */
  int line0;			/* Line number of 1st deleted line.  */
  int line1;			/* Line number of 1st inserted line.  */
  char ignore;			/* Flag used in context.c */
  char trivial;                 /* Flag if change is trivial (ignored blanks or filtered regexps) */
  int match0;                   /* side0 matching line for line 1 */ /* WinMerge moved block code */
  int match1;                   /* side1 matching line for line 0 */ /* WinMerge moved block code */
};

/* Structures that describe the input files.  */

/* Data on one input file being compared.  */

struct file_data {
    int             desc;	/* File descriptor  */
    char const      *name;	/* File name  */
#ifdef _WIN32
    struct _stat64  stat;	/* File status from fstat()  */
#else
    struct stat     stat;	/* File status from fstat()  */
#endif
    int             dir_p;	/* nonzero if file is a directory  */

    /* Buffer in which text of file is read.  */
    char HUGE *	    buffer;
    /* Allocated size of buffer.  */
    FSIZE	    bufsize;
    /* Number of valid characters now in the buffer. */
    FSIZE	    buffered_chars;

    /* Array of pointers to lines in the file.  */
    char const HUGE **linbuf;

    /* linbuf_base <= buffered_lines <= valid_lines <= alloc_lines.
       linebuf[linbuf_base ... buffered_lines - 1] are possibly differing.
       linebuf[linbuf_base ... valid_lines - 1] contain valid data.
       linebuf[linbuf_base ... alloc_lines - 1] are allocated.  */
    int linbuf_base, buffered_lines, valid_lines, alloc_lines;

    /* Pointer to end of prefix of this file to ignore when hashing. */
    char const HUGE *prefix_end;

    /* Count of lines in the prefix.
       There are this many lines in the file before linbuf[0].  */
    int prefix_lines;

    /* Pointer to start of suffix of this file to ignore when hashing. */
    char const HUGE *suffix_begin;

    /* Vector, indexed by line number, containing an equivalence code for
       each line.  It is this vector that is actually compared with that
       of another file to generate differences. */
    int		   *equivs;

    /* Vector, like the previous one except that
       the elements for discarded lines have been squeezed out.  */
    int		   *undiscarded;

    /* Vector mapping virtual line numbers (not counting discarded lines)
       to real ones (counting those lines).  Both are origin-0.  */
    int		   *realindexes;

    /* Total number of nondiscarded lines. */
    int		    nondiscarded_lines;

    /* Vector, indexed by real origin-0 line number,
       containing 1 for a line that is an insertion or a deletion.
       The results of comparison are stored here.  */
    char	   *changed_flag;

    /* 1 if file ends in a line with no final newline. */
    int		    missing_newline;

    /* 1 more than the maximum equivalence value used for this or its
       sibling file. */
    int equiv_max;

    /* text stats for WinMerge */
    int count_crlfs, count_crs, count_lfs, count_zeros;
};

/* Describe the two files currently being compared.  */

EXTERN struct file_data files[2];

/* Stdio stream to output diffs to.  */

EXTERN FILE *outfile;

/* Declare various functions.  */

/* analyze.c */
/* WinMerge: add last two params */
struct change * diff_2_files (struct file_data[], int, int *, int, int*);
void moved_block_analysis(struct change ** pscript, struct file_data fd[]);

/* context.c */
void print_context_header (struct file_data[], int);
void print_context_script (struct change *, int);

/* diff.c */
int excluded_filename (char const *);

/* dir.c */
//int diff_dirs (CDiffContext*);

/* ed.c */
void print_ed_script (struct change *);
void pr_forward_ed_script (struct change *);

/* ifdef.c */
void print_ifdef_script (struct change *);

/* io.c */
/* WinMerge: add last pointer param */
int read_files (struct file_data[], int, int *);
int sip (struct file_data *, int);
void slurp (struct file_data *);

/* normal.c */
void print_normal_script (struct change *);

/* rcs.c */
void print_rcs_script (struct change *);

/* side.c */
void print_sdiff_script (struct change *);

/* util.c */
void *xmalloc (size_t);
void *xrealloc (void *, size_t);
char *concat (char const *, char const *, char const *);
char *dir_file_pathname (char const *, char const *);
int change_letter (int, int);
int line_cmp (char const HUGE *, size_t, char const HUGE *, size_t);
int translate_line_number (struct file_data const *, int);
struct change *find_change (struct change *);
struct change *find_reverse_change (struct change *);
void analyze_hunk (struct change *, int *, int *, int *, int *, int *, int *, const struct file_data fd[]);
void begin_output (void);
void debug_script (struct change *);
void error (char const *, char const *, char const *);
void fatal (char const *);
void finish_output (void);
void message (char const *, char const *, char const *);
void message5 (char const *, char const *, char const *, char const *, char const *);
void output_1_line (char const HUGE *, char const HUGE *, char const *, char const *);
void perror_with_name (char const *);
void pfatal_with_name (char const *);
void print_1_line (char const *, char const HUGE * const *);
void print_message_queue (void);
void print_number_range (int, struct file_data *, int, int);
void print_script (struct change *, struct change * (*) (struct change *), void (*) (struct change *));
void setup_output (char const *, char const *, int);
void translate_range (struct file_data const *, int, int, int *, int *);
void cleanup_file_buffers(struct file_data fd[]);

/* version.c */
extern char const version_string[];

#ifdef _WIN32
/* mystat.cpp */
int myfstat(int fd, struct _stat64 *buf);
int mywstat(const wchar_t *filename, struct _stat64 *buf);
#else
#define myfstat fstat
#endif

#ifdef __cplusplus
#undef HUGE
}
#endif

