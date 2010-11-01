/**
 * @file     reader_buffer.c
 * @brief    reader_buffer.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Tue Aug 25, 2009 01:49
 *
 * @if copyright
 *
 * Copyright (C) 2009 Aleix Conchillo Flaque
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

#include "reader_buffer.h"

#include "str.h"

#include <assert.h>

#include <stdlib.h>
#include <string.h>


/* Private */

typedef struct
{
  XML_Char const *buffer;
  size_t size;
  size_t current;
} scew_reader_buffer;

static size_t buffer_read_ (scew_reader *reader,
                            XML_Char *buffer,
                            size_t char_no);
static scew_bool buffer_end_ (scew_reader *reader);
static scew_bool buffer_error_ (scew_reader *reader);
static scew_bool buffer_close_ (scew_reader *reader);
static void buffer_free_ (scew_reader *reader);

static scew_reader_hooks const buffer_hooks_ =
  {
    buffer_read_,
    buffer_end_,
    buffer_error_,
    buffer_close_,
    buffer_free_
  };


/* Public */

scew_reader*
scew_reader_buffer_create (XML_Char const *buffer, size_t size)
{
  scew_reader *reader = NULL;
  scew_reader_buffer *buf_reader = NULL;

  assert (buffer != NULL);
  assert (size > 0);

  buf_reader = calloc (1, sizeof (scew_reader_buffer));
  if (buf_reader != NULL)
    {
      buf_reader->buffer = buffer;
      buf_reader->size = size;
      buf_reader->current = 0;

      /* Create reader */
      reader = scew_reader_create (&buffer_hooks_, buf_reader);
      if (NULL == reader)
        {
          free (buf_reader);
        }
    }

  return reader;
}


/* Private */

size_t
buffer_read_ (scew_reader *reader, XML_Char *buffer, size_t char_no)
{
  size_t read_no = 0;
  size_t maxlen = 0;
  scew_reader_buffer *buf_reader = NULL;

  assert (reader != NULL);
  assert (buffer != NULL);

  buf_reader = scew_reader_data (reader);

  /* Get maximum number of available bytes in buffer. */
  maxlen = buf_reader->size - buf_reader->current;
  read_no = (char_no > maxlen) ? maxlen : char_no;

  scew_memcpy (buffer, buf_reader->buffer + buf_reader->current, read_no);
  buf_reader->current += read_no;

  return read_no;
}

scew_bool
buffer_end_ (scew_reader *reader)
{
  scew_reader_buffer *buf_reader = NULL;

  assert (reader != NULL);

  buf_reader = scew_reader_data (reader);

  return (buf_reader->current >= buf_reader->size);
}

scew_bool
buffer_error_ (scew_reader *reader)
{
  return SCEW_FALSE;
}

scew_bool
buffer_close_ (scew_reader *reader)
{
  scew_reader_buffer *buf_reader = NULL;

  assert (reader != NULL);

  buf_reader = scew_reader_data (reader);

  /* This will mark it as EOR. */
  buf_reader->current = buf_reader->size;

  return SCEW_TRUE;
}

void
buffer_free_ (scew_reader *reader)
{
  scew_reader_buffer *buf_reader = NULL;

  assert (reader != NULL);

  /* Close the buffer before freeing the reader. */
  buffer_close_ (reader);

  buf_reader = scew_reader_data (reader);
  free (buf_reader);
}
