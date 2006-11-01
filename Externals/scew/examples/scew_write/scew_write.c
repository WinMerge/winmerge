/**
 *
 * @file     scew_write.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Mar 30, 2003 12:21
 * @brief    SCEW usage example
 *
 * $Id: scew_write.c,v 1.8 2004/05/25 18:17:45 aleix Exp $
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
 * This example shows the usage of the SCEW API. It will create a new
 * XML and write it to a file.
 *
 * We will create an XML with the follwing structure:
 *
 *   <scew_test>
 *     <element>
 *       element contents.
 *     </element>
 *     <element attribute="value"/>
 *     <element attribute1="value1" attribute2="value2"/>
 *     <element>
 *       <sub_element attribute="value"/>
 *       <sub_element attribute1="value1" attribute2="value2">
 *         <sub_sub_element attribute1="value1" attribute2="new_value2" attribute3="value3">
 *           element contents.
 *         </sub_sub_element>
 *       </sub_element>
 *     </element>
 *   </scew_test>
 */

#include <scew/scew.h>

#include <stdio.h>

int
main(int argc, char** argv)
{
    scew_tree* tree = NULL;
    scew_element* root = NULL;
    scew_element* element = NULL;
    scew_element* sub_element = NULL;
    scew_element* sub_sub_element = NULL;
    scew_attribute* attribute = NULL;

    if (argc < 2)
    {
        printf("usage: scew_write new_file.xml\n");
        return EXIT_FAILURE;
    }

    /**
     * Create an empty XML tree in memory, and add a root element
     * "scew_test".
     */
    tree = scew_tree_create();
    root = scew_tree_add_root(tree, "scew_test");

    /* Add an element and set element contents. */
    element = scew_element_add(root, "element");
    scew_element_set_contents(element, "element contents.");

    /* Add an element with an attribute pair (name, value). */
    element = scew_element_add(root, "element");
    scew_element_add_attr_pair(element, "attribute", "value");

    element = scew_element_add(root, "element");
    scew_element_add_attr_pair(element, "attribute1", "value1");

    /**
     * Another way to add an attribute. You loose attribute ownership,
     * so there is no need to free it.
     */
    attribute = scew_attribute_create("attribute2", "value2");
    scew_element_add_attr(element, attribute);

    element = scew_element_add(root, "element");
    sub_element = scew_element_add(element, "sub_element");
    scew_element_add_attr_pair(sub_element, "attribute", "value");

    sub_element = scew_element_add(element, "sub_element");
    scew_element_add_attr_pair(sub_element, "attribute1", "value1");
    scew_element_add_attr_pair(sub_element, "attribute2", "value2");

    sub_sub_element = scew_element_add(sub_element, "sub_sub_element");
    scew_element_add_attr_pair(sub_sub_element, "attribute1", "value1");
    scew_element_add_attr_pair(sub_sub_element, "attribute2", "value2");
    scew_element_add_attr_pair(sub_sub_element, "attribute3", "value3");
    /* Check attribute2 replacement. */
    scew_element_add_attr_pair(sub_sub_element, "attribute2", "new_value2");
    scew_element_set_contents(sub_sub_element, "element contents.");

    /**
     * Save an XML tree to a file.
     */
    if (!scew_writer_tree_file(tree, argv[1]))
    {
        printf("Unable to create %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* Frees the SCEW tree */
    scew_tree_free(tree);

    return 0;
}
