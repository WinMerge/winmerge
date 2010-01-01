/**
 * @file     element_attribute.c
 * @brief    element.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Aug 27, 2009 01:43
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

#include "xattribute.h"
#include "xerror.h"

#include <assert.h>



/* Private */

static scew_bool cmp_attr_name_ (void const *attribute, void const *name);

static scew_attribute* add_new_attribute_ (scew_element *element,
                                           scew_attribute *attribute);

static scew_attribute* update_attribute_ (scew_element *element,
                                          scew_attribute *attribute,
                                          XML_Char const *value);


/* Public */

unsigned int
scew_element_attribute_count (scew_element const *element)
{
  assert (element != NULL);

  return element->n_attributes;
}

scew_list*
scew_element_attributes (scew_element const *element)
{
  assert (element != NULL);

  return element->attributes;
}

scew_attribute*
scew_element_attribute_by_name (scew_element const *element,
                                XML_Char const *name)
{
  scew_list *item = NULL;

  assert (element != NULL);
  assert (name != NULL);

  if (element->attributes != NULL)
    {
      item = scew_list_find_custom (element->attributes, name, cmp_attr_name_);
    }

  return (NULL == item) ? NULL : (scew_attribute *) scew_list_data (item);
}

scew_attribute*
scew_element_attribute_by_index (scew_element const *element,
                                 unsigned int index)
{
  scew_list *item = NULL;

  assert (element != NULL);
  assert (index < element->n_attributes);

  if (element->attributes != NULL)
    {
      item = scew_list_index (element->attributes, index);
    }

  return (NULL == item) ? NULL : (scew_attribute *) scew_list_data (item);
}

scew_attribute*
scew_element_add_attribute (scew_element *element, scew_attribute *attribute)
{
  scew_attribute *new_attribute = NULL;

  assert (element != NULL);
  assert (attribute != NULL);

  if (scew_attribute_parent (attribute) == NULL)
    {
      XML_Char const *name = scew_attribute_name (attribute);
      XML_Char const *value = scew_attribute_value (attribute);

      /* Try to find an existent attribute. */
      scew_attribute *old_attribute = scew_element_attribute_by_name (element,
                                                                      name);

      /* Add new attribute or update existent one. */
      new_attribute = (NULL == old_attribute)
        ? add_new_attribute_ (element, attribute)
        : update_attribute_ (element, old_attribute, value);
    }

  return new_attribute;
}


scew_attribute*
scew_element_add_attribute_pair (scew_element *element,
                                 XML_Char const *name,
                                 XML_Char const *value)
{
  scew_attribute *old_attribute = NULL;
  scew_attribute *new_attribute = NULL;

  assert (element != NULL);
  assert (name != NULL);
  assert (value != NULL);

  /* Try to find an existent attribute. */
  old_attribute = scew_element_attribute_by_name (element, name);

  if (NULL == old_attribute)
    {
      /**
       * If the attribute does not exist, create a new one and try to
       * add it.
       */
      scew_attribute *attribute = scew_attribute_create (name, value);

      if (attribute != NULL)
        {
          new_attribute = add_new_attribute_ (element, attribute);

          if (NULL == new_attribute)
            {
              /* The new attribute could not be added, free it. */
              scew_attribute_free (attribute);
            }
        }
    }
  else
    {
      /* If the attribtue already exists we update its value. */
      new_attribute = update_attribute_ (element, old_attribute, value);
    }

  return new_attribute;
}

void
scew_element_delete_attribute (scew_element *element,
                               scew_attribute *attribute)
{
  assert (element != NULL);
  assert (attribute != NULL);

  element->attributes = scew_list_delete (element->attributes, attribute);
  element->n_attributes -= 1;

  scew_attribute_free (attribute);
}

void
scew_element_delete_attribute_all (scew_element *element)
{
  scew_list *list = NULL;

  assert (element != NULL);

  /* Free all attributes. */
  list = element->attributes;
  while (list != NULL)
    {
      scew_attribute *aux = scew_list_data (list);
      list = scew_list_next (list);
      scew_attribute_free (aux);
    }
  scew_list_free (element->attributes);

  element->attributes = NULL;
  element->last_attribute = NULL;
  element->n_attributes = 0;
}

void
scew_element_delete_attribute_by_name (scew_element *element,
                                       XML_Char const* name)
{
  assert (element != NULL);
  assert (name != NULL);

  if (element->attributes != NULL)
    {
      scew_list *item =
        scew_list_find_custom (element->attributes, name, cmp_attr_name_);

      if (item != NULL)
        {
          scew_attribute *attribute = scew_list_data (item);
          scew_element_delete_attribute (element, attribute);
        }
    }
}

void
scew_element_delete_attribute_by_index (scew_element *element,
                                        unsigned int index)
{
  assert (element != NULL);
  assert (index < element->n_attributes);

  if (element->attributes != NULL)
    {
      scew_list *item = scew_list_index (element->attributes, index);

      if (item != NULL)
        {
          scew_attribute *attribute = scew_list_data (item);
          scew_element_delete_attribute (element, attribute);
        }
    }
}


/* Private */

scew_bool
cmp_attr_name_ (void const *attribute, void const *name)
{
  return (scew_strcmp (scew_attribute_name ((scew_attribute *) attribute),
                       (XML_Char *) name) == 0);
}

scew_attribute*
add_new_attribute_ (scew_element *element, scew_attribute *attribute)
{
  scew_list *item = NULL;
  scew_attribute *new_attribute = NULL;

  assert (element != NULL);
  assert (attribute != NULL);

  item = scew_list_append (element->last_attribute, attribute);

  if (item != NULL)
    {
      /* Initialise attributes list. */
      if (NULL == element->attributes)
        {
          element->attributes = item;
        }

      scew_attribute_set_parent_ (attribute, element);

      /* Update performance variables. */
      element->last_attribute = item;
      element->n_attributes += 1;

      /* Update the return value. */
      new_attribute = attribute;
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return new_attribute;
}

scew_attribute*
update_attribute_ (scew_element *element,
                   scew_attribute *attribute,
                   XML_Char const *value)
{
  XML_Char const *new_value = NULL;

  assert (element != NULL);
  assert (attribute != NULL);
  assert (value != NULL);

  new_value = scew_attribute_set_value (attribute, value);

  return (NULL == new_value) ? NULL : attribute;
}
