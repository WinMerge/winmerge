/**
 * @file     error.c
 * @brief    error.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon May 05, 2003 10:35
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

#include "error.h"

#include "xerror.h"

#include "str.h"
#include "parser.h"

#include <assert.h>


scew_error
scew_error_code (void)
{
  return scew_error_last_error_ ();
}

XML_Char const*
scew_error_string (scew_error code)
{
  static XML_Char const *message[] =
    {
      _XT("No error"),
      _XT("Out of memory"),
      _XT("Input/Output error"),
      _XT("Error while calling hook"),
      _XT("Internal Expat parser error"),
      _XT("Internal SCEW error")
    };

  assert (sizeof(message) / sizeof(message[0]) == scew_error_unknown);

  if (code < scew_error_unknown)
    {
      return message[code];
    }
  else
    {
      static XML_Char const *unk_message = _XT("Unknown error code");

      return unk_message;
    }
}

enum XML_Error
scew_error_expat_code (scew_parser *parser)
{
  assert (parser != NULL);

  return XML_GetErrorCode (scew_parser_expat (parser));
}

XML_Char const*
scew_error_expat_string (enum XML_Error code)
{
  return XML_ErrorString (code);
}

int
scew_error_expat_line (scew_parser *parser)
{
  assert (parser != NULL);

  return XML_GetCurrentLineNumber (scew_parser_expat (parser));
}

int
scew_error_expat_column (scew_parser *parser)
{
  assert (parser != NULL);

  return XML_GetCurrentColumnNumber (scew_parser_expat (parser));
}
