#include "pch.h"
#include "OptionsCustomColors.h"
#include "OptionsMgr.h"

namespace Options { namespace CustomColors
{

/** @brief Setting name for user-defined custom colors. */
const tchar_t Section[] = _T("Custom Colors");

void Init(COptionsMgr *pOptionsMgr)
{
	for (int i = 0; i < 16; i++)
	{
		String valuename = strutils::format(_T("%s/%d"), Section, i);
		pOptionsMgr->InitOption(valuename, RGB(255, 255, 255), true);
	}
}

void Load(COptionsMgr *pOptionsMgr, COLORREF * colors)
{
	for (int i = 0; i < 16; i++)
	{
		String valuename = strutils::format(_T("%s/%d"), Section, i);
		colors[i] = pOptionsMgr->GetInt(valuename);
	}
}

void Save(COptionsMgr *pOptionsMgr, const COLORREF * colors)
{
	for (int i = 0; i < 16; i++)
	{
		String valuename = strutils::format(_T("%s/%d"), Section, i);
		pOptionsMgr->SaveOption(valuename, static_cast<int>(colors[i]));
	}
}

}}