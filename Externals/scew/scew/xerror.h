/**
 * @file     xerror.h
 * @brief    Internal error functions
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon May 05, 2003 10:40
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

#ifndef XERROR_H_0305051040
#define XERROR_H_0305051040

#include "export.h"

#include "error.h"

/**
 * Sets SCEW internal last error.
 */
extern SCEW_LOCAL void scew_error_set_last_error_ (scew_error code);

/**
 * Gets SCEW internal last error.
 */
extern SCEW_LOCAL scew_error scew_error_last_error_ (void);

#endif /* XERROR_H_0305051040 */
