/**
 * @file  codepage.h
 */
// RCS ID line follows -- this is updated by SVN
// $Id$

#ifndef __CODEPAGE_H__
#define __CODEPAGE_H__

void updateDefaultCodepage(int cpDefaultMode, int customCodepage);
int getDefaultCodepage();

bool isCodepageInstalled(int codepage);

#endif //__CODEPAGE_H__
