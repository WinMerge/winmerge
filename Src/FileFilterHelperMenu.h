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
	int m_targetSide = 0; // 0 = any, 1 = left, 2 = middle, 3 = right
	int m_targetDiffSide = 0; // 0 = left&right 1 = left&middle, 2 = middle&right
};
