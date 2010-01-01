/**
 * @file     writer_buffer.h
 * @brief    SCEW writer functions for memory buffers
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Nov 13, 2008 11:03
 * @ingroup  SCEWWriterMemory
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
 * @defgroup SCEWWriterMemory Memory
 * Write data to memory buffers.
 * @ingroup SCEWWriter
 */

#ifndef WRITER_BUFFER_H_0811131103
#define WRITER_BUFFER_H_0811131103

#include "export.h"

#include "writer.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Creates a new SCEW writer for the given memory @a buffer of the
 * specified @a size. The buffer should exist before calling this
 * function and the @a size of the buffer should be large enough to
 * store the desired information (e.g. an XML tree, an
 * element...). Once the writer is created, any of the @ref SCEWWriter
 * functions might be called in order to store data to the buffer.
 *
 * @pre buffer != NULL
 * @pre size > 0
 *
 * @param buffer the memory area where the new SCEW writer will write to.
 * @param size the size of the memory area.
 *
 * @return a new SCEW writer for the given buffer or NULL if the
 * writer could not be created.
 *
 * @ingroup SCEWWriterMemory
 */
extern SCEW_API scew_writer* scew_writer_buffer_create (XML_Char *buffer,
                                                        size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WRITER_BUFFER_H_0811131103 */
