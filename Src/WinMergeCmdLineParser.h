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
 * @brief WinMergeCmdLineParser class declaration.
 */

// ID line follows -- this is updated by SVN
// $Id$

#ifndef _WINMERGE_CMDLINE_PARSER_H_
#define __WINMERGE_CMDLINE_PARSER_H_

#include "CmdLineParser.h"

/**
 * @brief WinMerge's default command line parser.
 * This class parses all command line parameters WinMerge (core code) knows
 * of. Paths get some special handling if needed:
 *  - if file (as first param) and folder (as second param) are given, that is
 *    expanded to file (first path) and folder/file (second path).
 */
class WinMergeCmdLineParser : public CmdLineParser
{
public:
	WinMergeCmdLineParser(MergeCmdLineInfo& cmdLineInfo);
	virtual ~WinMergeCmdLineParser() { }
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

private:
	bool m_bPreDiff;     /**< Have prediffer? */
	bool m_bFileFilter;  /**< Have file filter? */
	bool m_bLeftDesc;    /**< Have left-side description? */
	bool m_bRightDesc;   /**< Have right-side description? */
	bool m_bLeftDescRead; /**< Has the left-side decription been read? */
	bool m_bRightDescRead; /**< Has the right-side description been read? */
};

#endif // _WINMERGE_CMDLINE_PARSER_H_
