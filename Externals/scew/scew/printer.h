/**
 * @file     printer.h
 * @brief    SCEW printer routines for XML output
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Fri Jan 16, 2009 22:34
 * @ingroup  SCEWPrinter, SCEWPrinterAlloc, SCEWPrinterProp, SCEWPrinterOutput
 *
 * @if copyright
 *
 * Copyright (C) 2009 Aleix Conchillo Flaque
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
 * @defgroup SCEWIO Input/Output
 *
 * The SCEW I/O system is based on SCEW @ref SCEWReader, @ref
 * SCEWWriter and @ref SCEWPrinter. A reader is a common mechanism to
 * load data from different sources (files, memory, ...). A common
 * mechanism means that the functions to read data, for example, from
 * a file or from a memory buffer, are the same. SCEW writers follows
 * the same idea behind the readers, that is, common routines are used
 * to write data to any kind of sources (files, memory, ...).
 *
 * It is worth mentioning that a user might implement its own SCEW
 * readers and writers.
 *
 * The SCEW printer provides routines to write SCEW XML data (trees,
 * elements and attributes) to a SCEW writer.
 */

/**
 * @defgroup SCEWPrinter Printer
 *
 * A SCEW printer provides a set of routines to send XML data to a
 * given SCEW writer.
 *
 * @ingroup SCEWIO
 */

#ifndef PRINTER_H_0901162234
#define PRINTER_H_0901162234

#include "export.h"

#include "writer.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type delcaration for the SCEW printer.
 *
 * @ingroup SCEWPrinter
 */
typedef struct scew_printer scew_printer;


/**
 * @defgroup SCEWPrinterAlloc Allocation
 * Allocate and free printers.
 * @ingroup SCEWPrinter
 */

/**
 * Creates a new SCEW printer that will use the given @a writer by
 * default. The SCEW writer will be used by the @ref SCEWPrinterOutput
 * calls. It is possible to re-use a SCEW printer by setting a new
 * writer via #scew_printer_set_writer.
 *
 * @pre writer != NULL
 *
 * @param writer the SCEW writer to be used by the putput functions.
 *
 * @return a new SCEW printer or NULL if the printer could not be
 * created.
 *
 * @ingroup SCEWPrinterAlloc
 */
extern SCEW_API scew_printer* scew_printer_create (scew_writer *writer);

/**
 * Frees the given SCEW @a printer. This will not free the writer
 * being used by the printer.
 *
 * @param printer the SCEW printer to free.
 *
 * @ingroup SCEWPrinterAlloc
 */
extern SCEW_API void scew_printer_free (scew_printer *printer);


/**
 * @defgroup SCEWPrinterProp Properties
 * Set printer properties.
 * @ingroup SCEWPrinter
 */

/**
 * Tells whether the output sent to the given SCEW @a printer should be
 * @a indented or not.
 *
 * @pre printer != NULL
 *
 * @param printer the SCEW printer to change its indentation for.
 * @param indented true if the output should be indented, false
 * otherwise.
 *
 * @ingroup SCEWPrinterProp
 */
extern SCEW_API void scew_printer_set_indented (scew_printer *printer,
                                                scew_bool indented);

/**
 * Sets the number of @a spaces to use when indenting output for the
 * given SCEW @a printer.
 *
 * @pre printer != NULL
 *
 * @param printer the SCEW printer to change its indentation spaces for.
 * @param spaces the number of spaces to use for indentation.
 *
 * @ingroup SCEWPrinterProp
 */
extern SCEW_API void scew_printer_set_indentation (scew_printer *printer,
                                                   unsigned int spaces);


/**
 * @defgroup SCEWPrinterOutput Output
 * A set of routines to print XML data.
 * @ingroup SCEWPrinter
 */

/**
 * Sets the given SCEW @a writer to the specified @a printer. After
 * this call, subsequent calls to output functions will use the given
 * writer internally. This means that the printer can be used to
 * writer to a file or memory buffer indistinctly.
 *
 * @param printer the SCEW printer to change its writer for.
 * @param writer the SCEW writer to be used in next output calls.
 *
 * @return the old SCEW writer.
 *
 * @ingroup SCEWPrinterOutput
 */
extern SCEW_API scew_writer* scew_printer_set_writer (scew_printer *printer,
                                                      scew_writer *writer);
/**
 * Prints the given SCEW @a tree to the specified @a printer. This
 * will print the XML declaration, the preamble and the root element
 * with all its children.
 *
 * @pre printer != NULL
 * @pre tree != NULL
 *
 * @param printer the printer to be used for printing data.
 * @param tree the SCEW tree to print.
 *
 * @ingroup SCEWPrinterOutput
 */
extern SCEW_API scew_bool scew_printer_print_tree (scew_printer *printer,
                                                   scew_tree const *tree);

/**
 * Prints the given SCEW @a element to the specified @a printer. This
 * will print the element (with its attributes) and all its children
 * recursively.
 *
 * @pre printer != NULL
 * @pre element != NULL
 *
 * @param printer the printer to be used for printing data.
 * @param element the SCEW element to print.
 *
 * @ingroup SCEWPrinterOutput
 */
extern SCEW_API scew_bool
scew_printer_print_element (scew_printer *printer,
                            scew_element const *element);

/**
 * Prints the given SCEW @a element children to the specified @a
 * printer. This will print the element children recursively.
 *
 * @pre printer != NULL
 * @pre element != NULL
 *
 * @param printer the printer to be used for printing data.
 * @param element the SCEW element to print its children for. The
 * element itself is not printed.
 *
 * @ingroup SCEWPrinterOutput
 */
extern SCEW_API scew_bool
scew_printer_print_element_children (scew_printer *printer,
                                     scew_element const *element);

/**
 * Prints the given SCEW @a element attributes to the specified @a
 * printer. This will print the list of the element attributes. Note
 * that this will note generate any valid XML data, but might be
 * useful in some cases.
 *
 * @pre printer != NULL
 * @pre element != NULL
 *
 * @param printer the printer to be used for printing data.
 * @param element the SCEW element to print its attributes for.
 *
 @ingroup SCEWPrinterOutput
 */
extern SCEW_API scew_bool
scew_printer_print_element_attributes (scew_printer *printer,
                                       scew_element const *element);

/**
 * Prints the given SCEW @a attribute to the specified @a
 * printer. Note that this will note generate any valid XML data, but
 * might be useful in some cases.
 *
 * @pre printer != NULL
 * @pre attribute != NULL
 *
 * @param printer the printer to be used for printing data.
 * @param attribute the SCEW attribute to print.
 *
 * @ingroup SCEWPrinterOutput
 */
extern SCEW_API scew_bool
scew_printer_print_attribute (scew_printer *printer,
                              scew_attribute const *attribute);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PRINTER_H_0901162234 */
