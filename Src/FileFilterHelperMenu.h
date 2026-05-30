/** 
 * @file  FileFilterHelperMenu.h
 *
 * @brief Declaration file for FileFilterHelperMenu class
 */
#pragma once

#include "FilterHelperMenuBase.h"

class CFileFilterHelperMenu : public CFilterHelperMenuBase
{
public:
	std::optional<String> ShowMenu(const String& masks, int x, int y, CWnd* pParentWnd);
	static std::unique_ptr<CFileFilterHelperMenu> AppendColumnFilterMenu(CMenu* pPopup, const tchar_t* pRegName, bool is3Way);
	std::optional<String> HandleMenuCommand(const String& masks, int command, CWnd* pParentWnd);
private:
	std::optional<String> ShowPropMenu(int command, const String& masks, CWnd* pParentWnd);
	String defaultProp(const String& name) const;
	String folderPropName(const String& name) const;
	String defaultDiffProp(const String& name, int i) const;
	String defaultAllProp(const String& name, bool not) const;
	std::optional<String> OnCommand(const String& masks, int command, CWnd* pParentWnd);
	// Note: m_targetSide and m_targetDiffSide are now inherited from CFilterHelperMenuBase
	bool m_recursive = false;
	bool m_is3Way = false;
	String m_propName;
	String m_colName;
};
