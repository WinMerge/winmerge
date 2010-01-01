/**
 * @file     check_element.c
 * @brief    Unit testing for SCEW elements
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Aug 02, 2007 17:17
 *
 * @if copyright
 *
 * Copyright (C) 2007-2009 Aleix Conchillo Flaque
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

#include <scew/element.h>
#include <scew/attribute.h>

#include <check.h>


/* Unit tests */

/* Allocation */

START_TEST (test_alloc)
{
  static XML_Char const *NAME = _XT("root");
  static XML_Char const *CHILD_NAME = _XT("element");
  static XML_Char const *CONTENTS = _XT("first child");
  static unsigned int const N_ELEMENTS = 12;
  unsigned int i = 0;

  scew_element *root = scew_element_create (NAME);

  CHECK_PTR (root, "Unable to create element");

  /* Create elements */
  for (i = 0; i < N_ELEMENTS; ++i)
    {
      scew_element *child = scew_element_add (root, CHILD_NAME);

      CHECK_PTR (child, "Unable to create child");

      if (i == 0)
        {
          scew_element_set_contents (child, CONTENTS);
        }
    }

  /* Copy */
  scew_element *root_copy = scew_element_copy (root);

  CHECK_PTR (root_copy, "Unable to copy root element");

  /* Check elements have different addresses */
  CHECK_BOOL ((root != root_copy), SCEW_TRUE,
              "Root element address should be different");

  scew_list *list = scew_element_children (root);
  scew_list *list_copy = scew_element_children (root_copy);
  while ((list != NULL) && (list_copy != NULL))
    {
      CHECK_BOOL ((list != list_copy), SCEW_TRUE,
                  "Element address should be different");
      list = scew_list_next (list);
      list_copy = scew_list_next (list_copy);
    }

  CHECK_NULL_PTR (list, "There should be no more elements in root");
  CHECK_NULL_PTR (list_copy, "There should be no more elements in root copy");

  scew_element_free (root);
  scew_element_free (root_copy);
}
END_TEST


/* Accessors */

START_TEST (test_accessors)
{
  static XML_Char const *NAME = _XT("root");
  static XML_Char const *NEW_NAME = _XT("new_root");
  static XML_Char const *CONTENTS = _XT("root element contents");

  scew_element *element = scew_element_create (NAME);

  CHECK_PTR (element, "Unable to create element");

  /* Name */
  CHECK_STR (scew_element_name (element), NAME,
             "Element name do not match");

  scew_element_set_name (element, NEW_NAME);

  CHECK_STR (scew_element_name (element), NEW_NAME,
             "Element name do not match (NEW_NAME)");

  /* Contents */
  CHECK_NULL_PTR (scew_element_contents (element), "Element has no contents");

  scew_element_set_contents (element, CONTENTS);

  CHECK_STR (scew_element_contents (element), CONTENTS,
             "Element contents do not match");

  scew_element_free_contents (element);

  CHECK_NULL_PTR (scew_element_contents (element), "Element has no contents");

  scew_element_free (element);
}
END_TEST


/* Attributes */

