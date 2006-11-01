/**
 *
 * @file     xhandler.h
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 00:23
 * @brief    SCEW Expat handlers
 *
 * $Id: xhandler.h,v 1.1 2004/01/28 00:43:21 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2002, 2003 Aleix Conchillo Flaque
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * @endif
 */


#ifndef XHANDLER_H_ALEIX0211250023
#define XHANDLER_H_ALEIX0211250023

#include <expat.h>

/* Expat callback for XML declaration. */
void
xmldecl_handler(void* data, XML_Char const* version, XML_Char const* encoding,
                int standalone);

/* Expat callback for starting elements. */
void
start_handler(void* data, XML_Char const* elem, XML_Char const** attr);

/* Expat callback for ending elements. */
void
end_handler(void* data, XML_Char const* elem);

/* Expat callback for element contents. */
void
char_handler(void* data, XML_Char const* s, int len);

#endif /* XHANDLER_H_ALEIX0211250023 */
