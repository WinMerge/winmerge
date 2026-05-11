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
	CLineFilterHelperMenu() = default;
	CLineFilterHelperMenu(int targetSide, int op) : m_targetSide(targetSide), m_operator(op) {}
	std::optional<String> ShowMenu(const String& filterExpr, int x, int y, CWnd* pParentWnd);
	std::optional<String> HandleMenuCommand(const String& filterExpr, int command, CWnd* pParentWnd);
private:
	String defaultProp(const String& name) const;
	String op() const;
	String matchContext(const String& expr, int context) const;
	std::optional<String> OnCommand(const String& filterExpr, int command, CWnd* pParentWnd);
	int m_targetSide = 0; // 0 = any, 1 = left, 2 = middle, 3 = right
	int m_operator = 0; // 0 = and, 1 = or
};
