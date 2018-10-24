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
#include <io.h>
#include <assert.h>

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

/** @brief Known Unicode encodings. */
enum UNICODESET
{
  NONE = 0,  /**< No unicode. */
  UCS2LE,    /**< UCS-2 / UTF-16 little endian. */
  UCS2BE,    /**< UCS-2 / UTF-16 big endian. */
  UTF8,      /**< UTF-8. */
  UCS4LE,    /**< UTF-32 little endian */
  UCS4BE,    /**< UTF-32 big-endian */
};

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
static DECL_TLS int *buckets;
  
/* Number of buckets in the hash table array. */
static DECL_TLS int nbuckets;

/* Array in which the equivalence classes are allocated.
   The bucket-chains go through the elements in this array.
   The number of an equivalence class is its index in this array.  */
static DECL_TLS struct equivclass HUGE *equivs;

/* Index of first free element in the array `equivs'.  */
static DECL_TLS int equivs_index;

/* Number of elements allocated in the array `equivs'.  */
static DECL_TLS int equivs_alloc;

static void find_and_hash_each_line (struct file_data *);
static void find_identical_ends (struct file_data[]);
static char *prepare_text_end (struct file_data *, short);
static enum UNICODESET get_unicode_signature(struct file_data *, int *pBomsize);

/* Check for binary files and compare them for exact identity.  */

/* Return 1 if BUF contains a non text character.
   SIZE is the number of characters in BUF.  */

#define binary_file_p(buf, size) (size != 0 && memchr (buf, '\0', size) != 0)

/** @brief Get unicode signature from file_data. */
static enum UNICODESET get_unicode_signature(struct file_data *current, int *pBomsize)
{
  // initialize to a pattern that differs everywhere from all possible unicode signatures
  unsigned long sig = 0x3F3F3F3F;
  // copy at most 4 bytes from buffer
  memcpy(&sig, current->buffer, min(current->buffered_chars, 4));
  // check for the two possible 4 bytes signatures
  int tmp;
  int *bomsize = pBomsize ? pBomsize : &tmp;
  
  if (sig == 0x0000FEFF)
    {
      *bomsize = 4;
      return UCS4LE;
    }
  if (sig == 0xFFFE0000)
    {
      *bomsize = 4;
      return UCS4BE;
    }
  // check for the only possible 3 bytes signature
  sig &= 0xFFFFFF;
  if (sig == 0xBFBBEF)
    {
      *bomsize = 3;
      return UTF8;
    }
  // check for the two possible 2 bytes signatures
  sig &= 0xFFFF;
  if (sig == 0xFEFF)
    {
      *bomsize = 2;
      return UCS2LE;
    }
  if (sig == 0xFFFE)
    {
      *bomsize = 2;
      return UCS2BE;
    }
  // none of the above checks has passed, so probably no unicode
  *bomsize = 0;
  return NONE;
}

/* Get ready to read the current file.
   Return nonzero if SKIP_TEST is zero,
   and if it appears to be a binary file.  */

int
sip (struct file_data *current, int skip_test)
{
  int isbinary = 0;
  /* If we have a nonexistent file (or NUL: device) at this stage, treat it as empty.  */
  if (current->desc < 0 || !(S_ISREG (current->stat.st_mode)))
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
          current->buffered_chars = _read (current->desc,
            current->buffer,
            (unsigned int)current->buffered_chars);
          if (current->buffered_chars == -1)
            pfatal_with_name (current->name);
          if (!get_unicode_signature(current, NULL))
            isbinary = binary_file_p(current->buffer, current->buffered_chars);
        }
    }
  
  return isbinary;
}

/* Slurp the rest of the current file completely into memory.  */

