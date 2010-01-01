/**
 * @file     check_attribute.c
 * @brief    Unit testing for SCEW attributes
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Aug 24, 2009 23:17
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

#include <scew/attribute.h>

#include <check.h>


/* Unit tests */

/* Allocation */

START_TEST (test_alloc)
{
  static XML_Char const *NAME = _XT("attribute1");
  static XML_Char const *VALUE = _XT("value1");

  scew_attribute *attribute = scew_attribute_create (NAME, VALUE);

  CHECK_PTR (attribute, "Unable to create attribute");

  scew_attribute_free (attribute);
}
END_TEST


/* Accesors */

START_TEST (test_accessors)
{
  static XML_Char const *NAME = _XT("attribute1");
  static XML_Char const *NAME_AUX = _XT("attribute2");
  static XML_Char const *VALUE = _XT("value1");
  static XML_Char const *VALUE_AUX = _XT("value2");

  scew_attribute *attribute = scew_attribute_create (NAME, VALUE);

  CHECK_PTR (attribute, "Unable to create attribute");

  /* Name */
  CHECK_STR (scew_attribute_name (attribute), NAME,
             "Attribute name do not match");

  /* Value */
  CHECK_STR (scew_attribute_value (attribute), VALUE,
             "Attribute value do not match");

  /* Name */
  CHECK_STR (scew_attribute_set_name (attribute, NAME_AUX), NAME_AUX,
             "New attribute name do not match");
  CHECK_STR (scew_attribute_name (attribute), NAME_AUX,
             "Attribute name do not match");

  /* Value */
  CHECK_STR (scew_attribute_set_value (attribute, VALUE_AUX), VALUE_AUX,
             "New attribute value do not match");
  CHECK_STR (scew_attribute_value (attribute), VALUE_AUX,
             "Attribute value do not match");

  scew_attribute_free (attribute);
}
END_TEST


/* Hierarchy */

START_TEST (test_hierarchy)
{
  static XML_Char const *ELEMENT_NAME = _XT("element");
  static XML_Char const *NAME_1 = _XT("attribute1");
  static XML_Char const *VALUE_1 = _XT("value1");
  static XML_Char const *NAME_2 = _XT("attribute2");
  static XML_Char const *VALUE_2 = _XT("value2");
  static XML_Char const *NAME_3 = _XT("attribute3");
  static XML_Char const *VALUE_3 = _XT("value3");
  static unsigned int const N_ATTRIBUTES = 3;

  scew_element *element = scew_element_create (ELEMENT_NAME);
  scew_attribute *attribute_1 = scew_attribute_create (NAME_1, VALUE_1);
  scew_attribute *attribute_2 = scew_attribute_create (NAME_2, VALUE_2);
  scew_attribute *attribute_3 = scew_attribute_create (NAME_3, VALUE_3);

  CHECK_U_INT (scew_element_attribute_count (element), 0,
               "Element has no attributes");

  CHECK_PTR (attribute_1, "Unable to create attribute 1");
  CHECK_PTR (attribute_2, "Unable to create attribute 2");
  CHECK_PTR (attribute_3, "Unable to create attribute 3");

  /* Add attributes to element */
  scew_element_add_attribute (element, attribute_1);
  scew_element_add_attribute (element, attribute_2);
  scew_element_add_attribute (element, attribute_3);

  CHECK_U_INT (scew_element_attribute_count (element), N_ATTRIBUTES,
               "Number of attributes mismatch");

  /* Check correct element */
  CHECK_BOOL (element == scew_attribute_parent (attribute_1), SCEW_TRUE,
              "Attribute 1 has wrong parent");
  CHECK_BOOL (element == scew_attribute_parent (attribute_2), SCEW_TRUE,
              "Attribute 2 has wrong parent");
  CHECK_BOOL (element == scew_attribute_parent (attribute_3), SCEW_TRUE,
              "Attribute 3 has wrong parent");

  /* Detaching */
  scew_element_delete_attribute (element, attribute_2);
  CHECK_BOOL (element == scew_attribute_parent (attribute_1), SCEW_TRUE,
              "Attribute 1 has wrong parent");
  CHECK_BOOL (element == scew_attribute_parent (attribute_3), SCEW_TRUE,
              "Attribute 3 has wrong parent");

  CHECK_U_INT (scew_element_attribute_count (element), N_ATTRIBUTES - 1,
               "Number of attributes mismatch");

  scew_element_free (element);
}
END_TEST


/* Comparison */

START_TEST (test_compare)
{
  static XML_Char const *NAME_1 = _XT("attribute_1");
  static XML_Char const *NAME_1_1 = _XT("attribute_1");
  static XML_Char const *NAME_2 = _XT("attribute_2");
  static XML_Char const *VALUE_1 = _XT("value_1");
  static XML_Char const *VALUE_2 = _XT("value_2");

  scew_attribute *attribute = scew_attribute_create (NAME_1, VALUE_1);

  CHECK_PTR (attribute, "Unable to create attribute");

  /* Copy */
  scew_attribute *attr_copy = scew_attribute_copy (attribute);

  CHECK_PTR (attr_copy, "Unable to copy attribute");

  CHECK_BOOL (scew_attribute_compare (attribute, attr_copy), SCEW_TRUE,
              "Attribute and attribute copy should be equal");

  /* Update and compare (OK) */
  CHECK_STR (scew_attribute_set_name (attribute, NAME_1_1), NAME_1_1,
             "New attribute name do not match");
  CHECK_BOOL (scew_attribute_compare (attribute, attr_copy), SCEW_TRUE,
              "Attribute and attribute copy should still be equal");

  /* Update (different name) and compare (FAIL) */
  CHECK_STR (scew_attribute_set_name (attribute, NAME_2), NAME_2,
             "New attribute name do not match");
  CHECK_BOOL (scew_attribute_compare (attribute, attr_copy), SCEW_FALSE,
              "Attribute and attribute copy should be different");

  /* Update (fix) and compare (OK) */
  CHECK_STR (scew_attribute_set_name (attribute, NAME_1_1), NAME_1_1,
             "New attribute name do not match");
  CHECK_BOOL (scew_attribute_compare (attribute, attr_copy), SCEW_TRUE,
              "Attribute and attribute copy should be different");

  /* Update (different value) and compare (FAIL) */
  CHECK_STR (scew_attribute_set_value (attribute, VALUE_2), VALUE_2,
             "New attribute value do not match");
  CHECK_BOOL (scew_attribute_compare (attribute, attr_copy), SCEW_FALSE,
              "Attribute and attribute copy should be different");

  scew_attribute_free (attribute);
  scew_attribute_free (attr_copy);
}
END_TEST


/* Suite */

static Suite*
attribute_suite (void)
{
  Suite *s = suite_create ("SCEW attributes");

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_alloc);
  tcase_add_test (tc_core, test_accessors);
  tcase_add_test (tc_core, test_hierarchy);
  tcase_add_test (tc_core, test_compare);
  suite_add_tcase (s, tc_core);

  return s;
}

void
run_tests (SRunner *sr)
{
  srunner_add_suite (sr, attribute_suite ());
}
