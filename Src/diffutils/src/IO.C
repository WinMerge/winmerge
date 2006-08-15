/* File I/O for GNU DIFF.
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

#include "diff.h"

// reduce some noise produced with the MSVC compiler
#if defined (_AFXDLL)
#pragma warning(disable : 4131 4013 4090)
#endif


/* Rotate a value n bits to the left. */
#define UINT_BIT (sizeof (unsigned) * CHAR_BIT)
#define ROL(v, n) ((v) << (n) | (v) >> (UINT_BIT - (n)))

/* Given a hash value and a new character, return a new hash value. */
#define HASH(h, c) ((c) + ROL (h, 7))

/* Guess remaining number of lines from number N of lines so far,
   size S so far, and total size T.  */
#define GUESS_LINES(n,s,t) (((t) - (s)) / ((n) < 10 ? 32 : (s) / ((n)-1)) + 5)

/* Type used for fast prefix comparison in find_identical_ends.  */
typedef unsigned word;

/* Lines are put into equivalence classes (of lines that match in line_cmp).
   Each equivalence class is represented by one of these structures,
   but only while the classes are being computed.
   Afterward, each class is represented by a number.  */
struct equivclass
{
  int next;	/* Next item in this bucket. */
  unsigned hash;	/* Hash of lines in this class.  */
  char const HUGE *line;	/* A line that fits this class. */
  size_t length;	/* The length of that line.  */
};

/* Hash-table: array of buckets, each being a chain of equivalence classes.  */
static int *buckets;
  
/* Number of buckets in the hash table array. */
static int nbuckets;

/* Array in which the equivalence classes are allocated.
   The bucket-chains go through the elements in this array.
   The number of an equivalence class is its index in this array.  */
static struct equivclass HUGE *equivs;

/* Index of first free element in the array `equivs'.  */
static int equivs_index;

/* Number of elements allocated in the array `equivs'.  */
static int equivs_alloc;

static void find_and_hash_each_line PARAMS((struct file_data *));
static void find_identical_ends PARAMS((struct file_data[]));
static void prepare_text_end PARAMS((struct file_data *));

/* Check for binary files and compare them for exact identity.  */

/* Return 1 if BUF contains a non text character.
   SIZE is the number of characters in BUF.  */

#define binary_file_p(buf, size) (size != 0 && memchr (buf, '\0', size) != 0)

/* Get ready to read the current file.
   Return nonzero if SKIP_TEST is zero,
   and if it appears to be a binary file.  */

int
sip (current, skip_test)
     struct file_data *current;
     int skip_test;
{
  /* If we have a nonexistent file at this stage, treat it as empty.  */
  if (current->desc < 0)
    {
      /* Leave room for a sentinel.  */
      current->buffer = xmalloc (sizeof (word));
      current->bufsize = sizeof (word);
      current->buffered_chars = 0;
    }
  else
    {
      current->bufsize = current->buffered_chars
        = STAT_BLOCKSIZE (current->stat);

      if (S_ISREG (current->stat.st_mode))
      /* Get the size out of the stat block.
         Allocate enough room for appended newline and sentinel.
         Allocate at least one block, to prevent overrunning the buffer
         when comparing growing binary files.  */
      current->bufsize = max (current->bufsize,
        current->stat.st_size + sizeof (word) + 1);

#ifdef __MSDOS__
      if ((current->buffer = (char HUGE *) farmalloc (current->bufsize)) == NULL)
         fatal ("far memory exhausted");
#else
      current->buffer = xmalloc (current->bufsize);
#endif /*__MSDOS__*/

      if (skip_test)
        current->buffered_chars = 0;
      else
        {
          /* Check first part of file to see if it's a binary file.  */
          current->buffered_chars = read (current->desc,
            current->buffer,
#ifdef __MSDOS__
            (unsigned int)
#endif /*__MSDOS__*/
            current->buffered_chars);
          if (current->buffered_chars == -1)
            pfatal_with_name (current->name);
          return binary_file_p (current->buffer, current->buffered_chars);
        }
    }
  
  return 0;
}

