/**
 * @file     tree.h
 * @brief    SCEW tree handling routines
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Feb 20, 2003 23:32
 * @ingroup  SCEWTree, SCEWTreeAlloc, SCEWTreeProp, SCEWTreeContent
 *
 * @if copyright
 *
 * Copyright (C) 2003-2009 Aleix Conchillo Flaque
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
 * @defgroup SCEWTree Trees
 *
 * Tree related functions. SCEW provides functions to create new XML
 * trees. Trees are SCEW internal XML document representation. A tree
 * contains basic information, such as XML version and enconding, and
 * contains a root element which is the first XML node.
 */

#ifndef TREE_H_0302202332
#define TREE_H_0302202332

#include "export.h"

#include "element.h"

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * List of possible values for the standalone attribute.
 *
 * The standalone attribute in an XML declaration defines whether the
 * XML document is self consistent or not, that is, whether it needs
 * to load any extra files.
 *
 * @ingroup SCEWTreeProp
 */
typedef enum
  {
    scew_tree_standalone_unknown, /**< Standalone attribute not defined. */
    scew_tree_standalone_no,    /**< Extra files are necessary. */
    scew_tree_standalone_yes    /**< Document stands on its own. */
  } scew_tree_standalone;

/**
 * This is the type delcaration for XML trees.
 *
 * @ingroup SCEWTree
 */
typedef struct scew_tree scew_tree;

/**
 * SCEW tree compare hooks might be used to define new user XML tree
 * comparisons. The hooks are used by #scew_tree_compare.
 *
 * @return true if the given XML trees are considered equal, false
 * otherwise.
 *
 * @ingroup SCEWTreeCompare
 */
typedef scew_bool (*scew_tree_cmp_hook) (scew_tree const *, scew_tree const *);


/**
 * @defgroup SCEWTreeAlloc Allocation
 * Allocate and free XML trees.
 * @ingroup SCEWTree
 */

/**
 * Creates a new empty XML tree in memory. By default, the XML version
 * is set to 1.0, and the encoding to UTF-8, also a standalone
 * document is considered.
 *
 * @ingroup SCEWTreeAlloc
 */
extern SCEW_API scew_tree* scew_tree_create (void);

/**
 * Makes a deep copy of the given @a tree. A deep copy means that the
 * root element and its children will be copied recursively. XML
 * encoding, version and standalone attributes are also copied.
 *
 * @pre tree != NULL
 *
 * @param tree the tree to be duplicated.
 *
 * @return a new tree, or NULL if the copy failed.
 *
 * @ingroup SCEWTreeAlloc
 */
extern SCEW_API scew_tree* scew_tree_copy (scew_tree const *tree);

/**
 * Frees a tree memory structure. Call this function when you are done
 * with your XML document. This will also free the root element
 * recursively.
 *
 * @param tree the tree to delete.
 *
 * @ingroup SCEWTreeAlloc
 */
extern SCEW_API void scew_tree_free (scew_tree *tree);


/**
 * @defgroup SCEWTreeCompare Comparison
 * Tree comparison routines.
 * @ingroup SCEWTree
 */

/**
 * Performs a deep comparison for the given trees. The comparison is
 * done via the comparison @a hook. If @a hook is NULL, the default
 * comparison is done:
 *
 * - XML declaration: version, encoding and standalone attribute
 *   (encoding is considered case-sensitive).
 * - Preamble is considered case-sensitive as well.
 * - The root element comparison uses #scew_element_compare with a
 *   NULL element comparison hook.
 *
 * There is no restriction on the provided comparison hook (if any),
 * thus the user is responsible to define how the comparison is to be
 * done.
 *
 * @pre a != NULL
 * @pre b != NULL
 *
 * @param a one of the trees to compare.
 * @param b one of the trees to compare.
 * @param hook the user defined comparison function. If NULL, the
 * default comparison is used.
 *
 * @return true if trees are considered equal, false otherwise.
 *
 * @ingroup SCEWTreeCompare
 */
extern SCEW_API scew_bool scew_tree_compare (scew_tree const *a,
                                             scew_tree const *b,
                                             scew_tree_cmp_hook hook);


/**
 * @defgroup SCEWTreeProp Properties
 * Handle XML trees properties.
 * @ingroup SCEWTree
 */

/**
 * Returns the current XML version for the given @a tree. This is the
 * version specified in the "version" attribute in the XML
 * declaration.
 *
 * @pre tree != NULL
 *
 * @param tree the tree to return its version for.
 *
 * @return a string representing the XML version.
 *
 * @ingroup SCEWTreeProp
 */
extern SCEW_API XML_Char const*
scew_tree_xml_version (scew_tree const *tree);

/**
 * Sets the XML @a version in the XML declaration to the given @a
 * tree. Currently there is one XML version, so the value is always
 * 1.0. If there were more XML versions, this property tells to the
 * XML processor which one to use.
 *
 * @pre tree != NULL
 * @pre version != NULL
 *
 * @param tree the XML tree to set the new XML version to.
 * @param version the new XML version for the given tree.
 *
 * @ingroup SCEWTreeProp
 */
extern SCEW_API void scew_tree_set_xml_version (scew_tree *tree,
                                                XML_Char const *version);

