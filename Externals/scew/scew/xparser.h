/**
 * @file     xparser.h
 * @brief    SCEW private parser type declaration
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:57
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


#ifndef XPARSER_H_0211250057
#define XPARSER_H_0211250057

#include "export.h"

#include "parser.h"


/* Types */

/**
 * Stack to keep previous parsed elements.
 */
typedef struct stack_element stack_element;

typedef struct
{
  scew_parser_load_hook hook;   /**< Hook */
  void *data;                   /**< Hook user's data */
} load_hook;

struct scew_parser
{
  XML_Parser parser;            /**< Expat parser */
  scew_tree *tree;              /**< Current parsed XML document tree */
  XML_Char *preamble;           /**< Current XML document tree preamble */
  stack_element *stack;         /**< Current parsed element stack */
  scew_bool ignore_whitespaces; /**< Whether to ignore white spaces */
  load_hook element_hook;       /**< Hook for loaded elements */
  load_hook tree_hook;          /**< Hook for loaded trees */
};


/* Functions */

/**
 * Frees the stack of elements used while parsing XML elements.
 */
extern SCEW_LOCAL void scew_parser_stack_free_ (scew_parser *parser);

/**
 * Install SCEW Expat handlers. The Expat handlers are the main
 * interface between Expat and SCEW. The handlers will be called by
 * Expat while parsing XML documents and SCEW will create the
 * necessary data structures representing the XML being read.
 */
extern SCEW_LOCAL void
scew_parser_expat_install_handlers_ (scew_parser *parser);

#endif /* XPARSER_H_0211250057 */
