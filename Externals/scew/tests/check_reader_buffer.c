/**
 * @file     check_reader_buffer.c
 * @brief    Unit testing for SCEW buffer reader
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Fri Aug 28, 2009 19:38
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

#include <scew/reader_buffer.h>

#include <check.h>


/* Unit tests */

/* Allocation */

START_TEST (test_alloc)
{
  static XML_Char const *BUFFER = _XT("This is a buffer for the reader");

  scew_reader *reader = scew_reader_buffer_create (BUFFER,
                                                   scew_strlen (BUFFER));

  CHECK_PTR (reader, "Unable to create buffer reader");

  scew_reader_free (reader);
}
END_TEST

/* Read */

START_TEST (test_read)
{
  enum { MAX_BUFFER_SIZE = 512 };

  static XML_Char const *BUFFER = _XT("This is a buffer for the reader");

  XML_Char read_buffer[MAX_BUFFER_SIZE] = _XT("");

  scew_reader *reader = scew_reader_buffer_create (BUFFER,
                                                   scew_strlen (BUFFER));

  CHECK_PTR (reader, "Unable to create buffer reader");

  unsigned int i = 0;
  while (i < scew_strlen (BUFFER))
    {
      CHECK_U_INT (scew_reader_read (reader, read_buffer + i, 1), 1,
                   "Invalid number of read bytes");
      i += 1;
    }
  read_buffer[i] = _XT('\0');

  CHECK_STR (read_buffer, BUFFER, "Buffers do not match");

  CHECK_BOOL (scew_reader_end (reader), SCEW_TRUE,
              "Reader should be at the end");

  scew_reader_free (reader);

  /* Try to read full buffer */
  reader = scew_reader_buffer_create (BUFFER, scew_strlen (BUFFER) + 1);

  scew_reader_read (reader, read_buffer, scew_strlen (BUFFER) + 1);

  CHECK_STR (read_buffer, BUFFER, "Buffers do not match");

  CHECK_BOOL (scew_reader_end (reader), SCEW_TRUE,
              "Reader should be at the end");

  scew_reader_free (reader);
}
END_TEST

/* Miscellaneous */

START_TEST (test_misc)
{
  static XML_Char const *BUFFER = _XT("This is a buffer for the reader");

  scew_reader *reader = scew_reader_buffer_create (BUFFER,
                                                   scew_strlen (BUFFER));

  CHECK_PTR (reader, "Unable to create buffer reader");

  CHECK_BOOL (scew_reader_end (reader), SCEW_FALSE,
              "Reader should be at the beginning");

  CHECK_BOOL (scew_reader_error (reader), SCEW_FALSE,
              "Reader should have no error (nothing done yet)");

  /* Close reader */
  CHECK_BOOL (scew_reader_close (reader), SCEW_TRUE,
              "Unable to close buffer reader");

  CHECK_BOOL (scew_reader_end (reader), SCEW_TRUE,
              "Reader is closed, thus at the end");

  scew_reader_free (reader);
}
END_TEST


/* Suite */

static Suite*
reader_buffer_suite (void)
{
  Suite *s = suite_create ("SCEW buffer reader");

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_alloc);
  tcase_add_test (tc_core, test_read);
  tcase_add_test (tc_core, test_misc);
  suite_add_tcase (s, tc_core);

  return s;
}

void
run_tests (SRunner *sr)
{
  srunner_add_suite (sr, reader_buffer_suite ());
}
