#pragma once

#include "cecolor.h"

/** 
 * @brief Color settings.
 */
struct COLORSETTINGS
{
	CEColor	clrDiff;			/**< Difference color */
	CEColor	clrDiffDeleted;		/**< Difference deleted color */
	CEColor	clrDiffText;		/**< Difference text color */
	CEColor	clrSelDiff;			/**< Selected difference color */
	CEColor	clrSelDiffDeleted;	/**< Selected difference deleted color */
	CEColor	clrSelDiffText;		/**< Selected difference text color */
	CEColor	clrTrivial;			/**< Ignored difference color */
	CEColor	clrTrivialDeleted;	/**< Ignored difference deleted color */
	CEColor	clrTrivialText;		/**< Ignored difference text color */
	CEColor	clrMoved;			/**< Moved block color */
	CEColor	clrMovedDeleted;	/**< Moved block deleted color */
	CEColor	clrMovedText;		/**< Moved block text color */
	CEColor	clrSelMoved;		/**< Selected moved block color */
	CEColor	clrSelMovedDeleted;	/**< Selected moved block deleted color */
	CEColor	clrSelMovedText;	/**< Selected moved block text color */
	CEColor	clrSNP;				/**< SNP block color */
	CEColor	clrSNPDeleted;		/**< SNP block deleted color */
	CEColor	clrSNPText;			/**< SNP block text color */
	CEColor	clrSelSNP;			/**< Selected SNP block color */
	CEColor	clrSelSNPDeleted;	/**< Selected SNP block deleted color */
	CEColor	clrSelSNPText;		/**< Selected SNP block text color */
	CEColor	clrWordDiff;		/**< Word difference color */
	CEColor	clrWordDiffDeleted;	/**< Word differenceDeleted color */
	CEColor	clrWordDiffText;	/**< Word difference text color */
	CEColor	clrSelWordDiff;		/**< Selected word difference color */
	CEColor	clrSelWordDiffDeleted;	/**< Selected word difference deleted color */
	CEColor	clrSelWordDiffText;	/**< Selected word difference text color */
};

class COptionsMgr;

namespace Options { namespace DiffColors {

void Init(COptionsMgr *pOptionsMgr);
void Load(const COptionsMgr *pOptionsMgr, COLORSETTINGS& colors);
void Save(COptionsMgr *pOptionsMgr, const COLORSETTINGS& colors);

}}
