#pragma once

#include "cecolor.h"

/** 
 * @brief Dir color settings.
 */
struct DIRCOLORSETTINGS
{
	CEColor	clrDirItemEqual;			/**< Item equal background color */
	CEColor	clrDirItemEqualText;		/**< Item equal text color */
	CEColor	clrDirItemDiff;				/**< Item diff background color */
	CEColor	clrDirItemDiffText;			/**< Item diff text color */
	CEColor	clrDirItemNotExistAll;		/**< Item not-exist-all background color */
	CEColor	clrDirItemNotExistAllText;	/**< Item not-exist-all text color */
	CEColor	clrDirItemFiltered;			/**< Item filtered background color */
	CEColor	clrDirItemFilteredText;		/**< Item filtered text color */
	CEColor	clrDirMargin;				/**< Background color */
};

class COptionsMgr;

namespace Options { namespace DirColors {

void Init(COptionsMgr *pOptionsMgr);
void Load(const COptionsMgr *pOptionsMgr, DIRCOLORSETTINGS& colors);
void Save(COptionsMgr *pOptionsMgr, const DIRCOLORSETTINGS& colors);

}}
