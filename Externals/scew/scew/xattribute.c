/**
 * @file     xattribute.c
 * @brief    xattribute.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Aug 24, 2009 23:43
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

#include "xattribute.h"

#include <assert.h>



/* Protected */

void
scew_attribute_set_parent_ (scew_attribute *attribute,
                            scew_element const *parent)
{
  assert (attribute != NULL);

  attribute->parent = (scew_element *) parent;
}