START_TEST (test_attributes)
{
  static XML_Char const *NAME = _XT("root");
  static XML_Char const *ATTRIBUTE = _XT("attribute");
  static XML_Char const *VALUE = _XT("value");
  static unsigned int const N_ATTRIBUTES = 5;
  unsigned int i = 0;

  scew_element *element = scew_element_create (NAME);

  CHECK_PTR (element, "Unable to create element");

  CHECK_NULL_PTR (scew_element_attributes (element),
                  "Element has no attributes");

  CHECK_U_INT (scew_element_attribute_count (element), 0,
               "Element has no attributes");

  enum { MAX_BUFFER = 100 };
  XML_Char attr_name[MAX_BUFFER];
  XML_Char attr_value[MAX_BUFFER];

  /* Create attributes */
  for (i = 0; i < N_ATTRIBUTES; ++i)
    {
      check_sprintf (attr_name, _XT("%s_%d"), ATTRIBUTE, i);
      check_sprintf (attr_value, _XT("%s_%d"), VALUE, i);

      scew_attribute *attr =
        scew_element_add_attribute_pair (element, attr_name, attr_value);

      CHECK_PTR (attr, "Unable to create attribute");
    }

  CHECK_U_INT (scew_element_attribute_count (element), N_ATTRIBUTES,
               "Number of attributes mismatch");

  /* Check attributes */
  i = 0;
  scew_list *list = scew_element_attributes (element);
  while (list != NULL)
    {
      scew_attribute *attr = scew_list_data (list);

      check_sprintf (attr_name, _XT("%s_%d"), ATTRIBUTE, i);
      check_sprintf (attr_value, _XT("%s_%d"), VALUE, i);

      CHECK_STR (scew_attribute_name (attr), attr_name,
                 "Attribute name do not match");
      CHECK_STR (scew_attribute_value (attr), attr_value,
                 "Attribute value do not match");

      list = scew_list_next (list);

      i += 1;
    }

  /* Add attributes with same name */
  XML_Char attr_1_name[MAX_BUFFER];
  XML_Char attr_2_name[MAX_BUFFER];
  XML_Char attr_1_value[MAX_BUFFER];
  XML_Char attr_2_value[MAX_BUFFER];

  check_sprintf (attr_1_name, _XT("%s_1"), ATTRIBUTE);
  check_sprintf (attr_1_value, _XT("%s_1_new"), VALUE);
  scew_attribute *attr_1 =
    scew_element_add_attribute_pair (element, attr_1_name, attr_1_value);

  check_sprintf (attr_2_name, _XT("%s_2"), ATTRIBUTE);
  check_sprintf (attr_2_value, _XT("%s_2_new"), VALUE);
  scew_attribute *attr_2 =
    scew_element_add_attribute_pair (element, attr_2_name, attr_2_value);

  CHECK_U_INT (scew_element_attribute_count (element), N_ATTRIBUTES,
               "Number of attributes mismatch after repetition");

  /* Check attributes new values */
  CHECK_STR (scew_attribute_value (attr_1), attr_1_value,
             "Attribute 1 new value do not match");
  CHECK_STR (scew_attribute_value (attr_2), attr_2_value,
             "Attribute 2 new value do not match");

  /* Delete attribute */
  scew_element_delete_attribute (element, attr_1);
  CHECK_U_INT (scew_element_attribute_count (element), N_ATTRIBUTES - 1,
               "Number of attributes mismatch after deleting");

  /* Delete attribute by name */
  scew_element_delete_attribute_by_name (element, attr_2_name);
  CHECK_U_INT (scew_element_attribute_count (element), N_ATTRIBUTES - 2,
               "Number of attributes mismatch after deleting by name");

  /* Delete all attributes */
  scew_element_delete_attribute_all (element);
  CHECK_U_INT (scew_element_attribute_count (element), 0,
               "Number of attributes mismatch after deleting all");

  scew_element_free (element);
}
END_TEST


/* Hierarchy (basic) */

START_TEST (test_hierarchy_basic)
{
  static XML_Char const *NAME = _XT("element");
  static XML_Char const *SUB_NAME = _XT("subelement");
  static XML_Char const *CONTENTS = _XT("contents");
  static unsigned int const N_ELEMENTS = 12;
  unsigned int i = 0;

  scew_element *element = scew_element_create (_XT("root"));

  CHECK_PTR (element, "Unable to create element");

  CHECK_NULL_PTR (scew_element_parent (element), "Element has no parent");

  CHECK_NULL_PTR (scew_element_children (element), "Element has no children");

  CHECK_U_INT (scew_element_count (element), 0, "Element has no children");

  for (i = 0; i < N_ELEMENTS; ++i)
    {
      scew_element *child = scew_element_add (element, NAME);

      CHECK_PTR (child, "Unable to create child");

      CHECK_BOOL (element == scew_element_parent (child), SCEW_TRUE,
                  "Element has wrong parent");

      scew_element *sub_child =
        scew_element_add_pair (child, SUB_NAME, CONTENTS);

      CHECK_PTR (sub_child, "Unable to create sub-child");

      CHECK_BOOL (child == scew_element_parent (sub_child), SCEW_TRUE,
                  "Sub-child has wrong parent");

      CHECK_STR (scew_element_name (sub_child), SUB_NAME,
                 "Sub-child name do not match");

      CHECK_STR (scew_element_contents (sub_child), CONTENTS,
                 "Sub-child contents do not match");
    }

  CHECK_U_INT (scew_element_count (element), N_ELEMENTS,
               "Number of children mismatch");

  scew_element_free (element);
}
END_TEST


/* Hierarchy (delete) */

