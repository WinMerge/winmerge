#include "StdAfx.h"
#include "CustomColors.h"

namespace CustomColors
{

/** @brief Setting name for user-defined custom colors. */
const TCHAR Section[] = _T("Custom Colors");

void Load(COLORREF * colors)
{
	for (int i = 0; i < 16; i++)
		colors[i] = ::AfxGetApp()->GetProfileInt(Section, string_to_str(i).c_str(), RGB(255, 255, 255));
}

void Save(const COLORREF * colors)
{
	for (int i = 0; i < 16; i++)
	{
		String sEntry = string_to_str(i);
		if (colors[i] == RGB(255, 255, 255))
			::AfxGetApp()->WriteProfileString(Section, sEntry.c_str(), NULL);
		else 
			::AfxGetApp()->WriteProfileInt(Section, sEntry.c_str(), colors[i]);
	}
}

}