/*
 *  dirent.h - POSIX directory access routines for MS-DOS, OS/2 and Windows/NT
 *
 *  Author: Frank Whaley (few@autodesk.com)
 *
 *  Copyright Frank Whaley 1993.  All rights reserved.
 *
 *  Permission to use, copy, modify, distribute, and sell this software
 *  and its documentation for any purpose is hereby granted without fee,
 *  provided that the above copyright notice appears in all copies of the
 *  source code.  The name of the author may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Read the "CAVEATS" section in dirent.c.
 */

#ifndef __DIRENT_H
#define __DIRENT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__OS2__) || defined(__NT__)
#define d_MAXNAMLEN 255
#else
#define d_MAXNAMLEN 12
#endif

typedef struct __DIRENT DIR;

struct dirent
{
 char d_name[d_MAXNAMLEN + 1];
};

DIR *opendir(char const *__name);
struct dirent *readdir(DIR *__dir);
void rewinddir(DIR *__dir);
int closedir(DIR *__dir);

void __seekdir(DIR *__dir, long __pos);
long __telldir(DIR *__dir);

#ifndef _POSIX_SOURCE
#define seekdir __seekdir
#define telldir __telldir
#endif

#ifdef __cplusplus
}
#endif

#endif /*__DIRENT_H*/

/*  END of dirent.h  */
