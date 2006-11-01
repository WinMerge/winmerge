/**
 *
 * @file     parser.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:58
 * @brief    SCEW parser type implementation
 *
 * $Id: parser.c,v 1.2 2004/05/25 20:23:04 aleix Exp $
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

#include "parser.h"

#include "tree.h"

#include "xerror.h"
#include "xparser.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* This code does not compile under VC .NET */
/* static int const  MAX_BUFFER_SIZE = 5000; */
#define MAX_BUFFER_SIZE 5000


scew_parser*
scew_parser_create()
{
    scew_parser* parser = NULL;

    parser = (scew_parser*) calloc(1, sizeof(scew_parser));
    if (parser == NULL)
    {
        set_last_error(scew_error_no_memory);
        return NULL;
    }

    if (!init_expat_parser(parser))
    {
        scew_parser_free(parser);
        return NULL;
    }

    /* ignore white spaces by default */
    parser->ignore_whitespaces = 1;

    /* no callback by default */
    parser->stream_callback = NULL;

    return parser;
}

void
scew_parser_free(scew_parser* parser)
{
    if (parser != NULL)
    {
        if (parser->parser)
        {
            XML_ParserFree(parser->parser);
        }
        free(parser);
    }
}

unsigned int
scew_parser_load_file(scew_parser* parser, char const* file_name)
{
    FILE* in = NULL;
    unsigned int res = 0;

    assert(parser != NULL);
    assert(file_name != NULL);

    in = fopen(file_name, "rb");
    if (in == NULL)
    {
        set_last_error(scew_error_io);
        return 0;
    }

    res = scew_parser_load_file_fp(parser, in);
    fclose(in);

    return res;
}

unsigned int
scew_parser_load_file_fp(scew_parser* parser, FILE* in)
{
    int len = 0;
    int done = 0;
    char buffer[MAX_BUFFER_SIZE];

    assert(parser != NULL);
    assert(in != NULL);

    while (!done)
    {
        len = fread(buffer, 1, MAX_BUFFER_SIZE, in);
        if (ferror(in))
        {
            set_last_error(scew_error_io);
            return 0;
        }

        done = feof(in);
        if (!XML_Parse(parser->parser, buffer, len, done))
        {
            set_last_error(scew_error_expat);
            return 0;
        }
    }

    return 1;
}

unsigned int
scew_parser_load_buffer(scew_parser* parser, char const* buffer,
                        unsigned int size)
{
    assert(parser != NULL);
    assert(buffer != NULL);

    if (!XML_Parse(parser->parser, buffer, size, 1))
    {
        set_last_error(scew_error_expat);
        return 0;
    }

    return 1;
}

unsigned int
scew_parser_load_stream(scew_parser* parser, char const* buffer,
                        unsigned int size)
{
    int start;
    int end;
    int length;

    assert(parser != NULL);
    assert(buffer != NULL);

    start = 0;
    end = 0;

    /**
     * Loop through the buffer.
     * if we encounter a '>', send the chunk to Expat.
     * if we hit the end of the buffer, send whatever remains to Expat.
     * if the we have a full element (stack is empty) we call the callback.
     */
    while ((start < size) && (end <= size))
    {
        if ((end == size) || (buffer[end] == '>'))
        {
            length = end - start;
            if (end < size)
            {
                length++;
            }

            if (!XML_Parse(parser->parser, &buffer[start], length, 0))
            {
                set_last_error(scew_error_expat);
                return 0;
            }

            if ((parser->tree != NULL) && (parser->current == NULL)
                && (parser->stack == NULL) && parser->stream_callback)
            {
                /* tell Expat we're done */
                XML_Parse(parser->parser, "", 0, 1);

                /* call the callback */
                if (!parser->stream_callback(parser))
                {
                    set_last_error(scew_error_callback);
                    return 0;
                }

                XML_ParserFree(parser->parser);
                scew_tree_free(parser->tree);
                parser->tree = NULL;
                init_expat_parser(parser);
            }
            start = end + 1;
        }
        end++;
    }

    return 1;
}

void
scew_parser_set_stream_callback(scew_parser* parser, SCEW_CALLBACK* cb)
{
    assert(parser != NULL);

    parser->stream_callback = cb;
}

scew_tree*
scew_parser_tree(scew_parser const* parser)
{
    assert(parser != NULL);

    return parser->tree;
}

XML_Parser
scew_parser_expat(scew_parser* parser)
{
    assert(parser != NULL);

    return parser->parser;
}

void
scew_parser_ignore_whitespaces(scew_parser* parser, int ignore)
{
    assert(parser != NULL);

    parser->ignore_whitespaces = ignore;
}
