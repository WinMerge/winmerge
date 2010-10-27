/**
 * @file     parser.h
 * @brief    SCEW parser handling routines
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:57
 * @ingroup  SCEWParser, SCEWParserAlloc, SCEWParserLoad, SCEWParserAcc
 *
 * @if copyright
 *
 * Copyright (C) 2002-2009 Aleix Conchillo Flaque
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
 */

/**
 * @defgroup SCEWParser Parser
 *
 * These are the parser functions that allow reading XML documents
 * from a given SCEW writer (file, memory...).
 */

#ifndef PARSER_H_0211250057
#define PARSER_H_0211250057

#include "export.h"

#include "bool.h"
#include "reader.h"
#include "tree.h"

#include <expat.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * This is the type declaration of the SCEW parser.
 *
 * @ingroup SCEWParser
 */
typedef struct scew_parser scew_parser;

/**
 * SCEW parser hooks might be used as notifications to know when XML
 * elements or trees are parsed. Two types of hooks might be
 * registered, one for elements (#scew_parser_set_element_hook) and
 * one for trees (#scew_parser_set_tree_hook) . Whenever the parser
 * loads a complete element (when the end of tag is found) the user
 * will be notified via the registered hook, and the same for XML
 * trees.
 *
 * @param parser the parser that is loading the XML contents.
 * @param data this is the pointer to an SCEW element or tree.
 * @param user_data an optional user data pointer to be used by the
 * hook (might be NULL).
 *
 * @return true if the hook call had no errors, false otherwise.
 *
 * @ingroup SCEWParserLoad
 */
typedef scew_bool (*scew_parser_load_hook) (scew_parser *, void *, void *);


/**
 * @defgroup SCEWParserAlloc Allocation
 * Allocate and free a parser.
 * @ingroup SCEWParser
 */

/**
 * Creates a new parser. A parser is necessary to load XML
 * documents. Note that a parser might be re-used to load multiple XML
 * documents, thus it is not necessary to create a parser for each XML
 * document, but to call #scew_parser_load.
 *
 * @return a new parser, or NULL if parser is not successfully
 * created.
 *
 * @ingroup SCEWParserAlloc
 */
extern SCEW_API scew_parser* scew_parser_create (void);

/**
 * Creates a new parser with namespaces support. Note that Expat
 * expands the resulting elements and attributes, that is, they are
 * formed by the namespace URI, the given namespace @a separator and
 * the local part of the name.
 *
 * @param separator the character between namespace URI and
 * identifier. If 0 is given, no separation is performed.
 *
 * @return a new parser with namespace support, or NULL if parser is
 * not successfully created.
 *
 * @ingroup SCEWParserAlloc
 */
extern SCEW_API scew_parser* scew_parser_namespace_create (XML_Char separator);

/**
 * Frees a @a parser memory structure. If a NULL @a parser is given,
 * this function takes no action.
 *
 * @ingroup SCEWParserAlloc
 */
extern SCEW_API void scew_parser_free (scew_parser *parser);


/**
 * @defgroup SCEWParserLoad Load
 * Load XML documents from different sources.
 * @ingroup SCEWParser
 */

/**
 * Loads an XML tree from the specified @a reader. This will get data
 * from the reader and it will try to parse it. The reader might be of
 * any type. Once the parser loads elements or the complete XML tree,
 * the appropiate registered hooks will be called.
 *
 * Note that this function can only load one XML tree. Concatenated
 * XML documents might be loaded via #scew_parser_load_stream.
 *
 * XML declarations are not mandatory, and if none is found, the SCEW
 * tree will still be created with a default one.
 *
 * At startup, the @a parser is reset (via #scew_parser_reset).
 *
 * @pre parser != NULL
 * @pre reader != NULL
 *
 * @param parser the SCEW @a parser that parses the @a reader
 * contents.
 * @param reader the reader from where to load the XML.
 *
 * @return the XML parsed tree or NULL if an error was found.
 *
 * @ingroup SCEWParserLoad
 */
extern SCEW_API scew_tree* scew_parser_load (scew_parser *parser,
                                             scew_reader *reader);

