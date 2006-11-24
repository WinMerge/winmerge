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

#ifndef _CMD_LINE_PARSER_INCLUDED_
#define _CMD_LINE_PARSER_INCLUDED_

/** 
 * @file  CmdLineParser.h
 *
 * @brief CmdLineParser class declaration.
 *
 */

// RCS ID line follows -- this is updated by CVS
// $Id$

class MergeCmdLineInfo;

/**
 * @brief Base command line parser interface.
 *
 * WinMerge is able to parse command line arguments other than its own. In
 * order to add this capability, a developer should inherit this class and
 * implement the ParseParam method.
 *
 */
class CmdLineParser
{
public:

	/** @brief CmdLineParser's constructor.
	 *
	 * @param [in] CmdLineInfo A class which hold the information obtained
	 *	from the command line arguments.
	 *
	 */
	CmdLineParser(MergeCmdLineInfo& CmdLineInfo) :
		m_CmdLineInfo(CmdLineInfo)
	{

	}

	/** @brief CmdLineParser's destructor. */
	virtual ~CmdLineParser() { }


	/** @brief The command line argument parser.
	 *
	 * This method should be implemented by the derived class and do the
	 * actual parsing.
	 *
	 * @param [in] pszParam The parameter or flag.
	 * @param [in] bFlag Indicates whether pszParam is a parameter or a
	 *	flag.
	 * @param [in] bLast Indicates if this is the last parameter or flag
	 *	on the command line.
	 */
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast) = 0;

protected:

	MergeCmdLineInfo& m_CmdLineInfo;
};

#endif // _CMD_LINE_PARSER_INCLUDED_
