/**
 * @file     check_writer_file.c
 * @brief    Unit testing for SCEW file writer
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Tue Sep 01, 2009 16:55
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

#include <scew/writer_file.h>
#include <scew/reader_file.h>

#include <check.h>


/* Unit tests */

static char const *TEST_FILE = SCEW_TESTSDIR"/check_writer_file.txt";

static XML_Char const *TEST_CONTENTS =
  _XT("This is just a dummy file to test the SCEW writer for "
      "files. We don't need to use an XML file as SCEW writers "
      "do not bother about file contents.");

/* Allocation */

START_TEST (test_alloc)
{
  scew_writer *writer = scew_writer_file_create (TEST_FILE);

  CHECK_PTR (writer, "Unable to create file writer");

  scew_writer_free (writer);

  /* Now try with the file pointer */

  FILE *file = fopen (TEST_FILE, "wb");

  writer = scew_writer_fp_create (file);

  CHECK_PTR (writer, "Unable to create file pointer writer: %s", TEST_FILE);

  scew_writer_free (writer);

  /* Remove test file from hard drive */
  remove (TEST_FILE);
}
END_TEST

/* Write */

START_TEST (test_write)
{
  scew_writer *writer = scew_writer_file_create (TEST_FILE);

  CHECK_PTR (writer, "Unable to create file writer");

  unsigned int i = 0;
  while (i < scew_strlen (TEST_CONTENTS))
    {
      CHECK_U_INT (scew_writer_write (writer, TEST_CONTENTS + i, 1), 1,
                   "Invalid number of written bytes");
      i += 1;
    }

  CHECK_BOOL (scew_writer_end (writer), SCEW_FALSE,
              "Writer file is always bigger than read buffer (no end yet)");

  scew_writer_free (writer);

  /* Try to read the whole file */
  enum { MAX_BUFFER_SIZE = 512 };

  XML_Char read_buffer[MAX_BUFFER_SIZE] = _XT("");

  scew_reader *reader = scew_reader_file_create (TEST_FILE);

  scew_reader_read (reader, read_buffer, scew_strlen (TEST_CONTENTS) + 1);

  CHECK_STR (read_buffer, TEST_CONTENTS, "Buffers do not match");

  /* Remove test file from hard drive */
  remove (TEST_FILE);

  scew_reader_free (reader);

  /* Try to writer full buffer */
  writer = scew_writer_file_create (TEST_FILE);

  scew_writer_write (writer, TEST_CONTENTS, scew_strlen (TEST_CONTENTS) + 1);

  scew_writer_free (writer);

  /* Try to read the whole file again */
  reader = scew_reader_file_create (TEST_FILE);

  memset (read_buffer, 0, MAX_BUFFER_SIZE); /* Clear buffer before */
  scew_reader_read (reader, read_buffer, scew_strlen (TEST_CONTENTS) + 1);

  CHECK_STR (read_buffer, TEST_CONTENTS, "Buffers do not match");

  scew_reader_free (reader);

  /* Remove test file from hard drive */
  remove (TEST_FILE);
}
END_TEST

/* Miscellaneous */

START_TEST (test_misc)
{
  scew_writer *writer = scew_writer_file_create (TEST_FILE);

  CHECK_PTR (writer, "Unable to create file writer");

  CHECK_BOOL (scew_writer_end (writer), SCEW_FALSE,
              "Writer should be at the beginning");

  CHECK_BOOL (scew_writer_error (writer), SCEW_FALSE,
              "Writer should have no error (nothing done yet)");

  /* Close writer */
  scew_writer_close (writer);

  CHECK_BOOL (scew_writer_end (writer), SCEW_TRUE,
              "Writer is closed, thus at the end");

  scew_writer_free (writer);

  /* Remove test file from hard drive */
  remove (TEST_FILE);
}
END_TEST


/* Suite */

static Suite*
writer_file_suite (void)
{
  Suite *s = suite_create ("SCEW file writer");

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
  srunner_add_suite (sr, writer_file_suite ());
}
