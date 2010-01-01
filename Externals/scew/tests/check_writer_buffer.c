/**
 * @file     check_writer_buffer.c
 * @brief    Unit testing for SCEW buffer writer
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sat Aug 29, 2009 19:44
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
 */

#include "test.h"

#include <scew/writer_buffer.h>

#include <check.h>


/* Unit tests */

/* Allocation */

START_TEST (test_alloc)
{
  enum { MAX_BUFFER_SIZE = 512 };

  XML_Char buffer[MAX_BUFFER_SIZE] = _XT("");

  scew_writer *writer = scew_writer_buffer_create (buffer, MAX_BUFFER_SIZE);

  CHECK_PTR (writer, "Unable to create buffer writer");

  scew_writer_free (writer);
}
END_TEST

/* Write */

START_TEST (test_write)
{
  enum { MAX_BUFFER_SIZE = 512 };

  static XML_Char const *BUFFER = _XT("This is a buffer for the reader");

  XML_Char write_buffer[MAX_BUFFER_SIZE] = _XT("");

  scew_writer *writer = scew_writer_buffer_create (write_buffer,
                                                   scew_strlen (BUFFER) + 1);

  CHECK_PTR (writer, "Unable to create buffer writer");

  unsigned int i = 0;
  while (i < scew_strlen (BUFFER))
    {
      CHECK_U_INT (scew_writer_write (writer, BUFFER + i, 1), 1,
                   "Invalid number of written bytes");
      i += 1;
    }

  CHECK_STR (write_buffer, BUFFER, "Buffers do not match");

  CHECK_BOOL (scew_writer_end (writer), SCEW_FALSE,
              "Writer buffer is bigger than read buffer (no end yet)");

  scew_writer_free (writer);

  /* Try to writer full buffer */
  writer = scew_writer_buffer_create (write_buffer, scew_strlen (BUFFER) + 1);

  scew_writer_write (writer, write_buffer, scew_strlen (BUFFER) + 1);

  CHECK_STR (write_buffer, BUFFER, "Buffers do not match");

  CHECK_BOOL (scew_writer_end (writer), SCEW_FALSE,
              "Writer buffer is bigger than read buffer (no end yet)");

  scew_writer_free (writer);
}
END_TEST

/* Miscellaneous */

START_TEST (test_misc)
{
  enum { MAX_BUFFER_SIZE = 512 };

  XML_Char buffer[MAX_BUFFER_SIZE] = _XT("");

  scew_writer *writer = scew_writer_buffer_create (buffer, MAX_BUFFER_SIZE);

  CHECK_PTR (writer, "Unable to create buffer writer");

  CHECK_BOOL (scew_writer_end (writer), SCEW_FALSE,
              "Writer should be at the beginning");

  CHECK_BOOL (scew_writer_error (writer), SCEW_FALSE,
              "Writer should have no error (nothing done yet)");

  /* Close writer */
  scew_writer_close (writer);

  CHECK_BOOL (scew_writer_end (writer), SCEW_TRUE,
              "Writer is closed, thus at the end");

  scew_writer_free (writer);
}
END_TEST


/* Suite */

static Suite*
writer_buffer_suite (void)
{
  Suite *s = suite_create ("SCEW buffer writer");

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_alloc);
  tcase_add_test (tc_core, test_write);
  tcase_add_test (tc_core, test_misc);
  suite_add_tcase (s, tc_core);

  return s;
}

void
run_tests (SRunner *sr)
{
  srunner_add_suite (sr, writer_buffer_suite ());
}
