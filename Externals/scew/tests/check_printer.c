/**
 * @file     check_printer.c
 * @brief    Unit testing for SCEW printer
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Sep 03, 2009 00:10
 *
 * @if copyright
 *
 * Copyright (C) 2009 Aleix Conchillo Flaque
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

#include "test.h"

#include <scew/printer.h>
#include <scew/writer_buffer.h>

#include <check.h>


/* Unit tests */

static XML_Char const *TEST_TREE_CONTENTS =
  _XT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n"
      "<test>\n"
      "   <element>element contents</element>\n"
      "   <element attribute=\"value\"/>\n"
      "   <element attribute1=\"value1\" attribute2=\"value2\"/>\n"
      "   <element>\n"
      "      <subelement attribute=\"value\"/>\n"
      "      <subelement attribute1=\"value1\" attribute2=\"value2\">\n"
      "         <subsubelement>With accents: à é è í ó ú</subsubelement>\n"
      "      </subelement>\n"
      "   </element>\n"
      "</test>\n");

static XML_Char const *TEST_ROOT_CONTENTS =
  _XT("<test>\n"
      "   <element>element contents</element>\n"
      "   <element attribute=\"value\"/>\n"
      "   <element attribute1=\"value1\" attribute2=\"value2\"/>\n"
      "   <element>\n"
      "      <subelement attribute=\"value\"/>\n"
      "      <subelement attribute1=\"value1\" attribute2=\"value2\">\n"
      "         <subsubelement>With accents: à é è í ó ú</subsubelement>\n"
      "      </subelement>\n"
      "   </element>\n"
      "</test>\n");

static XML_Char const *TEST_CHILDREN_CONTENTS =
  _XT("   <subelement attribute=\"value\"/>\n"
      "   <subelement attribute1=\"value1\" attribute2=\"value2\">\n"
      "      <subsubelement>With accents: à é è í ó ú</subsubelement>\n"
      "   </subelement>\n");

static XML_Char const *TEST_ATTRIBUTE_CONTENTS =
  _XT(" attribute1=\"value1\" attribute2=\"value2\"");

static scew_writer* test_writer_create_ (XML_Char **buffer);

static scew_tree* test_tree_create_ (void);

/* Allocation */

START_TEST (test_alloc)
{
  enum { MAX_BUFFER = 512 };

  XML_Char BUFFER[MAX_BUFFER] = _XT("");

  scew_writer *writer = scew_writer_buffer_create (BUFFER, MAX_BUFFER);

  CHECK_PTR (writer, "Unable to create buffer writer");

  scew_printer *printer = scew_printer_create (writer);

  scew_writer_free (writer);
  scew_printer_free (printer);
}
END_TEST

/* Print tree */

START_TEST (test_print_tree)
{
  XML_Char *write_buffer = NULL;

  scew_writer *writer = test_writer_create_ (&write_buffer);

  scew_printer *printer = scew_printer_create (writer);

  /* Create XML tree */
  scew_tree *tree = test_tree_create_ ();

  /* Print tree */
  CHECK_BOOL (scew_printer_print_tree (printer, tree), SCEW_TRUE,
              "Unable to print XML tree");

  CHECK_STR (write_buffer, TEST_TREE_CONTENTS,
             "Printed element does not match");

  scew_writer_free (writer);
  scew_printer_free (printer);
}
END_TEST

/* Print element */

START_TEST (test_print_element)
{
  XML_Char *write_buffer = NULL;

  scew_writer *writer = test_writer_create_ (&write_buffer);

  scew_printer *printer = scew_printer_create (writer);

  /* Create XML tree */
  scew_tree *tree = test_tree_create_ ();

  /* Print root element */
  scew_element *root = scew_tree_root (tree);
  CHECK_BOOL (scew_printer_print_element (printer, root), SCEW_TRUE,
              "Unable to print root element");

  CHECK_STR (write_buffer, TEST_ROOT_CONTENTS,
             "Printed element does not match");

  scew_writer_free (writer);

  /* Create a new writer */
  writer = test_writer_create_ (&write_buffer);

  scew_printer_set_writer (printer, writer);

  /* Print element children (subelement) */
  scew_element *element = scew_element_by_index (root, 3);
  CHECK_BOOL (scew_printer_print_element_children (printer, element),
              SCEW_TRUE, "Unable to print children (subelement)");

  CHECK_STR (write_buffer, TEST_CHILDREN_CONTENTS,
             "Printed element children do not match");

  scew_writer_free (writer);
  scew_printer_free (printer);
}
END_TEST

