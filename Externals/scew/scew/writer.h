/**
 * @file     writer.h
 * @brief    SCEW writer common functions
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Thu Sep 11, 2003 00:36
 * @ingroup  SCEWWriter
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
 * @defgroup SCEWWriter Writers
 *
 * Write data to different destinations: files, memory, etc.
 *
 * SCEW writers provide a common mechanism to write data to different
 * destinations. This is done by implementing the set of functions
 * declared in #scew_writer_hooks. A user might create new SCEW
 * writers by implementing those functions.
 *
 * Once a SCEW writer is created, functions in this section should be
 * used no matter the writer type.
 *
 * @ingroup SCEWIO
 */

#ifndef WRITER_H_0309110036
#define WRITER_H_0309110036

#include "export.h"

#include "tree.h"
#include "attribute.h"
#include "bool.h"

#include <expat.h>

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type delcaration for SCEW writers.
 *
 * @ingroup SCEWWriter
 */
typedef struct scew_writer scew_writer;

/**
 * This is the set of functions that are implemented by all SCEW
 * writers. They must not be used directly, but through the common
 * routines to be used with any type of SCEW writer.
 *
 * @ingroup SCEWWriter
 */
typedef struct
{
  /**
   * @see scew_writer_write
   */
  size_t (*write) (scew_writer *, XML_Char const *, size_t);

  /**
   * @see scew_writer_end
   */
  scew_bool (*end) (scew_writer *);

  /**
   * @see scew_writer_error
   */
  scew_bool (*error) (scew_writer *);

  /**
   * @see scew_writer_close
   */
  scew_bool (*close) (scew_writer *);

  /**
   * @see scew_writer_free
   */
  void (*free) (scew_writer *);
} scew_writer_hooks;


/**
 * Creates a new SCEW writer with the given #scew_writer_hooks
 * implementation. This function should be called internally when
 * implementing a new SCEW writer destination. The @a data argument is
 * a reference to some internal data used by the SCEW writer (file
 * stream pointer, current memory buffer pointer, etc.). This data
 * might be later obtained, by the SCEW writer implementation, via
 * #scew_writer_data.
 *
 * @pre hooks != NULL
 *
 * @param hooks the implementation of the new SCEW writer.
 * @param data data to be used by the new SCEW writer. This is usually
 * a reference to a file stream (in case of files) or a memory buffer
 * pointer, etc.
 *
 * @return a new SCEW writer, or NULL if the writer could not be
 * created.
 *
 * @ingroup SCEWWriter
 */
extern SCEW_API scew_writer*
scew_writer_create (scew_writer_hooks const *hooks, void *data);

/**
 * Returns the reference to the internal data structure being used by
 * the given @a writer.
 *
 * @pre writer != NULL
 *
 * @param writer the writer to obtain its internal data for.
 *
 * @return a refrence to the writer's internal data, or NULL if no
 * data was set at creation time.
 *
 * @ingroup SCEWWriter
 */
extern SCEW_API void* scew_writer_data (scew_writer *writer);

/**
 * Writes data from the given memory @a buffer to the specified @a
 * writer. This function will write as many characters (of size
 * XML_Char) as specified by @a char_no. #scew_writer_error and
 * #scew_writer_end need to be consulted to check whether an error is
 * found or the end of the writer is reached, respectively.
 *
 * This function will call the actual @a write function provided by
 * the SCEW writer hooks (#scew_writer_hooks).
 *
 * @pre writer != NULL
 * @pre buffer != NULL
 *
 * @param writer the writer where to send the data.
 * @param buffer the memory buffer from where to read data from.
 * @param char_no the number of characters to write.
 *
 * @return the number of characters successfully written.
 *
 * @ingroup SCEWWriter
 */
extern SCEW_API size_t scew_writer_write (scew_writer *writer,
                                          XML_Char const *buffer,
                                          size_t char_no);

/**
 * Tells whether the given @a writer has reached its end. That is, no
 * more data can be written to the .
 *
 * This function will call the actual @a end function provided by the
 * SCEW writer hooks (#scew_writer_hooks).
 *
 * @pre writer != NULL
 *
 * @param writer the writer to check its end status for.
 *
 * @return true if we are at the end of the writer, false otherwise.
 *
 * @ingroup SCEWWriter
 */
extern SCEW_API scew_bool scew_writer_end (scew_writer *writer);

/**
 * Tells whether an error was found while sending data to the given @a
 * writer.
 *
 * This function will call the actual @a error function provided by
 * the SCEW writer hooks (#scew_writer_hooks).
 *
 * @pre writer != NULL
 *
 * @param writer the writer to check its status for.
 *
 * @return true if we an error was found while sending data to the
 * writer, false otherwise.
 *
 * @ingroup SCEWWriter
 */
extern SCEW_API scew_bool scew_writer_error (scew_writer *writer);

/**
 * Closes the given @a writer. This function will have different
 * effects depending on the SCEW writer type (e.g. it will close the
 * file for file streams). After calling this function, none of the
 * SCEW writer functions should be used, otherwise undefined behavior
 * is expected.
 *
 * This function will call the actual @a close function provided by
 * the SCEW writer hooks (#scew_writer_hooks).
 *
 * @pre writer != NULL
 *
 * @param writer the writer to close.
 *
 * @return true if the writer was successfully closed, false
 * otherwise.
 *
 * @ingroup SCEWWriter
 */
extern SCEW_API scew_bool scew_writer_close (scew_writer *writer);

/**
 * Frees the memory allocated by the given @a writer.
 *
 * This function will call the actual @a free function provided by the
 * SCEW writer hooks (#scew_writer_hooks).
 *
 * @pre writer != NULL
 *
 * @param writer the writer to free.
 *
 * @ingroup SCEWWriter
 */
extern SCEW_API void scew_writer_free (scew_writer *writer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WRITER_H_0309110036 */
