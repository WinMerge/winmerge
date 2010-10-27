/**
 * @file     element.h
 * @brief    SCEW element's handling routines
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:48
 * @ingroup  SCEWElement, SCEWElementAcc, SCEWElementAttr, SCEWElementHier
 * @ingroup  SCEWElementAlloc, SCEWElementSearch
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
 * @defgroup SCEWElement Elements
 *
 * Element related functions. SCEW provides functions to access and
 * manipulate the elements of an XML tree.
 */

#ifndef ELEMENT_H_0211250048
#define ELEMENT_H_0211250048

#include "export.h"

#include "list.h"

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type delcaration for SCEW elements.
 *
 * @ingroup SCEWElement
 */
typedef struct scew_element scew_element;

/**
 * This is the type declaration for SCEW attributes.
 *
 * @ingroup SCEWAttribute
 */
typedef struct scew_attribute scew_attribute;

/**
 * SCEW element compare hooks might be used to define new user XML
 * element comparisons. This hook must only compare the element's name
 * and contents and the list of attributes. The new hook is to be used
 * by #scew_element_compare.
 *
 * @return true if the given elements are considered equal, false
 * otherwise.
 *
 * @ingroup SCEWElementCompare
 */
typedef scew_bool (*scew_element_cmp_hook) (scew_element const *,
                                            scew_element const *);


/**
 * @defgroup SCEWElementAlloc Allocation
 * Allocate and free elements.
 * @ingroup SCEWElement
 */

/**
 * Creates a new element with the given @a name. This element is not
 * yet related to any XML tree.
 *
 * @pre name != NULL
 *
 * @return the created element, or NULL if an error is found.
 *
 * @ingroup SCEWElementAlloc
 */
extern SCEW_API scew_element* scew_element_create (XML_Char const *name);

/**
 * Makes a deep copy of the given @a element. Attributes and children
 * elements will be copied. The new element will not belong to any XML
 * tree.
 *
 * @pre element != NULL
 *
 * @return a new element, or NULL if the copy failed.
 *
 * @ingroup SCEWElementAlloc
 */
extern SCEW_API scew_element* scew_element_copy (scew_element const *element);

/**
 * Frees the given @a element recursively. That is, it frees all its
 * children and attributes. If the @a element has a parent, it is also
 * detached from it. If a NULL @a element is given, this function does
 * not have any effect.
 *
 * @ingroup SCEWElementAlloc
 */
extern SCEW_API void scew_element_free (scew_element *element);


/**
 * @defgroup SCEWElementSearch Search and iteration
 * Iterate and search for elements.
 * @ingroup SCEWElement
 */

/**
 * Returns the first child from the specified @a element that matches
 * the given @a name. Remember that XML names are case-sensitive.
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @return the first child that matches the given @a name, or NULL if
 * not found.
 *
 * @ingroup SCEWElementSearch
 */
extern SCEW_API scew_element*
scew_element_by_name (scew_element const *element, XML_Char const *name);

/**
 * Returns the child of the given @a element at the specified
 * zero-based @a index.
 *
 * @pre element != NULL
 * @pre index < #scew_element_count
 *
 * @return the child at the specified position, or NULL if there are
 * no children elements.
 *
 * @ingroup SCEWElementSearch
 */
extern SCEW_API scew_element*
scew_element_by_index (scew_element const *element, unsigned int index);

/**
 * Returns a list of children from the specified @a element that
 * matches the given @a name. This list must be freed after using it
 * via #scew_list_free (the elements will not be freed, only the list
 * pointing to them).
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @return a list of elements that matches the @a name specified, or
 * NULL if no element is found.
 *
 * @ingroup SCEWElementSearch
 */
extern SCEW_API scew_list*
scew_element_list_by_name (scew_element const *element, XML_Char const *name);


/**
 * @defgroup SCEWElementCompare Comparison
 * Element comparison routines.
 * @ingroup SCEWElement
 */

/**
 * Performs a deep comparison of the two given elements. The
 * comparison is done via the comparison @a hook. If @a hook is NULL,
 * the default comparison is done:
 *
 * - Name and contents are equal (case-sensitive).
 * - Number of attributes match.
 * - Attribute names and values match (case-sensitive).
 *
 * It is important to note that, for any given hook (or if NULL), the
 * children are automatically traversed recursively using the given @a
 * hook. Therefore, the hook must only provide comparisons for
 * element's name and contents and the list of attribtues.
 *
 * There is no restriction on the provided comparison hook (if any),
 * thus the user is responsible to define how the comparison is to be
 * done.
 *
 * @pre a != NULL
 * @pre b != NULL
 *
 * @param a one of the elements to compare.
 * @param b one of the elements to compare.
 * @param hook the user defined comparison function. If NULL, the
 * default comparison is used.
 *
 * @return true if both elements are considered equal, false
 * otherwise.
 *
 * @ingroup SCEWElementCompare
 */
