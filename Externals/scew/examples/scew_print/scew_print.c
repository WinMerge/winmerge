/**
 * @file     scew_print.c
 * @brief    SCEW usage example
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Wed Dec 04, 2002 01:11
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
 *
 * This example shows the usage of the SCEW API. It will print an XML
 * file given as the first program parameter.
 */

#include <scew/scew.h>

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
#include <fcntl.h>
#include <io.h>
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

#include <stdio.h>

static void
print_indent (unsigned int indent)
{
  /* Indentation size (in whitespaces). */
  static unsigned int const INDENT_SIZE = 4;

    if (indent > 0)
    {
      scew_printf (_XT("%*s"), indent * INDENT_SIZE, " ");
    }
}

static void
print_attributes (scew_element *element)
{
    if (element != NULL)
    {
        /**
         * Iterates through the element's attribute list, printing the
         * pair name-value.
         */
      scew_list *list = scew_element_attributes (element);
      while (list != NULL)
        {
          scew_attribute *attribute = scew_list_data (list);
          scew_printf (_XT(" %s=\"%s\""),
                       scew_attribute_name (attribute),
                       scew_attribute_value (attribute));
          list = scew_list_next (list);
        }
    }
}

static void
print_element (scew_element *element, unsigned int indent)
{
  XML_Char const *contents = NULL;
  scew_list *list = NULL;

    if (element == NULL)
    {
        return;
    }

  /* Prints the starting element tag with its attributes. */
  print_indent (indent);
  scew_printf (_XT("<%s"), scew_element_name (element));
  print_attributes (element);
  scew_printf (_XT(">"));

  contents = scew_element_contents (element);

    if (contents == NULL)
    {
      scew_printf (_XT("\n"));
    }

    /**
   * Call print_element function again for each child of the current
   * element.
     */
  list = scew_element_children (element);
  while (list != NULL)
    {
      scew_element *child = scew_list_data (list);
      print_element (child, indent + 1);
      list = scew_list_next (list);
    }

    /* Prints element's content. */
    if (contents != NULL)
    {
      scew_printf (_XT("%s"), contents);
    }
    else
    {
      print_indent (indent);
    }

  /* Prints the closing element tag. */
  scew_printf (_XT("</%s>\n"), scew_element_name (element));
}

int
main (int argc, char *argv[])
{
  scew_reader *reader = NULL;
  scew_parser *parser = NULL;
  scew_tree *tree = NULL;
  scew_writer *writer = NULL;
  scew_printer *printer = NULL;

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
  /* Change stdout to Unicode before writing anything. */
  _setmode(_fileno(stdout), _O_U16TEXT);
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

    if (argc < 2)
    {
      scew_printf (_XT("Usage: scew_print file.xml\n"));
        return EXIT_FAILURE;
    }

  /* Creates an SCEW parser. This is the first function to call. */
  parser = scew_parser_create ();

  scew_parser_ignore_whitespaces (parser, SCEW_TRUE);

  /* Loads an XML file. */
  reader = scew_reader_file_create (argv[1]);
  if (reader == NULL)
    {
      scew_error code = scew_error_code ();
      scew_printf (_XT("Unable to load file (error #%d: %s)\n"),
                   code, scew_error_string (code));
    }

  tree = scew_parser_load (parser, reader);
  if (tree == NULL)
    {
      scew_error code = scew_error_code ();
      scew_printf (_XT("Unable to parse file (error #%d: %s)\n"),
                   code, scew_error_string (code));
        if (code == scew_error_expat)
        {
          enum XML_Error expat_code = scew_error_expat_code (parser);
          scew_printf (_XT("Expat error #%d (line %d, column %d): %s\n"),
                       expat_code,
                       scew_error_expat_line (parser),
                       scew_error_expat_column (parser),
                       scew_error_expat_string (expat_code));
    }

      /* Frees the SCEW parser and reader. */
      scew_reader_free (reader);
      scew_parser_free (parser);

      return EXIT_FAILURE;
    }

  /* Prints full tree. */
  scew_printf (_XT("\n*** Manual print:\n\n"));
  print_element (scew_tree_root (tree), 0);

  /* Prints full tree using SCEW writer. */
  scew_printf (_XT("\n\n*** SCEW writer (stdout) print:\n\n"));

  writer = scew_writer_fp_create (stdout);
  printer = scew_printer_create (writer);
  scew_printer_print_tree (printer, tree);
  scew_printf (_XT("\n"));

  /* Remember to free tree (scew_parser_free does not free it). */
  scew_tree_free (tree);

  /* Also free the printer and writer. */
  scew_writer_free (writer);
  scew_printer_free (printer);

  /* Frees the SCEW parser and reader. */
  scew_reader_free (reader);
  scew_parser_free (parser);

    return 0;
}
