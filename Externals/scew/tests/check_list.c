/**
 * @file     check_element.c
 * @brief    Unit testing for SCEW lists
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Fri Aug 03, 2007 17:11
 *
 * @if copyright
 *
 * Copyright (C) 2007-2009 Aleix Conchillo Flaque
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

#include "test.h"

#include <scew/list.h>

#include <check.h>


/* Private */

typedef struct
{
  int value;
} item_t;

enum { N_ELEMENTS_ = 10 };

static item_t data_[N_ELEMENTS_];


/* Unit tests */

/* Allocation */

START_TEST (test_alloc)
{
  item_t data = { 3 };

  scew_list *list = scew_list_create (&data);

  CHECK_PTR (list, "Unable to create list");

  CHECK_BOOL (scew_list_data (list) == &data, SCEW_TRUE,
              "Invalid data pointer");

  scew_list_free (list);
}
END_TEST


/* Accessors */

START_TEST (test_accessors)
{
  /* Add items to list */
  scew_list *list = NULL;
  scew_list *last = NULL;
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      last = scew_list_append (last, &data_[i]);

      CHECK_PTR (last, "Unable to append item %d", i);
      CHECK_BOOL (scew_list_data (last) == &data_[i], SCEW_TRUE,
                  "Invalid data pointer (item %d)", i);

      item_t *tmp = scew_list_data (last);
      CHECK_S_INT (tmp->value, i, "Invalid data value (item %d)", i);

      if (list == NULL)
        {
          list = last;
          CHECK_NULL_PTR (scew_list_previous (list),
                          "First item should have no previous");
        }
    }

  CHECK_U_INT (scew_list_size (list), N_ELEMENTS_, "Number of items mismatch");

  scew_list_free (list);
}
END_TEST


/* Append */

START_TEST (test_append)
{
  /* Append items to list */
  scew_list *list = NULL;
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      scew_list *item = scew_list_append (list, &data_[i]);
      if (list == NULL)
        {
          list = item;
        }
      CHECK_PTR (item, "Unable to append item %d", i);
      CHECK_BOOL (scew_list_data (item) == &data_[i], SCEW_TRUE,
                  "Invalid data pointer (item %d)", i);

      item_t *tmp = scew_list_data (item);
      CHECK_S_INT (tmp->value, i, "Invalid data value (item %d)", i);
    }

  CHECK_U_INT (scew_list_size (list), N_ELEMENTS_, "Number of items mismatch");

  scew_list_free (list);
}
END_TEST


/* Prepend */

START_TEST (test_prepend)
{
  /* Prepend items to list */
  scew_list *list = NULL;
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      list = scew_list_prepend (list, &data_[i]);

      CHECK_PTR (list, "Unable to prepend item %d", i);
      CHECK_BOOL (scew_list_data (list) == &data_[i], SCEW_TRUE,
                  "Invalid data pointer (item %d)", i);

      item_t *tmp = scew_list_data (list);
      CHECK_S_INT (tmp->value, i, "Invalid data value (item %d)", i);
    }

  CHECK_U_INT (scew_list_size (list), N_ELEMENTS_, "Number of items mismatch");

  scew_list_free (list);
}
END_TEST


/* Delete */

START_TEST (test_delete)
{
  /* Append items to list */
  scew_list *list = NULL;
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      scew_list *item = scew_list_append (list, &data_[i]);
      if (list == NULL)
        {
          list = item;
        }
    }

  /* Delete items 2 and 6 */
  list = scew_list_delete (list, &data_[2]);
  list = scew_list_delete (list, &data_[6]);

  CHECK_U_INT (scew_list_size (list), N_ELEMENTS_ - 2,
             "Number of items mismatch");

  /* Delete item 1 */
  scew_list *item = scew_list_next (list);
  list = scew_list_delete_item (list, item);

  CHECK_U_INT (scew_list_size (list), N_ELEMENTS_ - 3,
             "Number of items mismatch");

  /* Check for all remaining items */
  enum { N_REMOVED = 3 };
  unsigned int const REMOVED[N_REMOVED] = { 1, 2, 6 };
  item = list;
  while (item != NULL)
    {
      item_t *tmp = scew_list_data (item);
      unsigned int j = 0;
      for (j = 0; j < N_REMOVED; ++j)
        {
          CHECK_BOOL (tmp->value != REMOVED[j], SCEW_TRUE,
                      "Item %d should have been previously removed",
                      tmp->value);
        }
      item = scew_list_next (item);
    }

  scew_list_free (list);
}
END_TEST


