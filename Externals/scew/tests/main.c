/**
 * @file     main.c
 * @brief    Unit testing main
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Fri Aug 03, 2007 17:52
 *
 * @if copyright
 *
 * Copyright (C) 2007 Aleix Conchillo Flaque
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

#include <check.h>

#include <stdlib.h>

extern void run_tests (SRunner *sr);

int
main (void)
{
  SRunner *sr = srunner_create (NULL);

  run_tests (sr);

  srunner_run_all (sr, CK_NORMAL);

  int number_failed = srunner_ntests_failed (sr);

  srunner_free (sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
