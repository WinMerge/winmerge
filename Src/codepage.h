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

void getCodepages(CArray<int, int> * pages, bool includeUninstalled=false);

/**
 * @brief Information about one supported codepage
 */
struct codepage_wminfo {
	int codepage;     /**< actual MS-Windows numeric codepage value */
	int idsnum;       /**< resource holding localized name */
	int idsxnum;      /**< if non-zero, resource holding localized additional information */
	int idsgroup;     /**< geographic grouping (resource identifier) */
	int idsplatform;  /**< primary associated O/S (resource identifier) */
};

const codepage_wminfo * getCodepageInfo(int codepage);

#endif //__CODEPAGE_H__