/* Traverse */

START_TEST (test_traverse)
{
  /* Append items to list */
  scew_list *item = NULL;
  scew_list *list = NULL;
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      item = scew_list_append (list, &data_[i]);
      if (list == NULL)
        {
          list = item;
        }
    }

  CHECK_BOOL (list == scew_list_first (list), SCEW_TRUE,
              "First list item should be itself");

  CHECK_BOOL (item == scew_list_last (list), SCEW_TRUE,
              "Last list item should be last item added");

  CHECK_NULL_PTR (scew_list_previous (list),
              "First previous item should be NULL");

  CHECK_NULL_PTR (scew_list_next (item),
              "Last next item should be NULL");

  item = list;
  while (item != NULL)
    {
      scew_list *old_item = item;

      item = scew_list_next (item);

      if (item != NULL)
        {
          CHECK_BOOL (old_item == scew_list_previous (item), SCEW_TRUE,
                      "Wrong previous item");
        }
    }
}
END_TEST


/* Traverse (foreach) */

static unsigned int foreach_calls_ = 0; /* keep track of number of calls */

static void
foreach_check_ (scew_list *item, void *user_data)
{
  item_t *fe_data = (item_t *) user_data;

  item_t *tmp = scew_list_data (item);

  CHECK_BOOL (tmp == &fe_data[foreach_calls_], SCEW_TRUE,
              "Data pointer does not match in foreach call");

  CHECK_S_INT (tmp->value, foreach_calls_,
               "Invalid data value in foreach call (item %d)", foreach_calls_);

  ++foreach_calls_;
}

START_TEST (test_traverse_foreach)
{
  /* Append items to list */
  scew_list *list = NULL;
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      scew_list *item = scew_list_append (list, &data_[i]);
      if (list == NULL)
        {
          list = item;
        }
    }

  foreach_calls_ = 0;

  scew_list_foreach (list, foreach_check_, data_);

  CHECK_U_INT (foreach_calls_, N_ELEMENTS_,
               "Number of foreach calls mismatch");
}
END_TEST


/* Search */

static scew_bool
search_cmp_ (void const *a, void const *b)
{
  return (((item_t *) a)->value == ((item_t *) b)->value);
}

START_TEST (test_search)
{
  /* Append items to list */
  scew_list *item_5 = NULL;
  scew_list *list = NULL;
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      scew_list *item = scew_list_append (list, &data_[i]);
      if (i == 5)
        {
          item_5 = item;
        }
      if (list == NULL)
        {
          list = item;
        }
    }

  CHECK_BOOL (item_5 == scew_list_find (list, &data_[5]), SCEW_TRUE,
              "Item 5 search failed");

  item_t value_5 = { 5 };
  CHECK_BOOL (item_5 == scew_list_find_custom (list, &value_5, search_cmp_),
              SCEW_TRUE, "Item 5 custom search failed");

  /* Sequential indexing */
  scew_list *item = list;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      CHECK_BOOL (item == scew_list_index (list, i), SCEW_TRUE,
                  "Sequential index item mismatch");
      item = scew_list_next (item);
    }

  CHECK_NULL_PTR (scew_list_index (list, N_ELEMENTS_),
                  "Sequential index out of range");
}
END_TEST


/* Suite */

static Suite*
list_suite (void)
{
  Suite *s = suite_create ("SCEW lists");

  /* Setup items */
  unsigned int i = 0;
  for (i = 0; i < N_ELEMENTS_; ++i)
    {
      data_[i].value = i;
    }

  /* Core test case */
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_alloc);
  tcase_add_test (tc_core, test_accessors);
  tcase_add_test (tc_core, test_append);
  tcase_add_test (tc_core, test_prepend);
  tcase_add_test (tc_core, test_delete);
  tcase_add_test (tc_core, test_traverse);
  tcase_add_test (tc_core, test_traverse_foreach);
  tcase_add_test (tc_core, test_search);
  suite_add_tcase (s, tc_core);

  return s;
}

void
run_tests (SRunner *sr)
{
  srunner_add_suite (sr, list_suite ());
}
