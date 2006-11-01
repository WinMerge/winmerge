/**
 *
 * @file     scew.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 01:34
 * @brief    SCEW main header file
 *
 * $Id: scew.h,v 1.1 2004/01/28 00:43:21 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2002, 2003, 2004 Aleix Conchillo Flaque
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * @endif
 */


#ifndef SCEW_H_ALEIX0211250134
#define SCEW_H_ALEIX0211250134

#include "attribute.h"
#include "element.h"
#include "error.h"
#include "parser.h"
#include "str.h"
#include "tree.h"
#include "writer.h"

/* automatically include the correct library on windows */
#ifdef _WIN32

# ifdef XML_UNICODE_WCHAR_T
#  define SCEW_LIB_U    "u"
# else
#  define SCEW_LIB_U
# endif /* XML_UNICODE_WCHAR_T */

# ifdef XML_STATIC
#  define SCEW_LIB_S    "s"
# else
#  define SCEW_LIB_S
# endif /* XML_STATIC */

# ifdef _DEBUG
#  define SCEW_LIB_D    "d"
# else
#  define SCEW_LIB_D
# endif /* _DEBUG */

# if defined(SCEW_LIB_U) || defined(SCEW_LIB_S) || defined(SCEW_LIB_D)
# pragma comment( lib, "scew_" SCEW_LIB_U SCEW_LIB_S SCEW_LIB_D ".lib" )
# else
# pragma comment( lib, "scew.lib" )
# endif

#endif /* _WIN32 */

#endif /* SCEW_H_ALEIX0211250134 */
