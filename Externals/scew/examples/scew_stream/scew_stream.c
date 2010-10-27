/**
 * @file     scew_stream.c
 * @brief    SCEW usage example
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun May 23, 2004 20:58
 *
 * @if copyright
 *
 * Copyright (C) 2004-2009 Aleix Conchillo Flaque
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
 *
 * This example shows the usage of the SCEW API. It loads an XML file
 * using a one byte buffer (any size can be used). Then, the function
 * scew_parser_load_stream is called until the end of file is
 * reached. While elements are encountered a callback function,
 * previously set, is called each time a complete element is read.
 *
 * Example 1:
 *
 *   <command>command_1</command>                <-- element hook
 *   <command><option>option2</option></command> <-- element, element hook
 *   <command>command_3</command>                <-- element hook
 *
 * Example 2:
 *
 *   <commands>
 *     <command>command_1</command>              <-- element hook
 *     <command>command_2</command>              <-- element hook
 *   </commands>                                 <-- tree, element hook
 */

#include <scew/scew.h>

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
#include <fcntl.h>
#include <io.h>
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

#include <stdio.h>

static scew_printer *stdout_printer_ = NULL;
static scew_writer *stdout_writer_ = NULL;

static scew_bool
tree_hook_ (scew_parser *parser, void *tree, void *user_data)
{
  scew_printf (_XT("*** SCEW stream tree loaded!\n\n"));

  scew_printer_print_tree (stdout_printer_, (scew_tree *) tree);

  scew_printf (_XT("\n----------------------------------\n"));

  /**
   * We free the tree here as we are not going to using it. We should
   * save the pointer if we want to play with it.
   */
  scew_tree_free ((scew_tree *) tree);

  return SCEW_TRUE;
}

static scew_bool
element_hook_ (scew_parser *parser, void *element, void *user_data)
{
  scew_printf (_XT("*** SCEW stream element loaded!\n"));

  return SCEW_TRUE;
}

int
main (int argc, char *argv[])
{
  scew_reader *reader = NULL;
  scew_parser *parser = NULL;

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
  /* Change stdout to Unicode before writing anything. */
  _setmode(_fileno(stdout), _O_U16TEXT);
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

  if (argc < 2)
    {
      scew_printf (_XT("Usage: scew_stream file.xml\n"));
      return EXIT_FAILURE;
    }

  /* Create a writer for the standard output. */
  stdout_writer_ = scew_writer_fp_create (stdout);

  /* Create a writer for the standard output. */
  stdout_printer_ = scew_printer_create (stdout_writer_);

  /* Creates an SCEW parser. This is the first function to call. */
  parser = scew_parser_create ();

  scew_parser_ignore_whitespaces (parser, SCEW_TRUE);

  /* Loads an XML file. */
  reader = scew_reader_file_create (argv[1]);
  if (reader == NULL)
    {
      scew_error code = scew_error_code ();
      scew_printf (_XT("Unable to load file (error #%d: %s)\n"),
                   code, scew_error_string (code));
    }

  /* Setup element and tree hooks. */
  scew_parser_set_tree_hook (parser, tree_hook_, NULL);
  scew_parser_set_element_hook (parser, element_hook_, NULL);
  if (!scew_parser_load_stream (parser, reader))
    {
      scew_error code = scew_error_code ();
      scew_printf (_XT("Unable to load file (error #%d: %s)\n"),
                   code, scew_error_string (code));
      if (code == scew_error_expat)
        {
          enum XML_Error expat_code = scew_error_expat_code (parser);
          scew_printf (_XT("Expat error #%d (line %d, column %d): %s\n"),
                       expat_code,
                       scew_error_expat_line (parser),
                       scew_error_expat_column (parser),
                       scew_error_expat_string (expat_code));
        }

      /* Frees the SCEW parser, printer, reader and writer. */
      scew_reader_free (reader);
      scew_parser_free (parser);
      scew_writer_free (stdout_writer_);
      scew_printer_free (stdout_printer_);

      return EXIT_FAILURE;
    }

  /* Frees the SCEW parser, printer, reader and writer. */
  scew_reader_free (reader);
  scew_parser_free (parser);
  scew_writer_free (stdout_writer_);
  scew_printer_free (stdout_printer_);

  return 0;
}
