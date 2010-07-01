/**
 * @file     xparser.c
 * @brief    xparser.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Tue Dec 03, 2002 00:21
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

#include "xparser.h"

#include "str.h"

#include "xerror.h"

#include <assert.h>


/* Private */

struct stack_element
{
  scew_element* element;
  struct stack_element* prev;
};

/**
 * Expat callback for XML declaration.
 */
static void expat_xmldecl_handler_ (void *data,
                                    XML_Char const *version,
                                    XML_Char const *encoding,
                                    int standalone);

/**
 * Expat callback for data not handled by any other handler. This
 * handler will be used to read the preamble (between XML declaration
 * and root element).
 */
static void expat_default_handler_ (void *data, XML_Char const *str, int len);

/**
 * Expat callback for starting elements.
 */
static void expat_start_handler_ (void *data,
                                  XML_Char const *name,
                                  XML_Char const **attr);

/**
 * Expat callback for ending elements.
 */
static void expat_end_handler_ (void *data, XML_Char const *name);

/**
 * Expat callback for element contents.
 */
static void expat_char_handler_ (void *data, XML_Char const *str, int len);

/**
 * Tells Expat parser to stop due to a SCEW error.
 */
static void stop_expat_parsing_ (scew_parser *parser, scew_error error);

/**
 * Creates a new tree for the given parser (if not created already).
 */
static scew_tree* create_tree_ (scew_parser *parser);

/**
 * Creates a new element with the given name and attributes.
 */
static scew_element* create_element_ (XML_Char const *name,
                                      XML_Char const **attrs);

/**
 * Pushes an element into the stack returning the pushed element.
 */
static stack_element* parser_stack_push_ (scew_parser *parser,
                                          scew_element *element);

/**
 * Pops an element from the stack returning the new top element (not
 * the actual top).
 */
static scew_element* parser_stack_pop_ (scew_parser *parser);


/* Protected */

void
scew_parser_stack_free_ (scew_parser *parser)
{
  if (parser != NULL)
    {
      scew_element *element = parser_stack_pop_ (parser);
      while (element != NULL)
        {
          scew_element_free (element);
          element = parser_stack_pop_ (parser);
        }
    }
}

void
scew_parser_expat_install_handlers_ (scew_parser *parser)
{
  XML_SetXmlDeclHandler (parser->parser, expat_xmldecl_handler_);
  XML_SetDefaultHandler (parser->parser, expat_default_handler_);
  XML_SetElementHandler (parser->parser,
                         expat_start_handler_,
                         expat_end_handler_);
  XML_SetCharacterDataHandler (parser->parser, expat_char_handler_);

  /* Data to be passed to all handlers is the SCEW parser. */
  XML_SetUserData (parser->parser, parser);
}


/* Private (handlers) */

void
expat_xmldecl_handler_ (void *data,
                        XML_Char const *version,
                        XML_Char const *encoding,
                        int standalone)
{
  scew_parser *parser = (scew_parser *) data;

  if (NULL == parser)
    {
      stop_expat_parsing_ (parser, scew_error_internal);
      return;
    }

  /* If version is NULL this is a text declaration. */
  if (NULL == version)
    {
      return;
    }

  parser->tree = create_tree_ (parser);
  if (NULL == parser->tree)
    {
      stop_expat_parsing_ (parser, scew_error_no_memory);
      return;
    }

  scew_tree_set_xml_version (parser->tree, version);

  if (encoding != NULL)
    {
      scew_tree_set_xml_encoding (parser->tree, encoding);
    }

  /**
   * We need to add 1 as our standalone enumeration starts at 0. Expat
   * returns -1, 0 or 1.
   */
  scew_tree_set_xml_standalone (parser->tree, standalone + 1);
}

void
expat_default_handler_ (void *data, XML_Char const *str, int len)
{
  scew_parser *parser = (scew_parser *) data;

  if (NULL == parser)
    {
      stop_expat_parsing_ (parser, scew_error_internal);
      return;
    }

  /* Only analyze data if we still have to reach the root element. */
  if (NULL == parser->stack)
    {
      unsigned int total = 0;
      unsigned int total_old = 0;
      XML_Char *new_preamble = NULL;

      /* Get size of current preamble. */
      XML_Char *preamble = parser->preamble;
      if (preamble != NULL)
        {
          total_old = scew_strlen (preamble);
        }

      /**
       * Calculate new size and allocate enough space (+ 1 for
       * null-terminated string).
       */
      total = (total_old + len + 1) * sizeof (XML_Char);
      new_preamble = calloc (total, 1);

      /* Copy old preamble (if any) and concatenate new one. */
      if (preamble != NULL)
        {
          scew_strcpy (new_preamble, preamble);
        }
      scew_strncat (new_preamble, str, len);

      /* Get rid of old preamble and set new one. */
      free (preamble);

      parser->preamble = new_preamble;
    }
}

void
expat_start_handler_ (void *data,
                      XML_Char const *name,
                      XML_Char const **attrs)
{
  scew_parser *parser = (scew_parser *) data;
  scew_element *element = NULL;
  stack_element *stack = NULL;

  if (NULL == parser)
    {
      stop_expat_parsing_ (parser, scew_error_internal);
      return;
    }

  /* Create element. */
  element = create_element_ (name, attrs);
  if (NULL == element)
    {
      stop_expat_parsing_ (parser, scew_error_no_memory);
      return;
    }

  /* Add the element to its parent (if any). */
  if (parser->stack != NULL)
    {
      scew_element *parent = parser->stack->element;
      scew_element_add_element (parent, element);
    }

  /* Push element onto the stack. */
  stack = parser_stack_push_ (parser, element);
  if (NULL == stack)
    {
      stop_expat_parsing_ (parser, scew_error_no_memory);
      return;
    }
}

