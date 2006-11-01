/**
 *
 * @file     xprint.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Mar 30, 2003 13:30
 * @brief    SCEW print functions
 *
 * $Id: xprint.h,v 1.1 2004/01/28 00:43:21 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2003 Aleix Conchillo Flaque
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


#ifndef XPRINT_H_ALEIX0303301330
#define XPRINT_H_ALEIX0303301330

#include "xtree.h"

#include <stdio.h>


/* Prints space indentation. */
void
indent_print(FILE* out, unsigned int indent);

/* Prints an XML tree into a file. */
void
tree_print(scew_tree const* tree, FILE* out);

/* Prints an XML element into a file. */
void
element_print(scew_element const* element, FILE* out, unsigned int indent);

/* Prints an XML attribute pair into a file. */
void
attribute_print(scew_attribute const* attribute, FILE* out);

#endif /* XPRINT_H_ALEIX0303301330 */