extern SCEW_API scew_bool scew_element_compare (scew_element const *a,
                                                scew_element const *b,
                                                scew_element_cmp_hook hook);


/**
 * @defgroup SCEWElementAcc Accessors
 * Access elements' data, such as name and contents.
 * @ingroup SCEWElement
 */

/**
 * Returns the given @a element's name.
 *
 * @pre element != NULL
 *
 * @return the @a element's name. It is not possible to get a NULL
 * value, as element names are mandatory.
 *
 * @ingroup SCEWElementAcc
 */
extern SCEW_API XML_Char const*
scew_element_name (scew_element const *element);

/**
 * Returns the given @a element's contents. That is, the text between
 * the start and end element tags.
 *
 * @pre element != NULL
 *
 * @return the @a element's contents, or NULL if the element has no
 * contents.
 *
 * @ingroup SCEWElementAcc
 */
extern SCEW_API XML_Char const*
scew_element_contents (scew_element const *element);

/**
 * Sets a new @a name to the given @a element and frees the old
 * one. If the new name can not be set, the old one is not freed.
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @return the new @a element's name, or NULL if the name can not be
 * set.
 *
 * @ingroup SCEWElementAcc
 */
extern SCEW_API XML_Char const* scew_element_set_name (scew_element *element,
                                                       XML_Char const *name);

/**
 * Sets a new @a contents to the given element and frees the old
 * one. If the new contents can not be set, the old one is not freed.
 *
 * @pre element != NULL
 *
 * @return the new @a element's contents, or NULL if the contents can
 * not be set.
 *
 * @ingroup SCEWElementAcc
 */
extern SCEW_API XML_Char const*
scew_element_set_contents (scew_element *element, XML_Char const *contents);

/**
 * Frees the current contents of the given @a element. If the @a
 * element has no contents, this functions does not have any effect.
 *
 * @pre element != NULL
 *
 * @ingroup SCEWElementAcc
 */
extern SCEW_API void scew_element_free_contents (scew_element *element);


/**
 * @defgroup SCEWElementHier Hierarchy
 * Handle element's hierarchy.
 * @ingroup SCEWElement
 */

/**
 * Returns the number of children of the specified @a element. An
 * element can have zero or more children.
 *
 * @pre element != NULL
 *
 * @return the number of children, or 0 if the @a element has no
 * children.
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API unsigned int scew_element_count (scew_element const *element);

/**
 * Returns the parent of the given @a element.
 *
 * @pre element != NULL
 *
 * @return the @a element's parent, or NULL if the given @a element
 * has no parent (e.g. root element).
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API scew_element*
scew_element_parent (scew_element const *element);

/**
 * Returns the list of all the @a element's children. This is the
 * internal list where @a element's children are stored, so no
 * modifications or deletions should be performed on this list.
 *
 * @pre element != NULL
 *
 * @return the list of the given @a element's children, or NULL if the
 * @a element has no children.
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API scew_list* scew_element_children (scew_element const *element);

/**
 * Creates and adds, as a child of @a element, a new element with the
 * given @a name.
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @return the new created element, or NULL if an error is found.
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API scew_element* scew_element_add (scew_element *element,
                                                XML_Char const *name);

/**
 * Creates and adds, as a child of @a element, a new element with the
 * given @a name and @a contents.
 *
 * @pre element != NULL
 * @pre name != NULL
 * @pre contents != NULL
 *
 * @return the new created element, or NULL if an error is found.
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API scew_element* scew_element_add_pair (scew_element *element,
                                                     XML_Char const *name,
                                                     XML_Char const *contents);

/**
 * Adds a @a child to the given @a element. Note that the element
 * being added should be a clean element, that is, an element created
 * with #scew_element_create or an element detached from another tree
 * after being detached (via #scew_element_detach).
 *
 * @pre element != NULL
 * @pre child != NULL
 * @pre #scew_element_parent (child) == NULL
 *
 * @return the element being added, or NULL if the element could not
 * be added.
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API scew_element* scew_element_add_element (scew_element *element,
                                                        scew_element *child);

/**
 * Deletes all the children for the given @a element. This function
 * deletes all subchildren recursively. This will automatically free
 * the elements.
 *
 * @pre element != NULL
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API void scew_element_delete_all (scew_element *element);

/**
 * Deletes all the children of the given @a element that matches @a
 * name. This will automatically free the element.
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API void scew_element_delete_all_by_name (scew_element *element,
                                                      XML_Char const *name);

/**
 * Deletes the first child of the given @a element that matches @a
 * name. This will automatically free the element.
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API void scew_element_delete_by_name (scew_element *element,
                                                  XML_Char const *name);

/**
 * Deletes the child of the given @a element at the specified
 * zero-based @a index. This will automatically free the element.
 *
 * @pre element != NULL
 * @pre index < #scew_element_count
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API void scew_element_delete_by_index (scew_element *element,
                                                   unsigned int index);

/**
 * Detaches the given @a element from its parent, if any. This
 * function only detaches the element, but does not free it. If the @a
 * element has no parent, this function does not have any effect.
 *
 * @pre element != NULL
 *
 * @ingroup SCEWElementHier
 */