void
slurp (struct file_data *current)
{
  size_t cc;

  if (current->desc < 0)
    /* The file is nonexistent.  */
    ;
  else if (always_text_flag || current->buffered_chars != 0)
    {
      enum UNICODESET sig = get_unicode_signature(current, NULL);
      size_t alloc_extra
        = (1 << sig) & ((1 << UCS2LE) | (1 << UCS2BE) | (1 << UCS4LE) | (1 << UCS4BE))
          // some flavor of non octet encoded unicode?
          ? ~0U	// yes, allocate extra room for transcoding
          : 0U;	// no, allocate no extra room for transcoding

      for (;;)
        {
          if (current->buffered_chars == current->bufsize)
            {
              if (S_ISREG (current->stat.st_mode))
                {
              /* Get the size out of the stat block.
                 Allocate 50% extra room for a necessary transcoding to UTF-8.
                 Allocate enough room for appended newline and sentinel.
                 Allocate at least one block, to prevent overrunning the buffer
                 when comparing growing binary files. */
                  current->bufsize = max (current->bufsize,
                    (size_t)current->stat.st_size + (alloc_extra & (size_t)current->stat.st_size / 2) + sizeof (word) + 1);
                }
              else
                {
#ifdef __MSDOS__
                  current->bufsize += 4096;
#else
                  current->bufsize = current->bufsize * 2;
#endif /*__MSDOS__*/
                }
#ifdef __MSDOS__
              current->buffer = (char HUGE *) farrealloc (current->buffer, current->bufsize);
#else
              current->buffer = xrealloc (current->buffer, current->bufsize);
#endif /*__MSDOS__*/
            }
          unsigned int bytes_to_read = min((unsigned int)(current->bufsize - current->buffered_chars), INT_MAX);
          if (bytes_to_read == 0)
            break;
          cc = _read (current->desc,
                      current->buffer + current->buffered_chars,
                      bytes_to_read);
          if (cc == 0)
            break;
          if (cc == -1)
            pfatal_with_name (current->name);
          current->buffered_chars += cc;
        }
#ifndef __MSDOS__
      /* Allocate 50% extra room for a necessary transcoding to UTF-8.
         Allocate enough room for appended newline and sentinel. 
		 But don't reallocate if the buffer is already big enough */
	  FSIZE tmp_bufsize = current->buffered_chars + (alloc_extra & current->buffered_chars / 2) + sizeof (word) + 1;
	  if (tmp_bufsize > current->bufsize) 
	    { 
		  current->buffer = xrealloc (current->buffer, tmp_bufsize);
		  current->bufsize = tmp_bufsize;
	    }
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
find_and_hash_each_line (struct file_data *current)
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
      ? bufend : (char const HUGE *) NULL;
  int varies = length_varies;

  /* prepare_text_end put a zero word at the end of the buffer, 
  so we're not in danger of overrunning the end of the file */

  while ((char const HUGE *) p < suffix_begin)
    {
      char const HUGE *ip = (char const HUGE *) p;

      /* Compute the equivalence class (hash) for this line.  */

      h = 0;


      /* loops advance pointer to eol (end of line)
         respecting UNIX (\r), MS-DOS/Windows (\r\n), and MAC (\r) eols */

      /* Hash this line until we find a newline. */
      if (ignore_case_flag)
        {
          if (ignore_all_space_flag)
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (! ISWSPACE (c))
                  h = HASH (h, isupper (c) ? tolower (c) : c);
              }
          else if (ignore_space_change_flag)
            /* Note that \r must be hashed (if !ignore_eol_diff) */
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ISWSPACE (c))
                  {
                    /* skip whitespace after whitespace */
                    while (ISWSPACE (c = *p++))
                      ;
                    if (c == '\n')
                      {
                        goto hashing_done; /* never hash trailing \n */
                      }
                    else if (c != '\r')
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
                h = HASH (h, isupper (c) ? tolower (c) : c);
              }
        }
      else
        {
          if (ignore_all_space_flag)
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (! ISWSPACE (c))
                  h = HASH (h, c);
              }
          else if (ignore_space_change_flag)
            /* Note that \r must be hashed (if !ignore_eol_diff) */
            while ((c = *p++) != '\n' && (c != '\r' || *p == '\n'))
              {
                if (ISWSPACE (c))
                  {
                    /* skip whitespace after whitespace */
                    while (ISWSPACE (c = *p++))
                      ;
                    if (c == '\n')
                      {
                        goto hashing_done; /* never hash trailing \n */
                      }
                    else if (c != '\r')
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
                h = HASH (h, c);
              }
        }
hashing_done:;

      bucket = &buckets[h % nbuckets];
      length = (char const HUGE *) p - ip - ((char const HUGE *) p == incomplete_tail);
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

/* Convert any non octet encoded unicode text to UTF-8.
   Prepare the end of the text. Make sure it's initialized.
   Make sure text ends in a newline,
   but remember that we had to add one unless -B is in effect.
   Return effective start of text to be compared. */

# pragma warning(push)          // Saves the current warning state.
# pragma warning(disable:4244)  // Temporarily disables warning 4244: "conversion from 'int' to 'char', possible loss of data"
static char *
prepare_text_end (struct file_data *current, short side)
{
  FSIZE buffered_chars = current->buffered_chars;
  char *const p = current->buffer;
  char *r = p; // receives the return value
  char *q0, *t;
  int bomsize = 0;
  enum UNICODESET sig = get_unicode_signature(current, &bomsize);
  char *const u0 = p + bomsize;

  if (sig == UCS4LE)
    {
      FSIZE buffered_words = buffered_chars / 2;
      unsigned long *q1 = (unsigned long *)p + buffered_words / 2;
      buffered_chars += buffered_words;
      r = p + buffered_chars;
      while (--q1 >= (unsigned long *)u0) // exclude the BOM
        {
          unsigned long u = *q1;
          if (u >= 0x80000000)
            {
              *--r = '?';
            }
          else if (u >= 0x4000000)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0x80 + ((u >> 12) & 0x3F);
              *--r = 0x80 + ((u >> 18) & 0x3F);
              *--r = 0x80 + ((u >> 24) & 0x3F);
              *--r = 0xFC + (u >> 30);
            }
            else if (u >= 0x200000)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0x80 + ((u >> 12) & 0x3F);
              *--r = 0x80 + ((u >> 18) & 0x3F);
              *--r = 0xF8 + (u >> 24);
            }
            else if (u >= 0x10000)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0x80 + ((u >> 12) & 0x3F);
              *--r = 0xF0 + (char)(u >> 18);
            }
            else if (u >= 0x800)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0xE0 + (char)(u >> 12);
            }
            else if (u >= 0x80 || u == 0) // map NUL to 2 byte sequence so as to prevent it from confusing diff algorithm
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0xC0 + (char)(u >> 6);
            }
            else
            {
              *--r = (char)u;
            }
        }
    }
  else if (sig == UCS4BE)
    {
      FSIZE buffered_words = buffered_chars / 2;
      unsigned long *q = (unsigned long *)p + buffered_words / 2;
      buffered_chars += buffered_words;
      r = p + buffered_chars;
      while (--q >= (unsigned long *)u0) // exclude the BOM
        {
          unsigned long u =
          ((*q & 0x000000FF) << 24) |
          ((*q & 0x0000FF00) << 8) |
          ((*q & 0x00FF0000) >> 8) |
          ((*q & 0xFF000000) >> 24); // fix byte order
          if (u >= 0x80000000)
            {
              *--r = '?';
            }
          else if (u >= 0x4000000)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0x80 + ((u >> 12) & 0x3F);
              *--r = 0x80 + ((u >> 18) & 0x3F);
              *--r = 0x80 + ((u >> 24) & 0x3F);
              *--r = 0xFC + (u >> 30);
            }
          else if (u >= 0x200000)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0x80 + ((u >> 12) & 0x3F);
              *--r = 0x80 + ((u >> 18) & 0x3F);
              *--r = 0xF8 + (u >> 24);
            }
          else if (u >= 0x10000)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0x80 + ((u >> 12) & 0x3F);
              *--r = 0xF0 + (char)(u >> 18);
            }
          else if (u >= 0x800)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0xE0 + (char)(u >> 12);
            }
          else if (u >= 0x80 || u == 0) // map NUL to 2 byte sequence so as to prevent it from confusing diff algorithm
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0xC0 + (char)(u >> 6);
            }
          else
            {
              *--r = (char)u;
            }
        }
    }
  else if (sig == UCS2LE)
    {
      FSIZE buffered_words = buffered_chars / 2;
      unsigned short *q = (unsigned short *)p + buffered_words;
      buffered_chars += buffered_words;
      r = p + buffered_chars;
      while (--q >= (unsigned short *)u0) // exclude the BOM
        {
          unsigned short u = *q;
          if (u >= 0x800)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0xE0 + (u >> 12);
            }
          else if (u >= 0x80 || u == 0) // map NUL to 2 byte sequence so as to prevent it from confusing diff algorithm
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0xC0 + (u >> 6);
            }
          else
            {
              *--r = (char)u;
            }
        }
    }
  else if (sig == UCS2BE)
    {
      FSIZE buffered_words = buffered_chars / 2;
      unsigned short *q = (unsigned short *)p + buffered_words;
      buffered_chars += buffered_words;
      r = p + buffered_chars;
      while (--q >= (unsigned short *)u0) // exclude the BOM
        {
          unsigned short u = (*q << 8) | (*q >> 8); // fix byte order
          if (u >= 0x800)
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0x80 + ((u >> 6) & 0x3F);
              *--r = 0xE0 + (u >> 12);
            }
          else if (u >= 0x80 || u == 0) // map NUL to 2 byte sequence so as to prevent it from confusing diff algorithm
            {
              *--r = 0x80 + (u & 0x3F);
              *--r = 0xC0 + (u >> 6);
            }
          else
            {
              *--r = (char)u;
            }
        }
    }
  else if (sig == UTF8)
    {
      r = u0; // skip the BOM
    }

  if (buffered_chars == 0 || p[buffered_chars - 1] == '\n' || p[buffered_chars - 1] == '\r')
    current->missing_newline = 0;
  else
    {
      p[buffered_chars++] = '\n';
      current->missing_newline = 1;
      --current->count_lfs; // compensate for extra newline
    }

	current->buffered_chars = buffered_chars;

	/* Count line endings and map them to '\n' if ignore_eol_diff is set. */
	t = q0 = p + buffered_chars;
	while (q0 > r)
	{
		switch (*--t = *--q0)
		{
		case '\r':
			++current->count_crs;
			if (ignore_eol_diff)
				*t = '\n';
			break;
		case '\n':
			if (q0 > r && q0[-1] == '\r')
			{
				++current->count_crlfs;
				--current->count_crs; // compensate for bogus increment
				if (ignore_eol_diff)
					++t;
			}
			else
			{
				++current->count_lfs;
			}
			break;
		case '\0':
			++current->count_zeros;
			break;
		}
	}

  /* Don't use uninitialized storage when planting or using sentinels.  */
  bzero (p + buffered_chars, sizeof (word));
  return t;
}
# pragma warning(pop)           // Restores the warning state.

