/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ConflictFileParser.cpp
 *
 * @brief Implementation for conflict file parser.
 */
// ID line follows -- this is updated by SVN
// $Id$

// Modified from original code got from:
// TortoiseCVS - a Windows shell extension for easy version control
// Copyright (C) 2000 - Francis Irving
// <francis@flourish.org> - January 2001

#include "StdAfx.h"
#include "UnicodeString.h"
#include "UniFile.h"

#include "ConflictFileParser.h"

// Parse a file
bool ParseConflictFile(const String &conflictFileName,
		const String &workingCopyFileName, const String &newRevisionFileName,
		bool &bNestedConflicts)
{
	UniMemFile conflictFile;
	UniStdioFile workingCopy;
	UniStdioFile newRevision;
	String line;
	std::string::size_type pos;
	int state;
	int iNestingLevel = 0;
	bool bResult = false;
	bool bFirstLineNR = true;
	bool bFirstLineWC = true;
	String revision = _T("none");
	bNestedConflicts = false;


	// open input file
	BOOL success = conflictFile.OpenReadOnly(conflictFileName.c_str());

	// Create output files
	BOOL success2 = workingCopy.Open(workingCopyFileName.c_str(), _T("w"));
	BOOL success3 = newRevision.Open(newRevisionFileName.c_str(), _T("w"));

	state = 0;
	BOOL linesToRead = TRUE;
	do
	{
		CString cline;
		bool lossy;
		CString eol;
		linesToRead = conflictFile.ReadString(cline, eol, &lossy);
		line = (LPCTSTR) cline;
		switch (state)
		{
			// in common section
		case 0:
			// search beginning of conflict section
			pos = line.find(_T("<<<<<<< "));
			if (pos == 0)
			{
				// working copy section starts
				state = 1;
				bResult = true;
			}
			else
			{
				// we're in the common section, so write to both files
				if (!bFirstLineNR)
					newRevision.WriteString(eol);
				else
					bFirstLineNR = false;
				newRevision.WriteString(line.c_str());

				if (!bFirstLineWC)
					workingCopy.WriteString(eol);
				else
					bFirstLineWC = false;
				workingCopy.WriteString(line.c_str());
			}
			break;

			// in working copy section
		case 1:
			// search beginning of conflict section
			pos = line.find(_T("<<<<<<< "));
			if (pos == 0)
			{
				// nested conflict section starts
				state = 3;
				if(!bFirstLineWC)
					workingCopy.WriteString(eol);
				else
					bFirstLineWC = false;
				workingCopy.WriteString(line.c_str());
			}
			else
			{
				pos = line.find(_T("======="));
				if ((pos != std::string::npos) && (pos == (line.length() - 7)))
				{
					line = line.substr(0, pos);
					if (!line.empty())
					{
						if (!bFirstLineWC)
							workingCopy.WriteString(eol);
						else
							bFirstLineWC = false;
						workingCopy.WriteString(line.c_str());
					}

					//  new revision section
					state = 2;
				}
				else
				{
					if (!bFirstLineWC)
						workingCopy.WriteString(eol);
					else
						bFirstLineWC = false;
					workingCopy.WriteString(line.c_str());
				}
			}
			break;

			// in new revision section
		case 2:
			// search beginning of nested conflict section
			pos = line.find(_T("<<<<<<< "));
			if (pos == 0)
			{
				// nested conflict section starts
				state = 4;
				if (!bFirstLineNR)
					newRevision.WriteString(eol);
				else
					bFirstLineNR = false;
				newRevision.WriteString(line.c_str());
			}
			else
			{
				pos = line.find(_T(">>>>>>> "));
				if (pos != std::string::npos)
				{
					revision = line.substr(pos + 8);
					line = line.substr(0, pos);
					if (!line.empty())
					{
						if (!bFirstLineNR)
							newRevision.WriteString(eol);
						else
							bFirstLineNR = false;
						newRevision.WriteString(line.c_str());
					}

					//  common section
					state = 0;
				}
				else
				{
					if (!bFirstLineNR)
						newRevision.WriteString(eol);
					else
						bFirstLineNR = false;
					newRevision.WriteString(line.c_str());
				}
			}
			break;


			// in nested section in working copy section
		case 3:
			// search beginning of nested conflict section
			bNestedConflicts = true;
			pos = line.find(_T("<<<<<<< "));
			if (pos == 0)
			{
				iNestingLevel++;
			}
			else
			{
				pos = line.find(_T(">>>>>>> "));
				if (pos != std::string::npos)
				{
					if (iNestingLevel == 0)
					{
						state = 1;
					}
					else
					{
						iNestingLevel--;
					}
				}
			}
			if (!bFirstLineWC)
				workingCopy.WriteString(eol);
			else
				bFirstLineWC = false;
			workingCopy.WriteString(line.c_str());
			break;

			// in nested section in new revision section
		case 4:
			// search beginning of nested conflict section
			pos = line.find(_T("<<<<<<< "));
			if (pos == 0)
			{
				iNestingLevel++;
			}
			else
			{
				pos = line.find(_T(">>>>>>> "));
				if (pos != std::string::npos)
				{
					if (iNestingLevel == 0)
					{
						state = 2;
					}
					else
					{
						iNestingLevel--;
					}
				}
			}
			if (!bFirstLineNR)
				newRevision.WriteString(eol);
			else
				bFirstLineNR = false;
			newRevision.WriteString(line.c_str());
			break;
		}
	} while (linesToRead);

	// Close
	newRevision.Close();
	workingCopy.Close();
	conflictFile.Close();
	return bResult;
}
