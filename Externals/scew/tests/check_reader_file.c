/**
 * @file     check_reader_file.c
 * @brief    Unit testing for SCEW file reader
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Aug 30, 2009 22:34
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

#include <scew/reader_file.h>

#include <check.h>


/* Unit tests */

static char const *TEST_FILE = SCEW_TESTSDIR"/check_reader_file.txt";

static XML_Char const *TEST_CONTENTS =
  _XT("This is just a dummy file to test the SCEW reader for "
      "files. We don't need to use an XML file as SCEW readers "
      "do not bother about file contents.");

/* Allocation */

START_TEST (test_alloc)
{
  scew_reader *reader = scew_reader_file_create (TEST_FILE);

  CHECK_PTR (reader, "Unable to create file name reader: %s", TEST_FILE);

  scew_reader_free (reader);

  /* Now try with the file pointer */

  FILE *file = fopen (TEST_FILE, "rb");

  reader = scew_reader_fp_create (file);

  CHECK_PTR (reader, "Unable to create file pointer reader: %s", TEST_FILE);

  scew_reader_free (reader);
}
END_TEST

/* Read */

START_TEST (test_read)
{
  enum { MAX_BUFFER_SIZE = 512 };

  XML_Char read_buffer[MAX_BUFFER_SIZE] = _XT("");

  scew_reader *reader = scew_reader_file_create (TEST_FILE);

  CHECK_PTR (reader, "Unable to create file reader");

  unsigned int i = 0;
  while (i < scew_strlen (TEST_CONTENTS))
    {
      CHECK_U_INT (scew_reader_read (reader, read_buffer + i, 1), 1,
                   "Invalid number of read bytes");
      i += 1;
    }
  read_buffer[i] = _XT('\0');

  CHECK_STR (read_buffer, TEST_CONTENTS, "Buffers do not match");

  CHECK_U_INT (scew_reader_read (reader, read_buffer + i, 1), 0,
               "There are no more bytes to read");

  CHECK_BOOL (scew_reader_end (reader), SCEW_TRUE,
              "Reader should be at the end");

  scew_reader_free (reader);

  /* Try to read full buffer */
  reader = scew_reader_file_create (TEST_FILE);

  scew_reader_read (reader, read_buffer, scew_strlen (TEST_CONTENTS) + 1);

  CHECK_STR (read_buffer, TEST_CONTENTS, "Buffers do not match");

  CHECK_BOOL (scew_reader_end (reader), SCEW_TRUE,
              "Reader should be at the end");

  scew_reader_free (reader);
}
END_TEST

/* Miscellaneous */

START_TEST (test_misc)
{
  scew_reader *reader = scew_reader_file_create (TEST_FILE);

  CHECK_PTR (reader, "Unable to create file reader: %s", TEST_FILE);

  CHECK_BOOL (scew_reader_end (reader), SCEW_FALSE,
              "Reader should be at the beginning");

  CHECK_BOOL (scew_reader_error (reader), SCEW_FALSE,
              "Reader should have no error (nothing done yet)");

  /* Close reader */
  CHECK_BOOL (scew_reader_close (reader), SCEW_TRUE,
              "Unable to close file reader");

  CHECK_BOOL (scew_reader_end (reader), SCEW_TRUE,
              "Reader is closed, thus at the end");

  scew_reader_free (reader);
}
END_TEST


/* Suite */

static Suite*
reader_file_suite (void)
{
  Suite *s = suite_create ("SCEW file reader");

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
  srunner_add_suite (sr, reader_file_suite ());
}
