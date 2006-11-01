/**
 *
 * @file     str.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Dec 01, 2002 13:05
 * @brief    SCEW string functions
 *
 * $Id: str.h,v 1.2 2004/01/29 00:34:38 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2002, 2003, 2004 Aleix Conchillo Flaque
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * @endif
 */


#ifndef STR_H_ALEIX0212011305
#define STR_H_ALEIX0212011305

#include <expat.h>

#include <string.h>

#define scew_memcpy(dst,src,n)  memcpy(dst,src,sizeof(XML_Char)*(n))
#define scew_memmove(dst,src,n) memmove(dst,src,sizeof(XML_Char)*(n))

#ifdef XML_UNICODE_WCHAR_T

#include <wchar.h>
#include <wctype.h>

#define _XT(str)                L##str

#define scew_fprintf            fwprintf
#define scew_sprintf            swprintf

#define scew_strcmp(s1,s2)      wcscmp(s1,s2)
#define scew_strspn(s1,s2)      wcsspn(s1,s2)
#define scew_strcpy(s1,s2)      wcscpy(s1,s2)
#define scew_strcat(s1,s2)      wcscat(s1,s2)
#define scew_strncpy(s1,s2,n)   wcsncpy(s1,s2,n)
#define scew_strncat(s1,s2,n)   wcsncat(s1,s2,n)
#define scew_strlen(s)          wcslen(s)

#define scew_isalnum(c)         iswalnum((c))
#define scew_isalpha(c)         iswalpha((c))
#define scew_iscntrl(c)         iswcntrl((c))
#define scew_isdigit(c)         iswdigit((c))
#define scew_isxdigit(c)        iswxdigit((c))
#define scew_isgraph(c)         iswgraph((c))
#define scew_islower(c)         iswlower((c))
#define scew_isupper(c)         iswupper((c))
#define scew_isprint(c)         iswprint((c))
#define scew_ispunct(c)         iswpunct((c))
#define scew_isspace(c)         iswspace((c))

#else /* not XML_UNICODE_WCHAR_T */

#include <ctype.h>

#define _XT(str)                str

#define scew_fprintf            fprintf
#define scew_sprintf            sprintf

#define scew_strcmp(s1,s2)      strcmp(s1,s2)
#define scew_strspn(s1,s2)      strspn(s1,s2)
#define scew_strcpy(s1,s2)      strcpy(s1,s2)
#define scew_strcat(s1,s2)      strcat(s1,s2)
#define scew_strncpy(s1,s2,n)   strncpy(s1,s2,n)
#define scew_strncat(s1,s2,n)   strncat(s1,s2,n)
#define scew_strlen(s)          strlen(s)

#define scew_isalnum(c)         isalnum((unsigned char)(c))
#define scew_isalpha(c)         isalpha((unsigned char)(c))
#define scew_iscntrl(c)         iscntrl((unsigned char)(c))
#define scew_isdigit(c)         isdigit((unsigned char)(c))
#define scew_isxdigit(c)        isxdigit((unsigned char)(c))
#define scew_isgraph(c)         isgraph((unsigned char)(c))
#define scew_islower(c)         islower((unsigned char)(c))
#define scew_isupper(c)         isupper((unsigned char)(c))
#define scew_isprint(c)         isprint((unsigned char)(c))
#define scew_ispunct(c)         ispunct((unsigned char)(c))
#define scew_isspace(c)         isspace((unsigned char)(c))

#endif /* XML_UNICODE_WCHAR_T */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Creates a new copy of the given string. Client must free it.
 */
extern XML_Char*
scew_strdup(XML_Char const* src);

/**
 * Trims off extra spaces from the beginning and end of a string.
 */
extern void
scew_strtrim(XML_Char* src);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STR_H_ALEIX0212011305 */