/**
 * Loads multiple XML trees from the specified stream @a reader. This
 * will get data from the reader and it will try to parse it. The
 * difference between #scew_parser_load and this function is that,
 * here, at some point the reader might not have any more data to be
 * read, so the function will return. Once more data becomes available
 * subsequent calls to this function are needed to continue parsing.
 *
 * Another important difference is that concatenated XML documents are
 * allowed. Once the parser loads elements or complete XML trees, the
 * appropiate registered hooks will be called.
 *
 * It is necessary to register an XML tree hook, otherwise it will not
 * be possible to get a reference to parsed XML trees, causing a
 * memory leak.
 *
 * @pre parser != NULL
 * @pre reader != NULL
 * @pre tree hook registered (#scew_parser_set_tree_hook)
 *
 * @param parser the SCEW @a parser that parses the @a reader
 * contents.
 * @param reader the stream @a reader from where to load XML
 * information.
 *
 * @return true if the parsing is being successful, false if an error
 * is found.
 *
 * @ingroup SCEWParserLoad
 */
extern SCEW_API scew_bool scew_parser_load_stream (scew_parser *parser,
                                                   scew_reader *reader);

/**
 * Resets the given @a parser for further uses. Resetting a parser
 * allows the parser to be re-used. This function is automatically
 * called in #scew_parser_load, but needs to be called when loading
 * streams, as #scew_parser_load_stream does not reset the parser.
 *
 * @pre parser != NULL
 *
 * @param parser the parser to reset.
 *
 * @ingroup SCEWParserLoad
 */
extern SCEW_API void scew_parser_reset (scew_parser *parser);

/**
 * Registers a @a hook to be called once an XML element is
 * successfully parsed. The hook will only be called once the complete
 * element is parsed, that is, when the end tag is found.
 *
 * This hook might be useful as a notification mechanism when parsing
 * big XML documents.
 *
 * Note that no modification or deletion should be performed on the
 * elements as they might still be needed by the parser.
 *
 * @pre parser != NULL
 * @pre hook != NULL
 *
 * @param parser the parser that is loading the XML contents.
 * @param hook this is the hook to be called once an XML element is
 * parsed.
 * @param user_data an optional user data pointer to be used by the
 * hook (might be NULL).
 *
 * @ingroup SCEWParserLoad
 */
extern SCEW_API void scew_parser_set_element_hook (scew_parser *parser,
                                                   scew_parser_load_hook hook,
                                                   void *user_data);

/**
 * Registers a @a hook to be called once an XML tree is successfully
 * parsed. The hook will only be called once the complete XML tree is
 * parsed.
 *
 * This hook is necessary when loading streams (via
 * #scew_parser_load_stream), as no XML tree is returned there.
 *
 * @pre parser != NULL
 * @pre hook != NULL
 *
 * @param parser the parser that is loading the XML contents.
 * @param hook this is the hook to be called once an XML tree is
 * parsed.
 * @param user_data an optional user data pointer to be used by the
 * hook (might be NULL).
 *
 * @ingroup SCEWParserLoad
 */
extern SCEW_API void scew_parser_set_tree_hook (scew_parser *parser,
                                                scew_parser_load_hook hook,
                                                void *user_data);

/**
 * Tells the @a parser how to treat white spaces. The default is to
 * ignore heading and trailing white spaces.
 *
 * There is a new section in XML specification which talks about how
 * to handle white spaces in XML. One can set an optional attribtue to
 * an element which is called @a xml:space, and it can be set to @a
 * default or @a preserve, and it inherits its value from parent
 * elements.
 *
 * - @b preserve: leave white spaces as their are found.
 * - @b default: white spaces are handled by the XML processor (Expat in
 *  our case) the way it wants to.
 *
 * This function gives the possibility to change the XML processor
 * behaviour.
 *
 * @param parser the parser to set the option to.
 * @param ignore whether the @a parser should ignore white spaces,
 * false otherwise.
 *
 * @ingroup SCEWParserLoad
 */
extern SCEW_API void scew_parser_ignore_whitespaces (scew_parser *parser,
                                                     scew_bool ignore);


/**
 * @defgroup SCEWParserAcc Accessors
 * Obtain information from parser.
 * @ingroup SCEWParser
 */

/**
 * Returns the internal Expat parser being used by the given SCEW @a
 * parser. Probably some extra low-level Expat functions need to be
 * called by the user. This function gives access to the Expat parser
 * so it is possible to call these functions.
 *
 * Note that if the Expat parser event handling routines are modified,
 * SCEW will not be able to load XML documents.
 *
 * @ingroup SCEWParserAcc
 */
extern SCEW_API XML_Parser scew_parser_expat (scew_parser *parser);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PARSER_H_0211250057 */
