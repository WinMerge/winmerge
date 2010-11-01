/**
 * @file     element_search.c
 * @brief    element.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Aug 27, 2009 01:36
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

#include "str.h"

#include <assert.h>



/* Private */

static scew_bool cmp_name_ (void const *element, void const *name);



/* Public */

scew_element*
scew_element_by_name (scew_element const *element, XML_Char const *name)
{
  scew_list *item = NULL;

  assert (element != NULL);
  assert (name != NULL);

  if (element->children != NULL)
    {
      item = scew_list_find_custom (element->children, name, cmp_name_);
    }

  return (NULL == item) ? NULL : (scew_element *) scew_list_data (item);
}

scew_element*
scew_element_by_index (scew_element const *element, unsigned int index)
{
  scew_list *item = NULL;

  assert (element != NULL);
  assert (index < element->n_children);

  item = scew_list_index (element->children, index);

  return (NULL == item) ? NULL : (scew_element *) scew_list_data (item);
}

scew_list*
scew_element_list_by_name (scew_element const *element, XML_Char const *name)
{
  scew_list *list = NULL;
  scew_list *last = NULL;
  scew_list *item = NULL;

  assert (element != NULL);
  assert (name != NULL);

  item = element->children;
  while (item != NULL)
    {
      item = scew_list_find_custom (item, name, cmp_name_);
      if (item != NULL)
        {
          last = scew_list_append (last, scew_list_data (item));
          if (NULL == list)
            {
              list = last;
            }
          item = scew_list_next (item);
        }
    }

  return list;
}


/* Private */

scew_bool
cmp_name_ (void const *element, void const *name)
{
  return (scew_strcmp (((scew_element *) element)->name,
                       (XML_Char *) name) == 0);
}
