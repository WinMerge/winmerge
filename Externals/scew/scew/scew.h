/**
 * @file     scew.h
 * @brief    SCEW main header file
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 01:34
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
 */

#ifndef SCEW_H_0211250134
#define SCEW_H_0211250134

#include "export.h"

#include "attribute.h"
#include "bool.h"
#include "element.h"
#include "error.h"
#include "list.h"
#include "parser.h"
#include "printer.h"
#include "reader.h"
#include "reader_buffer.h"
#include "reader_file.h"
#include "str.h"
#include "tree.h"
#include "writer.h"
#include "writer_buffer.h"
#include "writer_file.h"

/* Automatically include the correct library on Windows. */
#if defined (_MSC_VER) && defined(XML_STATIC)

#ifdef XML_UNICODE_WCHAR_T
#define SCEW_LIB_W "w"
#else
#define SCEW_LIB_W
#endif /* XML_UNICODE_WCHAR_T */

#ifdef _DEBUG
#define SCEW_LIB_D "d"
#else
#define SCEW_LIB_D
#endif /* _DEBUG */

/**
 * By default SCEW uses "Multi-threaded (/MT)" run-time library. See
 * 'win32/README' if you wish to use a different run-time library.
 */
#define SCEW_LIB_M "MT"

#pragma comment (lib, "libscew" SCEW_LIB_W SCEW_LIB_D SCEW_LIB_M ".lib")

#endif /* _MSC_VER && XML_STATIC */

#endif /* SCEW_H_0211250134 */
