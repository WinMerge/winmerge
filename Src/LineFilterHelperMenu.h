/** 
 * @file  LineFilterHelperMenu.h
 *
 * @brief Declaration file for CLineFilterHelperMenu class
 */
#pragma once

#include "UnicodeString.h"

class CLineFilterHelperMenu : public CMenu
{
public:
	std::optional<String> ShowMenu(const String& masks, int x, int y, CWnd* pParentWnd);
	std::optional<String> HandleMenuCommand(const String& masks, int command, CWnd* pParentWnd);
private:
	std::optional<String> OnCommand(const String& masks, int command, CWnd* pParentWnd);
	int m_targetSide = 0; // 0 = any, 1 = left, 2 = middle, 3 = right
};
