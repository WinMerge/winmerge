/* config.h for CMake builds */

/* #undef HAVE_DIRENT_H */
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
/* #undef HAVE_UNISTD_H */
#define HAVE_WINDOWS_H 1

/* #undef HAVE_TYPE_TRAITS_H */
/* #undef HAVE_BITS_TYPE_TRAITS_H */

/* #undef HAVE_BCOPY */
#define HAVE_MEMMOVE 1
#define HAVE_STRERROR 1
/* #undef HAVE_STRTOLL */
/* #undef HAVE_STRTOQ */
#define HAVE__STRTOI64 1

/* #undef PCRE_STATIC */

#define SUPPORT_UTF8 1
/* #undef SUPPORT_UCP */
/* #undef EBCDIC */
/* #undef BSR_ANYCRLF */
/* #undef NO_RECURSE */

#define HAVE_LONG_LONG 1
#define HAVE_UNSIGNED_LONG_LONG 1

/* #undef SUPPORT_LIBBZ2 */
/* #undef SUPPORT_LIBZ */
/* #undef SUPPORT_LIBREADLINE */

#define NEWLINE			-1
#define POSIX_MALLOC_THRESHOLD	10
#define LINK_SIZE		2
#define MATCH_LIMIT		10000000
#define MATCH_LIMIT_RECURSION	MATCH_LIMIT


#define MAX_NAME_SIZE	32
#define MAX_NAME_COUNT	10000

/* end config.h for CMake builds */
