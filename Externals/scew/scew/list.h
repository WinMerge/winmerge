/**
 * @file     list.h
 * @brief    SCEW general list implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Jul 12, 2007 20:09
 * @ingroup  SCEWList, SCEWListAlloc, SCEWListAcc, SCEWListMod
 * @ingroup  SCEWListTrav, SCEWListSearch, SCEWList
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
 **/

/**
 * @defgroup SCEWList Lists
 *
 * This is a generic list implementation currenlty used by element's
 * children and attributes, though, as a generic list, it can be used
 * with any other type of data.
 */

#ifndef LIST_H_0707122009
#define LIST_H_0707122009

#include "export.h"

#include "bool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type delcaration for SCEW lists.
 *
 * @ingroup SCEWList
 */
typedef struct scew_list scew_list;

/**
 * SCEW lists hooks (functions) are used by #scew_list_foreach. The
 * hook will be used to perform some custom action, defined by the
 * hook, to every list item. These functions take two arguments, the
 * list item where the action should be performed and an additional
 * argument for any data that could be of use to the action.
 *
 * @param item the list item currently being traversed.
 * @param user_data an optional user data pointer to be used by the
 * hook (might be NULL).
 *
 * @ingroup SCEWList
 */
typedef void (*scew_list_hook) (scew_list *, void *);

/**
 * SCEW lists comparison hooks are used by #scew_list_find_custom. The
 * hook takes the two arguments to be compared (of the same type).
 *
 * @return true if the given arguments are considered (by the provided
 * comparison hook) equal, false otherwise.
 *
 * @ingroup SCEWList
 */
typedef scew_bool (*scew_cmp_hook) (void const *, void const *);


/**
 * @defgroup SCEWListAlloc Allocation
 * Allocate and free new lists.
 * @ingroup SCEWList
 */

/**
 * Creates a new list item with the given @a data. Note that there is
 * no difference between list items and lists, that is, a list item is
 * a list itself.
 *
 * @pre data != NULL
 *
 * @return a new list, or NULL if the list could not be created.
 *
 * @ingroup SCEWListAlloc
 */
extern SCEW_API scew_list* scew_list_create (void *data);

/**
 * Frees all the items from the given @a list. The data pointers are
 * not freed, thus they need to be freed separately.
 *
 * @ingroup SCEWListAlloc
 */
extern SCEW_API void scew_list_free (scew_list *list);


/**
 * @defgroup SCEWListAcc Accessors
 * Access lists' data and information.
 * @ingroup SCEWList
 */

/**
 * Returns the data pointer of the given @a list item. Note that this
 * routine does not know if the data pointed by the @a list item has
 * been freed, so it might return a valid address without useful
 * content.
 *
 * @pre list != NULL
 *
 * @return the data pointer for the given @a list.
 *
 * @ingroup SCEWListAcc
 */
extern SCEW_API void* scew_list_data (scew_list *list);

/**
 * Returns the number of items in the given @a list.
 *
 * @return the number of items in the given @a list or 0 if @a list is
 * NULL.
 *
 * @ingroup SCEWListAcc
 */
extern SCEW_API unsigned int scew_list_size (scew_list *list);


/**
 * @defgroup SCEWListMod Modifiers
 * Add and remove items from lists.
 * @ingroup SCEWList
 */

/**
 * Creates a new list item with the given @a data and appends it to @a
 * list. If the given @a list is NULL this function acts like
 * #scew_list_create.
 *
 * @pre data != NULL
 *
 * @return the item appended to @a list or NULL if an item could not
 * be created.
 *
 * @ingroup SCEWListMod
 */
extern SCEW_API scew_list* scew_list_append (scew_list *list, void *data);

/**
 * Creates a new list item with the given @a data and prepends it to
 * @a list. If the given @a list is NULL this function acts like
 * #scew_list_create.
 *
 * @pre data != NULL
 *
 * @return the item prepended to @a list or NULL if an item could not
 * be created.
 *
 * @ingroup SCEWListMod
 */
extern SCEW_API scew_list* scew_list_prepend (scew_list *list, void *data);

