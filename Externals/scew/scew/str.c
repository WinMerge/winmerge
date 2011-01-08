/**
 * @file     str.c
 * @brief    str.h implementation
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Mon Nov 25, 2002 23:32
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

#include "str.h"

#include "xerror.h"
#include "str.h"

#include <assert.h>


/* Private */

#define CHR_LT_   _XT('<')
#define CHR_GT_   _XT('>')
#define CHR_AMP_  _XT('&')
#define CHR_APOS_ _XT('\'')
#define CHR_QUOT_ _XT('"')

#define XML_LT_   _XT("&lt;")
#define XML_GT_   _XT("&gt;")
#define XML_AMP_  _XT("&amp;")
#define XML_APOS_ _XT("&apos;")
#define XML_QUOT_ _XT("&quot;")

enum
  {
    LT_SIZE_ = 4,               /**< Size of &lt; */
    GT_SIZE_ = 4,               /**< Size of &gt; */
    AMP_SIZE_ = 5,              /**< Size of &amp; */
    APOS_SIZE_ = 6,             /**< Size of &apos; */
    QUOT_SIZE_ = 6              /**< Size of &quot; */
  };


/* Public */

int
scew_strcmp (XML_Char const *s1, XML_Char const *s2)
{
  int result = ((s1 == NULL) && (s2 == NULL)) ? 0 : 1;

  if (result != 0)
    {
      /* We don't know if s1 or s2 are NULL. */
      result = 0;

      if (s1 == NULL)
        {
          /* s1 is lesser than s2. */
          result = -1;
        }
      if (s2 == NULL)
        {
          /* s1 is greater than s2. */
          result = 1;
        }

      /* s1 and s2 are *not* NULL, so we need to compare them. */
      if (result == 0)
        {
#ifdef XML_UNICODE_WCHAR_T
          result = wcscmp (s1, s2);
#else
          result = strcmp (s1, s2);
#endif /* XML_UNICODE_WCHAR_T */
        }
    }

  return result;
}

XML_Char*
scew_strdup (XML_Char const *src)
{
  XML_Char *out = NULL;

  if (src != NULL)
    {
      size_t len = scew_strlen (src);
      out = calloc (len + 1, sizeof (XML_Char));
      scew_strcpy (out, src);
    }

  return out;
}

void
scew_strtrim (XML_Char *src)
{
  size_t start = 0;
  size_t end = 0;
  size_t total = 0;

  assert (src != NULL);

  end = scew_strlen (src);

  /* Strip trailing whitespace. */
  while ((end > 0) && scew_isspace (src[end - 1]))
    {
      src[--end] = _XT('\0');
    }

  /* Strip leading whitespace. */
  start = scew_strspn (src, _XT(" \n\r\t\v"));
  total = end - start;
  scew_memmove (src, &src[start], total);
  src[total] = _XT('\0');
}

scew_bool
scew_isempty (XML_Char const *src)
{
  scew_bool empty = SCEW_TRUE;
  XML_Char *p = (XML_Char *) src;

  assert (src != NULL);

  while (empty && (*p != _XT('\0')))
    {
      empty = (scew_isspace (*p) != 0);
      p += 1;
    }

  return empty;
}

XML_Char*
scew_strescape (XML_Char const *src)
{
  XML_Char *p = (XML_Char *) src;
  XML_Char *escaped = NULL;
  unsigned int len = 0;

  assert (src != NULL);

  /* We first need to calculate the size of the new escaped string. */
  while (*p != _XT('\0'))
    {
      switch (*p)
        {
        case CHR_LT_:
          len += LT_SIZE_;
          break;
        case CHR_GT_:
          len += GT_SIZE_;
          break;
        case CHR_AMP_:
          len += AMP_SIZE_;
          break;
        case CHR_APOS_:
          len += APOS_SIZE_;
          break;
        case CHR_QUOT_:
          len += QUOT_SIZE_;
          break;
        default:
          len += 1;
          break;
        }
      p += 1;
    }

  /* Allocate new string (if necessary). */
  escaped = calloc (len + 1, sizeof (XML_Char));

  /* Append characters to new string, escaping the needed ones. */
  p = (XML_Char *) src;
  len = 0;
  while (*p != _XT('\0'))
    {
      switch (*p)
        {
        case CHR_LT_:
          scew_memcpy (&escaped[len], XML_LT_, LT_SIZE_);
          len += LT_SIZE_;
          break;
        case CHR_GT_:
          scew_memcpy (&escaped[len], XML_GT_, GT_SIZE_);
          len += GT_SIZE_;
          break;
        case CHR_AMP_:
          scew_memcpy (&escaped[len], XML_AMP_, AMP_SIZE_);
          len += AMP_SIZE_;
          break;
        case CHR_APOS_:
          scew_memcpy (&escaped[len], XML_APOS_, APOS_SIZE_);
          len += APOS_SIZE_;
          break;
        case CHR_QUOT_:
          scew_memcpy (&escaped[len], XML_QUOT_, QUOT_SIZE_);
          len += QUOT_SIZE_;
          break;
        default:
          escaped[len] = *p;
          len += 1;
          break;
        }
      p += 1;
    }

  return escaped;
}
