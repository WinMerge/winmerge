/**
 *
 * @file     error.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon May 05, 2003 10:29
 * @brief    SCEW error handling functions
 *
 * $Id: error.h,v 1.3 2004/05/25 20:23:04 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2003, 2004 Aleix Conchillo Flaque
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * @endif
 *
 * These are SCEW error functions which return error codes and
 * strings. It also return Expat errors.
 *
 */


#ifndef ERROR_H_ALEIX0305051029
#define ERROR_H_ALEIX0305051029

#include "types.h"

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type declaration of the SCEW error. That is, an
 * enumeration of all SCEW possible errors.
 */
typedef enum
{
    /** No error has occured. */
    scew_error_none,
    /** No more memory available. */
    scew_error_no_memory,
    /** General Input/Output error. */
    scew_error_io,
    /** Callback error. */
    scew_error_callback,
    /** Expat parser error. */
    scew_error_expat,
    /** end of list marker */
    scew_error_count
} scew_error;

/**
 * Returns the SCEW internal error code. If the error code returned is
 * <code>scew_error_expat</code> it means that an internal Expat error
 * has occurred, so you will probably want to check Expat error using
 * <code>scew_error_expat_code</code> and
 * <code>scew_error_expat_string</code>.
 *
 * @see scew_error_expat_code
 * @see scew_error_expat_string
 */
extern scew_error
scew_error_code();

/**
 * Returns a string describing the internal SCEW error.
 *
 * Note: if an unknown error code is given a non thread safe string is
 * returned.
 */
extern XML_Char const*
scew_error_string(scew_error code);

/**
 * Returns the Expat internal error code.
 */
extern enum XML_Error
scew_error_expat_code(scew_parser* parser);

/**
 * Returns a string describing the internal Expat error.
 */
extern XML_Char const*
scew_error_expat_string(enum XML_Error code);

/**
 * Returns the current line at which the error was detected.
 */
extern int
scew_error_expat_line(scew_parser* parser);

/**
 * Returns the current column at which the error was detected.
 */
extern int
scew_error_expat_column(scew_parser* parser);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ERROR_H_ALEIX0305051029 */
