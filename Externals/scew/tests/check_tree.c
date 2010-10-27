/**
 * @file     check_tree.c
 * @brief    Unit testing for SCEW trees
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Aug 27, 2009 23:56
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

#include <scew/tree.h>

#include <check.h>


/* Unit tests */

/* Allocation */

START_TEST (test_alloc)
{
  scew_tree *tree = scew_tree_create ();

  CHECK_PTR (tree, "Unable to create tree");

  scew_tree_free (tree);
}
END_TEST

/* Tree properties (version, encoding...) */

START_TEST (test_properties)
{
  static XML_Char const *DEF_VERSION = _XT("1.0");
  static XML_Char const *DEF_ENCODING = _XT("UTF-8");
  static scew_tree_standalone DEF_STANDALONE = scew_tree_standalone_unknown;

  static XML_Char const *NEW_VERSION = _XT("1.1");
  static XML_Char const *NEW_ENCODING = _XT("UTF-16");
  static scew_tree_standalone NEW_STANDALONE = scew_tree_standalone_yes;

  scew_tree *tree = scew_tree_create ();

  CHECK_PTR (tree, "Unable to create tree");

  /* Check defaults */
  CHECK_STR (scew_tree_xml_version (tree), DEF_VERSION,
             "Wrong default XML version");
  CHECK_STR (scew_tree_xml_encoding (tree), DEF_ENCODING,
             "Wrong default XML encoding");
  CHECK_U_INT (scew_tree_xml_standalone (tree), DEF_STANDALONE,
               "Wrong default XML standalone attribute");

  /* Set new properties and check again */
  scew_tree_set_xml_version (tree, NEW_VERSION);
  scew_tree_set_xml_encoding (tree, NEW_ENCODING);
  scew_tree_set_xml_standalone (tree, NEW_STANDALONE);
  CHECK_STR (scew_tree_xml_version (tree), NEW_VERSION,
             "Wrong new XML version");
  CHECK_STR (scew_tree_xml_encoding (tree), NEW_ENCODING,
             "Wrong new XML encoding");
  CHECK_U_INT (scew_tree_xml_standalone (tree), NEW_STANDALONE,
               "Wrong new XML standalone attribute");

  scew_tree_free (tree);
}
END_TEST

/* Tree contents */

START_TEST (test_contents)
{
  static XML_Char const *NAME = _XT("root");
  static XML_Char const *PREAMBLE = _XT("Completly wrong dummy preamble");

  scew_tree *tree = scew_tree_create ();

  CHECK_PTR (tree, "Unable to create tree");

  CHECK_NULL_PTR (scew_tree_root (tree), "No root tree added yet");

  /* Add root element by name */
  scew_element *root = scew_tree_set_root (tree, NAME);
  CHECK_BOOL ((scew_tree_root (tree) == root), SCEW_TRUE,
              "Returned root element does not match tree root");
  scew_element_free (root);

  /* Create element and add root */
  root = scew_element_create (NAME);
  scew_tree_set_root_element (tree, root);
  CHECK_BOOL ((scew_tree_root (tree) == root), SCEW_TRUE,
              "Returned root element does not match tree root");

  /* Set XML preamble */
  CHECK_NULL_PTR (scew_tree_xml_preamble (tree),
                  "Default preamble should be NULL");
  scew_tree_set_xml_preamble (tree, PREAMBLE);
  CHECK_STR (scew_tree_xml_preamble (tree), PREAMBLE,
             "Set XML preamble do not match");

  scew_tree_free (tree);
}
END_TEST

/* Comparison */

START_TEST (test_compare)
{
  static XML_Char const *NAME = _XT("root");
  static XML_Char const *CHILD_NAME = _XT("element");
  static XML_Char const *CONTENTS = _XT("first child");
  static unsigned int const N_ELEMENTS = 12;

  scew_element *root = scew_element_create (NAME);

  CHECK_PTR (root, "Unable to create element");

  /* Create elements */
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS; ++i)
    {
      scew_element *child = scew_element_add (root, CHILD_NAME);

      CHECK_PTR (child, "Unable to create child");

      if (i == 0)
        {
          scew_element_set_contents (child, CONTENTS);
        }
    }

  /* Create tree */
  scew_tree *tree = scew_tree_create ();
  scew_tree_set_root_element (tree, root);

  /* Copy */
  scew_tree *tree_copy = scew_tree_copy (tree);

  CHECK_PTR (tree_copy, "Unable to copy tree");

  CHECK_BOOL (scew_tree_compare (tree, tree_copy, NULL), SCEW_TRUE,
              "Tree and tree copy should be equal");

  /* Modify and compare again */
  scew_tree_set_xml_standalone (tree_copy, scew_tree_standalone_no);

  CHECK_BOOL (scew_tree_compare (tree, tree_copy, NULL), SCEW_FALSE,
              "Tree and tree copy should be different (standalone)");

  scew_tree_free (tree);
  scew_tree_free (tree_copy);
}
END_TEST



/* Suite */

static Suite*
tree_suite (void)
{
  Suite *s = suite_create ("SCEW trees");

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_alloc);
  tcase_add_test (tc_core, test_properties);
  tcase_add_test (tc_core, test_contents);
  tcase_add_test (tc_core, test_compare);
  suite_add_tcase (s, tc_core);

  return s;
}

void
run_tests (SRunner *sr)
{
  srunner_add_suite (sr, tree_suite ());
}
