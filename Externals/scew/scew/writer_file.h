/**
 * @file     writer_file.h
 * @brief    SCEW writer functions for files
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Nov 13, 2008 11:01
 * @ingroup  SCEWWriterFile
 *
 * @if copyright
 *
 * Copyright (C) 2008-2009 Aleix Conchillo Flaque
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

/**
 * @defgroup SCEWWriterFile Files
 * Write data to files.
 * @ingroup SCEWWriter
 */

#ifndef WRITER_FILE_H_0811131101
#define WRITER_FILE_H_0811131101

#include "export.h"

#include "writer.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Creates a new SCEW writer for the given file name. This routine
 * will create a new file if the file does not exist or it will
 * overwrite the existing one. The file will be created in text
 * mode. Once the writer is created, the @ref SCEWWriter routines must
 * be called in order to store data to the file or to know the file
 * status.
 *
 * For UTF-16 encoding (only in Windows paltforms) the BOM (Byte Order
 * Mask) is automatically handled by the Windows API.
 *
 * @pre file_name != NULL
 *
 * @param file_name the file name to create for the new SCEW writer.
 *
 * @return a new SCEW writer for the given file name or NULL if the
 * writer could not be created (e.g. memory allocation, file
 * permissions, etc.).
 *
 * @ingroup SCEWWriterFile
 */
extern SCEW_API scew_writer* scew_writer_file_create (char const *file_name);

/**
 * Creates a new SCEW writer for the given @a file stream. The file
 * stream is created in text mode. Once the writer is created, any of
 * the @ref SCEWWriter routines must be called in order to store data
 * to the file or to know the file status.
 *
 * For UTF-16 encoding (only in Windows paltforms) the BOM (Byte Order
 * Mask) is automatically handled by the Windows API.
 *
 * @pre file != NULL
 *
 * @param file the file where the new SCEW writer will write to.
 *
 * @return a new SCEW writer for the given file stream or NULL if the
 * writer could not be created (e.g. memory allocation, file
 * permissions, etc.).
 *
 * @ingroup SCEWWriterFile
 */
extern SCEW_API scew_writer* scew_writer_fp_create (FILE *file);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WRITER_FILE_H_0811131101 */
