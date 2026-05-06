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
	String defaultProp(const String& name) const;
	String op() const;
	String matchContext(const String& expr) const;
	std::optional<String> OnCommand(const String& masks, int command, CWnd* pParentWnd);
	int m_targetSide = 0; // 0 = any, 1 = left, 2 = middle, 3 = right
	int m_operator = 0; // 0 = or, 1 = and
	int m_context = 0;
};
