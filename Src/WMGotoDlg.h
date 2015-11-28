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
 * @file  WMGotoDlg.h
 *
 * @brief Declaration file for WMGotoDlg dialog.
 *
 */
#pragma once

#include <memory>
#include "UnicodeString.h"

class WMGotoDlg
{
// Construction
public:
	WMGotoDlg();
	~WMGotoDlg();
	int DoModal();

	String m_strParam;   /**< Line/difference number. */
	int m_nFile;         /**< Target file number. */
	int m_nGotoWhat;     /**< Goto line or difference? */
private:
	WMGotoDlg(const WMGotoDlg &);
	WMGotoDlg & operator=(const WMGotoDlg &);

	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};