/* Slurp the rest of the current file completely into memory.  */

void
slurp (current)
     struct file_data *current;
{
  size_t cc;

  if (current->desc < 0)
    /* The file is nonexistent.  */
    ;
  else if (S_ISREG (current->stat.st_mode))
    {
      /* It's a regular file; slurp in the rest all at once.  */
#ifdef __MSDOS__
      /* read inside of segment boundaries */
      while ((current->stat.st_size - current->buffered_chars) > 0)
      {
        unsigned nr;
        char HUGE *rp;

        rp = current->buffer + current->buffered_chars;
        nr = 0 - (unsigned)((long)rp & 0x0000FFFF);
        if (nr == 0)
          nr = 0xFFF0;
        nr = read (current->desc, rp, nr);
        if (nr == 0)
          break;
        if (nr == 0xFFFF)
          pfatal_with_name (current->name);
        current->buffered_chars += nr ;
      }
#else
      cc = current->stat.st_size - current->buffered_chars;
      if (cc)
        {
          cc = read (current->desc,
             current->buffer + current->buffered_chars,
             cc);
          if (cc == -1)
            pfatal_with_name (current->name);
          current->buffered_chars += cc;
        }
#endif /*__MSDOS__*/
    }
      /* It's not a regular file; read it, growing the buffer as needed.  */
  else if (always_text_flag || current->buffered_chars != 0)
    {
      for (;;)
        {
          if (current->buffered_chars == current->bufsize)
            {
#ifdef __MSDOS__
              current->bufsize += 4096;
              current->buffer = (char HUGE *) farrealloc (current->buffer, current->bufsize);
#else
              current->bufsize = current->bufsize * 2;
              current->buffer = xrealloc (current->buffer, current->bufsize);
#endif /*__MSDOS__*/
            }
          cc = read (current->desc,
          current->buffer + current->buffered_chars,
          current->bufsize - current->buffered_chars);
          if (cc == 0)
            break;
          if (cc == -1)
            pfatal_with_name (current->name);
          current->buffered_chars += cc;
        }
#ifndef __MSDOS__
      /* Allocate just enough room for appended newline and sentinel.  */
      current->bufsize = current->buffered_chars + sizeof (word) + 1;
      current->buffer = xrealloc (current->buffer, current->bufsize);
#endif /*!__MSDOS__*/
    }
}

static int
ISWSPACE (char ch)
{
  return ch==' ' || ch=='\t';
}

/* Split the file into lines, simultaneously computing the equivalence class for
   each line. */
static void
find_and_hash_each_line (current)
     struct file_data *current;
{
  unsigned h;
  unsigned char const HUGE *p = (unsigned char const HUGE *) current->prefix_end;
  unsigned char c;
  int i, *bucket;
  size_t length;

  /* Cache often-used quantities in local variables to help the compiler.  */
  char const HUGE **linbuf = current->linbuf;
  int alloc_lines = current->alloc_lines;
  int line = 0;
  int linbuf_base = current->linbuf_base;
  int *cureqs = (int *) xmalloc (alloc_lines * sizeof (int));
  struct equivclass HUGE *eqs = equivs;
  int eqs_index = equivs_index;
  int eqs_alloc = equivs_alloc;
  char const HUGE *suffix_begin = current->suffix_begin;
  char const HUGE *bufend = current->buffer + current->buffered_chars;
  char const HUGE *incomplete_tail
    = current->missing_newline && ROBUST_OUTPUT_STYLE (output_style)
      ? bufend : (char const HUGE *) 0;
  int varies = length_varies;

  /* prepare_text_end put a zero word at the end of the buffer, 
  so we're not in danger of overrunning the end of the file */

