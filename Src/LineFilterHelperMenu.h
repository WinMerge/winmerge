/** 
 * @file  LineFilterHelperMenu.h
 *
 * @brief Declaration file for CLineFilterHelperMenu class
 */
#pragma once

#include "FilterHelperMenuBase.h"

class CLineFilterHelperMenu : public CFilterHelperMenuBase
{
public:
	CLineFilterHelperMenu() = default;
	CLineFilterHelperMenu(int targetSide, int op, int columnIndex) : m_operator(op), m_columnIndex(columnIndex) 
	{
		m_targetSide = targetSide;
	}
	std::optional<String> ShowMenu(const String& filterExpr, int x, int y, CWnd* pParentWnd);
	std::optional<String> HandleMenuCommand(const String& filterExpr, int command, CWnd* pParentWnd);
private:
	String defaultProp(const String& name) const;
	String defaultDiffProp(const String& name, int i) const;
	String defaultAllProp(const String& name, bool not) const;
	String op() const;
	std::optional<String> OnCommand(const String& filterExpr, int command, CWnd* pParentWnd);
	// Note: m_targetSide and m_targetDiffSide are now inherited from CFilterHelperMenuBase
	int m_operator = 0; // 0 = and, 1 = or
	int m_columnIndex = 0;
	bool m_byBlock = false; // false = by line, true = by block
};
