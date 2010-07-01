/**
 * @file     tree.c
 * @brief    tree.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Feb 20, 2003 23:45
 *
 * @if copyright
 *
 * Copyright (C) 2003-2009 Aleix Conchillo Flaque
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

#include "tree.h"

#include "xerror.h"

#include "element.h"
#include "str.h"

#include <assert.h>


/* Private */

struct scew_tree
{
  XML_Char *version;
  XML_Char *encoding;
  XML_Char *preamble;
  scew_tree_standalone standalone;
  scew_element *root;
};

static scew_bool compare_tree_ (scew_tree const *a, scew_tree const *b);

static XML_Char const *DEFAULT_XML_VERSION_ = (XML_Char *) _XT("1.0");
static XML_Char const *DEFAULT_ENCODING_ = (XML_Char *) _XT("UTF-8");


/* Allocation */

scew_tree*
scew_tree_create (void)
{
  scew_tree *tree = calloc (1, sizeof (scew_tree));

  if (tree != NULL)
    {
      scew_tree_set_xml_version (tree, DEFAULT_XML_VERSION_);
      scew_tree_set_xml_encoding (tree, DEFAULT_ENCODING_);
      scew_tree_set_xml_standalone (tree, scew_tree_standalone_unknown);
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return tree;
}

scew_tree*
scew_tree_copy (scew_tree const *tree)
{
  scew_tree *new_tree = NULL;

  assert (tree != NULL);

  new_tree = calloc (1, sizeof (scew_tree));

  if (new_tree != NULL)
    {
      scew_bool copied = SCEW_FALSE;

      new_tree->version = scew_strdup (tree->version);
      new_tree->encoding = scew_strdup (tree->encoding);
      new_tree->preamble = scew_strdup (tree->preamble);
      new_tree->standalone = tree->standalone;
      new_tree->root = scew_element_copy (tree->root);

      copied =
        ((tree->version == NULL) || (new_tree->version != NULL))
        && ((tree->encoding == NULL) || (new_tree->encoding != NULL))
        && ((tree->preamble == NULL) || (new_tree->preamble != NULL))
        && ((tree->root == NULL) || (new_tree->root != NULL));

      if (!copied)
        {
          scew_tree_free (new_tree);
          new_tree = NULL;
        }
    }

  return new_tree;
}

void
scew_tree_free (scew_tree *tree)
{
  if (tree != NULL)
    {
      free (tree->version);
      free (tree->encoding);
      free (tree->preamble);
      scew_element_free (tree->root);
      free (tree);
    }
}


/* Comparison */

scew_bool
scew_tree_compare (scew_tree const *a,
                   scew_tree const *b,
                   scew_tree_cmp_hook hook)
{
  scew_tree_cmp_hook cmp_hook = NULL;

  assert (a != NULL);
  assert (b != NULL);

  cmp_hook = (NULL == hook) ? compare_tree_ : hook;

  return cmp_hook (a, b);
}


/* Properties */

XML_Char const*
scew_tree_xml_version (scew_tree const *tree)
{
  assert(tree != NULL);

  return tree->version;
}

void
scew_tree_set_xml_version (scew_tree *tree, XML_Char const *version)
{
  assert (tree != NULL);
  assert (version != NULL);

  free (tree->version);

  tree->version = scew_strdup (version);
}

XML_Char const*
scew_tree_xml_encoding (scew_tree const *tree)
{
  assert(tree != NULL);

  return tree->encoding;
}

void
scew_tree_set_xml_encoding (scew_tree *tree, XML_Char const *encoding)
{
  assert (tree != NULL);
  assert (encoding != NULL);

  free (tree->encoding);

  tree->encoding = scew_strdup (encoding);
}

scew_tree_standalone
scew_tree_xml_standalone (scew_tree const *tree)
{
  assert (tree != NULL);

  return tree->standalone;
}

void
scew_tree_set_xml_standalone (scew_tree *tree, scew_tree_standalone standalone)
{
  assert(tree != NULL);

  tree->standalone = standalone;
}


/* Contents */

scew_element*
scew_tree_root (scew_tree const *tree)
{
  assert (tree != NULL);

  return tree->root;
}

scew_element*
scew_tree_set_root (scew_tree *tree, XML_Char const *name)
{
  scew_element *root = NULL;
  scew_element *new_root = NULL;

  assert (tree != NULL);
  assert (name != NULL);

  root = scew_element_create (name);

  if (root != NULL)
    {
      new_root = scew_tree_set_root_element (tree, root);
    }
  else
    {
      scew_error_set_last_error_ (scew_error_no_memory);
    }

  return new_root;
}

scew_element*
scew_tree_set_root_element (scew_tree *tree, scew_element *root)
{
  assert (tree != NULL);
  assert (root != NULL);

  tree->root = root;

  return root;
}

XML_Char const*
scew_tree_xml_preamble (scew_tree const *tree)
{
  assert (tree != NULL);

  return tree->preamble;
}

void
scew_tree_set_xml_preamble (scew_tree *tree, XML_Char const *preamble)
{
  assert (tree != NULL);
  assert (preamble != NULL);

  free (tree->preamble);

  tree->preamble = scew_strdup (preamble);
}


/* Private*/

scew_bool
compare_tree_ (scew_tree const *a, scew_tree const *b)
{
  scew_bool equal = SCEW_FALSE;

  assert (a != NULL);
  assert (b != NULL);

  equal = (scew_strcmp (a->version, b->version) == 0)
    && (scew_strcmp (a->encoding, b->encoding) == 0)
    && (scew_strcmp (a->preamble, b->preamble) == 0)
    && (a->standalone == b->standalone)
    && scew_element_compare (a->root, b->root, NULL);

  return equal;
}
