/**
 *
 * @file     types.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sat Mar 29, 2003 19:50
 * @brief    SCEW main types definition
 *
 * $Id: types.h,v 1.2 2004/05/25 20:23:05 aleix Exp $
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * @endif
 */


#ifndef TYPES_H_ALEIX0303291951
#define TYPES_H_ALEIX0303291951

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type declaration of the SCEW parser.
 */
typedef struct _scew_parser scew_parser;

/**
 * This is the type delcaration for XML trees.
 */
typedef struct _scew_tree scew_tree;

/**
 * This is the type delcaration for elements.
 */
typedef struct _scew_element scew_element;

/**
 * This is the type declaration for element attributes.
 */
typedef struct _scew_attribute scew_attribute;

/**
 * Callback function type.
 *
 * @return 1 if callback call had no errors, 0 otherwise.
 */
typedef unsigned int SCEW_CALLBACK(scew_parser* parser);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TYPES_H_ALEIX0303291951 */
