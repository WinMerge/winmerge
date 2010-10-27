/**
 * @file     writer.c
 * @brief    writer.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Sep 11, 2003 00:39
 *
 * @if copyright
 *
 * Copyright (C) 2003-2009 Aleix Conchillo Flaque
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

#include "writer.h"

#include <assert.h>


/* Private */

struct scew_writer
{
  scew_writer_hooks const *hooks;
  void *data;
};


/* Public */

scew_writer*
scew_writer_create (scew_writer_hooks const *hooks, void *data)
{
  scew_writer *writer = NULL;

  assert (hooks != NULL);

  writer = calloc (1, sizeof (scew_writer));

  if (writer != NULL)
    {
      writer->hooks = hooks;
      writer->data = data;
    }

  return writer;
}

void*
scew_writer_data (scew_writer *writer)
{
  assert (writer != NULL);

  return writer->data;
}

size_t
scew_writer_write (scew_writer *writer, XML_Char const *buffer, size_t char_no)
{
  assert (writer != NULL);
  assert (writer->hooks != NULL);
  assert (writer->hooks->write != NULL);

  return writer->hooks->write (writer, buffer, char_no);
}

scew_bool
scew_writer_end (scew_writer *writer)
{
  assert (writer != NULL);
  assert (writer->hooks != NULL);
  assert (writer->hooks->end != NULL);

  return writer->hooks->end (writer);
}

scew_bool
scew_writer_error (scew_writer *writer)
{
  assert (writer != NULL);
  assert (writer->hooks != NULL);
  assert (writer->hooks->error != NULL);

  return writer->hooks->error (writer);
}

scew_bool
scew_writer_close (scew_writer *writer)
{
  assert (writer != NULL);
  assert (writer->hooks != NULL);
  assert (writer->hooks->close != NULL);

  return writer->hooks->close (writer);
}

void
scew_writer_free (scew_writer *writer)
{
  assert (writer != NULL);
  assert (writer->hooks != NULL);
  assert (writer->hooks->free != NULL);

  writer->hooks->free (writer);
  free (writer);
}
