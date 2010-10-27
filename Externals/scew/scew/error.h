/**
 * @file     error.h
 * @brief    SCEW error handling functions
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon May 05, 2003 10:29
 * @ingroup  SCEWError, SCEWErrorCodes, SCEWErrorExpat
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

/**
 * @defgroup SCEWError Errors
 *
 * These are SCEW error functions which return error codes and
 * strings. Expat related errors can also be obtained.
 */

#ifndef ERROR_H_0305051029
#define ERROR_H_0305051029

#include "export.h"

#include "parser.h"

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup SCEWErrorCodes Codes and descriptions
 *
 * SCEW internal error codes and associated descriptions.
 *
 * @ingroup SCEWError
 */

/**
 * This is the type declaration of the SCEW error. That is, an
 * enumeration of all SCEW possible errors.
 *
 * @ingroup SCEWErrorCodes
 */
typedef enum
  {
    scew_error_none,            /**< No error has occured. */
    scew_error_no_memory,       /**< No more memory available. */
    scew_error_io,              /**< General Input/Output error. */
    scew_error_hook,            /**< Hook returned error. */
    scew_error_expat,           /**< Expat parser error. */
    scew_error_internal,        /**< Internal SCEW error. */
    scew_error_unknown          /**< end of list marker */
  } scew_error;

/**
 * Returns the SCEW internal error code. If the error code returned is
 * #scew_error_expat it means that an internal Expat error has
 * occurred, so you will probably want to check Expat error using
 * #scew_error_expat_code and #scew_error_expat_string.
 *
 * @return the current internal SCEW error code, if any.
 *
 * @ingroup SCEWErrorCodes
 */
extern SCEW_API scew_error scew_error_code (void);

/**
 * Returns a string describing the given internal SCEW error @a code.
 *
 * @return the associated string for the given error @a code.
 *
 * @ingroup SCEWErrorCodes
 */
extern SCEW_API XML_Char const* scew_error_string (scew_error code);


/**
 * @defgroup SCEWErrorExpat Expat errors
 *
 * Routines to access Expat internal error information.
 *
 * @ingroup SCEWError
 */

/**
 * Returns the Expat internal error code.
 *
 * @return the internal Expat error code.
 *
 * @ingroup SCEWErrorExpat
 */
extern SCEW_API enum XML_Error scew_error_expat_code (scew_parser *parser);

/**
 * Returns a string describing the internal Expat error for the given
 * error @a code.
 *
 * @return the internal Expat error string for the given error @a
 * code.
 *
 * @ingroup SCEWErrorExpat
 */
extern SCEW_API XML_Char const* scew_error_expat_string (enum XML_Error code);

/**
 * Returns the current line at which the error was detected.
 *
 * @return the line where Expat detected the error.
 *
 * @ingroup SCEWErrorExpat
 */
extern SCEW_API int scew_error_expat_line (scew_parser *parser);

/**
 * Returns the current column at which the error was detected.
 *
 * @return the column where Expat detected the error.
 *
 * @ingroup SCEWErrorExpat
 */
extern SCEW_API int scew_error_expat_column (scew_parser *parser);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ERROR_H_0305051029 */
