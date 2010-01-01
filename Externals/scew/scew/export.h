/**
 * @file     export.h
 * @brief    SCEW shared library support
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Fri Sep 04, 2009 00:14
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
 */

#ifndef EXPORT_H_0909040014
#define EXPORT_H_0909040014

#if !defined (_MSC_VER) && defined (XML_UNICODE_WCHAR_T)
#error UTF-16 support is only available in Windows platforms
#endif

#if defined (_MSC_VER) || defined (__CYGWIN__)

  #define SCEW_DLL_IMPORT __declspec(dllimport)
  #define SCEW_DLL_EXPORT __declspec(dllexport)
  #define SCEW_DLL_LOCAL

#else

  #if __GNUC__ >= 4
    #define SCEW_DLL_IMPORT __attribute__ ((visibility("default")))
    #define SCEW_DLL_EXPORT __attribute__ ((visibility("default")))
    #define SCEW_DLL_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define SCEW_DLL_IMPORT
    #define SCEW_DLL_EXPORT
    #define SCEW_DLL_LOCAL
  #endif /* __GNUC__ >= 4 */

  #ifdef PIC /* This will tell us if we are building a shared library. */
    #define SCEW_DLL
  #endif /* PIC */

#endif /* _MSC_VER || __CYGWIN__ */

#ifdef SCEW_DLL /* Defined if SCEW is compiled as a DLL. */

  #ifdef DLL_EXPORTS /* Defined if we are building the SCEW DLL. */
    #define SCEW_API SCEW_DLL_EXPORT
  #else
    #define SCEW_API SCEW_DLL_IMPORT
  #endif /* DLL_EXPORTS */

  #define SCEW_LOCAL SCEW_DLL_LOCAL

#else /* SCEW_DLL is not defined: this means SCEW is a static lib. */

  #define SCEW_API
  #define SCEW_LOCAL

#endif /* SCEW_DLL */

#endif /* EXPORT_H_0909040014 */
