/**
 *
 * @file     scew_print.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Wed Dec 04, 2002 01:11
 * @brief    SCEW usage example
 *
 * $Id: scew_print.c,v 1.21 2004/05/25 18:22:47 aleix Exp $
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
 * This example shows the usage of the SCEW API. It will print an XML
 * file given as the first program parameter.
 */

#include <scew/scew.h>

#include <stdio.h>


/* indentation size (in whitespaces) */
int const indent_size = 4;

void
print_indent(unsigned int indent)
{
    if (indent > 0)
    {
        printf("%*s", indent * indent_size, " ");
    }
}

void
print_attributes(scew_element* element)
{
    scew_attribute* attribute = NULL;

    if (element != NULL)
    {
        /**
         * Iterates through the element's attribute list, printing the
         * pair name-value.
         */
        attribute = NULL;
        while ((attribute = scew_attribute_next(element, attribute)) != NULL)
        {
            printf(" %s=\"%s\"", scew_attribute_name(attribute),
                   scew_attribute_value(attribute));
        }
    }
}

void
print_element(scew_element* element, unsigned int indent)
{
    scew_element* child = NULL;
    XML_Char const* contents = NULL;

    if (element == NULL)
    {
        return;
    }

    /**
     * Prints the starting element tag with its attributes.
     */
    print_indent(indent);
    printf("<%s", scew_element_name(element));
    print_attributes(element);
    printf(">");
    contents = scew_element_contents(element);
    if (contents == NULL)
    {
        printf("\n");
    }

    /**
     * Call print_element function again for each child of the
     * current element.
     */
    child = NULL;
    while ((child = scew_element_next(element, child)) != NULL)
    {
        print_element(child, indent + 1);
    }

    /* Prints element's content. */
    if (contents != NULL)
    {
        printf("%s", contents);
    }
    else
    {
        print_indent(indent);
    }

    /**
     * Prints the closing element tag.
     */
    printf("</%s>\n", scew_element_name(element));
}

int
main(int argc, char** argv)
{
    scew_tree* tree = NULL;
    scew_parser* parser = NULL;

    if (argc < 2)
    {
        printf("usage: scew_print file.xml\n");
        return EXIT_FAILURE;
    }

    /**
     * Creates an SCEW parser. This is the first function to call.
     */
    parser = scew_parser_create();

    scew_parser_ignore_whitespaces(parser, 1);

    /* Loads an XML file */
    if (!scew_parser_load_file(parser, argv[1]))
    {
        scew_error code = scew_error_code();
        printf("Unable to load file (error #%d: %s)\n", code,
               scew_error_string(code));
        if (code == scew_error_expat)
        {
            enum XML_Error expat_code = scew_error_expat_code(parser);
            printf("Expat error #%d (line %d, column %d): %s\n", expat_code,
                   scew_error_expat_line(parser),
                   scew_error_expat_column(parser),
                   scew_error_expat_string(expat_code));
        }
        return EXIT_FAILURE;
    }

    tree = scew_parser_tree(parser);

    /* Prints full tree */
    print_element(scew_tree_root(tree), 0);

    /* Remember to free tree (scew_parser_free does not free it) */
    scew_tree_free(tree);

    /* Frees the SCEW parser */
    scew_parser_free(parser);

    return 0;
}
