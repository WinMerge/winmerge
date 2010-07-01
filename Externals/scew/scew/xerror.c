/**
 * @file     xerror.c
 * @brief    xerror.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon May 05, 2003 10:41
 *
 * @if copyright
 *
 * Copyright (C) 2003-2009 Aleix Conchillo Flaque
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "xerror.h"

/* Define a single threading macro common for all platforms */
#ifndef _MT
#ifndef HAVE_LIBPTHREAD
#define SINGLE_THREADED
#endif /* HAVE_LIBPTHREAD */
#endif /* _MT */

#ifdef SINGLE_THREADED

/* Single-threaded version */

static scew_error last_error = scew_error_none;

void
scew_error_set_last_error_ (scew_error code)
{
  last_error = code;
}

scew_error
scew_error_last_error_ (void)
{
  return last_error;
}

#else /* SINGLE_THREADED */


/* Multi-threaded versions */

#ifdef _MSC_VER

/* Visual C++ multi-thread version */

/**
 * Note: This code isn't 100% thread safe without an initializer
 * called from a single-thread first. The current problem is the small
 * chance that another thread could enter the if() statement which
 * initializes the TLS variable before the current thread calls
 * TlsAlloc(). This would result in TlsAlloc() being called twice
 * losing the first TLS index and possibly the first thread's error
 * value. However with the current API, this is the best we can do.
 */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

static DWORD last_error_key_ = TLS_OUT_OF_INDEXES;

void
scew_error_set_last_error_ (scew_error code)
{
  if (TLS_OUT_OF_INDEXES == last_error_key_)
    {
      last_error_key_ = TlsAlloc ();
    }
  TlsSetValue (last_error_key_, (LPVOID) code);
}

scew_error
scew_error_last_error_ (void)
{
  if (TLS_OUT_OF_INDEXES == last_error_key_)
    {
      last_error_key_ = TlsAlloc ();
      TlsSetValue (last_error_key_, (LPVOID) scew_error_none);
    }
  return (scew_error) TlsGetValue (last_error_key_);
}

#else /* _MSC_VER */


/* pthread multi-threaded version */

#include <pthread.h>

static pthread_key_t key_error_;
static pthread_once_t key_once_ = PTHREAD_ONCE_INIT;

static void
create_keys_ (void)
{
  scew_error* error = NULL;

  pthread_key_create (&key_error_, free);

  error = (scew_error *) malloc (sizeof (scew_error));
  *error = scew_error_none;
  pthread_setspecific (key_error_, error);
}

void
scew_error_set_last_error_ (scew_error code)
{
  scew_error *error = NULL;

  /* Initialize error code per thread. */
  pthread_once (&key_once_, create_keys_);

  error = (scew_error *) pthread_getspecific (key_error_);
  *error = code;
}

scew_error
scew_error_last_error_ (void)
{
  scew_error *error = NULL;

  /* Initialize error code per thread. */
  pthread_once (&key_once_, create_keys_);

  error = (scew_error*) pthread_getspecific (key_error_);
  if (NULL == error)
    {
      return scew_error_none;
    }

  return *error;
}

#endif /* _MSC_VER */

#endif /* SINGLE_THREADED */
