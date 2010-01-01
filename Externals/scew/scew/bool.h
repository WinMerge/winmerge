/**
 * @file     bool.h
 * @brief    SCEW boolean type declaration
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Sep 04, 2008 11:42
 *
 * @if copyright
 *
 * Copyright (C) 2008, 2009 Aleix Conchillo Flaque
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

#ifndef BOOL_H_0809041142
#define BOOL_H_0809041142

/**
 * This should be defined using stdbool.h when C99 is available.
 */
typedef unsigned char scew_bool;

#define SCEW_TRUE   ((scew_bool) 1) /**< True */
#define SCEW_FALSE  ((scew_bool) 0) /**< False */

#endif /* BOOL_H_0809041142 */
