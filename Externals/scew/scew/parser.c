/**
 * @file     parser.c
 * @brief    parser.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:58
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

#include "parser.h"

#include "xparser.h"
#include "xerror.h"

#include "tree.h"
#include "str.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


/* Private */

enum
  {
    MAX_PARSE_BUFFER_ = 1024    /**< Size (bytes) of the internal buffer */
  };

static scew_parser* parser_create_ (scew_bool namespace, XML_Char separator);

static scew_bool parse_reader_ (scew_parser *parser, scew_reader *reader);
static scew_bool parse_buffer_ (scew_parser *parser,
                                XML_Char const *buffer,
                                size_t size,
                                scew_bool done);

static scew_bool parse_stream_reader_ (scew_parser *parser,
                                       scew_reader *reader);
static scew_bool parse_stream_buffer_ (scew_parser *parser,
                                       XML_Char const *buffer,
                                       size_t size);



/* Public */

scew_parser*
scew_parser_create (void)
{
  return parser_create_ (SCEW_FALSE, 0);
}

scew_parser*
scew_parser_namespace_create (XML_Char separator)
{
  return parser_create_ (SCEW_TRUE, separator);
}

void
scew_parser_free (scew_parser *parser)
{
  if (parser != NULL)
    {
      /* Free all intermediate parser data (if used before). */
      scew_parser_reset (parser);

      /* Free Expat parser. */
      if (parser->parser)
        {
          XML_ParserFree (parser->parser);
        }

      free (parser);
    }
}

scew_tree*
scew_parser_load (scew_parser *parser, scew_reader *reader)
{
  scew_tree *tree = NULL;

  assert (parser != NULL);
  assert (reader != NULL);

  scew_parser_reset (parser);

  if (!parse_reader_ (parser, reader))
    {
      /* Free the allocated tree if something goes wrong. */
      scew_tree_free (parser->tree);
      parser->tree = NULL;
    }
  else
    {
      tree = parser->tree;
    }

  return tree;
}

scew_bool
scew_parser_load_stream (scew_parser *parser, scew_reader *reader)
{
  scew_bool result = SCEW_TRUE;

  assert (parser != NULL);
  assert (reader != NULL);
  assert (parser->tree_hook.hook != NULL);

  result = parse_stream_reader_ (parser, reader);
  if (!result)
    {
      /* Free the last allocated tree if something goes wrong. */
      scew_tree_free (parser->tree);
      parser->tree = NULL;
    }

  return result;
}

void
scew_parser_reset (scew_parser *parser)
{
  assert (parser != NULL);

  /* Free stack (to avoid memory leak if last load went wrong). */
  scew_parser_stack_free_ (parser);

  /* Free last loaded preamble. */
  free (parser->preamble);

  /* Reset Expat parser. */
  XML_ParserReset (parser->parser, NULL);
  scew_parser_expat_install_handlers_ (parser);

  /* Initialise structure fields to NULL. */
  parser->tree = NULL;
  parser->preamble = NULL;
  parser->stack = NULL;
}

void
scew_parser_set_element_hook (scew_parser *parser,
                              scew_parser_load_hook hook,
                              void *user_data)
{
  assert (parser != NULL);

  parser->element_hook.hook = hook;
  parser->element_hook.data = user_data;
}

void
scew_parser_set_tree_hook (scew_parser *parser,
                           scew_parser_load_hook hook,
                           void *user_data)
{
  assert (parser != NULL);

  parser->tree_hook.hook = hook;
  parser->tree_hook.data = user_data;
}

XML_Parser
scew_parser_expat (scew_parser *parser)
{
  assert (parser != NULL);

  return parser->parser;
}

void
scew_parser_ignore_whitespaces (scew_parser *parser, scew_bool ignore)
{
  assert (parser != NULL);

  parser->ignore_whitespaces = ignore;
}


/* Private */

