#pragma once

#include <afxwin.h>
#include <vector>

#include "UnicodeString.h"

namespace PopupList
{
	struct Selection
	{
		int index = -1;
		bool checkBoxChecked = false;
	};

	/** @brief Show a scrollable popup list with a checkbox. */
	Selection Show(CWnd* pOwner, CFont* pFont, const std::vector<String>& items, int selectedIndex,
		bool enabled, const String& checkBoxText, bool checkBoxChecked, CPoint ptScreen);
}
