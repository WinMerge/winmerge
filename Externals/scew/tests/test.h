/**
 * @file     test.h
 * @brief    Common unit testing macros
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Aug 09, 2007 12:25
 *
 * Copyright (C) 2007-2009 Aleix Conchillo Flaque
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 */


#ifndef TEST_H_ALEIX0708091225
#define TEST_H_ALEIX0708091225

#include <stdio.h>

#include <scew/str.h>

enum
  {
    CHECK_MAX_BUFFER_ = 512      /**< Max buffer size */
  };


#ifdef XML_UNICODE_WCHAR_T

#define check_sprintf(BUFF, ...) \
  swprintf (BUFF, CHECK_MAX_BUFFER_, ##__VA_ARGS__)

#else /* XML_UNICODE_WCHAR_T */

#define check_sprintf sprintf

#endif /* XML_UNICODE_WCHAR_T */


#define CHECK_U_INT(A, B, MSG, ...)                                     \
  do                                                                    \
    {                                                                   \
      static XML_Char buffer[CHECK_MAX_BUFFER_];                        \
      check_sprintf (buffer, _XT(MSG), ##__VA_ARGS__);                  \
      unsigned int v_a = (A);                                           \
      unsigned int v_b = (B);                                           \
      fail_unless ((v_a) == (v_b),                                      \
                   "(%s) == (%s) \n  Actual: %d \n  Expected: %d \n  %s", \
                   #A, #B, v_a, v_b, buffer);                           \
    }                                                                   \
  while (0);

#define CHECK_S_INT(A, B, MSG, ...)                                     \
  do                                                                    \
    {                                                                   \
      static XML_Char buffer[CHECK_MAX_BUFFER_];                        \
      check_sprintf (buffer, _XT(MSG), ##__VA_ARGS__);                  \
      int v_a = (A);                                                    \
      int v_b = (B);                                                    \
      fail_unless (v_a == v_b,                                          \
                   "(%s) == (%s) \n  Actual: %d \n  Expected: %d \n  %s", \
                   #A, #B, v_a, v_b, buffer);                           \
    }                                                                   \
  while (0);

#define CHECK_BOOL(A, B, MSG, ...) CHECK_U_INT (A, B, MSG, ##__VA_ARGS__)

#define CHECK_STR(A, B, MSG, ...)                                       \
  do                                                                    \
    {                                                                   \
      XML_Char const *str_a = (A);                                      \
      XML_Char const *str_b = (B);                                      \
      if (scew_strcmp (str_a, str_b) != 0)                              \
        {                                                               \
          static XML_Char buffer[CHECK_MAX_BUFFER_];                    \
          check_sprintf (buffer, _XT(MSG), ##__VA_ARGS__);              \
          fail ("(%s) == (%s) \n  Actual: %s \n  Expected: %s \n  %s",  \
                #A, #B, str_a, str_b, buffer);                          \
        }                                                               \
    }                                                                   \
  while (0);

#define CHECK_PTR(A, MSG, ...)                                          \
  do                                                                    \
    {                                                                   \
      static XML_Char buffer[CHECK_MAX_BUFFER_];                        \
      check_sprintf (buffer, _XT(MSG), ##__VA_ARGS__);                  \
      fail_unless ((A) != NULL, "(%s) != NULL \n  %s", #A, buffer);     \
    }                                                                   \
  while (0);

#define CHECK_NULL_PTR(A, MSG, ...)                                     \
  do                                                                    \
    {                                                                   \
      static XML_Char buffer[CHECK_MAX_BUFFER_];                        \
      check_sprintf (buffer, _XT(MSG), ##__VA_ARGS__);                  \
      fail_unless ((A) == NULL, "(%s) == NULL \n  %s", #A, buffer);     \
    }                                                                   \
  while (0);

#endif /* TEST_H_ALEIX0708091225 */
