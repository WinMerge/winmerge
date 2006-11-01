/**
 *
 * @file     xerror.c
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon May 05, 2003 10:41
 * @brief    Internal error functions
 *
 * $Id: xerror.c,v 1.2 2004/01/29 00:13:42 aleix Exp $
 *
 * @if copyright
 *
 * Copyright (C) 2003, 2004 Aleix Conchillo Flaque
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

#include "xerror.h"

#ifndef _MT
/* single-threaded version */

static scew_error last_error = scew_error_none;

void
set_last_error(scew_error code)
{
    last_error = code;
}

scew_error
get_last_error()
{
    return last_error;
}

#else /* _MT */
/* multi-threaded versions */

#ifdef _MSC_VER
/* Microsoft Visual C++ multi-thread version */

/**
 * Note: This code isn't 100% thread safe without an initializer called
 * from a single-thread first. The current problem is the small chance
 * that another thread could enter the if() statement which initializes
 * the TLS variable before the current thread calls TlsAlloc(). This
 * would result in TlsAlloc() being called twice losing the first TLS
 * index and possibly the first thread's error value. However with the
 * current API, this is the best we can do.
 */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

static DWORD last_error_key = TLS_OUT_OF_INDEXES;

void
set_last_error(scew_error code)
{
    if (last_error_key == TLS_OUT_OF_INDEXES)
    {
        last_error_key = TlsAlloc();
    }
    TlsSetValue(last_error_key, (LPVOID) code);
}

scew_error
get_last_error()
{
    if (last_error_key == TLS_OUT_OF_INDEXES)
    {
        last_error_key = TlsAlloc();
        TlsSetValue(last_error_key, (LPVOID) scew_error_none);
    }
    return (scew_error) TlsGetValue(last_error_key);
}

#else /* _MSC_VER */
/* pthread multi-threaded version */

#include <pthread.h>

static pthread_key_t key_error;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

void
create_keys()
{
    scew_error* code = NULL;

    pthread_key_create(&key_error, free);

    code = (scew_error*) malloc(sizeof(scew_error));
    *code = scew_error_none;
    pthread_setspecific(key_error, code);
}

void
set_last_error(scew_error code)
{
    scew_error* old_code = NULL;
    scew_error* new_code = NULL;

    /* Initialize error code per thread */
    pthread_once(&key_once, create_keys);

    old_code = (scew_error*) pthread_getspecific(key_error);
    new_code = (scew_error*) malloc(sizeof(scew_error));
    *new_code = code;
    free(old_code);
    pthread_setspecific(key_error, new_code);
}

scew_error
get_last_error()
{
    scew_error* code = NULL;

    /* Initialize error code per thread */
    pthread_once(&key_once, create_keys);

    code = (scew_error*) pthread_getspecific(key_error);
    if (code == NULL)
    {
        return scew_error_none;
    }
    return *code;
}

#endif /* _MSC_VER */

#endif /* _MT */
