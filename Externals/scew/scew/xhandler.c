/**
 *
 * @file     xhandler.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:21
 * @brief    SCEW Expat handlers
 *
 * $Id: xhandler.c,v 1.2 2004/05/25 20:23:05 aleix Exp $
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

#include "xhandler.h"

#include "xparser.h"

#include "str.h"

#include <stdio.h>

void
xmldecl_handler(void* data, XML_Char const* version, XML_Char const* encoding,
                int standalone)
{
    scew_parser* parser = (scew_parser*) data;

    /* Avoid warning: standalone is unused */
    (void) standalone;

    if (parser == NULL)
    {
        return;
    }

    if (parser->tree == NULL)
    {
        parser->tree = scew_tree_create();
    }

    if (parser->tree == NULL)
    {
        return;
    }

    if (version != NULL)
    {
        parser->tree->version = scew_strdup(version);
    }
    if (encoding != NULL)
    {
        parser->tree->encoding = scew_strdup(encoding);
    }

    /* by now, we ignore standalone attribute */
}

void
start_handler(void* data, XML_Char const* elem, XML_Char const** attr)
{
    int i = 0;
    scew_parser* parser = (scew_parser*) data;

    if (parser == NULL)
    {
        return;
    }

    if ((parser->tree == NULL) || (scew_tree_root(parser->tree) == NULL))
    {
        if (parser->tree == NULL)
        {
            parser->tree = scew_tree_create();
        }
        parser->current = scew_tree_add_root(parser->tree, elem);
    }
    else
    {
        stack_push(&parser->stack, parser->current);
        parser->current = scew_element_add(parser->current, elem);
    }

    for (i = 0; attr[i]; i += 2)
    {
        scew_element_add_attr_pair(parser->current, attr[i], attr[i + 1]);
    }
}

void
end_handler(void* data, XML_Char const* elem)
{
    XML_Char* contents = NULL;
    scew_element* current = NULL;
    scew_parser* parser = (scew_parser*) data;

    /* Avoid warning: elem is unused */
    (void) elem;

    if (parser == NULL)
    {
        return;
    }

    current = parser->current;
    if ((current != NULL) && (current->contents != NULL))
    {
        if (parser->ignore_whitespaces)
        {
            scew_strtrim(current->contents);
            if (scew_strlen(current->contents) == 0)
            {
                free(current->contents);
                current->contents = NULL;
            }
        }
        else
        {
            contents = scew_strdup(current->contents);
            scew_strtrim(contents);
            if (scew_strlen(contents) == 0)
            {
                free(current->contents);
                current->contents = NULL;
            }
            free(contents);
        }
    }
    parser->current = stack_pop(&parser->stack);
}

void
char_handler(void* data, XML_Char const* s, int len)
{
    int total = 0;
    int total_old = 0;
    scew_element* current = NULL;
    scew_parser* parser = (scew_parser*) data;

    if (parser == NULL)
    {
        return;
    }

    current = parser->current;

    if (current == NULL)
    {
        return;
    }

    if (current->contents != NULL)
    {
        total_old = scew_strlen(current->contents);
    }
    total = (total_old + len + 1) * sizeof(XML_Char);
    current->contents = (XML_Char*) realloc(current->contents, total);

    if (total_old == 0)
    {
        current->contents[0] = '\0';
    }

    scew_strncat(current->contents, s, len);
}
