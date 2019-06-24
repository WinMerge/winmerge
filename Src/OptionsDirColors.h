#pragma once

#include <windows.h>

/** 
 * @brief Dir color settings.
 */
struct DIRCOLORSETTINGS
{
	COLORREF	clrDirItemEqual;			/**< Item equal background color */
	COLORREF	clrDirItemEqualText;		/**< Item equal text color */
	COLORREF	clrDirItemDiff;				/**< Item diff background color */
	COLORREF	clrDirItemDiffText;			/**< Item diff text color */
	COLORREF	clrDirItemNotExistAll;		/**< Item not-exist-all background color */
	COLORREF	clrDirItemNotExistAllText;	/**< Item not-exist-all text color */
	COLORREF	clrDirItemFiltered;			/**< Item filtered background color */
	COLORREF	clrDirItemFilteredText;		/**< Item filtered text color */
};

class COptionsMgr;

namespace Options { namespace DirColors {

void SetDefaults(COptionsMgr *pOptionsMgr);
void Load(const COptionsMgr *pOptionsMgr, DIRCOLORSETTINGS& colors);
void Save(COptionsMgr *pOptionsMgr, const DIRCOLORSETTINGS& colors);

}}
