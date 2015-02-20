/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  CCPromptDlg.h
 *
 * @brief Declaration file for ClearCase dialog.
 *
 */
#pragma once

#include <memory>
#include "UnicodeString.h"

class CCCPromptDlg
{
// Construction
public:
	CCCPromptDlg();
	~CCCPromptDlg();
	int DoModal();

	String	m_comments;
	bool m_bMultiCheckouts;
	bool m_bCheckin;

private:
	CCCPromptDlg(const CCCPromptDlg &);
	CCCPromptDlg & operator=(const CCCPromptDlg &);

	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};