/**
 * Returns the current XML character encoding for the given @a
 * tree. The default, when creating new SCEW trees, is UTF-8.
 *
 * Expat supports the following encodings:
 *
 * - UTF-8, ASCII and ISO-8859-1.
 * - UTF-16.
 *
 * As SCEW is based on Expat the same encodings are supported when
 * parsing XML documents. However, SCEW only supports UTF-16 in
 * Windows platforms.
 *
 * Note that these encodings are only supported when parsing files,
 * but not when creating new ones. So, it is the responsibility of the
 * user to provide the correct characters.
 *
 * @pre tree != NULL
 *
 * @param tree the XML tree to obtain its character encoding for.
 *
 * @return the character encoding for the given tree.
 *
 * @ingroup SCEWTreeProp
 */
extern SCEW_API XML_Char const* scew_tree_xml_encoding (scew_tree const *tree);

/**
 * Sets the character encoding used in the given XML @a tree. Note
 * that a user might want to use another encoding, different than the
 * ones supported by Expat. And, as SCEW does not provide, or force,
 * any encoding, the user is allowed to do so.
 *
 * @pre tree != NULL
 * @pre encoding != NULL
 *
 * @param tree the XML tree to set the new encoding to.
 * @param encoding the new character encoding for the given tree.
 *
 * @ingroup SCEWTreeProp
 */
extern SCEW_API void scew_tree_set_xml_encoding (scew_tree *tree,
                                                 XML_Char const *encoding);

/**
 * Returns whether the given @a tree is an standalone document. The
 * standalone property tells the XML processor whether there are any
 * other extra files to load, such as external entities or DTDs.
 *
 * @pre tree != NULL
 *
 * @param tree the tree to check its standalone property for.
 *
 * @return the XML tree standalone property.
 *
 * @ingroup SCEWTreeProp
 */
extern SCEW_API scew_tree_standalone
scew_tree_xml_standalone (scew_tree const *tree);

/**
 * The standalone property tells the XML processor whether there are
 * any other extra files to load, such as external entities or
 * DTDs. If the XML document can stand on its own, set it to
 * #scew_tree_standalone_yes.
 *
 * @pre tree != NULL
 *
 * @param tree the XML tree to set the option to.
 * @param standalone the new XML tree standalone property.
 *
 * @ingroup SCEWTreeProp
 */
extern SCEW_API void
scew_tree_set_xml_standalone (scew_tree *tree,
                              scew_tree_standalone standalone);


/**
 * @defgroup SCEWTreeContent Contents
 * Accessors for XML root elements and preambles.
 * @ingroup SCEWTree
 */

/**
 * Returns the root element of the given @a tree.
 *
 * @pre tree != NULL
 *
 * @return the tree's root element, or NULL if the tree does not have
 * a root element yet.
 *
 * @ingroup SCEWTreeContent
 */
extern SCEW_API scew_element* scew_tree_root (scew_tree const *tree);

/**
 * Creates the root element of an XML @a tree with the given @a
 * name. Note that if the tree already had a root element, it will be
 * overwritten, possibly causing a memory leak, as the old root
 * element is *not* automatically freed. So, if you plan to set a new
 * root element, remember to free the old one first.
 *
 * @pre tree != NULL
 * @pre name != NULL
 *
 * @param tree the XML tree to set a new root element to.
 * @param name the name of the new XML root element.
 *
 * @return the tree's root element, or NULL if the element could not
 * be created.
 *
 * @ingroup SCEWTreeContent
 */
extern SCEW_API scew_element* scew_tree_set_root (scew_tree *tree,
                                                  XML_Char const *name);

/**
 * Sets the root element of an XML @a tree with the given
 * element. Note that if the tree already had a root element, it will
 * be overwritten, possibly causing a memory leak, as the old root
 * element is *not* automatically freed. So, if you plan to set a new
 * root element, remember to free the old one first.
 *
 * @pre tree != NULL
 * @pre root != NULL
 *
 * @param tree the XML tree to set a new root element to.
 * @param root the new XML root element.
 *
 * @return the tree's root element, or NULL if the element could not
 * be created.
 *
 * @ingroup SCEWTreeContent
 */
extern SCEW_API scew_element* scew_tree_set_root_element (scew_tree *tree,
                                                          scew_element *root);
/**
 * Return the XML preamble for the given @a tree. The XML preamble is
 * the text between the XML declaration and the first element. It
 * typically contains DOCTYPE declarations or processing instructions.
 *
 * SCEW does not provide specific functions for DOCTYPEs or processing
 * instructions, but they are treated as a whole.
 *
 * @pre tree != NULL
 *
 * @param tree the XML tree to obtain the preamble for.
 *
 * @return the XML preamble, or NULL if no preamble is found.
 *
 * @ingroup SCEWTreeContent
 */
extern SCEW_API XML_Char const* scew_tree_xml_preamble (scew_tree const *tree);

/**
 * Sets the preamble string for the XML document. Typically this
 * contains DOCTYPE declarations or processing instructions. The old
 * XML tree preamble will be freed, if any.
 *
 * SCEW does not provide specific functions for DOCTYPEs or processing
 * instructions, but they can be added as a whole.
 *
 * @pre tree != NULL
 * @pre preamble != NULL
 *
 * @param tree the XML tree to set the preamble to.
 * @param preamble the XML preamble text for the given tree.
 *
 * @ingroup SCEWTreeContent
 */
extern SCEW_API void scew_tree_set_xml_preamble (scew_tree *tree,
                                                 XML_Char const *preamble);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TREE_H_0302202332 */
