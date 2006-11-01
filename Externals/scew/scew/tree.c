/**
 *
 * @file     tree.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Feb 20, 2003 23:45
 * @brief    SCEW tree type declaration
 *
 * $Id: tree.c,v 1.1 2004/01/28 00:43:21 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2003 Aleix Conchillo Flaque
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * @endif
 */

#include "tree.h"

#include "str.h"

#include "xerror.h"
#include "xtree.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


scew_tree*
scew_tree_create()
{
    scew_tree* tree = NULL;

    tree = (scew_tree*) calloc(1, sizeof(scew_tree));
    if (tree == NULL)
    {
        set_last_error(scew_error_no_memory);
    }

    return tree;
}

void
scew_tree_free(scew_tree* tree)
{
    if (tree != NULL)
    {
        free(tree->version);
        free(tree->encoding);
        scew_element_free(tree->root);
        free(tree);
    }
}

scew_element*
scew_tree_root(scew_tree const* tree)
{
    assert(tree != NULL);

    return tree->root;
}

scew_element*
scew_tree_add_root(scew_tree* tree, XML_Char const* name)
{
    scew_element* root = NULL;

    assert(tree != NULL);
    assert(name != NULL);

    root = scew_element_create(name);
    tree->root = root;

    return root;
}

void
scew_tree_set_xml_version(scew_tree* tree, XML_Char const* version)
{
    assert(tree != NULL);

    free(tree->version);
    tree->version = scew_strdup(version);
}

void
scew_tree_set_xml_encoding(scew_tree* tree, XML_Char const* encoding)
{
    assert(tree != NULL);

    free(tree->encoding);
    tree->encoding = scew_strdup(encoding);
}

void
scew_tree_set_xml_preamble(scew_tree* tree, XML_Char const* preamble)
{
    assert(tree != NULL);

    free(tree->preamble);
    tree->preamble = scew_strdup(preamble);
}

void
scew_tree_set_xml_standalone(scew_tree* tree, int standalone)
{
    assert(tree != NULL);

    tree->standalone = standalone;
}
