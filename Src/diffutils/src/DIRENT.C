/*
 *  dirent.c - POSIX directory access routines for MS-DOS, OS/2 and Windows/NT
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
 *  CAVEATS:
 *    The associated 'dirent.h' file should be copied into your system
 *    include directory if the '#include <dirent.h>' syntax will be used,
 *    otherwise a '-I.' switch must be added to command lines.
 *
 *    This code was originally developed with Turbo C, and continues to
 *    use TC's function and structure names.  Numerous macros make the
 *    code palatable to MSC 5.1/6.0 for MS-DOS and OS/2.  The macros
 *    depend on four defines: __TURBOC__, __MSC__, __MSDOS__, and __OS2__.
 *    The TC and BC compilers provide __TURBOC__ and __MSDOS__ as
 *    appropriate; MSC doesn't provide any of these flags so they must
 *    be given on the command line.  Sample commands for building test
 *    programs (see '#ifdef TEST' below):
 *      tcc -DTEST dirent.c
 *      bcc -DTEST dirent.c
 *      cl -DTEST -D__MSC__ -D__MSDOS__ dirent.c
 *      cl -Lp -DTEST -D__MSC__ -D__OS2__ dirent.c
 *      cl -DTEST -D__MSC__ -D__NT__ dirent.c
 *
 *    This code reads an entire directory into memory, and thus is not
 *    a good choice for scanning very large directories.  The maximum
 *    number of names allowed is controlled by MAXNAMES, defined below.
 *    This value is used to allocate an array of pointers, so making it
 *    ridiculously large may cause the code to fail silently.  The array
 *    of pointers could be realloc()'ed in the loadDir() function, but
 *    this can be dangerous with some 'weak' memory allocation packages.
 *
 *    POSIX requires that the rewinddir() function re-scan the directory,
 *    so this code must preserve the original directory name.  If the
 *    name given is a relative path (".", "..", etc.) and the current
 *    directory is changed between opendir() and rewinddir(), a different
 *    directory will be scanned by rewinddir().  (The directory name
 *    could be "qualified" by opendir(), but this process yields unusable
 *    names for network drives).
 *
 *    This code provides only file names, as that is all that is required
 *    by POSIX.  Considerable other information is available from the
 *    MS-DOS and OS/2 directory search functions.  This package should not
 *    be considered as a general-purpose directory scanner, but rather as
 *    a tool to simplify porting other programs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include <string.h>
#include <dirent.h>

#define MAXNAMES	1024		/*  max names in dir list  */
#define MAXLEN		260		/*  max len of any path/filename  */

#define NULLP(type)	((type *)0)	/*  readability macro  */

#ifdef __TURBOC__
#include <alloc.h>
#include <dir.h>
#include <mem.h>
typedef struct ffblk	FIND_T;
#define findclose(f)
#define FA_RDONLY	0x01
#define FA_HIDDEN	0x02
#define FA_SYSTEM	0x04
#define FA_LABEL	0x08
#define FA_DIREC	0x10
#define FA_ARCH		0x20
#endif /*__TURBOC__*/

#if defined(__MSC__) && defined(__MSDOS__)
#include <dos.h>
#include <malloc.h>
#include <memory.h>
typedef struct find_t	FIND_T;
#define findfirst(n,f,a)	_dos_findfirst(n,a,f)
#define findnext(f)		_dos_findnext(f)
#define findclose(f)
#define ff_name			name
#define FA_RDONLY		_A_RDONLY
#define FA_HIDDEN		_A_HIDDEN
#define FA_SYSTEM		_A_SYSTEM
#define FA_LABEL		_A_VOLID
#define FA_DIREC		_A_SUBDIR
#define FA_ARCH			_A_ARCH
#endif /*__MSC__&&__MSDOS__*/

