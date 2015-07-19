#pragma once

#include <windows.h>

/** 
 * @brief Color settings.
 */
struct COLORSETTINGS
{
	COLORREF	clrDiff;			/**< Difference color */
	COLORREF	clrSelDiff;			/**< Selected difference color */
	COLORREF	clrDiffDeleted;		/**< Difference deleted color */
	COLORREF	clrSelDiffDeleted;	/**< Selected difference deleted color */
	COLORREF	clrDiffText;		/**< Difference text color */
	COLORREF	clrSelDiffText;		/**< Selected difference text color */
	COLORREF	clrTrivial;			/**< Ignored difference color */
	COLORREF	clrTrivialDeleted;	/**< Ignored difference deleted color */
	COLORREF	clrTrivialText;		/**< Ignored difference text color */
	COLORREF	clrMoved;			/**< Moved block color */
	COLORREF	clrMovedDeleted;	/**< Moved block deleted color */
	COLORREF	clrMovedText;		/**< Moved block text color */
	COLORREF	clrSelMoved;		/**< Selected moved block color */
	COLORREF	clrSelMovedDeleted;	/**< Selected moved block deleted color */
	COLORREF	clrSelMovedText;	/**< Selected moved block text color */
	COLORREF	clrSNP;				/**< SNP block color */
	COLORREF	clrSNPDeleted;		/**< SNP block deleted color */
	COLORREF	clrSNPText;			/**< SNP block text color */
	COLORREF	clrSelSNP;			/**< Selected SNP block color */
	COLORREF	clrSelSNPDeleted;	/**< Selected SNP block deleted color */
	COLORREF	clrSelSNPText;		/**< Selected SNP block text color */
	COLORREF	clrWordDiff;		/**< Word difference color */
	COLORREF	clrWordDiffDeleted;	/**< Word differenceDeleted color */
	COLORREF	clrWordDiffText;	/**< Word difference text color */
	COLORREF	clrSelWordDiff;		/**< Selected word difference color */
	COLORREF	clrSelWordDiffDeleted;	/**< Selected word difference deleted color */
	COLORREF	clrSelWordDiffText;	/**< Selected word difference text color */
};

class COptionsMgr;

namespace Options { namespace DiffColors {

void SetDefaults(COptionsMgr *pOptionsMgr);
void Load(const COptionsMgr *pOptionsMgr, COLORSETTINGS& colors);
void Save(COptionsMgr *pOptionsMgr, const COLORSETTINGS& colors);

}}
