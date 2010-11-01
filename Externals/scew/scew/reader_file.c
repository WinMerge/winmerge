/**
 * @file     reader_file.c
 * @brief    reader_file.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Nov 23, 2008 13:50
 *
 * @if copyright
 *
 * Copyright (C) 2008-2009 Aleix Conchillo Flaque
 *
 * SCEW is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SCEW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * @endif
 **/

#include "reader_file.h"

#include "str.h"

#include <assert.h>

#include <stdlib.h>


/* Private */

typedef struct
{
  FILE *file;
  scew_bool closed;
} scew_reader_fp;

static size_t file_read_ (scew_reader *reader,
                          XML_Char *buffer,
                          size_t char_no);
static scew_bool file_end_ (scew_reader *reader);
static scew_bool file_error_ (scew_reader *reader);
static scew_bool file_close_ (scew_reader *reader);
static void file_free_ (scew_reader *reader);

static scew_reader_hooks const file_hooks_ =
  {
    file_read_,
    file_end_,
    file_error_,
    file_close_,
    file_free_
  };


/* Public */

scew_reader*
scew_reader_file_create (char const *file_name)
{
  FILE *file = NULL;
  scew_reader *reader = NULL;

  assert (file_name != NULL);

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
  file = fopen (file_name, "rt, ccs=UNICODE");
#else
  file = fopen (file_name, "rt");
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

  if (file != NULL)
    {
      reader = scew_reader_fp_create (file);
    }

  return reader;
}

scew_reader*
scew_reader_fp_create (FILE *file)
{
  scew_reader *reader = NULL;
  scew_reader_fp *fp_reader = NULL;

  assert (file != NULL);

  fp_reader = calloc (1, sizeof (scew_reader_fp));

  if (fp_reader != NULL)
    {
      fp_reader->file = file;
      fp_reader->closed = SCEW_FALSE;

      /* Create reader */
      reader = scew_reader_create (&file_hooks_, fp_reader);
      if (NULL == reader)
        {
          free (fp_reader);
        }
    }

  return reader;
}


/* Private */

size_t
file_read_ (scew_reader *reader, XML_Char *buffer, size_t char_no)
{
  size_t read_no = 0;
  scew_reader_fp *fp_reader = NULL;

  assert (reader != NULL);
  assert (buffer != NULL);

  fp_reader = scew_reader_data (reader);

  read_no = fread (buffer, sizeof (XML_Char), char_no, fp_reader->file);

  buffer[read_no] = _XT('\0');

  return read_no;
}

scew_bool
file_end_ (scew_reader *reader)
{
  scew_bool closed = SCEW_FALSE;
  scew_reader_fp *fp_reader = NULL;

  assert (reader != NULL);

  fp_reader = scew_reader_data (reader);

  /* If file is already closed, return true as well. */
  closed = fp_reader->closed;
  if (!closed)
    {
      /* Check end of file. */
      int end = feof (fp_reader->file);
      closed = ((-1 == end) || (end != 0)) ? SCEW_TRUE : SCEW_FALSE;
    }

  return closed;
}

scew_bool
file_error_ (scew_reader *reader)
{
  scew_reader_fp *fp_reader = NULL;

  assert (reader != NULL);

  fp_reader = scew_reader_data (reader);

  return (ferror (fp_reader->file) != 0);
}

scew_bool
file_close_ (scew_reader *reader)
{
  int status = 0;
  scew_reader_fp *fp_reader = NULL;

  assert (reader != NULL);

  fp_reader = scew_reader_data (reader);

  /* Do not close already closed file or standard input stream. */
  if (fp_reader->closed || (stdin == fp_reader->file))
    {
      fp_reader->closed = SCEW_TRUE;
    }
  else
    {
      /* Set closed flag if we are actually able to close it. */
      status = fclose (fp_reader->file);
      fp_reader->closed = (0 == status);
    }

  return fp_reader->closed;
}

void
file_free_ (scew_reader *reader)
{
  scew_reader_fp *fp_reader = NULL;

  assert (reader != NULL);

  /* Close the file before freeing the reader. */
  file_close_ (reader);

  fp_reader = scew_reader_data (reader);
  free (fp_reader);
}
