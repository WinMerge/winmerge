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
	CLineFilterHelperMenu(int targetSide, int op, int columnIndex) : m_targetSide(targetSide), m_operator(op), m_columnIndex(columnIndex) {}
	std::optional<String> ShowMenu(const String& filterExpr, int x, int y, CWnd* pParentWnd);
	std::optional<String> HandleMenuCommand(const String& filterExpr, int command, CWnd* pParentWnd);
private:
	String defaultProp(const String& name) const;
	String defaultDiffProp(const String& name, int i) const;
	String defaultAllProp(const String& name, bool not) const;
	String op() const;
	std::optional<String> OnCommand(const String& filterExpr, int command, CWnd* pParentWnd);
	int m_targetSide = 0; // 0 = any, 1 = left, 2 = middle, 3 = right
	int m_targetDiffSide = 0; // 0 = left&right 1 = left&middle, 2 = middle&right, 3 = all
	int m_operator = 0; // 0 = and, 1 = or
	int m_columnIndex = 0;
};
