/**
 *
 * @file     attribute.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:39
 * @brief    SCEW attribute type declaration
 *
 * $Id: attribute.h,v 1.1 2004/01/28 00:43:21 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2002, 2003 Aleix Conchillo Flaque
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
 *
 * Attribute related functions. SCEW provides functions to access and
 * manipulate the attributes of all the elements in a tree. XML element
 * attributes are basically a name-value pair.
 */


#ifndef ATTRIBUTE_H_ALEIX0211250039
#define ATTRIBUTE_H_ALEIX0211250039

#include "types.h"

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Creates a new attribute with the given pair (name, value).
 *
 * @return the new created attribute.
 */
extern scew_attribute*
scew_attribute_create(XML_Char const* name, XML_Char const* value);

/**
 * Frees an attribute memory structure. That is, its name and value. You
 * should not call this function with an attribute coming from an
 * element, but created with <code>scew_attribute_create</code>.
 */
extern void
scew_attribute_free(scew_attribute* attribute);

/**
 * Returns the number of attributes of the specified element. An element
 * can have zero or more attributes.
 */
extern unsigned int
scew_attribute_count(scew_element const* element);

/**
 * Returns the <code>element</code>'s first attribute if
 * <code>attribute</code> is NULL, otherwise it returns the contiguous
 * attribute to the given one.
 *
 * Call this function a first time with <code>attribute</code> to NULL
 * and the first attribute will be returned. In the subsequent calls you
 * just need to provide the attribute returned and its contiguous
 * attribute will be returned.
 *
 * @return the first attribute of an element or a contiguous
 * attribute. NULL if there are no more attributes.
 */
extern scew_attribute*
scew_attribute_next(scew_element const* element,
                    scew_attribute const* attribute);

/**
 * Returns the element attribute on the specified position. Positions
 * are zero based.
 *
 * @return the attribute on the specified position, NULL if there is no
 * attribute in the position.
 */
extern scew_attribute*
scew_attribute_by_index(scew_element const* element, unsigned int idx);

/**
 * Returns the element attribute with the specified name. Remember that
 * XML names are case-sensitive.
 *
 * @return the attribute with the given name, NULL if not found.
 */
extern scew_attribute*
scew_attribute_by_name(scew_element const* element, XML_Char const* name);

/**
 * Returns the attribute name or NULL if the attribute does not exist.
 */
extern XML_Char const*
scew_attribute_name(scew_attribute const* attribute);

/**
 * Returns the attribute value or NULL if the attribute does not exist.
 */
extern XML_Char const*
scew_attribute_value(scew_attribute const* attribute);

/**
 * Sets a new name to the given attribute and frees the old one.
 *
 * @return the new attribute name.
 */
extern XML_Char const*
scew_attribute_set_name(scew_attribute* attribute, XML_Char const* name);

/**
 * Sets a new value to the given attribute and frees the old one.
 *
 * @return the new attribute value.
 */
extern XML_Char const*
scew_attribute_set_value(scew_attribute* attribute, XML_Char const* name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ATTRIBUTE_H_ALEIX0211250039 */