scew_parser*
parser_create_ (scew_bool namespace, XML_Char separator)
{
#ifdef XML_UNICODE_WCHAR_T
  static XML_Char *encoding = _XT("UTF-16");
#else
  static XML_Char *encoding = NULL;
#endif /* XML_UNICODE_WCHAR_T */
  scew_parser *parser = calloc (1, sizeof (scew_parser));

  if (NULL == parser)
    {
      scew_error_set_last_error_ (scew_error_no_memory);
      return NULL;
    }

  /* Create Expat parser. */
  parser->parser = namespace
    ? XML_ParserCreateNS (encoding, separator)
    : XML_ParserCreate (encoding);

  if (parser->parser != NULL)
    {
      /* Ignore white spaces by default. */
      parser->ignore_whitespaces = SCEW_TRUE;

      /* No load hooks by default. */
      parser->element_hook.hook = NULL;
      parser->element_hook.data = NULL;
      parser->tree_hook.hook = NULL;
      parser->tree_hook.data = NULL;

      scew_parser_reset (parser);
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
      scew_parser_free (parser);
      parser = NULL;
    }

  return parser;
}

scew_bool
parse_reader_ (scew_parser *parser, scew_reader *reader)
{
  scew_bool done = SCEW_FALSE;
  scew_bool result = SCEW_TRUE;

  assert (parser != NULL);
  assert (reader != NULL);

  while (!done && result)
    {
      XML_Char buffer[MAX_PARSE_BUFFER_ + 1];

      /* Read files in small chunks. */
      size_t length = scew_reader_read (reader, buffer, MAX_PARSE_BUFFER_);
      if (scew_reader_error (reader))
        {
          scew_error_set_last_error_ (scew_error_io);
          result = SCEW_FALSE;
        }
      else
        {
          done = scew_reader_end (reader);
          result = parse_buffer_ (parser, buffer, length, done);
        }
    }

  return result;
}

scew_bool
parse_buffer_ (scew_parser *parser,
               XML_Char const *buffer,
               size_t size,
               scew_bool done)
{
  scew_bool result = SCEW_TRUE;
  size_t byte_no = size * sizeof (XML_Char);

  if (done || !scew_isempty (buffer))
    {
      if (!XML_Parse (parser->parser, (char *) buffer, byte_no, done))
        {
          scew_error_set_last_error_ (scew_error_expat);
          result = SCEW_FALSE;
        }
    }

  return result;
}

scew_bool
parse_stream_reader_ (scew_parser *parser, scew_reader *reader)
{
  scew_bool done = SCEW_FALSE;
  scew_bool result = SCEW_TRUE;

  assert(parser != NULL);
  assert(reader != NULL);

  while (!done && result)
    {
      XML_Char buffer[MAX_PARSE_BUFFER_ + 1];

      /* Read files in small chunks. */
      size_t length = scew_reader_read (reader, buffer, MAX_PARSE_BUFFER_);
      if (scew_reader_error (reader))
        {
          scew_error_set_last_error_ (scew_error_io);
          result = SCEW_FALSE;
        }
      else
        {
          result = parse_stream_buffer_ (parser, buffer, length);
          done = ((0 == length) || scew_reader_end (reader));
        }
    }

  return result;
}

scew_bool
parse_stream_buffer_ (scew_parser *parser, XML_Char const *buffer, size_t size)
{
  unsigned int start = 0;
  unsigned int end = 0;
  unsigned int length = 0;

  assert(parser != NULL);
  assert(buffer != NULL);

  /**
   * Loop through the buffer and:
   *    if we encounter a '>', send the chunk to Expat.
   *    if we hit the end of the buffer, send whatever remains to Expat.
   */
  while ((start < size) && (end <= size))
    {
      /* Skip initial whitespaces. */
      while ((start < size) && scew_isspace (buffer[start]))
        {
          start += 1;
          end += 1;
        }

      if ((end == size) || (buffer[end] == _XT('>')))
        {
          length = end - start;
          if (end < size)
            {
              length += 1;
            }

          if (!parse_buffer_ (parser, &buffer[start], length, SCEW_FALSE))
            {
              return SCEW_FALSE;
            }

          if ((parser->tree != NULL)
              && (scew_tree_root (parser->tree) != NULL)
              && (NULL == parser->stack))
            {
              /* Tell Expat we're done. */
              if (!parse_buffer_ (parser, _XT(""), 0, SCEW_TRUE))
                {
                  return SCEW_FALSE;
                }

              /**
               * We don't need to free last loaded XML, as it's the
               * users responsibility.
               */
              parser->tree = NULL;

              /* Reset parser to continue using it. */
              scew_parser_reset (parser);
            }
          start = end + 1;
        }
      end += 1;
    }

  return SCEW_TRUE;
}
