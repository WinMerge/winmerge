#include "OptionsCustomColors.h"
#include "OptionsMgr.h"
#include "MergeApp.h"

namespace Options { namespace CustomColors
{

/** @brief Setting name for user-defined custom colors. */
const TCHAR Section[] = _T("Custom Colors");

void Load(COLORREF * colors)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	for (int i = 0; i < 16; i++)
	{
		String valuename = string_format(_T("%s/%d"), Section, i);
		pOptionsMgr->InitOption(valuename, RGB(255, 255, 255), true);
		colors[i] = pOptionsMgr->GetInt(valuename);
	}
}

void Save(const COLORREF * colors)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	for (int i = 0; i < 16; i++)
	{
		String valuename = string_format(_T("%s/%d"), Section, i);
		if (colors[i] == RGB(255, 255, 255))
			pOptionsMgr->RemoveOption(valuename);
		else 
			pOptionsMgr->SaveOption(valuename, static_cast<int>(colors[i]));
	}
}

}}