///////////////////////////////////////////////////////////////////////////
//  File:    fpattern.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl (I'm sorry, author unknown ...)
//  E-mail:     prantl@ff.cuni.cz
//
//  File-like string pattern matching routines
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "fpattern.h"

#if TEST
#define SUB     _T('~')
#else
#define SUB     FPAT_CLOSP
#endif

#ifndef DELIM
#define DELIM       0
#endif

#define DEL     FPAT_DEL

#ifdef __UNIX__
#define DEL2            FPAT_DEL
#define QUOTE           FPAT_QUOTE
#define lowercase( c )  ( c )
#else
#define DEL2            FPAT_DEL2
#define QUOTE           FPAT_QUOTE2
#define lowercase( c )  tc::totlower( c )
#endif

/*-----------------------------------------------------------------------------
 * fpattern_isvalid()
 *  Checks that filename pattern 'pat' is a well-formed pattern.
 *
 * Returns
 *  1 ( true ) if 'pat' is a valid filename pattern, otherwise 0 ( false ).
 *
 * Caveats
 *  If 'pat' is null, 0 ( false ) is returned.
 *
 *  If 'pat' is empty ( "" ), 1 ( true ) is returned, and it is considered a
 *  valid ( but degenerate ) pattern ( the only filename it matches is the
 *  empty ( "" ) string ).
 */

int
fpattern_isvalid (const tchar_t* pat)
{
  int len;

  /* Check args */
  if (pat == nullptr)
    return (false);

  /* Verify that the pattern is valid */
  for (len = 0; pat[len] != _T('\0'); len++)
    {
      switch (pat[len])
        {
        case FPAT_SET_L:
          /* Char set */
          len++;
          if (pat[len] == FPAT_SET_NOT)
            len++;              /* Set negation */

          while (pat[len] != FPAT_SET_R)
            {
              if (pat[len] == QUOTE)
                len++;          /* Quoted char */
              if (pat[len] == _T('\0'))
                return (false); /* Missing closing bracket */
              len++;

              if (pat[len] == FPAT_SET_THRU)
                {
                  /* Char range */
                  len++;
                  if (pat[len] == QUOTE)
                    len++;      /* Quoted char */
                  if (pat[len] == _T('\0'))
                    return (false);     /* Missing closing bracket */
                  len++;
                }

              if (pat[len] == _T('\0'))
                return (false); /* Missing closing bracket */
            }
          break;

        case QUOTE:
          /* Quoted char */
          len++;
          if (pat[len] == _T('\0'))
            return (false);     /* Missing quoted char */
          break;

        case FPAT_NOT:
          /* Negated pattern */
          len++;
          if (pat[len] == _T('\0'))
            return (false);     /* Missing subpattern */
          break;

        default:
          /* Valid character */
          break;
        }
    }

  return (true);
}


/*-----------------------------------------------------------------------------
 * fpattern_submatch()
 *  Attempts to match subpattern 'pat' to subfilename 'fname'.
 *
 * Returns
 *  1 ( true ) if the subfilename matches, otherwise 0 ( false ).
 *
 * Caveats
 *  This does not assume that 'pat' is well-formed.
 *
 *  If 'pat' is empty ( "" ), the only filename it matches is the empty ( "" )
 *  string.
 *
 *  Some non-empty patterns ( e.g., "" ) will match an empty filename ( "" ).
 */

