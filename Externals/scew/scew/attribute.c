/**
 * @file     attribute.c
 * @brief    attribute.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:41
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

#include "attribute.h"

#include "xattribute.h"

#include "xerror.h"

#include "str.h"

#include <assert.h>



/* Public */

/* Allocation */

scew_attribute*
scew_attribute_create (XML_Char const *name, XML_Char const *value)
{
  scew_attribute *attribute = NULL;

  assert (name != NULL);
  assert (value != NULL);

  attribute = calloc (1, sizeof (scew_attribute));

  if (attribute != NULL)
    {
      attribute->name = scew_strdup(name);
      attribute->value = scew_strdup(value);
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return attribute;
}

scew_attribute*
scew_attribute_copy (scew_attribute const *attribute)
{
  scew_attribute *new_attr = NULL;

  assert (attribute != NULL);

  new_attr = calloc (1, sizeof (scew_attribute));

  if (new_attr != NULL)
    {
      scew_bool copied =
        (scew_attribute_set_name (new_attr, attribute->name) != NULL)
        && (scew_attribute_set_value (new_attr, attribute->value) != NULL);

      if (!copied)
        {
          scew_attribute_free (new_attr);
          new_attr = NULL;
        }
    }

  return new_attr;
}

void
scew_attribute_free (scew_attribute *attribute)
{
  if (attribute != NULL)
    {
      free (attribute->name);
      free (attribute->value);
      free (attribute);
    }
}


/* Comparison */

scew_bool
scew_attribute_compare (scew_attribute const *a, scew_attribute const *b)
{
  assert (a != NULL);
  assert (b != NULL);

  return (scew_strcmp (a->name, b->name) == 0)
    && (scew_strcmp (a->value, b->value) == 0);
}


/* Accessors */

XML_Char const*
scew_attribute_name (scew_attribute const *attribute)
{
  assert (attribute != NULL);

  return attribute->name;
}

XML_Char const*
scew_attribute_value (scew_attribute const *attribute)
{
  assert (attribute != NULL);

  return attribute->value;
}

XML_Char const*
scew_attribute_set_name (scew_attribute *attribute, XML_Char const *name)
{
  XML_Char *new_name = NULL;

  assert (attribute != NULL);
  assert (name != NULL);

  new_name = scew_strdup (name);
  if (new_name != NULL)
    {
      free (attribute->name);
      attribute->name = new_name;
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return new_name;
}

XML_Char const*
scew_attribute_set_value (scew_attribute *attribute, XML_Char const *value)
{
  XML_Char *new_value = NULL;

  assert (attribute != NULL);
  assert (value != NULL);

  new_value = scew_strdup (value);
  if (new_value != NULL)
    {
      free (attribute->value);
      attribute->value = new_value;
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return new_value;
}

scew_element*
scew_attribute_parent (scew_attribute const *attribute)
{
  assert (attribute != NULL);

  return attribute->parent;
}