void
expat_end_handler_ (void *data, XML_Char const *elem)
{
  scew_parser *parser = (scew_parser *) data;
  scew_element *current = NULL;
  XML_Char const *contents = NULL;

  if (NULL == parser)
    {
      stop_expat_parsing_ (parser, scew_error_internal);
      return;
    }
  current = parser_stack_pop_ (parser);

  /* Trim element contents if necessary. */
  contents = scew_element_contents (current);
  if (parser->ignore_whitespaces && (contents != NULL))
    {
      /* We use the internal const pointer for performance reasons. */
      scew_strtrim ((XML_Char *) contents);
      if (scew_strlen (contents) == 0)
        {
          scew_element_free_contents (current);
        }
    }

  /* Call loaded element hook. */
  if (parser->element_hook.hook != NULL)
    {
      void *user_data = parser->element_hook.data;
      if (!parser->element_hook.hook (parser, current, user_data))
        {
          stop_expat_parsing_ (parser, scew_error_hook);
          return;
        }
    }

  /* If there are no more elements (root node) ... */
  if (NULL == parser->stack)
    {
      /**
       * ... we create the XML document tree. If we need to create the
       * tree here it means no XML declaration was found.
       */
      parser->tree = (NULL == parser->tree)
        ? create_tree_ (parser)
        : parser->tree;
      if (NULL == parser->tree)
        {
          stop_expat_parsing_ (parser, scew_error_no_memory);
          return;
        }

      /* Trim preamble and only use it if length is greater than 0. */
      if (parser->preamble != NULL)
        {
          scew_strtrim (parser->preamble);
          if (scew_strlen (parser->preamble) == 0)
            {
              free (parser->preamble);
              parser->preamble = NULL;
            }
          else
            {
              scew_tree_set_xml_preamble (parser->tree, parser->preamble);
            }
        }

      scew_tree_set_root_element (parser->tree, current);

      /* Call loaded tree hook. */
      if (parser->tree_hook.hook != NULL)
        {
          void *user_data = parser->tree_hook.data;
          if (!parser->tree_hook.hook (parser, parser->tree, user_data))
            {
              stop_expat_parsing_ (parser, scew_error_hook);
              return;
            }
        }
    }
}

void
expat_char_handler_ (void *data, XML_Char const *str, int len)
{
  scew_parser *parser = (scew_parser *) data;
  scew_element *current = NULL;
  XML_Char const *contents = NULL;
  XML_Char *new_contents = NULL;
  unsigned int total_old = 0;
  unsigned int total = 0;

  if (NULL == parser)
    {
      stop_expat_parsing_ (parser, scew_error_internal);
      return;
    }

  /**
   * We don't want to pop here, just get the current element. Pop is
   * done in the end handler.
   */
  current = parser->stack->element;

  /* Get size of current contents. */
  contents = scew_element_contents (current);
  if (contents != NULL)
    {
      total_old = scew_strlen (contents);
    }

  /**
   * Calculate new size and allocate enough space (+ 1 for
   * null-terminated string).
   */
  total = (total_old + len + 1) * sizeof (XML_Char);
  new_contents = calloc (total, 1);

  /* Copy old contents (if any) and concatenate new one. */
  if (contents != NULL)
    {
      scew_strcpy (new_contents, contents);
    }
  scew_strncat (new_contents, str, len);

  scew_element_set_contents (current, new_contents);

  /**
   * new_contents is duplicated inside scew_element_set_contents so it
   * is safe to free it here.
   */
  free (new_contents);
}


/* Private (miscellaneous) */

void
stop_expat_parsing_ (scew_parser *parser, scew_error error)
{
  if (parser != NULL)
    {
      XML_StopParser (parser->parser, XML_FALSE);
    }
  scew_error_set_last_error_ (error);
}

scew_tree*
create_tree_ (scew_parser *parser)
{
  scew_tree *tree = parser->tree;
  if (NULL == tree)
    {
      tree = scew_tree_create ();
    }

  return tree;
}

scew_element*
create_element_ (XML_Char const *name, XML_Char const **attrs)
{
  scew_element *element = scew_element_create (name);

  unsigned int i = 0;
  for (i = 0; (element != NULL) && (attrs[i] != NULL); i += 2)
    {
      scew_attribute *attr = scew_element_add_attribute_pair (element,
                                                              attrs[i],
                                                              attrs[i + 1]);
      if (NULL == attr)
        {
          scew_element_free (element);
          element = NULL;
        }
    }

  return element;
}


/* Private (stack) */

stack_element*
parser_stack_push_ (scew_parser *parser, scew_element *element)
{
  stack_element *stack = NULL;

  assert (parser != NULL);
  assert (element != NULL);

  stack = calloc (1, sizeof (stack_element));

  if (stack != NULL)
    {
      stack->element = element;
      if (parser->stack != NULL)
        {
          stack->prev = parser->stack;
        }
      parser->stack = stack;
    }

  return stack;
}

scew_element*
parser_stack_pop_ (scew_parser *parser)
{
  scew_element *element = NULL;
  stack_element *stack = NULL;

  assert (parser != NULL);

  stack = parser->stack;
  if (stack != NULL)
    {
      element = stack->element;
      parser->stack = stack->prev;
      free (stack);
    }

  return element;
}
