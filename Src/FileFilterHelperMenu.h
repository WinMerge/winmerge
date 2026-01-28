/** 
 * @file  FileFilterHelperMenu.h
 *
 * @brief Declaration file for FileFilterHelperMenu class
 */
#pragma once

#include "UnicodeString.h"

class CFileFilterHelperMenu : public CMenu
{
public:
	std::optional<String> ShowMenu(const String& masks, int x, int y, CWnd* pParentWnd);
private:
	std::optional<String> ShowPropMenu(int command, const String& masks, CWnd* pParentWnd);
	String defaultProp(const String& name) const;
	String folderPropName(const String& name) const;
	String defaultDiffProp(const String& name, int i) const;
	String defaultAllProp(const String& name, bool not) const;
	std::optional<String> OnCommand(const String& masks, int command, CWnd* pParentWnd);
	int m_targetSide = 0; // 0 = any, 1 = left, 2 = middle, 3 = right
	int m_targetDiffSide = 0; // 0 = left&right 1 = left&middle, 2 = middle&right, 3 = all
	bool m_recursive = false;
	String m_propName;
};
