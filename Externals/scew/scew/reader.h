/**
 * @file     reader.h
 * @brief    SCEW reader common functions
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Nov 23, 2008 13:36
 * @ingroup  SCEWReader
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
 * @defgroup SCEWReader Readers
 *
 * Read data from different sources: files, memory, etc.
 *
 * SCEW readers provide a common mechanism to read data from different
 * sources. This is done by implementing the set of functions declared
 * in #scew_reader_hooks. A user might create new SCEW readers by
 * implementing those functions.
 *
 * Once a SCEW reader is created, functions in this section should be
 * used no matter the reader type.
 *
 * @ingroup SCEWIO
 */

#ifndef READER_H_0811231336
#define READER_H_0811231336

#include "export.h"

#include "bool.h"

#include <expat.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type delcaration for SCEW readers.
 *
 * @ingroup SCEWReader
 */
typedef struct scew_reader scew_reader;

/**
 * This is the set of functions that are implemented by all SCEW
 * reader sources. They must not be used directly, but through the
 * common routines to be used with any type of SCEW reader.
 *
 * @ingroup SCEWReader
 */
typedef struct
{
  /**
   * @see scew_reader_read
   */
  size_t (*read) (scew_reader *, XML_Char *, size_t);

  /**
   * @see scew_reader_end
   */
  scew_bool (*end) (scew_reader *);

  /**
   * @see scew_reader_error
   */
  scew_bool (*error) (scew_reader *);

  /**
   * @see scew_reader_close
   */
  scew_bool (*close) (scew_reader *);

  /**
   * @see scew_reader_free
   */
  void (*free) (scew_reader *);
} scew_reader_hooks;

/**
 * Creates a new SCEW reader with the given #scew_reader_hooks
 * implementation. This function should be called internally when
 * implementing a new SCEW reader source. The @a data argument is a
 * reference to some internal data used by the SCEW reader (file
 * stream pointer, current memory buffer pointer, etc.). This data
 * might be later obtained, by the SCEW reader implementation, via
 * #scew_reader_data.
 *
 * @pre hooks != NULL
 *
 * @param hooks the implementation of the new SCEW reader source.
 * @param data data to be used by the new SCEW reader. This is usually
 * a reference to a file stream (in case of files) or a memory buffer
 * pointer, etc.
 *
 * @return a new SCEW reader, or NULL if the reader could not be
 * created.
 *
 * @ingroup SCEWReader
 */
extern SCEW_API scew_reader*
scew_reader_create (scew_reader_hooks const *hooks, void *data);

/**
 * Returns the reference to the internal data structure being used by
 * the given @a reader.
 *
 * @pre reader != NULL
 *
 * @param reader the reader to obtain its internal data for.
 *
 * @return a refrence to the reader's internal data, or NULL if no
 * data was set at creation time.
 *
 * @ingroup SCEWReader
 */
extern SCEW_API void* scew_reader_data (scew_reader *reader);

/**
 * Reads data from the given @a reader in store it in the specified @a
 * buffer. This function will read as many characters (of size
 * XML_Char) as specified by @a char_no. #scew_reader_error and
 * #scew_reader_end need to be consulted to check whether an error is
 * found or the end of the reader is reached, respectively.
 *
 * This function will call the actual @a read function provided by the
 * SCEW reader hooks (#scew_reader_hooks).
 *
 * @pre reader != NULL
 * @pre buffer != NULL
 *
 * @param reader the reader from where to read data from.
 * @param buffer the memory buffer where to store data.
 * @param char_no the number of characters to read.
 *
 * @return the number of characters successfully read.
 *
 * @ingroup SCEWReader
 */
extern SCEW_API size_t scew_reader_read (scew_reader *reader,
                                         XML_Char *buffer,
                                         size_t char_no);

/**
 * Tells whether the given @a reader has reached its end. That is, no
 * more data is available for reading.
 *
 * This function will call the actual @a end function provided by the
 * SCEW reader hooks (#scew_reader_hooks).
 *
 * @pre reader != NULL
 *
 * @param reader the reader to check its end status for.
 *
 * @return true if we are at the end of the reader, false otherwise.
 *
 * @ingroup SCEWReader
 */
extern SCEW_API scew_bool scew_reader_end (scew_reader *reader);

/**
 * Tells whether an error was found while reading from the given @a
 * reader.
 *
 * This function will call the actual @a error function provided by
 * the SCEW reader hooks (#scew_reader_hooks).
 *
 * @pre reader != NULL
 *
 * @param reader the reader to check its status for.
 *
 * @return true if we an error was found while reading data from the
 * reader, false otherwise.
 *
 * @ingroup SCEWReader
 */
extern SCEW_API scew_bool scew_reader_error (scew_reader *reader);

/**
 * Closes the given @a reader. This function will have different
 * effects depending on the SCEW reader type (e.g. it will close the
 * file for file streams). After calling this function, none of the
 * SCEW reader functions should be used, otherwise undefined behavior
 * is expected.
 *
 * This function will call the actual @a close function provided by
 * the SCEW reader hooks (#scew_reader_hooks).
 *
 * @pre reader != NULL
 *
 * @param reader the reader to close.
 *
 * @return true if the reader was successfully closed, false
 * otherwise.
 *
 * @ingroup SCEWReader
 */
extern SCEW_API scew_bool scew_reader_close (scew_reader *reader);

/**
 * Frees the memory allocated by the given @a reader.
 *
 * This function will call the actual @a free function provided by the
 * SCEW reader hooks (#scew_reader_hooks).
 *
 * @param reader the reader to free.
 *
 * @ingroup SCEWReader
 */
extern SCEW_API void scew_reader_free (scew_reader *reader);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* READER_H_0811231336 */
