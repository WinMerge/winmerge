/**
 * @file     str.h
 * @brief    SCEW string functions
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Dec 01, 2002 13:05
 * @ingroup  SCEWString
 *
 * @if copyright
 *
 * Copyright (C) 2002-2009 Aleix Conchillo Flaque
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
 * @defgroup SCEWString Text utilities
 *
 * This module defines a set of functions to work with text
 * strings. SCEW has defined wrappers for standard C routines in order
 * to work with regular and wide character strings (wchar_t). The
 * wrappers are simple macros to call the appropiate functions in both
 * cases.
 *
 * Right now, wide character strings are only availabe in Windows
 * platforms to provide UTF-16 support (XML_UNICODE_WCHAR_T needs to
 * be defined at compile time).
 */

#ifndef STR_H_0212011305
#define STR_H_0212011305

#include "bool.h"

#include "export.h"

#include <expat.h>

#include <string.h>

/**
 * @ingroup SCEWString
 */
/*@{*/
/**
 * Copy the number of given characters from @a src to @a dst. See
 * standard @a memcpy documentation.
 */
#define scew_memcpy(dst, src, n)  memcpy (dst, src, sizeof (XML_Char) * (n))

/**
 * Move the number of given characters from @a src to @a dst. See
 * standard @a memmove documentation.
 */
#define scew_memmove(dst, src, n) memmove (dst, src, sizeof (XML_Char) * (n))

#ifdef XML_UNICODE_WCHAR_T

#include <wchar.h>
#include <wctype.h>

#define _XT(str) L##str

#define scew_printf wprintf
#define scew_fprintf fwprintf
#define scew_vfprintf vfwprintf
#define scew_fputs fputws
#define scew_fgets fgetws
#define scew_fputc fputwc
#define scew_fgetc fgetwc

#define scew_strspn(wcs, accept) wcsspn (wcs, accept)
#define scew_strcpy(dest, src) wcscpy (dest, src)
#define scew_strcat(dest, src) wcscat (dest, src)
#define scew_strncpy(dest, src, n) wcsncpy (dest, src, n)
#define scew_strncat(dest, src, n) wcsncat (dest, src, n)
#define scew_strlen(s) wcslen (s)

#define scew_isalnum(c) iswalnum ((c))
#define scew_isalpha(c) iswalpha ((c))
#define scew_iscntrl(c) iswcntrl ((c))
#define scew_isdigit(c) iswdigit ((c))
#define scew_isxdigit(c) iswxdigit ((c))
#define scew_isgraph(c) iswgraph ((c))
#define scew_islower(c) iswlower ((c))
#define scew_isupper(c) iswupper ((c))
#define scew_isprint(c) iswprint ((c))
#define scew_ispunct(c) iswpunct ((c))
#define scew_isspace(c) iswspace ((c))

#else /* XML_UNICODE_WCHAR_T */

#include <ctype.h>

/**
 * Creates a regular string or a wide character string.
 */
#define _XT(str) str

/**
 * See standard @a printf documentation.
 */
#define scew_printf printf

/**
 * See standard @a fprintf documentation.
 */
#define scew_fprintf fprintf

/**
 * See standard @a vfprintf documentation.
 */
#define scew_vfprintf vfprintf

/**
 * See standard @a fputs documentation.
 */
#define scew_fputs fputs

/**
 * See standard @a fgets documentation.
 */
#define scew_fgets fgets

/**
 * See standard @a fputc documentation.
 */
#define scew_fputc fputc

/**
 * See standard @a fgetc documentation.
 */
#define scew_fgetc fgetc



/**
 * See standard @a strspn documentation.
 */
#define scew_strspn(s, accept) strspn (s, accept)

/**
 * See standard @a strcpy documentation.
 */
#define scew_strcpy(dest, src) strcpy (dest, src)

/**
 * See standard @a strcat documentation.
 */