START_TEST (test_hierarchy_delete)
{
  static XML_Char const *NAME = _XT("element");
  static unsigned int const N_ELEMENTS = 12;

  scew_element *element = scew_element_create (_XT("root"));

  CHECK_PTR (element, "Unable to create element");

  /* Add */
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS; ++i)
    {
      scew_element *child = scew_element_create (NAME);

      CHECK_PTR (child, "Unable to create child");

      CHECK_BOOL (child == scew_element_add_element (element, child),
                  SCEW_TRUE, "Unable to append child");
    }

  CHECK_U_INT (scew_element_count (element), N_ELEMENTS,
               "Number of children mismatch");

  /* Remove all children */
  scew_element_delete_all (element);

  CHECK_U_INT (scew_element_count (element), 0, "Number of children mismatch");

  /* Re-add and detach */
  for (i = 0; i < N_ELEMENTS; ++i)
    {
      scew_element *child = scew_element_add (element, NAME);

      CHECK_PTR (child, "Unable to create child");

      scew_element_detach (child);

      CHECK_NULL_PTR (scew_element_parent (child),
                      "Child should have no parent");
    }

  CHECK_U_INT (scew_element_count (element), 0, "Number of children mismatch");

  /* Re-add */
  for (i = 0; i < N_ELEMENTS; ++i)
    {
      scew_element *child = scew_element_add (element, NAME);

      CHECK_PTR (child, "Unable to create child");
    }

  CHECK_U_INT (scew_element_count (element), N_ELEMENTS,
               "Number of children mismatch");

  /* Delete first child */
  unsigned int total = N_ELEMENTS;

  scew_element_delete_by_name (element, NAME);
  --total;

  CHECK_U_INT (scew_element_count (element), total,
               "Number of children mismatch");

  /* Delete five next child */
  for (i = 0; i < 5; ++i)
    {
      scew_element_delete_by_index (element, i);
    }
  total -= 5;

  CHECK_U_INT (scew_element_count (element), total,
               "Number of children mismatch");

  scew_element_free (element);
}
END_TEST


/* Search */

START_TEST (test_search)
{
  static XML_Char const *NAME = _XT("element");
  static XML_Char const *NAME_AUX = _XT("element_aux");
  static XML_Char const *CONTENTS = _XT("first child");
  static unsigned int const N_ELEMENTS = 12;

  scew_element *root = scew_element_create (_XT("root"));

  CHECK_PTR (root, "Unable to create element");

  scew_element *child = scew_element_add (root, NAME);

  CHECK_PTR (child, "Unable to create child");

  scew_element_set_contents (child, CONTENTS);

  unsigned int i = 0;
  for (i = 1; i < N_ELEMENTS; ++i)
    {
      child = scew_element_add (root, ((i % 2) == 0) ? NAME : NAME_AUX);

      CHECK_PTR (child, "Unable to create child");
    }

  child = scew_element_by_name (root, NAME);

  CHECK_PTR (child, "Unable to find first child");

  /* First child should have contents */
  if (scew_strcmp (CONTENTS, scew_element_contents (child)) != 0)
    {
      fail ("First child does not have any contents");
    }

  /* Obtain the list of all elements */
  scew_list *list = scew_element_list_by_name (root, NAME);

  CHECK_PTR (list, "Unabe to find elements list by name");

  /* / 2 because we only insert NAME elements % 2 */
  CHECK_U_INT (scew_list_size (list), N_ELEMENTS / 2,
             "Number of children found searching by name");

  /* Make sure we have the right elements. */
  for (i = 0; i < N_ELEMENTS; ++i)
    {
      child = scew_element_by_index (root, i);

      CHECK_STR (scew_element_name (child),
                 ((i % 2) == 0) ? NAME : NAME_AUX,
                 "Searched elements do not match");
    }

  scew_list_free (list);

  /* Redo search and make sure nothing has been freed */
  list = scew_element_list_by_name (root, NAME);

  CHECK_PTR (list, "Unabe to find elements list by name");

  CHECK_U_INT (scew_list_size (list), N_ELEMENTS / 2,
             "Number of children found searching by name");

  scew_element_free (root);
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

  /* Copy */
  scew_element *root_copy = scew_element_copy (root);

  CHECK_PTR (root_copy, "Unable to copy root element");

  CHECK_BOOL (scew_element_compare (root, root_copy, NULL), SCEW_TRUE,
              "Root and root copy should be equal");

  /* Modify and compare again */
  scew_element *element = scew_element_by_index (root_copy, N_ELEMENTS - 1);
  scew_element_set_contents (element, CONTENTS);

  CHECK_BOOL (scew_element_compare (root, root_copy, NULL), SCEW_FALSE,
              "Root and root copy should be different (last child)");

  scew_element_free (root);
  scew_element_free (root_copy);
}
END_TEST


/* Suite */

static Suite*
element_suite (void)
{
  Suite *s = suite_create ("SCEW elements");

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_alloc);
  tcase_add_test (tc_core, test_accessors);
  tcase_add_test (tc_core, test_attributes);
  tcase_add_test (tc_core, test_hierarchy_basic);
  tcase_add_test (tc_core, test_hierarchy_delete);
  tcase_add_test (tc_core, test_search);
  tcase_add_test (tc_core, test_compare);
  suite_add_tcase (s, tc_core);

  return s;
}

void
run_tests (SRunner *sr)
{
  srunner_add_suite (sr, element_suite ());
}