extern SCEW_API void scew_element_detach (scew_element *element);


/**
 * @defgroup SCEWElementAttr Attributes
 * Handle element's attributes.
 * @ingroup SCEWElement
 */

/**
 * Returns the number of attributes of the given @a element. An
 * element can have zero or more attributes.
 *
 * @pre element != NULL
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API unsigned int
scew_element_attribute_count (scew_element const *element);

/**
 * Returns the list of all the @a element's attributes. This is the
 * internal list where @a element's attributes are stored, so no
 * modifications or deletions should be performed on this list.
 *
 * @pre element != NULL
 *
 * @return the list of the given @a element's attributes.
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API scew_list*
scew_element_attributes (scew_element const *element);

/**
 * Returns the first attribute from the specified @a element that matches
 * the given @a name. Remember that XML attributes are case-sensitive.
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @return the attribute with the given name, or NULL if not found.
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API scew_attribute*
scew_element_attribute_by_name (scew_element const *element,
                                XML_Char const *name);

/**
 * Returns the attribute of the given @a element at the specified
 * zero-based @a index.
 *
 * @pre element != NULL
 * @pre index < #scew_element_attribute_count
 *
 * @return the attribute at the specified position, or NULL if the @a
 * element has not attributes.
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API scew_attribute*
scew_element_attribute_by_index (scew_element const *element,
                                 unsigned int index);

/**
 * Adds an existent @a attribute to the given @a element. It is
 * important to note that the given @a attribute will be part of the
 * element's attributes (ownership is lost), so it should not be later
 * freed, and it should not be part of another attribute element list.
 *
 * Also note that, if the @a attribute already existed, the old value
 * will be overwritten and the given attribute will not become part of
 * the element's attribute list (only the old value is updated).
 *
 * @pre element != NULL
 * @pre attribute != NULL
 *
 * @return the new attribute added to the element, or NULL if the @a
 * attribute could not be added or updated.
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API scew_attribute*
scew_element_add_attribute (scew_element *element, scew_attribute *attribute);

/**
 * Creates and adds a new attribute to the given @a element. An
 * attribute is formed by a pair (name, value).
 *
 * If the attribute already existed, the old value will be
 * overwritten, thus the new attribute will not be created (only the
 * old value is updated).
 *
 * @pre element != NULL
 * @pre name != NULL
 * @pre value != NULL
 *
 * @return the new attribute added to the element, or NULL if the
 * attribute could not be added or updated.
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API scew_attribute*
scew_element_add_attribute_pair (scew_element *element,
                                 XML_Char const *name,
                                 XML_Char const *value);

/**
 * Deletes all the attributes of the given @a element. This will
 * also automatically free all the attributes.
 *
 * @pre element != NULL
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API void scew_element_delete_attribute_all (scew_element *element);

/**
 * Deletes the given @a attribute from the specified @a element. This
 * will also automatically free the given @a attribute.
 *
 * @pre element != NULL
 * @pre attribute != NULL
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API void scew_element_delete_attribute (scew_element *element,
                                                    scew_attribute *attribute);

/**
 * Deletes the first attribute of the given @a element that matches @a
 * name. This will also automatically free the attribute.
 *
 * @pre element != NULL
 * @pre name != NULL
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API void
scew_element_delete_attribute_by_name (scew_element *element,
                                       XML_Char const* name);

/**
 * Deletes the attribute of the given @a element at the specified
 * zero-based @a index. This will also automatically free the
 * attribute.
 *
 * @pre element != NULL
 * @pre index < #scew_element_attribute_count
 *
 * @ingroup SCEWElementAttr
 */
extern SCEW_API void
scew_element_delete_attribute_by_index (scew_element *element,
                                        unsigned int index);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ELEMENT_H_0211250048 */
