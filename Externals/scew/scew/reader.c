/**
 * @file     reader.c
 * @brief    reader.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Nov 23, 2008 13:44
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

#include "reader.h"

#include <assert.h>

#include <stdlib.h>


/* Private */

struct scew_reader
{
  scew_reader_hooks const *hooks;
  void *data;
};


/* Public */

scew_reader*
scew_reader_create (scew_reader_hooks const *hooks, void *data)
{
  scew_reader *reader = NULL;

  assert (hooks != NULL);

  reader = calloc (1, sizeof (scew_reader));

  if (reader != NULL)
    {
      reader->hooks = hooks;
      reader->data = data;
    }

  return reader;
}

void*
scew_reader_data (scew_reader *reader)
{
  assert (reader != NULL);

  return reader->data;
}

size_t
scew_reader_read (scew_reader *reader, XML_Char *buffer, size_t char_no)
{
  assert (reader != NULL);
  assert (reader->hooks != NULL);
  assert (reader->hooks->read != NULL);

  return reader->hooks->read (reader, buffer, char_no);
}

scew_bool
scew_reader_end (scew_reader *reader)
{
  assert (reader != NULL);
  assert (reader->hooks != NULL);
  assert (reader->hooks->end != NULL);

  return reader->hooks->end (reader);
}

scew_bool
scew_reader_error (scew_reader *reader)
{
  assert (reader != NULL);
  assert (reader->hooks != NULL);
  assert (reader->hooks->error != NULL);

  return reader->hooks->error (reader);
}

scew_bool
scew_reader_close (scew_reader *reader)
{
  assert (reader != NULL);
  assert (reader->hooks != NULL);
  assert (reader->hooks->close != NULL);

  return reader->hooks->close (reader);
}

void
scew_reader_free (scew_reader *reader)
{
  assert (reader != NULL);
  assert (reader->hooks != NULL);
  assert (reader->hooks->free != NULL);

  reader->hooks->free (reader);
  free (reader);
}