#if defined(__MSC__) && defined(__OS2__)
#define INCL_DOS
#include <os2.h>
#include <malloc.h>
#include <memory.h>
typedef struct
{
	HDIR ft_handle;
	FILEFINDBUF ft_ffb;
	int ft_count;
} FIND_T;
#define findfirst(n,f,a)	((f)->ft_handle=0xFFFF,(f)->ft_count=1,\
					DosFindFirst(n,&(f)->ft_handle,a,\
					&(f)->ft_ffb,sizeof((f)->ft_ffb),\
					&(f)->ft_count,0L))
#define findnext(f)		DosFindNext((f)->ft_handle,&(f)->ft_ffb,\
					sizeof((f)->ft_ffb),&(f)->ft_count)
#define findclose(f)		DosFindClose((f)->ft_handle);
#define ff_name			ft_ffb.achName
#define FA_RDONLY		0x01
#define FA_HIDDEN		0x02
#define FA_SYSTEM		0x04
#define FA_LABEL		0x08
#define FA_DIREC		0x10
#define FA_ARCH			0x20
#endif /*__MSC__&&__OS2__*/

#if defined(__MSC__) && defined(__NT__)
#include <windows.h>
typedef struct
{
	long ft_hdl;
	struct _finddata_t ft_ffb;
} FIND_T;

#define findfirst(n,f,a)	(((f)->ft_hdl=_findfirst(n,&(f)->ft_ffb))==-1)
#define findnext(f)		_findnext((f)->ft_hdl,&(f)->ft_ffb)
#define findclose(f)		_findclose((f)->ft_hdl)
#define ff_name			ft_ffb.name
#define FA_RDONLY		0x01
#define FA_HIDDEN		0x02
#define FA_SYSTEM		0x04
#define FA_LABEL		0x08
#define FA_DIREC		0x10
#define FA_ARCH			0x20
#endif /*__MSC__&&__NT__*/

	/*  mask for all interesting files  */
#define ALL	(FA_RDONLY+FA_HIDDEN+FA_SYSTEM+FA_DIREC)

typedef struct __DIRENT
{
	char path[MAXLEN];	/*  directory name  */
	char **names;		/*  array of ptrs to names  */
	int count;		/*  number of entries  */
	int current;		/*  current entry  */
} DIRENT;

	/*  forward declarations  */
static int loadDir(DIRENT *dir);

/*
-*	opendir - open a directory for reading
 */
	DIR *
opendir(char const *name)
{
	DIRENT *dir;

	/*  worth looking at ??  */
	if ( (name == NULL) || (*name == '\0') )
	{
		errno = ENOENT;
		return ( NULLP(DIR) );
	}

	/*  get space for DIRENT struct  */
	if ( (dir = malloc(sizeof(DIRENT))) == NULLP(DIRENT) )
	{
		errno = ENOMEM;
		return ( NULLP(DIR) );
	}

	/*  load the names  */
	strcpy(dir->path, name);
	if ( !loadDir(dir) )
	{
		free(dir);
		/*  errno already set  */
		return ( NULLP(DIR) );
	}

	return ( (DIR *)dir );
}

/*
-*	closedir - close a directory
 */
	int
closedir(DIR *dir)
{
	char **names = ((DIRENT *)dir)->names;
	int count = ((DIRENT *)dir)->count;

	while ( count )
		free(names[--count]);
	free(names);
	free(dir);
	return ( 0 );
}

/*
-*	readdir - return ptr to next directory entry
 */
	struct dirent *
readdir(DIR *dir)
{
	static struct dirent dp;
	DIRENT *de = (DIRENT *)dir;

	if ( de->current >= de->count )
		return ( NULLP(struct dirent) );

	strcpy(dp.d_name, de->names[de->current++]);
	return ( &dp );
}

/*
-*	rewinddir - rewind directory (re-open)
 */
	void
rewinddir(DIR *dir)
{
	char **names = ((DIRENT *)dir)->names;
	int count = ((DIRENT *)dir)->count;

	/*  free existing names  */
	while ( count )
		free(names[--count]);
	free(names);
	/*  reload  */
	loadDir((DIRENT *)dir);
}

