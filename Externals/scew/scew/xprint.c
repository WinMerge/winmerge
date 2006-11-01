/**
 *
 * @file     xtree.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Mar 30, 2003 13:23
 * @brief    SCEW private tree type declaration
 *
 * $Id: xprint.c,v 1.2 2004/03/20 11:14:22 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2003, 2004 Aleix Conchillo Flaque
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * @endif
 */

#include "xprint.h"
#include "str.h"

/* indentation size (in whitespaces) */
static int const indent_size = 3;

void
indent_print(FILE* out, unsigned int indent)
{
    unsigned int i = 0;

    for (i = 0; i < indent * indent_size; i++)
    {
        scew_fprintf(out, _XT(" "));
    }
}

void
tree_print(scew_tree const* tree, FILE* out)
{
    static XML_Char const* version = _XT("1.0");
#ifdef XML_UNICODE_WCHAR_T
    static XML_Char const* encoding = _XT("UTF-16");
#else
    static XML_Char const* encoding = _XT("UTF-8");
#endif /* XML_UNICODE_WCHAR_T */

    scew_fprintf(
        out,
        _XT("<?xml version=\"%s\" encoding=\"%s\" standalone=\"%s\"?>\n\n"),
        (tree->version == NULL ? version : tree->version),
        (tree->encoding == NULL ? encoding : tree->encoding),
        (tree->standalone > 0 ? _XT("yes") : _XT("no")));

    element_print(tree->root, out, 0);
}

void
element_print(scew_element const* element, FILE* out, unsigned int indent)
{
    unsigned int closed = 0;
    XML_Char const* contents;
    scew_element* child = NULL;
    scew_attribute* attribute = NULL;

    if (element == NULL)
    {
        return;
    }

    indent_print(out, indent);
    scew_fprintf(out, _XT("<%s"), scew_element_name(element));
    attribute = NULL;
    while ((attribute = scew_attribute_next(element, attribute)) != NULL)
    {
        attribute_print(attribute, out);
    }

    contents = scew_element_contents(element);
    if ((contents == NULL) && (element->child == NULL)
        && (element->parent != NULL))
    {
		scew_fprintf(out, _XT("/>\n"));
        closed = 1;
    }
    else
    {
        scew_fprintf(out, _XT(">"));
        if (contents == NULL)
        {
            scew_fprintf(out, _XT("\n"));
        }
    }

    child = NULL;
    while ((child = scew_element_next(element, child)) != NULL)
    {
        element_print(child, out, indent + 1);
    }

    if (contents != NULL)
    {
        scew_fprintf(out, _XT("%s"), contents);
    }
    else if (!closed)
    {
        indent_print(out, indent);
    }

    if (!closed)
    {
        scew_fprintf(out, _XT("</%s>\n"), scew_element_name(element));
    }
}

void
attribute_print(scew_attribute const* attribute, FILE* out)
{
    if (attribute == NULL)
    {
        return;
    }

    scew_fprintf(out, _XT(" %s=\"%s\""), attribute->name, attribute->value);
}