  while ((char const HUGE *) p < suffix_begin)
    {
      char const HUGE *ip = (char const HUGE *) p;

      /* Compute the equivalence class (hash) for this line.  */

      h = 0;


      /*
     loops advance pointer to eol (end of line)
     respecting UNIX (\r), MS-DOS/Windows (\r\n), and MAC (\r) eols
     Normally eol characters are hashed
     If ignore_eol_diff option is set, eol characters are not hashed
     and the eol characters are removed from line as well, in code
     further down (after hashing_done label)
      */

      /* Hash this line until we find a newline. */
      if (ignore_case_flag)
        {
          if (ignore_all_space_flag)
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ignore_eol_diff && (c=='\r' || c=='\n'))
                  continue;
                if (! ISWSPACE (c))
                  h = HASH (h, isupper (c) ? tolower (c) : c);
              }
          else if (ignore_space_change_flag)
            /* Note that \r must be hashed (if !ignore_eol_diff) */
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ignore_eol_diff && (c=='\r' || c=='\n'))
                  continue;
                if (ISWSPACE (c))
                  {
                    /* skip whitespace after whitespace */
                    while (ISWSPACE (c = *p++))
                      ;
                    if (c=='\n')
                      {
                        goto hashing_done; /* never hash trailing \n */
                      }
                    else if (c == '\r')
                      {
                        /*
                            \r must be hashed if !ignore_eol_diff
                            Also, we must always advance to end of line
                            which means we can only stop on \r if not
                            followed by \n
                        */
                        if (ignore_eol_diff)
                          {
                            if (*p == '\n') /* continue to LF after CR */
                              continue;
                            else
                              goto hashing_done;
                          }
                      }
                    else
                      {
                  /* runs of whitespace not ending line hashed as one space */
                        h = HASH (h, ' ');
                      }
                  }
                /* c is now the first non-space.  */
                /* c can be a \r (CR) if !ignore_eol_diff */
                h = HASH (h, isupper (c) ? tolower (c) : c);
                if (c == '\r' && *p != '\n')
                  goto hashing_done;
              }
          else
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ignore_eol_diff && (c=='\r' || c=='\n'))
                  continue;
                h = HASH (h, isupper (c) ? tolower (c) : c);
              }
        }
      else
        {
          if (ignore_all_space_flag)
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ignore_eol_diff && (c=='\r' || c=='\n'))
                  continue;
                if (! ISWSPACE (c))
                  h = HASH (h, c);
              }
          else if (ignore_space_change_flag)
            /* Note that \r must be hashed (if !ignore_eol_diff) */
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ignore_eol_diff && (c=='\r' || c=='\n'))
                  continue;
                if (ISWSPACE (c))
                  {
                    /* skip whitespace after whitespace */
                    while (ISWSPACE (c = *p++))
                      ;
                    if (c=='\n')
                      {
                        goto hashing_done; /* never hash trailing \n */
                      }
                    else if (c == '\r')
                      {
                        /*
                            \r must be hashed if !ignore_eol_diff
                            Also, we must always advance to end of line
                            which means we can only stop on \r if not
                            followed by \n
                        */
                        if (ignore_eol_diff)
                          {
                            if (*p == '\n') /* continue to LF after CR */
                              continue;
                            else
                              goto hashing_done;
                          }
                      }
                    else
                      {
                  /* runs of whitespace not ending line hashed as one space */
                        h = HASH (h, ' ');
                      }
                  }
                /* c is now the first non-space.  */
                /* c can be a \r (CR) if !ignore_eol_diff */
                h = HASH (h, c);
                if (c == '\r' && *p != '\n')
                  goto hashing_done;
              }
          else
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ignore_eol_diff && (c=='\r' || c=='\n'))
                  continue;
                h = HASH (h, c);
              }
        }
