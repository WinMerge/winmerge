/**
 *
 * @file     xparser.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Tue Dec 03, 2002 00:21
 * @brief    SCEW private parser type declaration
 *
 * $Id: xparser.c,v 1.2 2004/05/25 20:23:05 aleix Exp $
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * @endif
 */

#include "xparser.h"

#include "xerror.h"
#include "xhandler.h"

#include <assert.h>

unsigned int
init_expat_parser(scew_parser* parser)
{
    assert(parser != NULL);

    parser->parser = XML_ParserCreate(NULL);
    if (parser->parser == NULL)
    {
        set_last_error(scew_error_no_memory);
        return 0;
    }

    /* initialize Expat handlers */
    XML_SetXmlDeclHandler(parser->parser, xmldecl_handler);
    XML_SetElementHandler(parser->parser, start_handler, end_handler);
    XML_SetCharacterDataHandler(parser->parser, char_handler);
    XML_SetUserData(parser->parser, parser);

    return 1;
}

stack_element*
stack_push(stack_element** stack, scew_element* element)
{
    stack_element* new_elem = (stack_element*) calloc(1, sizeof(stack_element));

    if (new_elem != NULL)
    {
        new_elem->element = element;
        if (stack != NULL)
        {
            new_elem->prev = *stack;
        }
        *stack = new_elem;
    }

    return new_elem;
}

scew_element*
stack_pop(stack_element** stack)
{
    scew_element* element = NULL;
    stack_element* sk_elem = NULL;

    if (stack != NULL)
    {
        sk_elem = *stack;
        if (sk_elem != NULL)
        {
            *stack = sk_elem->prev;
            element = sk_elem->element;
            free(sk_elem);
        }
    }

    return element;
}