/*
-*	__seekdir - change directory position
 */
	void
__seekdir(DIR *dir, long off)
{
	DIRENT *de = (DIRENT *)dir;

	if ( (off < 0) || (off > de->count) )
		return;
	de->current = (int)off;
}

/*
-*	__telldir - return current directory position
 */
	long
__telldir(DIR *dir)
{
	return ( (long)((DIRENT *)dir)->current );
}

	/*  LOCAL ROUTINES  */
/*  load a directory list  */
	int
loadDir(DIRENT *dir)
{
	char pattern[MAXLEN];
	char **names;
	int count = 0;
	int mode;
	FIND_T ff;

	/*  do we have just a drive name ??  */
	if ( (dir->path[1] == ':') && (dir->path[2] == '\0') )
		strcat(dir->path, ".");

	/*  is it a directory ??  */
#ifdef __MSDOS__
#ifdef __TURBOC__
	if ( ((mode = _chmod(dir->path, 0)) < 0) ||
#endif /*__TURBOC__*/
#ifdef __MSC__
	if ( _dos_getfileattr(dir->path, &mode) ||
#endif /*__MSC__*/
#endif /*__MSDOS__*/
#ifdef __OS2__
	if ( DosQFileMode(dir->path, &mode, 0L) ||
#endif /*__OS2__*/
#ifdef __NT__
	if ( ((mode = GetFileAttributes(dir->path)) == 0xFFFFFFFF) ||
#endif /*__NT__*/
	     !(mode & FA_DIREC) )
	{
		errno = ENOTDIR;
		return ( 0 );
	}

	/*  get space for array of ptrs  */
	if ( (names = (char **)malloc(MAXNAMES * sizeof(char *))) ==
								NULLP(char *) )
	{
		errno = ENOMEM;
		return ( 0 );
	}

	/*  build pattern string  */
	strcpy(pattern, dir->path);
	if ( strchr("\\/:", pattern[strlen(pattern) - 1]) == NULL )
		strcat(pattern, "/");
	strcat(pattern, "*.*");

	if ( !findfirst(pattern, &ff, ALL) )
		do
		{
			/*  add name if not "." or ".."  */
			if ( ff.ff_name[0] != '.' )
			{
				/*  make a copy of the name  */
				if ( (names[count] = strdup(ff.ff_name))
								     == NULL )
				{
					/*  free all if error (out of mem)  */
					while ( count )
						free(names[--count]);
					free(names);
					errno = ENOMEM;
					return ( 0 );
				}
				count++;
			}
		}
		while ( !findnext(&ff) && (count < MAXNAMES) );
	findclose(&ff);
	dir->names = names;
	dir->count = count;
	dir->current = 0;
	return ( 1 );
}

#ifdef TEST
	int
main(int argc, char *argv[])
{
	DIR *dir;
	struct dirent *d;
	long pos;

	/*  check arguments  */
	if ( argc != 2 )
	{
		fprintf(stderr, "Usage: dirent <directory>\n");
		return ( 1 );
	}

	/*  try to open the given directory  */
	if ( (dir = opendir(argv[1])) == (DIR *)0 )
	{
		fprintf(stderr, "cannot open %s\n", argv[1]);
		return ( 1 );
	}

	/*  walk the directory once forward  */
	while ( (d = readdir(dir)) != NULLP(struct dirent) )
		printf("%s\n", d->d_name);

	/*  rewind  */
	rewinddir(dir);

	/*  scan to the end again  */
	while ( (d = readdir(dir)) != NULLP(struct dirent) )
		;

	/*  seek backwards to beginning  */
	for ( pos = __telldir(dir); pos >= 0; pos-- )
	{
		__seekdir(dir, pos);
		printf("%ld=%ld\n", __telldir(dir), pos);
	}

	/*  close and exit  */
	printf("closedir() returns %d\n", closedir(dir));
	return ( 0 );
}
#endif /*TEST*/

/*  END of dirent.c  */
