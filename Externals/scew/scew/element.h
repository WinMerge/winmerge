/**
 *
 * @file     element.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:48
 * @brief    SCEW element type declaration
 *
 * $Id: element.h,v 1.4 2004/03/08 22:46:04 aleix Exp $
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
 *
 * Element related functions. SCEW provides functions to access and
 * manipulate the elements of an XML tree.
 */


#ifndef ELEMENT_H_ALEIX0211250048
#define ELEMENT_H_ALEIX0211250048

#include "types.h"

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Creates a new element with the given name. This element is not yet
 * related to any XML tree.
 */
extern scew_element*
scew_element_create(XML_Char const* name);

/**
 * Frees an element recursively. That is, it frees all his childs and
 * attributes.
 */
extern void
scew_element_free(scew_element* element);

/**
 * Returns the number of children of the specified element. An element
 * can have zero or more children.
 */
extern unsigned int
scew_element_count(scew_element const* element);

/**
 * Returns the <code>parent</code>'s child element if
 * <code>element</code> is NULL, otherwise it returns the contiguous
 * element to the given one.
 *
 * Call this function a first time with <code>element</code> to NULL and
 * a parent's child element will be returned. In the subsequent calls
 * you just need to provide the element returned and its sibling will be
 * returned.
 *
 * @return a parent's child element or a contiguous element. NULL if
 * there are no more elements.
 */
extern scew_element*
scew_element_next(scew_element const* parent, scew_element const* element);

/**
 * Returns the child element on the specified position. Positions are
 * zero based.
 *
 * <b>Note:</b> Do not call this function if you just want to iterate
 * through the elements, because you would have a serious performance
 * degradation in large documents. Use <code>scew_element_next</code>
 * instead.
 *
 * This function is for element random access.
 *
 * @see scew_element_next
 *
 * @return the element on the specified position, NULL if there is no
 * element in the position.
 */
extern scew_element*
scew_element_by_index(scew_element* parent, unsigned int idx);

/**
 * Returns the first element child that matches the given name. Remember
 * that XML names are case-sensitive.
 *
 * @return the first element that matches the given name, NULL if not
 * found.
 */
extern scew_element*
scew_element_by_name(scew_element const* parent, XML_Char const* name);

/**
 * Returns a list of elements that matches the name specified. The
 * number of elements is returned in count. The returned pointer must be
 * freed after using it, calling the <code>scew_element_list_free</code>
 * function.
 */
extern scew_element**
scew_element_list(scew_element const* parent, XML_Char const* name,
                  unsigned int* count);

/**
 * Frees an element list created by <code>scew_element_list</code>.
 */
extern void
scew_element_list_free(scew_element** lst);

/**
 * Returns the element name or NULL if the element does not exist.
 */
extern XML_Char const*
scew_element_name(scew_element const* element);

/**
 * Returns the element contents value. That is the contents between the
 * start and end element tags. Returns NULL if element has no contents.
 */
extern XML_Char const*
scew_element_contents(scew_element const* element);

/**
 * Sets a new name to the given element and frees the old one.
 *
 * @return the new element name.
 */
extern XML_Char const*
scew_element_set_name(scew_element* element, XML_Char const* name);

/**
 * Sets the new contents to the given element and frees the old one.
 *
 * @return the new element's contents.
 */
extern XML_Char const*
scew_element_set_contents(scew_element* element, XML_Char const* data);

/**
 * Adds a new child to the given element with the specified name.
 *
 * @return the new element created.
 */
extern scew_element*
scew_element_add(scew_element* element, XML_Char const* name);

/**
 * Adds an already existent element (<code>new_elem</code>) as a child
 * of the given element (<code>element</code>). Note that the element
 * being added should be a clean element, that is, an element created
 * with <code>scew_element_create</code>, and not an element from
 * another XML tree.
 *
 * @see scew_element_create
 *
 * @return the element added.
 */
extern scew_element*
scew_element_add_elem(scew_element* element, scew_element* new_elem);

/**
 * This function is equivalent to <code>scew_element_free</code>.
 *
 * @see scew_element_free
 */
extern void
scew_element_del(scew_element* element);

/**
 * Deletes the first child of the given element that matches
 * <code>name</code>.
 */
extern void
scew_element_del_by_name(scew_element* element, XML_Char const* name);

/**
 * Deletes the child element at the specified position.
 */
extern void
scew_element_del_by_index(scew_element* element, unsigned int idx);

/**
 * Deletes all child elements of the given element that match
 * <code>name</code>. This function will get an element list created by
 * <code>scew_element_list</code> and will free all the elements in
 * it. Note that this function is not equivalent to
 * <code>scew_element_list_free</code>.
 *
 * @see scew_element_list
 */
extern void
scew_element_list_del(scew_element* element, XML_Char const* name);

/**
 * Adds an already created attribute to the element. If the attribute
 * already exist, the old value will be overwritten. It is important to
 * note that the attribute given will be part of the element's
 * attributes (ownership is lost), so it should not be freed, and it
 * should not be part of another attribute element list.
 *
 * @see attribute.h
 *
 * @return the new attribute added to the element.
 */
scew_attribute*
scew_element_add_attr(scew_element* element, scew_attribute* attribute);

/**
 * Adds a new attribute to the given element. An attribute is formed by
 * a pair (name, value). If the attribute already exist, the old value
 * will be overwritten.
 *
 * @see attribute.h
 *
 * @return the new attribute added to the element.
 */
extern scew_attribute*
scew_element_add_attr_pair(scew_element* element,
                           XML_Char const* name, XML_Char const* value);

/**
 * Deletes an attribute from an element.
 */
extern void
scew_element_del_attr(scew_element* element, XML_Char const* name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ELEMENT_H_ALEIX0211250048 */