#define scew_strcat(dest, src) strcat (dest, src)

/**
 * See standard @a strncpy documentation.
 */
#define scew_strncpy(dest, src, n) strncpy (dest, src, (n))

/**
 * See standard @a strncat documentation.
 */
#define scew_strncat(dest, src, n) strncat (dest, src, (n))

/**
 * See standard @a strlen documentation.
 */
#define scew_strlen(s) strlen (s)



/**
 * See standard @a isalnum documentation.
 */
#define scew_isalnum(c) isalnum ((unsigned char)(c))

/**
 * See standard @a isalpha documentation.
 */
#define scew_isalpha(c) isalpha ((unsigned char)(c))

/**
 * See standard @a iscntrl documentation.
 */
#define scew_iscntrl(c) iscntrl ((unsigned char)(c))

/**
 * See standard @a isdigit documentation.
 */
#define scew_isdigit(c) isdigit ((unsigned char)(c))

/**
 * See standard @a isxdigit documentation.
 */
#define scew_isxdigit(c) isxdigit ((unsigned char)(c))

/**
 * See standard @a isgraph documentation.
 */
#define scew_isgraph(c) isgraph ((unsigned char)(c))

/**
 * See standard @a islower documentation.
 */
#define scew_islower(c) islower ((unsigned char)(c))

/**
 * See standard @a isupper documentation.
 */
#define scew_isupper(c) isupper ((unsigned char)(c))

/**
 * See standard @a isprint documentation.
 */
#define scew_isprint(c) isprint ((unsigned char)(c))

/**
 * See standard @a ispunct documentation.
 */
#define scew_ispunct(c) ispunct ((unsigned char)(c))

/**
 * See standard @a isspace documentation.
 */
#define scew_isspace(c) isspace ((unsigned char)(c))
/*@}*/

#endif /* XML_UNICODE_WCHAR_T */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Compares the two given strings @a s1 and @a s2.
 *
 * @return 0 if the two strings are identical or NULL, less than zero
 * if @a s1 is less than @a s2 or greater than zero otherwise.
 *
 * @ingroup SCEWString
 */
extern SCEW_API int scew_strcmp (XML_Char const *a, XML_Char const *b);

/**
 * Creates a new copy of the given string.
 *
 * @param src the string to be duplicated (might be NULL).
 *
 * @return the duplicated string, or NULL if the given string is NULL.
 *
 * @ingroup SCEWString
 */
extern SCEW_API XML_Char* scew_strdup (XML_Char const *src);

/**
 * Trims off extra spaces from the beginning and end of a string. The
 * trimming is done in place.
 *
 * @pre src != NULL
 *
 * @param src the string to be trimmed off.
 *
 * @ingroup SCEWString
 */
extern SCEW_API void scew_strtrim (XML_Char *src);

/**
 * Tells whether the given string is empty. That is, all characters
 * are spaces, form-feed, newlines, etc. See @a isspace documentation
 * to see the list of characters considered space.
 *
 * @pre src != NULL
 *
 * @param src the string to tell if its empty or not.
 *
 * @return true if the given string is empty, false otherwise.
 *
 * @ingroup SCEWString
 */
extern SCEW_API scew_bool scew_isempty (XML_Char const *src);

/**
 * Escapes the given string for XML. This will substitute the general
 * XML delimiters:
 *
 * \verbatim < > & ' " \endverbatim
 *
 * to the pre-defined XML entities, respectively:
 *
 * \verbatim &lt; &gt; &amp; &apos; &quot; \endverbatim
 *
 * A new escaped string will be allocated. Thus, the user is
 * responsible of freeing the new string.
 *
 * @pre src != NULL
 *
 * @param src the string to be escaped.
 *
 * @return a new allocated string with the XML delimiters (if any)
 * escaped.
 *
 * @ingroup SCEWString
 */
extern XML_Char* scew_strescape (XML_Char const *src);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STR_H_0212011305 */
