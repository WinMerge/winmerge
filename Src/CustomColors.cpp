#include "StdAfx.h"
#include "CustomColors.h"

namespace CustomColors
{

/** @brief Setting name for user-defined custom colors. */
const TCHAR Section[] = _T("Custom Colors");

void Load(COLORREF * colors)
{
	for (int i = 0; i < 16; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		colors[i] = ::AfxGetApp()->GetProfileInt(Section,
			sEntry, RGB(255, 255, 255));
	}
}

void Save(const COLORREF * colors)
{
	for (int i = 0; i < 16; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		if (colors[i] == RGB(255, 255, 255))
			::AfxGetApp()->WriteProfileString(Section, sEntry, NULL);
		else 
			::AfxGetApp()->WriteProfileInt(Section, sEntry, colors[i]);
	}
}

}