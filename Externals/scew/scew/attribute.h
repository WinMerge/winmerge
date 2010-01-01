/**
 * @file     attribute.h
 * @brief    SCEW attribute's handling routines
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:39
 * @ingroup  SCEWAttribute, SCEWAttributeAlloc, SCEWAttributeAcc
 * @ingroup  SCEWAttributeCompare, SCEWAttributeHier
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

/**
 * @defgroup SCEWAttribute Attributes
 *
 * SCEW provides functions to access and manipulate the attributes of
 * an element. XML element attributes are basically a name-value pair.
 */

#ifndef ATTRIBUTE_H_0211250039
#define ATTRIBUTE_H_0211250039

#include "element.h"
#include "bool.h"

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @defgroup SCEWAttributeAlloc Allocation
 * Allocate and free attributes.
 * @ingroup SCEWAttribute
 */

/**
 * Creates a new attribute with the given pair (@a name, @a value).
 *
 * @pre name != NULL
 * @pre value != NULL
 *
 * @return the created attribute, or NULL if an error is found.
 *
 * @ingroup SCEWAttributeAlloc
 */
extern SCEW_API scew_attribute* scew_attribute_create (XML_Char const *name,
                                                       XML_Char const *value);

/**
 * Makes a copy of the given @a attribute. Note that the new copy does
 * not belong to any element.
 *
 * @pre attribute != NULL
 *
 * @return a new attribute, or NULL if the copy failed.
 *
 * @ingroup SCEWAttributeAlloc
 */
extern SCEW_API scew_attribute*
scew_attribute_copy (scew_attribute const *attribute);

/**
 * Frees the given @a attribute. That is, its name and value. You
 * should not call this function with an attribute obtained from an
 * element, use #scew_element_delete_attribute instead. If a NULL @a
 * attribute is given, this function does not have any effect.
 *
 * @ingroup SCEWAttributeAlloc
 */
extern SCEW_API void scew_attribute_free (scew_attribute *attribute);


/**
 * @defgroup SCEWAttributeCompare Comparison
 * Attribute comparison routines.
 * @ingroup SCEWAttribute
 */

/**
 * Performs a comparison between the two given attributes. That is,
 * name and value must be equal in both attributes. Attribute's
 * elements are not compared.
 *
 * Remember that XML is case-sensitive.
 *
 * @pre a != NULL
 * @pre b != NULL
 *
 * @return true if attributes are equal, false otherwise.
 *
 * @ingroup SCEWAttributeCompare
 */
extern SCEW_API scew_bool scew_attribute_compare (scew_attribute const *a,
                                                  scew_attribute const *b);


/**
 * @defgroup SCEWAttributeAcc Accessors
 * Access attributes' data, such as name and value.
 * @ingroup SCEWAttribute
 */

/**
 * Returns the given @a attribute's name.
 *
 * @pre attribute != NULL
 *
 * @ingroup SCEWAttributeAcc
 */
extern SCEW_API XML_Char const*
scew_attribute_name (scew_attribute const *attribute);

/**
 * Returns the given @a attribute's value.
 *
 * @pre attribute != NULL
 *
 * @ingroup SCEWAttributeAcc
 */
extern SCEW_API XML_Char const*
scew_attribute_value (scew_attribute const *attribute);

/**
 * Sets a new @a name to the given @a attribute and frees the old
 * one. If an error is found, the old name is not freed.
 *
 * @pre attribute != NULL
 * @pre name != NULL
 *
 * @return the new @a attribute's name, or NULL if the new name can
 * not be set.
 *
 * @ingroup SCEWAttributeAcc
 */
extern SCEW_API XML_Char const*
scew_attribute_set_name (scew_attribute *attribute, XML_Char const *name);

/**
 * Sets a new @a value to the given @a attribute and frees the old
 * one. If an error is found, the old value is not freed.
 *
 * @pre attribute != NULL
 * @pre name != NULL
 *
 * @return the new @a attribute's value, or NULL if the new value
 * could not be set.
 *
 * @ingroup SCEWAttributeAcc
 */
extern SCEW_API XML_Char const*
scew_attribute_set_value (scew_attribute *attribute, XML_Char const *value);


/**
 * @defgroup SCEWAttributeHier Hierarchy
 * Handle attribute's hierarchy.
 * @ingroup SCEWAttribute
 */

/**
 * Returns the element that the given @a attribute belongs to.
 *
 * @pre attribute != NULL
 *
 * @return the given @a attribute's element, or NULL if the @a
 * attribute is an standalone attribute.
 *
 * @ingroup SCEWAttributeHier
 */
extern SCEW_API scew_element*
scew_attribute_parent (scew_attribute const *attribute);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ATTRIBUTE_H_0211250039 */
