/**
 *
 * @file     error.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon May 05, 2003 10:35
 * @brief    SCEW error handling
 *
 * $Id: error.c,v 1.4 2004/05/25 20:23:04 aleix Exp $
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
 */

#include "error.h"

#include "str.h"

#include "xerror.h"
#include "xparser.h"

#include <assert.h>


scew_error
scew_error_code()
{
    return get_last_error();
}

XML_Char const*
scew_error_string(scew_error code)
{
    static const XML_Char *message[] = {
        _XT("No error"),
        _XT("Out of memory"),
        _XT("Input/Output error"),
        _XT("Callback error"),
        _XT("Internal Expat parser error")
    };

    assert(sizeof(message) / sizeof(message[0]) == scew_error_count);

    if ((code < 0) || (code > scew_error_count))
    {
        // This is not thread safe. Even though, no one should get in
        // here.
        static XML_Char unk_message[35];
        scew_sprintf(unk_message, _XT("Unknown error code (%d)"), code);
        return unk_message;
    }
    else
    {
        return message[code];
    }
}

enum XML_Error
scew_error_expat_code(scew_parser* parser)
{
    assert(parser != NULL);

    return XML_GetErrorCode(parser->parser);
}

XML_Char const*
scew_error_expat_string(enum XML_Error code)
{
    return XML_ErrorString(code);
}

int
scew_error_expat_line(scew_parser* parser)
{
    assert(parser != NULL);

    return XML_GetCurrentLineNumber(parser->parser);
}

int
scew_error_expat_column(scew_parser* parser)
{
    assert(parser != NULL);

    return XML_GetCurrentColumnNumber(parser->parser);
}