hashing_done:;

      bucket = &buckets[h % nbuckets];
      length = (char const HUGE *) p - ip - ((char const HUGE *) p == incomplete_tail);
      if (ignore_eol_diff)
        {
          /* Remove all eols characters and adjust line length */
          if (length>1 && p[-2]=='\r' && p[-1]=='\n')
            {
              ++current->count_crlfs;
              ((char HUGE *)p)[-2] = 0;
              length -= 2;
            }
          else if (p[-1] == '\n' || p[-1] == '\r')
            {
              if (p[-1] == '\n')
                ++current->count_lfs;
              else
                ++current->count_crs;
              ((char HUGE *)p)[-1] = 0;
              --length;
            }
        }
      for (i = *bucket;  ;  i = eqs[i].next)
        if (!i)
          {
            /* Create a new equivalence class in this bucket. */
            i = eqs_index++;
            if (i == eqs_alloc)
#ifdef __MSDOS__
              if ((eqs = (struct equivclass HUGE *) farrealloc (eqs, (long) (eqs_alloc*=2) * sizeof(*eqs))) == NULL)
                fatal ("far memory exhausted");
#else
              eqs = (struct equivclass *)
                xrealloc (eqs, (eqs_alloc*=2) * sizeof(*eqs));
#endif /*__MSDOS__*/
            eqs[i].next = *bucket;
            eqs[i].hash = h;
            eqs[i].line = ip;
            eqs[i].length = length;
            *bucket = i;
            break;
          }
        /* "line_cmp" changed to "lines_differ" by diffutils 2.8.1 */
        else if (eqs[i].hash == h
           && (eqs[i].length == length || varies)
           && ! line_cmp (eqs[i].line, eqs[i].length, ip, length))
          /* Reuse existing equivalence class.  */
            break;

      /* Maybe increase the size of the line table. */
      if (line == alloc_lines)
        {
          /* Double (alloc_lines - linbuf_base) by adding to alloc_lines.  */
          alloc_lines = 2 * alloc_lines - linbuf_base;
          cureqs = (int *) xrealloc (cureqs, alloc_lines * sizeof (*cureqs));
          linbuf = (char const HUGE **) xrealloc ((void *)(linbuf + linbuf_base),
                     (alloc_lines - linbuf_base)
                     * sizeof (*linbuf))
             - linbuf_base;
        }
      linbuf[line] = ip;
      cureqs[line] = i;
      ++line;
    }

  current->buffered_lines = line;

  for (i = 0;  ;  i++)
    {
      /* Record the line start for lines in the suffix that we care about.
         Record one more line start than lines,
         so that we can compute the length of any buffered line.  */
      if (line == alloc_lines)
        {
          /* Double (alloc_lines - linbuf_base) by adding to alloc_lines.  */
          alloc_lines = 2 * alloc_lines - linbuf_base;
          linbuf = (char const HUGE **) xrealloc ((void *)(linbuf + linbuf_base),
                     (alloc_lines - linbuf_base)
                     * sizeof (*linbuf))
             - linbuf_base;
        }
      linbuf[line] = (char const HUGE *) p;
    
     if ((char const HUGE *) p == bufend)
        {
          linbuf[line]  -=  (char const HUGE *) p == incomplete_tail;
          break;
        }

      if (context <= i && no_diff_means_no_output)
        break;

      line++;

      while (p[0] != '\n' && (p[0] != '\r' || p[1] == '\n'))
         p++;
      p++;
    }

  /* Done with cache in local variables.  */
  current->linbuf = linbuf;
  current->valid_lines = line;
  current->alloc_lines = alloc_lines;
  current->equivs = cureqs;
  equivs = eqs;
  equivs_alloc = eqs_alloc;
  equivs_index = eqs_index;
}

/* Prepare the end of the text.  Make sure it's initialized.
   Make sure text ends in a newline,
   but remember that we had to add one unless -B is in effect.  */

static void
prepare_text_end (current)
     struct file_data *current;
{
  FSIZE buffered_chars = current->buffered_chars;
  char HUGE *p = current->buffer;

  if (buffered_chars == 0 || p[buffered_chars - 1] == '\n' || p[buffered_chars - 1] == '\r')
    current->missing_newline = 0;
  else
    {
      p[buffered_chars++] = '\n';
      current->buffered_chars = buffered_chars;
      current->missing_newline = p[buffered_chars-2] != '\r' && ! ignore_blank_lines_flag;
    }
  
  /* Don't use uninitialized storage when planting or using sentinels.  */
  if (p)
    bzero (p + buffered_chars, sizeof (word));
}

