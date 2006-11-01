/**
 *
 * @file     xattribute.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Dec 02, 2002 23:03
 * @brief    SCEW private attribute type declaration
 *
 * $Id: xattribute.c,v 1.2 2004/02/25 20:33:40 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2002, 2003, 2004 Aleix Conchillo Flaque
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * @endif
 */

#include "xattribute.h"

#include "xerror.h"

#include "str.h"

#include <assert.h>
#include <string.h>


scew_attribute*
attribute_create(XML_Char const* name, XML_Char const* value)
{
    scew_attribute* attribute = NULL;

    assert(name != NULL);
    assert(value != NULL);

    attribute = (scew_attribute*) calloc(1, sizeof(scew_attribute));
    if (attribute == NULL)
    {
        set_last_error(scew_error_no_memory);
        return NULL;
    }
    attribute->name = scew_strdup(name);
    attribute->value = scew_strdup(value);

    return attribute;
}

void
attribute_free(scew_attribute* attribute)
{
    if (attribute != NULL)
    {
        free(attribute->name);
        free(attribute->value);
        free(attribute);
    }
}

attribute_list*
attribute_list_create()
{
    attribute_list* list = NULL;

    list = (attribute_list*) calloc(1, sizeof(attribute_list));
    if (list == NULL)
    {
        set_last_error(scew_error_no_memory);
    }

    return list;
}

void
attribute_list_free(attribute_list* list)
{
    scew_attribute* it = NULL;
    scew_attribute* tmp = NULL;

    if (list == NULL)
    {
        return;
    }

    it = list->first;
    while (it != NULL)
    {
        tmp = it;
        it = it->next;
        attribute_free(tmp);
    }

    free(list);
}

scew_attribute*
attribute_list_add(attribute_list* list, scew_attribute* attribute)
{
    scew_attribute* aux = NULL;

    assert(list != NULL);
    assert(attribute != NULL);

    aux = attribute_by_name(list, attribute->name);
    if (aux != NULL)
    {
        if (aux->prev != NULL)
        {
            aux->prev->next = attribute;
        }
        if (aux->next != NULL)
        {
            aux->next->prev = attribute;
        }
        if (list->first == aux)
        {
            list->first = attribute;
        }
        if (list->last == aux)
        {
            list->last = attribute;
        }
        attribute->prev = aux->prev;
        attribute->next = aux->next;
        attribute_free(aux);
        return attribute;
    }

    list->size++;
    if (list->first == NULL)
    {
        list->first = attribute;
    }
    else
    {
        list->last->next = attribute;
        attribute->prev = list->last;
    }
    list->last = attribute;

    return attribute;
}

void
attribute_list_del(attribute_list* list, XML_Char const* name)
{
    scew_attribute* attribute = NULL;
    scew_attribute* tmp_prev = NULL;
    scew_attribute* tmp_next = NULL;

    if ((list == NULL) || (name == NULL))
    {
        return;
    }

    attribute = attribute_by_name(list, name);

    if (attribute != NULL)
    {
        tmp_prev = attribute->prev;
        tmp_next = attribute->next;
        if (tmp_prev != NULL)
        {
            tmp_prev->next = tmp_next;
        }
        if (tmp_next != NULL)
        {
            tmp_next->prev = tmp_prev;
        }

        if (attribute == list->first)
        {
            list->first = tmp_next;
        }

        if (attribute == list->last)
        {
            list->last = tmp_prev;
        }

        attribute_free(attribute);
        list->size--;
    }
}

scew_attribute*
attribute_by_index(attribute_list* list, unsigned int idx)
{
    unsigned int i = 0;
    scew_attribute* attribute = NULL;

    if (list == NULL)
    {
        return NULL;
    }
    assert(idx < list->size);

    attribute = list->first;
    for (i = 0; (i < idx) && (attribute != NULL); ++i)
    {
        attribute = attribute->next;
    }

    return attribute;
}

scew_attribute*
attribute_by_name(attribute_list* list, XML_Char const* name)
{
    scew_attribute* attribute = NULL;

    assert(list != NULL);
    if (name == NULL)
    {
        return NULL;
    }

    attribute = list->first;
    while (attribute && scew_strcmp(attribute->name, name))
    {
        attribute = attribute->next;
    }

    return attribute;
}