/* Print attributes */

START_TEST (test_print_attribute)
{
  XML_Char *write_buffer = NULL;

  scew_writer *writer = test_writer_create_ (&write_buffer);

  scew_printer *printer = scew_printer_create (writer);

  /* Create XML tree */
  scew_tree *tree = test_tree_create_ ();

  /* Print root element */
  scew_element *root = scew_tree_root (tree);
  scew_element *element = scew_element_by_index (root, 2);
  CHECK_BOOL (scew_printer_print_element_attributes (printer, element),
              SCEW_TRUE, "Unable to print element attributes");

  CHECK_STR (write_buffer, TEST_ATTRIBUTE_CONTENTS,
             "Printed element attributes do not match");

  scew_writer_free (writer);
  scew_printer_free (printer);
}
END_TEST


/* Suite */

static Suite*
printer_suite (void)
{
  Suite *s = suite_create ("SCEW printer");

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_alloc);
  tcase_add_test (tc_core, test_print_tree);
  tcase_add_test (tc_core, test_print_element);
  tcase_add_test (tc_core, test_print_attribute);
  suite_add_tcase (s, tc_core);

  return s;
}

void
run_tests (SRunner *sr)
{
  srunner_add_suite (sr, printer_suite ());
}


/* Private */

scew_writer*
test_writer_create_ (XML_Char **buffer)
{
  enum { MAX_BUFFER = 512 };

  static XML_Char write_buffer[MAX_BUFFER] = _XT("");

  scew_writer *writer = scew_writer_buffer_create (write_buffer, MAX_BUFFER);

  CHECK_PTR (writer, "Unable to create buffer writer");

  /* Setup pointer to buffer (for comparisons) */
  *buffer = write_buffer;

  return writer;
}

scew_tree*
test_tree_create_ (void)
{
  /**
   * Create an empty XML tree in memory, and add a root element
   * "scew_test".
   */
  scew_tree *tree = scew_tree_create ();
  scew_element *root = scew_tree_set_root (tree, _XT("test"));

  /* Add an element and set element contents. */
  scew_element *element = scew_element_add (root, _XT("element"));
  scew_element_set_contents (element, _XT("element contents"));

  /**
   * Add an element with an attribute pair (name, value) and a
   * zero-length string.
   */
  element = scew_element_add (root, _XT("element"));
  scew_element_add_attribute_pair (element, _XT("attribute"), _XT("value"));
  scew_element_set_contents (element, _XT(""));

  element = scew_element_add (root, _XT("element"));
  scew_element_add_attribute_pair (element,
                                   _XT("attribute1"), _XT("value1"));

  /**
   * Another way to add an attribute. You loose attribute ownership,
   * so there is no need to free it.
   */
  scew_attribute *attribute =
    scew_attribute_create (_XT("attribute2"), _XT("value2"));
  scew_element_add_attribute (element, attribute);

  element = scew_element_add (root, _XT("element"));
  scew_element *sub_element = scew_element_add (element, _XT("subelement"));
  scew_element_add_attribute_pair (sub_element,
                                   _XT("attribute"), _XT("value"));

  sub_element = scew_element_add (element, _XT("subelement"));
  scew_element_add_attribute_pair (sub_element,
                                   _XT("attribute1"), _XT("value1"));
  scew_element_add_attribute_pair (sub_element,
                                   _XT("attribute2"), _XT("value2"));

  scew_element *sub_sub_element = scew_element_add (sub_element,
                                                    _XT("subsubelement"));
  scew_element_set_contents (sub_sub_element,
                             _XT("With accents: à é è í ó ú"));

  return tree;
}
