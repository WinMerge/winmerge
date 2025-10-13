/**
 * @file  PropertySystemMenu.h
 * @brief Declaration file for PropertySystemMenu
 */
#pragma once

#include "UnicodeString.h"
#include <optional>

class CPropertySystemMenu : public CMenu
{
public:
	std::optional<String> ShowMenu(CWnd* pParentWnd, UINT idFirst, const String& format);
};
