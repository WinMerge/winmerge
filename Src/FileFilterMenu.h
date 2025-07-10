/** 
 * @file  FileFilterMenu.h
 *
 * @brief Declaration file for FileFilterMenu class
 */
#pragma once

#include "UnicodeString.h"

class CFileFilterMenu : public CMenu
{
public:
	String ShowMenu(int x, int y, CWnd* pParentWnd);
	void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
private:
	unsigned m_topMenuId;
	CMenu m_menuFile;
};