/* Given a vector of two file_data objects, find the identical
   prefixes and suffixes of each object. */

static void
find_identical_ends (filevec)
     struct file_data filevec[];
{
  word HUGE *w0, HUGE *w1;
  char HUGE *p0, HUGE *p1, HUGE *buffer0, HUGE *buffer1;
  char const HUGE *end0, HUGE *beg0;
  char const HUGE **linbuf0, HUGE **linbuf1;
  int i, lines;
  FSIZE n0, n1;
  FSIZE tem;
  FSIZE alloc_lines0, alloc_lines1;
  int buffered_prefix, prefix_count, prefix_mask;
  int ttt;

  slurp (&filevec[0]);
  if (filevec[0].desc != filevec[1].desc)
    slurp (&filevec[1]);
  else
    {
      filevec[1].buffer = filevec[0].buffer;
      filevec[1].bufsize = filevec[0].bufsize;
      filevec[1].buffered_chars = filevec[0].buffered_chars;
    }
  for (i = 0; i < 2; i++)
    prepare_text_end (&filevec[i]);

  /* Find identical prefix.  */

  p0 = buffer0 = filevec[0].buffer;
  p1 = buffer1 = filevec[1].buffer;

  n0 = filevec[0].buffered_chars;
  n1 = filevec[1].buffered_chars;

  if (p0 == p1)
    /* The buffers are the same; sentinels won't work.  */
    p0 = p1 += n1;
  else
    {
      /* Insert end sentinels, in this case characters that are guaranteed
         to make the equality test false, and thus terminate the loop.  */

      if (n0 < n1)
        p0[n0] = (char)(~p1[n0]);
      else
        p1[n1] = (char)(~p0[n1]);

      /* Loop until first mismatch, or to the sentinel characters.  */

      /* Compare a word at a time for speed.  */
      w0 = (word *) p0;
      w1 = (word *) p1;
      while (*w0++ == *w1++)
        ;
      --w0, --w1;

      /* Do the last few bytes of comparison a byte at a time.  */
      p0 = (char *) w0;
      p1 = (char *) w1;
      while (*p0++ == *p1++)
        ;
      --p0, --p1;

      /* Don't mistakenly count missing newline as part of prefix. */
      if (ROBUST_OUTPUT_STYLE (output_style)
        && (buffer0 + n0 - filevec[0].missing_newline < p0)
        !=
        (buffer1 + n1 - filevec[1].missing_newline < p1))
        --p0, --p1;
    }

  /* Now P0 and P1 point at the first nonmatching characters.  */

  /* Skip back to last line-beginning in the prefix,
     and then discard up to HORIZON_LINES lines from the prefix.  */
  i = horizon_lines;
  /* This loop can be done in one line, but isn't not easy to read, so unrolled into simple statements */
  while (p0 != buffer0)
    {
      /* we know p0[-1] == p1[-1], but maybe p0[0] != p1[0] */
      int linestart=0;
      if (p0[-1] == '\n')
        linestart=1;
      /* only count \r if not followed by a \n on either side */
      if (p0[-1] == '\r' && p0[0] != '\n' && p1[0] != '\n')
        linestart=1;
      if (linestart && !(i--))
        break;
    --p0, --p1;
    }

  /* Record the prefix.  */
  filevec[0].prefix_end = p0;
  filevec[1].prefix_end = p1;

  /* Find identical suffix.  */

  /* P0 and P1 point beyond the last chars not yet compared.  */
  p0 = buffer0 + n0;
  p1 = buffer1 + n1;

