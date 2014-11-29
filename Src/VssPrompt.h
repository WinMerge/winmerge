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
 * @file  VssPrompt.h
 *
 * @brief Declaration file for CVssPrompt
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#pragma once

#include <memory>
#include "UnicodeString.h"

class CVssPrompt
{
// Construction
public:
	CVssPrompt();
	~CVssPrompt();
	int DoModal();

	String	m_strProject;
	String	m_strUser;
	String	m_strPassword;
	String	m_strMessage;
	String m_strSelectedDatabase;
	bool m_bMultiCheckouts;
	bool m_bVCProjSync;

private:
	CVssPrompt(const CVssPrompt &);
	CVssPrompt & operator=(const CVssPrompt &);

	class Impl;
	std::unique_ptr<Impl> m_pimpl;
};
