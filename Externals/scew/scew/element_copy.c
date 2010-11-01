/**
 * @file     element_search.c
 * @brief    element.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Aug 27, 2009 01:53
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

#include <assert.h>



/* Private */

static scew_bool copy_children_ (scew_element *new_element,
                                 scew_element const *element);
static scew_bool copy_attributes_ (scew_element *new_element,
                                   scew_element const *element);



/* Public */

scew_element*
scew_element_copy (scew_element const *element)
{
  scew_element *new_elem = NULL;

  assert (element != NULL);

  new_elem = calloc (1, sizeof (scew_element));

  if (new_elem != NULL)
    {
      scew_bool copied =
        ((NULL == element->contents)
         || (scew_element_set_contents (new_elem, element->contents) != NULL));

      copied = copied
        && (scew_element_set_name (new_elem, element->name) != NULL)
        && copy_children_ (new_elem, element)
        && copy_attributes_ (new_elem, element);

      if (!copied)
        {
          scew_element_free (new_elem);
          new_elem = NULL;
        }
    }

  return new_elem;
}



/* Private */

scew_bool
copy_children_ (scew_element *new_element, scew_element const *element)
{
  scew_bool copied = SCEW_TRUE;
  scew_list *list = NULL;

  assert (new_element != NULL);
  assert (element != NULL);

  list = element->children;
  while (copied && (list != NULL))
    {
      scew_element *child = scew_list_data (list);
      scew_element *new_child = scew_element_copy (child);
      copied =
        ((new_child != NULL)
         && (scew_element_add_element (new_element, new_child) != NULL));
      list = scew_list_next (list);
    }

  return copied;
}

scew_bool
copy_attributes_ (scew_element *new_element, scew_element const *element)
{
  scew_bool copied = SCEW_TRUE;
  scew_list *list = NULL;

  assert (new_element != NULL);
  assert (element != NULL);

  list = element->attributes;
  while (copied && (list != NULL))
    {
      scew_attribute *attr = scew_list_data (list);
      scew_attribute *new_attr = scew_attribute_copy (attr);
      copied =
        ((new_attr != NULL)
         && (scew_element_add_attribute (new_element, new_attr) != NULL));
      list = scew_list_next (list);
    }

  return copied;
}