  if (! ROBUST_OUTPUT_STYLE (output_style)
      || filevec[0].missing_newline == filevec[1].missing_newline)
    {
      end0 = p0;  /* Addr of last char in file 0.  */

      /* Get value of P0 at which we should stop scanning backward:
         this is when either P0 or P1 points just past the last char
         of the identical prefix.  */
      beg0 = filevec[0].prefix_end + (n0 < n1 ? 0 : n0 - n1);

      /* Scan back until chars don't match or we reach that point.  */
      while (p0 != beg0)
        if (*--p0 != *--p1)
          {
            /* Point at the first char of the matching suffix.  */
            ++p0, ++p1;
            beg0 = p0;
            break;
          }

      /* Are we at a line-beginning in both files?  If not, add the rest of
         this line to the main body.  Discard up to HORIZON_LINES lines from
         the identical suffix.  Also, discard one extra line,
         because shift_boundaries may need it.  */
      i = horizon_lines + !((buffer0 == p0 || p0[-1] == '\n' || (p0[-1] == '\r' && p0[0] != '\n'))
          &&
          (buffer1 == p1 || p1[-1] == '\n' || (p1[-1] == '\r' && p1[0] != '\n')));
      while (i-- && p0 != end0)
        while (*p0++ != '\n' && (p0[-1] != '\r' || p0[0] == '\n'))
          ;

      p1 += p0 - (char HUGE *)beg0;
    }

  /* Record the suffix.  */
  filevec[0].suffix_begin = p0;
  filevec[1].suffix_begin = p1;

  /* Calculate number of lines of prefix to save.

     prefix_count == 0 means save the whole prefix;
     we need this with for options like -D that output the whole file.
     We also need it for options like -F that output some preceding line;
     at least we will need to find the last few lines,
     but since we don't know how many, it's easiest to find them all.

     Otherwise, prefix_count != 0.  Save just prefix_count lines at start
     of the line buffer; they'll be moved to the proper location later.
     Handle 1 more line than the context says (because we count 1 too many),
     rounded up to the next power of 2 to speed index computation.  */

  if (no_diff_means_no_output && ! function_regexp_list)
    {
      for (prefix_count = 1;  prefix_count < context + 1;  prefix_count *= 2)
        ;
      prefix_mask = prefix_count - 1;
      ttt = p0 - (char HUGE *)filevec[0].prefix_end;
      alloc_lines0
        = prefix_count
        + GUESS_LINES (0, 0, ttt)
        + context;
    }
  else
    {
      prefix_count = 0;
      prefix_mask = ~0;
      alloc_lines0 = GUESS_LINES (0, 0, n0);
    }

  lines = 0;
  linbuf0 = (char const HUGE **) xmalloc (alloc_lines0 * sizeof (*linbuf0));

  /* If the prefix is needed, find the prefix lines.  */
  if (! (no_diff_means_no_output
   && filevec[0].prefix_end == p0
   && filevec[1].prefix_end == p1))
    {
      p0 = buffer0;
      end0 = filevec[0].prefix_end;
      while (p0 != end0)
        {
          int l = lines++ & prefix_mask;
          if ((FSIZE)l == alloc_lines0)
            linbuf0 = (char const HUGE **) xrealloc ((void *)linbuf0, (alloc_lines0 *= 2)
               * sizeof(*linbuf0));
          linbuf0[l] = p0;
          /* Perry/WinMerge (2004-01-05) altered original diffutils loop "while (*p0++ != '\n') ;" for other EOLs */
          while (1)
            {
              char ch = *p0++;
              /* stop at any eol, \n or \r or \r\n */
              if (ch == '\n') break;
              if (ch == '\r' && (p0==end0 || *p0!='\n')) break;
            }
        }
    }
  buffered_prefix = prefix_count && context < lines ? context : lines;

  /* Allocate line buffer 1.  */
  tem = prefix_count ? filevec[1].suffix_begin - buffer1 : n1;
  ttt = filevec[1].prefix_end - buffer1;
  alloc_lines1
    = (buffered_prefix
       + GUESS_LINES (lines, ttt, tem)
       + context);
  linbuf1 = (char const HUGE **) xmalloc (alloc_lines1 * sizeof (*linbuf1));

  if (buffered_prefix != lines)
    {
      /* Rotate prefix lines to proper location.  */
      for (i = 0;  i < buffered_prefix;  i++)
        linbuf1[i] = linbuf0[(lines - context + i) & prefix_mask];
      for (i = 0;  i < buffered_prefix;  i++)
        linbuf0[i] = linbuf1[i];
    }

