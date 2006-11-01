/**
 *
 * @file     writer.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Sep 11, 2003 00:36
 * @brief    SCEW writer functions
 *
 * $Id: writer.h,v 1.1 2004/01/28 00:43:21 aleix Exp $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * @endif
 */


#ifndef WRITER_H_ALEIX0309110036
#define WRITER_H_ALEIX0309110036

#include "types.h"

#include <expat.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Saves an XML tree to the specified file. If the file doesn't exist it
 * will create it, and if it exists it will overwrite it.
 *
 * @return 1 if tree was successfully saved, 0 otherwise.
 */
extern unsigned int
scew_writer_tree_file(scew_tree const* tree, char const* file_name);

/**
 * Saves an XML tree to the already created file pointer.
 *
 * @return 1 if tree was successfully saved, 0 otherwise.
 */
extern unsigned int
scew_writer_tree_fp(scew_tree const* tree, FILE* out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WRITER_H_ALEIX0309110036 */
