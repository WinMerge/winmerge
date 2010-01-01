/**
 * @file     xattribute.h
 * @brief    SCEW private attribute type declaration
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Aug 24, 2009 23:44
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


#ifndef XATTRIBUTE_H_0908242344
#define XATTRIBUTE_H_0908242344

#include "export.h"

#include "attribute.h"


/* Types */

struct scew_attribute
{
  XML_Char *name;               /**< The attribute's name */
  XML_Char *value;              /**< The attribute's value */
  scew_element *parent;         /**< The XML element parent (if any) */
};


/* Functions */

/**
 * Sets a new @a parent to the given @a attribute, NULL is also
 * allowed. Note that the element should be first detached from its
 * old parent.
 *
 * @pre attribute != NULL
 */
extern SCEW_LOCAL void scew_attribute_set_parent_ (scew_attribute *attribute,
                                                   scew_element const *parent);

#endif /* XATTRIBUTE_H_0908242344 */