  /* Initialize line buffer 1 from line buffer 0.  */
  for (i = 0; i < buffered_prefix; i++)
    linbuf1[i] = linbuf0[i] - buffer0 + buffer1;

  /* Record the line buffer, adjusted so that
     linbuf*[0] points at the first differing line.  */
  filevec[0].linbuf = linbuf0 + buffered_prefix;
  filevec[1].linbuf = linbuf1 + buffered_prefix;
  filevec[0].linbuf_base = filevec[1].linbuf_base = - buffered_prefix;
  filevec[0].alloc_lines = alloc_lines0 - buffered_prefix;
  filevec[1].alloc_lines = alloc_lines1 - buffered_prefix;
  filevec[0].prefix_lines = filevec[1].prefix_lines = lines;
}

/* Largest primes less than some power of two, for nbuckets.  Values range
   from useful to preposterous.  If one of these numbers isn't prime
   after all, don't blame it on me, blame it on primes (6) . . . */
static int const primes[] =
{
  509,
  1021,
  2039,
  4093,
  8191,
  16381,
  32749,
#if 32767 < INT_MAX
  65521,
  131071,
  262139,
  524287,
  1048573,
  2097143,
  4194301,
  8388593,
  16777213,
  33554393,
  67108859,     /* Preposterously large . . . */
  134217689,
  268435399,
  536870909,
  1073741789,
  2147483647,
#endif
  0
};

/* Given a vector of two file_data objects, read the file associated
   with each one, and build the table of equivalence classes.
   Return 1 if either file appears to be a binary file.
   If PRETEND_BINARY is nonzero, pretend they are binary regardless.  */
/* WinMerge: Add int * bin_file param for getting actual binary file
   If bin_file is given, then check both files for binary files,
   otherwise check second file only if first wasn't binary */
int
read_files (filevec, pretend_binary, bin_file)
     struct file_data filevec[];
     int pretend_binary;
     int * bin_file;
{
  int i;
  int skip_test = always_text_flag | pretend_binary;
  int appears_binary = 0;

  if (bin_file)
    *bin_file = 0;
  appears_binary = pretend_binary | sip (&filevec[0], skip_test);
  if (bin_file && appears_binary)
    {
      *bin_file = 1;
    }

  if (filevec[0].desc != filevec[1].desc)
    {
      if (bin_file)
        {
          appears_binary = pretend_binary | sip (&filevec[1], skip_test);
          if (appears_binary)
            *bin_file |= 0x2; // set second bit for second file
        }
      else
        appears_binary |= sip (&filevec[1], skip_test | appears_binary);
    }
  else
    {
      filevec[1].buffer = filevec[0].buffer;
      filevec[1].bufsize = filevec[0].bufsize;
      filevec[1].buffered_chars = filevec[0].buffered_chars;
    }
  if (appears_binary || (bin_file && *bin_file > 0))
    return 1;

  find_identical_ends (filevec);

  equivs_alloc = filevec[0].alloc_lines + filevec[1].alloc_lines + 1;
#ifdef __MSDOS__
  if ((equivs = (struct equivclass HUGE *) farmalloc ((long) equivs_alloc * sizeof(struct equivclass))) == NULL)
    fatal ("far memory exhausted");
#else
  equivs = (struct equivclass *) xmalloc (equivs_alloc * sizeof (struct equivclass));
#endif /*__MSDOS__*/
  /* Equivalence class 0 is permanently safe for lines that were not
     hashed.  Real equivalence classes start at 1. */
  equivs_index = 1;

  for (i = 0;  primes[i] < equivs_alloc / 3;  i++)
    if (! primes[i])
      abort ();
  nbuckets = primes[i];

  buckets = (int *) xmalloc (nbuckets * sizeof (*buckets));
  bzero (buckets, nbuckets * sizeof (*buckets));

  for (i = 0; i < 2; ++i)
    find_and_hash_each_line (&filevec[i]);

  filevec[0].equiv_max = filevec[1].equiv_max = equivs_index;

  free (equivs);
  free (buckets);

  return 0;
}
