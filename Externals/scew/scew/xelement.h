/**
 *
 * @file     xelement.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:55
 * @brief    SCEW private element type declaration
 *
 * $Id: xelement.h,v 1.2 2004/03/08 22:36:38 aleix Exp $
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


#ifndef XELEMENT_H_ALEIX0211250055
#define XELEMENT_H_ALEIX0211250055

#include "element.h"

#include "xattribute.h"


/* Triply linked tree node */
struct _scew_element
{
    XML_Char* name;
    XML_Char* contents;
    attribute_list* attributes;

    unsigned int n_children;

    /* Immediately neighbour pointers */
    scew_element* parent;
    scew_element* child;
    scew_element* left;
    scew_element* right;

    /* Last element in list */
    scew_element* last_child;
};

#endif /* XELEMENT_H_ALEIX0211250055 */
