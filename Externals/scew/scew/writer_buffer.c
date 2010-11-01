/**
 * @file     writer_buffer.c
 * @brief    writer_buffer.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Jul 21, 2008 23:40
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
 */

#include "writer_buffer.h"

#include "str.h"

#include <assert.h>
#include <stdio.h>


/* Private */

typedef struct
{
  XML_Char *buffer;
  size_t size;
  size_t current;
} scew_writer_buffer;

static size_t buffer_write_ (scew_writer *writer,
                             XML_Char const *buffer,
                             size_t char_no);
static scew_bool buffer_end_ (scew_writer *writer);
static scew_bool buffer_error_ (scew_writer *writer);
static scew_bool buffer_close_ (scew_writer *writer);
static void buffer_free_ (scew_writer *writer);

static scew_writer_hooks const buffer_hooks_ =
  {
    buffer_write_,
    buffer_end_,
    buffer_error_,
    buffer_close_,
    buffer_free_
  };



/* Public */

scew_writer*
scew_writer_buffer_create (XML_Char *buffer, size_t size)
{
  scew_writer *writer = NULL;
  scew_writer_buffer *buf_writer = NULL;

  assert (buffer != NULL);
  assert (size > 0);

  buf_writer = calloc (1, sizeof (scew_writer_buffer));

  if (buf_writer != NULL)
    {
      buf_writer->buffer = buffer;
      buf_writer->size = size;
      buf_writer->current = 0;

      /* Create writer */
      writer = scew_writer_create (&buffer_hooks_, buf_writer);
      if (NULL == writer)
        {
          free (buf_writer);
        }
    }

  return writer;
}


/* Private */

size_t
buffer_write_ (scew_writer *writer, XML_Char const *buffer, size_t char_no)
{
  size_t written = 0;
  size_t maxlen = 0;
  scew_writer_buffer *buf_writer = NULL;

  assert (writer != NULL);
  assert (buffer != NULL);

  buf_writer = scew_writer_data (writer);

  /* Always leave one space for null-terminated buffer. */
  maxlen = buf_writer->size - buf_writer->current - 1;
  written = (char_no > maxlen) ? maxlen : char_no;

  scew_memcpy (buf_writer->buffer + buf_writer->current, buffer, written);
  buf_writer->current += written;

  /* Set null-character to end of buffer. */
  buf_writer->buffer[buf_writer->current] = _XT('\0');

  return written;
}

scew_bool
buffer_end_ (scew_writer *writer)
{
  scew_writer_buffer *buf_writer = NULL;

  assert (writer != NULL);

  buf_writer = scew_writer_data (writer);

  return (buf_writer->current >= buf_writer->size);
}

scew_bool
buffer_error_ (scew_writer *writer)
{
  return SCEW_FALSE;
}

scew_bool
buffer_close_ (scew_writer *writer)
{
  scew_writer_buffer *buf_writer = NULL;

  assert (writer != NULL);

  buf_writer = scew_writer_data (writer);

  /* This will mark it as end of writer. */
  buf_writer->current = buf_writer->size;

  return SCEW_TRUE;
}

void
buffer_free_ (scew_writer *writer)
{
  scew_writer_buffer *buf_writer = NULL;

  assert (writer != NULL);

  buf_writer = scew_writer_data (writer);

  buffer_close_ (writer);

  free (buf_writer);
}
