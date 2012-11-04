/**
 * @file  codepage.h
 */
// RCS ID line follows -- this is updated by SVN
// $Id$

#ifndef __CODEPAGE_H__
#define __CODEPAGE_H__

#ifndef CP_UTF8
#define CP_UTF8 65001
#define CP_ACP 0
#define CP_THREAD_ACP 3
#endif

#ifndef CP_UCS2LE
#define CP_UCS2LE 1200
#define CP_UCS2BE 1201
#endif

bool isCodepageInstalled(int codepage);

#endif //__CODEPAGE_H__
