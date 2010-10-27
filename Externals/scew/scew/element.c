/**
 * @file     element.c
 * @brief    element.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:48
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
 */

#include "xelement.h"

#include "str.h"

#include "xerror.h"

#include <assert.h>



/* Public */


/* Allocation */

scew_element*
scew_element_create (XML_Char const *name)
{
  scew_element *element = NULL;

  assert (name != NULL);

  element = calloc (1, sizeof (scew_element));

  if (element != NULL)
    {
      scew_element_set_name (element, name);
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return element;
}

void
scew_element_free (scew_element *element)
{
  if (element != NULL)
    {
      scew_element_delete_all (element);
      scew_element_delete_attribute_all (element);
      scew_element_detach (element);

      free (element->name);
      free (element->contents);
      free (element);
    }
}


/* Accessors */

XML_Char const*
scew_element_name (scew_element const *element)
{
  assert (element != NULL);

  return element->name;
}

XML_Char const*
scew_element_contents (scew_element const *element)
{
  assert (element != NULL);

  return element->contents;
}

XML_Char const*
scew_element_set_name (scew_element *element, XML_Char const *name)
{
  XML_Char *new_name = NULL;

  assert (element != NULL);
  assert (name != NULL);

  new_name = scew_strdup (name);
  if (new_name != NULL)
    {
      free (element->name);
      element->name = new_name;
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return new_name;
}

XML_Char const*
scew_element_set_contents (scew_element *element, XML_Char const *contents)
{
  XML_Char *new_contents = NULL;

  assert (element != NULL);
  assert (contents != NULL);

  new_contents = scew_strdup (contents);
  if (new_contents != NULL)
    {
      free (element->contents);
      element->contents = new_contents;
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return new_contents;
}

void
scew_element_free_contents (scew_element *element)
{
  assert (element != NULL);

  if (element->contents != NULL)
    {
      free (element->contents);
      element->contents = NULL;
    }
}


/* Hierarchy */

unsigned int
scew_element_count (scew_element const *element)
{
  assert (element != NULL);

  return element->n_children;
}

scew_element*
scew_element_parent (scew_element const *element)
{
  assert (element != NULL);

  return element->parent;
}

scew_list*
scew_element_children (scew_element const *element)
{
  assert (element != NULL);

  return element->children;
}

scew_element*
scew_element_add (scew_element *element, XML_Char const *name)
{
  scew_element *add_elem = NULL;
  scew_element *new_elem = NULL;

  assert (element != NULL);
  assert (name != NULL);

  new_elem = scew_element_create (name);

  if (new_elem != NULL)
    {
      add_elem = scew_element_add_element (element, new_elem);

      /* Delete element if it can not be added */
      if (NULL == add_elem)
        {
          scew_element_free (new_elem);
        }
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return add_elem;
}

scew_element*
scew_element_add_pair (scew_element *element,
                       XML_Char const *name,
                       XML_Char const *contents)
{
  scew_element *add_elem = NULL;
  scew_element *new_elem = NULL;

  assert (element != NULL);
  assert (name != NULL);
  assert (contents != NULL);

  add_elem = NULL;
  new_elem = scew_element_create (name);

  if (new_elem != NULL)
    {
      XML_Char const *add_contents = scew_element_set_contents (new_elem,
                                                                contents);

      /**
       * If contents could not be created does not affect adding an
       * element (we will treat the error below).
       */
      add_elem = scew_element_add_element (element, new_elem);

      /**
       * Delete element if unable to add contents or new element can
       * not be added.
       */
      if ((NULL == add_contents) || (NULL == add_elem))
        {
          scew_element_free (new_elem);
        }
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return add_elem;
}

scew_element*
scew_element_add_element (scew_element *element, scew_element *child)
{
  scew_list *item = NULL;

  assert (element != NULL);
  assert (child != NULL);
  assert (scew_element_parent (child) == NULL);

  item = scew_list_append (element->last_child, child);

  if (item != NULL)
    {
      if (NULL == element->children)
        {
          element->children = item;
        }
      child->parent = element;
      child->myself = item;

      element->last_child = item;
      element->n_children += 1;
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
      child = NULL;
    }

  return child;
}

void
scew_element_delete_all (scew_element *element)
{
  scew_list *list = NULL;

  assert (element != NULL);

  list = element->children;
  while (list != NULL)
    {
      scew_element *aux = scew_list_data (list);
      list = scew_list_next (list);
      scew_element_free (aux);
    }

  /* Do not free element->children as childs are already detached. */

  element->children = NULL;
  element->last_child = NULL;
  element->n_children = 0;
}

void
scew_element_delete_all_by_name (scew_element *element, XML_Char const *name)
{
  scew_element *child = NULL;

  assert (element != NULL);
  assert (name != NULL);

  child = scew_element_by_name (element, name);
  while (child != NULL)
    {
      scew_element_free (child);

      child = scew_element_by_name (element, name);
    }
}

void
scew_element_delete_by_name (scew_element *element, XML_Char const *name)
{
  assert (element != NULL);
  assert (name != NULL);

  scew_element_free (scew_element_by_name (element, name));
}

void
scew_element_delete_by_index (scew_element *element, unsigned int index)
{
  assert (element != NULL);
  assert (index < element->n_children);

  scew_element_free (scew_element_by_index (element, index));
}

void
scew_element_detach (scew_element *element)
{
  scew_element *parent = NULL;

  assert (element != NULL);

  parent = element->parent;

  if (parent != NULL)
    {
      if (parent->last_child == element->myself)
        {
          parent->last_child = scew_list_previous (element->myself);
        }

      parent->children = scew_list_delete_item (parent->children,
                                                element->myself);

      --parent->n_children;
      if (0 == parent->n_children)
        {
          parent->children = NULL;
          parent->last_child = NULL;
        }

      element->parent = NULL;
      element->myself = NULL;
    }
}
