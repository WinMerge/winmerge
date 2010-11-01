/**
 * @file     reader_file.h
 * @brief    SCEW reader functions for files
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Nov 23, 2008 13:51
 * @ingroup  SCEWReaderFile
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
 * @defgroup SCEWReaderFile Files
 * Read data from files.
 * @ingroup SCEWReader
 */

#ifndef READER_FILE_H_0811231351
#define READER_FILE_H_0811231351

#include "export.h"

#include "reader.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Creates a new SCEW reader for the given file name. This routine
 * will open the given file in text mode. Once the reader is created,
 * the @ref SCEWReader routines must be called in order to read data
 * from the file or to know the file status.
 *
 * For UTF-16 encoding (only in Windows paltforms) the BOM (Byte Order
 * Mask) is automatically handled by the Windows API.
 *
 * @pre file_name != NULL
 *
 * @param file_name the file name to open for the new SCEW reader.
 *
 * @return a new SCEW reader for the given file name or NULL if the
 * reader could not be created (e.g. memory allocation, the file does
 * not exist, etc.).
 *
 * @ingroup SCEWReaderFile
 */
extern SCEW_API scew_reader* scew_reader_file_create (char const *file_name);

/**
 * Creates a new SCEW reader for the given @a file stream. The file
 * stream is opened in text mode. Once the reader is created, any of
 * the @ref SCEWReader routines must be called in order to read data
 * from the file or to know the file status.
 *
 * For UTF-16 encoding (only in Windows paltforms) the BOM (Byte Order
 * Mask) is automatically handled by the Windows API.
 *
 * @pre file != NULL
 *
 * @param file the file where the new SCEW reader should read data
 * from.
 *
 * @return a new SCEW reader for the given file stream or NULL if the
 * reader could not be created (e.g. memory allocation, the file does
 * not exist, etc.).
 *
 * @ingroup SCEWReaderFile
 */
extern SCEW_API scew_reader* scew_reader_fp_create (FILE *file);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* READER_FILE_H_0811231351 */