/**
 * Deletes the first item pointing to @a data from the given @a
 * list. This function will search from the given item list, not from
 * the beginning.
 *
 * @pre list != NULL
 * @pre data != NULL
 *
 * @return @a list if the item found was not the first list item, or
 * the new first item otherwise.
 *
 * @ingroup SCEWListMod
 */
extern SCEW_API scew_list* scew_list_delete (scew_list *list, void *data);

/**
 * Deletes the given list @a item from @a list.
 *
 * @pre list != NULL
 *
 * @return @a list if @a item was not the first list item, or the new
 * first item otherwise.
 *
 * @ingroup SCEWListMod
 */
extern SCEW_API scew_list* scew_list_delete_item (scew_list *list,
                                                  scew_list *item);


/**
 * @defgroup SCEWListTrav Traverse
 * Traverse list items.
 * @ingroup SCEWList
 */

/**
 * Finds the first item of the given @a list. This function traverses
 * all the @a list backwards until it finds an item whose previous
 * item is NULL.
 *
 * @pre list != NULL
 *
 * @return the first item of the given @a list or NULL if @a list is
 * NULL.
 *
 * @ingroup SCEWListTrav
 */
extern SCEW_API scew_list* scew_list_first (scew_list *list);

/**
 * Finds the last item of the given @a list. This function traverses
 * all the @a list forwards until it finds an item whose next item is
 * NULL.
 *
 * @pre list != NULL
 *
 * @return the last item of the given @a list or NULL if @a list is
 * NULL.
 *
 * @ingroup SCEWListTrav
 */
extern SCEW_API scew_list* scew_list_last (scew_list *list);

/**
 * Obtains the next item of the given @a list item.
 *
 * @pre list != NULL
 *
 * @return the next @a list item or NULL if @a list is the last item.
 *
 * @ingroup SCEWListTrav
 */
extern SCEW_API scew_list* scew_list_next (scew_list *list);

/**
 * Obtains the previous item of the given @a list item.
 *
 * @pre list != NULL
 *
 * @return the previous @a list item or NULL if @a list is the first
 * item.
 *
 * @ingroup SCEWListTrav
 */
extern SCEW_API scew_list* scew_list_previous (scew_list *list);

/**
 * Gets the @a list item at the given @a index.
 *
 * @pre list != NULL
 *
 * @return the @a list item at @a index, or NULL if @a list does not
 * contain sufficient items.
 *
 * @ingroup SCEWListSearch
 */
extern SCEW_API scew_list* scew_list_index (scew_list *list,
                                            unsigned int index);

/**
 * Traverses all @a list items and executes the given @a hook for each
 * item found. The hook takes an extra paramter, @a user_data, which
 * might be NULL.
 *
 * @pre list != NULL
 * @pre func != NULL
 *
 * @param list the list to traverse.
 * @param hook the action to be executed for every traversed item.
 * @param user_data an optional user data pointer (might be NULL).
 *
 * @ingroup SCEWListTrav
 */
extern SCEW_API void scew_list_foreach (scew_list *list,
                                        scew_list_hook hook,
                                        void *user_data);

/**
 * @defgroup SCEWListSearch Search
 * Search for list items.
 * @ingroup SCEWList
 */

/**
 * Finds the first @a list item that contains @a data.
 *
 * @pre list != NULL
 * @pre data != NULL
 *
 * @return the first @a list item that cointains @a data, or NULL if
 * @a data is not found.
 *
 * @ingroup SCEWListSearch
 */
extern SCEW_API scew_list* scew_list_find (scew_list *list, void *data);

/**
 * Finds the first @a list item that matches the given predicate, @a
 * hook. That is, all the @a list will be traversed calling the
 * comparison hook for every @a list item. The comparison hook takes
 * two parameters, the first one is the data of current traversed
 * item, the second is @a data.
 *
 * @pre list != NULL
 * @pre data != NULL
 * @pre func != NULL
 *
 * @param list the list to traverse.
 * @param data the user data to be used as one of the arguments for
 * the comparison.
 * @param hook the comparison function.
 *
 * @return the first @a list item that matches the predicate @a func,
 * or NULL if the predicate is never true.
 *
 * @ingroup SCEWListSearch
 */
extern SCEW_API scew_list* scew_list_find_custom (scew_list *list,
                                                  void const *data,
                                                  scew_cmp_hook hook);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIST_H_0707122009 */
