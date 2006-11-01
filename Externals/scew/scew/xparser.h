/**
 *
 * @file     xparser.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:57
 * @brief    SCEW private parser type declaration
 *
 * $Id: xparser.h,v 1.2 2004/05/25 20:23:05 aleix Exp $
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


#ifndef XPARSER_H_ALEIX0211250057
#define XPARSER_H_ALEIX0211250057

#include "parser.h"

#include "xtree.h"
#include "xelement.h"


/* Stack to keep previous parsed elements */
typedef struct _stack_element
{
    scew_element* element;
    struct _stack_element* prev;
} stack_element;

struct _scew_parser
{
    XML_Parser parser;      /* Expat parser */
    scew_tree* tree;        /* XML document tree */
    scew_element* current;  /* Current parsed element */
    stack_element* stack;   /* Current parsed element stack */
    int ignore_whitespaces; /* 1 if ignore white spaces, 0 otherwise */
    SCEW_CALLBACK* stream_callback; /* Function to call while reading streams */
};

/* Creates and initializes Expat parser. */
unsigned int
init_expat_parser(scew_parser* parser);

/* Pushes an element into the stack. */
stack_element*
stack_push(stack_element** stack, scew_element* element);

/* Pops an element from the stack. */
scew_element*
stack_pop(stack_element** stack);

#endif /* XPARSER_H_ALEIX0211250057 */