static int
fpattern_submatch (const tchar_t* pat, const tchar_t* fname)
{
  int i;
  bool yes, match;
  tchar_t lo, hi;

  /* Attempt to match subpattern against subfilename */
  while (*pat != _T('\0'))
    {
      tchar_t fch = *fname;
      tchar_t pch = *pat;
      pat++;

      switch (pch)
        {
        case FPAT_ANY:
          /* Match a single char */
#if DELIM
          if (fch == DEL || fch == DEL2 || fch == _T('\0'))
            return (false);
#else
          if (fch == _T('\0'))
            return (false);
#endif
          fname++;
          break;

        case FPAT_CLOS:
          /* Match zero or more chars */
          i = 0;
#if DELIM
          while (fname[i] != _T('\0') &&
                fname[i] != DEL && fname[i] != DEL2)
            i++;
#else
          while (fname[i] != _T('\0'))
            i++;
#endif
          while (i >= 0)
            {
              if (fpattern_submatch (pat, fname + i))
                return (true);
              i--;
            }
          return (false);

        case SUB:
          /* Match zero or more chars */
          i = 0;
          while (fname[i] != _T('\0') &&
#if DELIM
                fname[i] != DEL && fname[i] != DEL2 &&
#endif
                fname[i] != _T('.'))
            i++;
          while (i >= 0)
            {
              if (fpattern_submatch (pat, fname + i))
                return (true);
              i--;
            }
          return (false);

        case QUOTE:
          /* Match a quoted char */
          pch = *pat;
          if (lowercase (fch) != lowercase (pch) || pch == _T('\0'))
            return (false);
          fname++;
          pat++;
          break;

        case FPAT_SET_L:
          /* Match char set/range */
          yes = true;
          if (*pat == FPAT_SET_NOT)
            {
              pat++;
              yes = false;      /* Set negation */
            }

          /* Look for [ s ], [ - ], [ abc ], [ a-c ] */
          match = !yes;
          while (*pat != FPAT_SET_R && *pat != _T('\0'))
            {
              if (*pat == QUOTE)
                pat++;          /* Quoted char */

              if (*pat == _T('\0'))
                break;
              lo = *pat++;
              hi = lo;

              if (*pat == FPAT_SET_THRU)
                {
                  /* Range */
                  pat++;

                  if (*pat == QUOTE)
                    pat++;      /* Quoted char */

                  if (*pat == _T('\0'))
                    break;
                  hi = *pat++;
                }

              if (*pat == _T('\0'))
                break;

              /* Compare character to set range */
              if (lowercase (fch) >= lowercase (lo) &&
                    lowercase (fch) <= lowercase (hi))
                match = yes;
            }

          if (!match)
            return (false);

          if (*pat == _T('\0'))
            return (false);     /* Missing closing bracket */

          fname++;
          pat++;
          break;

        case FPAT_NOT:
          /* Match only if rest of pattern does not match */
          if (*pat == _T('\0'))
            return (false);     /* Missing subpattern */
          i = fpattern_submatch (pat, fname);
          return !i;

#if DELIM
        case DEL:
#if DEL2 != DEL
        case DEL2:
#endif
          /* Match path delimiter char */
          if (fch != DEL && fch != DEL2)
            return (false);
          fname++;
          break;
#endif

        default:
          /* Match a ( non-null ) char exactly */
          if (lowercase (fch) != lowercase (pch))
            return (false);
          fname++;
          break;
        }
    }

  /* Check for complete match */
  if (*fname != _T('\0'))
    return (false);

  /* Successful match */
  return (true);
}


/*-----------------------------------------------------------------------------
 * fpattern_match()
 *  Attempts to match pattern 'pat' to filename 'fname'.
 *
 * Returns
 *  1 ( true ) if the filename matches, otherwise 0 ( false ).
 *
 * Caveats
 *  If 'fname' is null, zero ( false ) is returned.
 *
 *  If 'pat' is null, zero ( false ) is returned.
 *
 *  If 'pat' is empty ( "" ), the only filename it matches is the empty
 *  string ( "" ).
 *
 *  If 'fname' is empty, the only pattern that will match it is the empty
 *  string ( "" ).
 *
 *  If 'pat' is not a well-formed pattern, zero ( false ) is returned.
 *
 *  Upper and lower case letters are treated the same ; alphabetic
 *  characters are converted to lower case before matching occurs.
 *  Conversion to lower case is dependent upon the current locale setting.
 */

int
fpattern_match (const tchar_t* pat, const tchar_t* fname)
{
  int rc;

  /* Check args */
  if (fname == nullptr)
    return (0);

  if (pat == nullptr)
    return (0);

  /* Verify that the pattern is valid, and get its length */
  if (!fpattern_isvalid (pat))
    return (0);

  /* Attempt to match pattern against filename */
  if (fname[0] == _T('\0'))
    return (pat[0] == _T('\0'));    /* Special case */
  rc = fpattern_submatch (pat, fname);

  return (rc);
}


/*-----------------------------------------------------------------------------
 * fpattern_matchn()
 *  Attempts to match pattern 'pat' to filename 'fname'.
 *  This operates like fpattern_match() except that it does not verify that
 *  pattern 'pat' is well-formed, assuming that it has been checked by a
 *  prior call to fpattern_isvalid().
 *
 * Returns
 *  1 ( true ) if the filename matches, otherwise 0 ( false ).
 *
 * Caveats
 *  If 'fname' is null, zero ( false ) is returned.
 *
 *  If 'pat' is null, zero ( false ) is returned.
 *
 *  If 'pat' is empty ( "" ), the only filename it matches is the empty ( "" )
 *  string.
 *
 *  If 'pat' is not a well-formed pattern, unpredictable results may occur.
 *
 *  Upper and lower case letters are treated the same ; alphabetic
 *  characters are converted to lower case before matching occurs.
 *  Conversion to lower case is dependent upon the current locale setting.
 *
 * See also
 *  fpattern_match().
 */

int
fpattern_matchn (const tchar_t* pat, const tchar_t* fname)
{
  int rc;

  /* Check args */
  if (fname == nullptr)
    return (false);

  if (pat == nullptr)
    return (false);

  /* Assume that pattern is well-formed */

  /* Attempt to match pattern against filename */
  rc = fpattern_submatch (pat, fname);

  return (rc);
}
