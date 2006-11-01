/**
 *
 * @file     str.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 23:32
 * @brief    SCEW string functions
 *
 * $Id: str.c,v 1.1 2004/01/28 00:43:21 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2002, 2003 Aleix Conchillo Flaque
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

#include "str.h"

#include "xerror.h"
#include "str.h"

#include <assert.h>

XML_Char*
scew_strdup(XML_Char const* src)
{
    unsigned int len = 0;
    XML_Char* out = NULL;

    assert(src != NULL);

    len = scew_strlen(src);
    out = (XML_Char*) calloc(len + 1, sizeof(XML_Char));
    return (XML_Char*) scew_memcpy(out, (XML_Char*) src, len);
}

void
scew_strtrim(XML_Char* src)
{
    int start = 0;
    int end = 0;
    int total = 0;

    assert(src != NULL);

    end = scew_strlen(src);

    /* strip trailing whitespace */
    while (end > 0 && scew_isspace(src[end-1]))
    {
        src[--end] = '\0';
    }

    /* strip leading whitespace */
    start = scew_strspn(src, _XT(" \n\r\t\v") );
    total = end - start;
    scew_memmove(src, &src[start], total);
    src[total] = _XT('\0');
}
