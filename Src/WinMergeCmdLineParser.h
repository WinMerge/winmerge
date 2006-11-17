/////////////////////////////////////////////////////////////////////////////
//
//    WinMerge: An interactive diff/merge utility
//    Copyright (C) 1997 Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////

/** 
* @file  WinMergeCmdLineParser.h
*
* @brief WinMergeCmdLineParser class decleration.
*
*/

// RCS ID line follows -- this is updated by CVS
// $Id$

#pragma once

#include "CmdLineParser.h"

/**
* @brief WinMerge's default command line parser.
*
*/
class WinMergeCmdLineParser : public CmdLineParser
{
	public:

		WinMergeCmdLineParser(MergeCmdLineInfo& CmdLineInfo);

		virtual ~WinMergeCmdLineParser() { }

		virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	private:

		bool m_bPreDiff;
		bool m_bFileFilter;
		bool m_bLeftDesc;
		bool m_bRightDesc;
};
