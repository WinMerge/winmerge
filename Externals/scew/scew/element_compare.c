/**
 * @file     element_compare.c
 * @brief    element.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Aug 27, 2009 01:38
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

#include "xelement.h"

#include "attribute.h"
#include "str.h"

#include <assert.h>



/* Private */

static scew_bool compare_element_ (scew_element const *a,
                                   scew_element const *b);
static scew_bool compare_children_ (scew_element const *a,
                                    scew_element const *b,
                                    scew_element_cmp_hook hook);
static scew_bool compare_attributes_ (scew_element const *a,
                                      scew_element const *b);



/* Public */

scew_bool
scew_element_compare (scew_element const *a,
                      scew_element const *b,
                      scew_element_cmp_hook hook)
{
  scew_element_cmp_hook cmp_hook = NULL;

  assert (a != NULL);
  assert (b != NULL);

  cmp_hook = (NULL == hook) ? compare_element_ : hook;

  return (cmp_hook (a, b) && compare_children_ (a, b, cmp_hook));
}


/* Private */

scew_bool
compare_element_ (scew_element const *a, scew_element const *b)
{
  scew_bool equal = SCEW_FALSE;

  assert (a != NULL);
  assert (b != NULL);

  equal = (scew_strcmp (a->name, b->name) == 0)
    && (scew_strcmp (a->contents, b->contents) == 0)
    && compare_attributes_ (a, b);

  return equal;
}

scew_bool
compare_attributes_ (scew_element const *a, scew_element const *b)
{
  scew_bool equal = SCEW_TRUE;
  scew_list *list_a = NULL;
  scew_list *list_b = NULL;

  assert (a != NULL);
  assert (b != NULL);

  equal = (a->n_attributes == b->n_attributes);

  list_a = a->attributes;
  list_b = b->attributes;
  while (equal && (list_a != NULL) && (list_b != NULL))
    {
      scew_attribute *attr_a = scew_list_data (list_a);
      scew_attribute *attr_b = scew_list_data (list_b);
      equal = scew_attribute_compare (attr_a, attr_b);
      list_a = scew_list_next (list_a);
      list_b = scew_list_next (list_b);
    }

  return equal;
}

scew_bool
compare_children_ (scew_element const *a,
                   scew_element const *b,
                   scew_element_cmp_hook hook)
{
  scew_bool equal = SCEW_TRUE;
  scew_list *list_a = NULL;
  scew_list *list_b = NULL;

  assert (a != NULL);
  assert (b != NULL);

  equal = (a->n_children == b->n_children);

  list_a = a->children;
  list_b = b->children;
  while (equal && (list_a != NULL) && (list_b != NULL))
    {
      scew_element *child_a = scew_list_data (list_a);
      scew_element *child_b = scew_list_data (list_b);
      equal = scew_element_compare (child_a, child_b, hook);
      list_a = scew_list_next (list_a);
      list_b = scew_list_next (list_b);
    }

  return equal;
}