/* Given a vector of two file_data objects, find the identical
   prefixes and suffixes of each object. */

static void
find_identical_ends (struct file_data filevec[])
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

  if (filevec[0].desc != filevec[1].desc)
    {
      slurp (&filevec[0]);
      buffer0 = prepare_text_end (&filevec[0], 0);
      slurp (&filevec[1]);
      buffer1 = prepare_text_end (&filevec[1], 1);
    }
  else
    {
      slurp (&filevec[0]);
      buffer0 = prepare_text_end (&filevec[0], -1);
      filevec[1].buffer = filevec[0].buffer;
      filevec[1].bufsize = filevec[0].bufsize;
      filevec[1].buffered_chars = filevec[0].buffered_chars;
      buffer1 = buffer0;
    }

  /* Find identical prefix.  */

  p0 = buffer0;
  p1 = buffer1;

  n0 = filevec[0].buffered_chars - (buffer0 - filevec[0].buffer);
  n1 = filevec[1].buffered_chars - (buffer1 - filevec[1].buffer);

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

  if (no_diff_means_no_output)
    {
      for (prefix_count = 1;  prefix_count < context + 1;  prefix_count *= 2)
        ;
      prefix_mask = prefix_count - 1;
	  assert((p0 - (char HUGE *)filevec[0].prefix_end) < INT_MAX);
      ttt = (int)(p0 - (char HUGE *)filevec[0].prefix_end);
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
  ttt = (int)(filevec[1].prefix_end - buffer1);
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
  assert((alloc_lines0 - buffered_prefix) < INT_MAX);
  assert((alloc_lines1 - buffered_prefix) < INT_MAX);
  filevec[0].alloc_lines = (int)(alloc_lines0 - buffered_prefix);
  filevec[1].alloc_lines = (int)(alloc_lines1 - buffered_prefix);
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
read_files (struct file_data filevec[], int pretend_binary, int *bin_file)
{
  int i;
  int skip_test = always_text_flag | pretend_binary;
  int appears_binary = 0;

  if (bin_file != NULL)
    *bin_file = 0;
  appears_binary = pretend_binary | sip (&filevec[0], skip_test);
  if (bin_file!=NULL && appears_binary)
    {
      *bin_file = 1;
    }

  if (filevec[0].desc != filevec[1].desc)
    {
      if (bin_file!=NULL)
        {
          appears_binary = pretend_binary | sip (&filevec[1], skip_test);
          if (appears_binary)
            *bin_file |= 0x2; // set second bit for second file
        }
      else
        appears_binary |= sip (&filevec[1], skip_test | appears_binary);
    }
	
	// Are both files Open and Regular (no Pipes, Directories, Devices (e.g. NUL))
	if (filevec[0].desc < 0 || filevec[1].desc < 0 ||
		!(S_ISREG (filevec[0].stat.st_mode)) || !(S_ISREG (filevec[1].stat.st_mode))   )
	{
		assert(!S_ISCHR(filevec[0].stat.st_mode) || strcmp(filevec[0].name, "NUL")==0);
		assert(!S_ISCHR(filevec[1].stat.st_mode) || strcmp(filevec[1].name, "NUL")==0);
		return appears_binary;
	}

  if (appears_binary)
	{
		// Because of the way 3-way binary comparison works, both buffers need
		// to be exactly the same size.  It also makes sense if the buffers are
		// large enough to hold a large chunk of the file with each read(); 
		// within reason of course.  Note: if the buffers are too big, the 
		// multi-processor performance is degraded.
		
		// Note that one or both buffers already have some amount of data.

		const FSIZE tmax_reasonable = (1 << 19) -1;		// 2**19 bytes, about 524KB

		FSIZE tmax_bufsize = max ((size_t)filevec[0].stat.st_size, 
								  (size_t)filevec[1].stat.st_size);
		tmax_bufsize = min (tmax_bufsize, tmax_reasonable);
		
		FSIZE tmin_bufsize = max(filevec[0].buffered_chars, filevec[1].buffered_chars);
		tmax_bufsize = max (tmax_bufsize, tmin_bufsize);

		if (tmax_bufsize > filevec[0].bufsize)
		  {
			filevec[0].buffer = xrealloc (filevec[0].buffer, tmax_bufsize);
			filevec[0].bufsize = tmax_bufsize;
		  }
		if (filevec[0].desc != filevec[1].desc && tmax_bufsize > filevec[1].bufsize)
		  {
			filevec[1].buffer = xrealloc (filevec[1].buffer, tmax_bufsize);
			filevec[1].bufsize = tmax_bufsize;
		  }
	}
	  
  if (filevec[0].desc == filevec[1].desc)
	{
		// The files may be exactly the same file.  Give them the same buffer, etc.
		assert( filevec[1].buffer == NULL );

		filevec[1].buffer = filevec[0].buffer;
		filevec[1].bufsize = filevec[0].bufsize;
		filevec[1].buffered_chars = filevec[0].buffered_chars;
	}
	
  // Binary comparisons *must not* go past here;  line-break sentinel markers may 
  // be put into the buffers.  Since read_files() only gets called for filevec[0], 
  // this causes a false mis-compare of all binary files (because filevec[1] would 
  // never get these non-necessary changes).
  if (appears_binary)
		return 1;
		
  find_identical_ends (filevec);

  /* Don't slurp rest of file when comparing file to itself. */
  if (filevec[0].desc == filevec[1].desc)
    {
	  filevec[1].count_crs = filevec[0].count_crs;
	  filevec[1].count_lfs = filevec[0].count_lfs;
	  filevec[1].count_crlfs = filevec[0].count_crlfs;
	  filevec[1].count_zeros = filevec[0].count_zeros;
      return 0;
    }

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
