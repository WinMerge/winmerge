/**
 * @file  codepage.h
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef __CODEPAGE_H__
#define __CODEPAGE_H__

class CRegOptions;

void updateDefaultCodepage(CRegOptions * options);
int getDefaultCodepage();

bool isCodepageInstalled(int codepage);
bool isCodepageSupported(int codepage);

#endif //__CODEPAGE_H__
