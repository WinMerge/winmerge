// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  ConflictFileParser.cpp
 *
 * @brief Implementation for conflict file parser.
 */

// Conflict file parsing method modified from original code got from:
// TortoiseCVS - a Windows shell extension for easy version control
// Copyright (C) 2000 - Francis Irving
// <francis@flourish.org> - January 2001

#include "pch.h"
#include "ConflictFileParser.h"
#include "UnicodeString.h"
#include "UniFile.h"
#include "FileTextEncoding.h"
#include "codepage_detect.h"


// Note: keep these strings in "wrong" order so we can resolve this file :)
/** @brief String separating Mine and Theirs blocks. */
static const tchar_t Separator[] = _T("=======");
/** @brief String ending Theirs block (and conflict). */
static const tchar_t TheirsEnd[] = _T(">>>>>>> ");
/** @brief String starting Mine block (and conflict). */
static const tchar_t MineBegin[] = _T("<<<<<<< ");
/** @brief String starting Base block (and conflict). */
static const tchar_t BaseBegin[] = _T("||||||| ");

namespace ConflictFileParser
{

/**
 * @brief Check if the file is a conflict file.
 * This function checks if the conflict file marker is found from given file.
 * This is faster than trying to parse a file that is not conflict file.
 * @param [in] conflictFileName Full path to file to check.
 * @return true if given file is a conflict file, false otherwise.
 */
bool IsConflictFile(const String& conflictFileName)
{
	UniMemFile conflictFile;
	bool startFound = false;

	// open input file
	bool success = conflictFile.OpenReadOnly(conflictFileName);
	if (!success)
		return false;

	// Search for a conflict marker
	bool linesToRead = true;
	while (linesToRead && !startFound)
	{
		String line;
		bool lossy;
		String eol;
		linesToRead = conflictFile.ReadString(line, eol, &lossy);

		std::string::size_type pos;
		pos = line.find(MineBegin);
		if (pos == 0)
			startFound = true;
	}
	conflictFile.Close();

	return startFound;
}

/**
 * @brief Parse a conflict file to separate files.
 * This function parses a conflict file to two different files which can be
 * opened into WinMerge's file compare.
 * @param [in] conflictFileName Full path to conflict file.
 * @param [in] workingCopyFileName Full path for user's modified file in
 *  working copy/working folder.
 * @param [in] newRevisionFileName Full path for revision control file.
 * @param [in] iGuessEncodingType Try to guess codepage (not just unicode encoding)
 * @param [out] bNestedConflicts returned as true if nested conflicts found.
 * @return true if conflict file was successfully parsed, false otherwise.
 */
bool ParseConflictFile(const String& conflictFileName,
		const String& workingCopyFileName, const String& newRevisionFileName, const String& baseRevisionFileName,
		int iGuessEncodingType, bool &bNestedConflicts, bool &b3way)
{
	UniMemFile conflictFile;
	UniStdioFile workingCopy;
	UniStdioFile newRevision;
	UniStdioFile baseRevision;
	String line;
	std::string::size_type pos;
	int state;
	int iNestingLevel = 0;
	bool bResult = false;
	String revision = _T("none");
	bNestedConflicts = false;
	b3way = false;

	// open input file
	bool success = conflictFile.OpenReadOnly(conflictFileName);
	if (!success)
		return false;

	// Create output files
	bool success2 = workingCopy.Open(workingCopyFileName, _T("wb"));
	if (!success2)
		return false;
	bool success3 = newRevision.Open(newRevisionFileName, _T("wb"));
	if (!success3)
		return false;
	bool success4 = baseRevision.Open(baseRevisionFileName, _T("wb"));
	if (!success4)
		return false;

	// detect codepage of conflict file
	FileTextEncoding encoding = codepage_detect::Guess(conflictFileName, iGuessEncodingType);

	conflictFile.SetUnicoding(encoding.m_unicoding);
	conflictFile.SetBom(encoding.m_bom);
	conflictFile.SetCodepage(encoding.m_codepage);
	workingCopy.SetUnicoding(encoding.m_unicoding);
	workingCopy.SetBom(encoding.m_bom);
	workingCopy.SetCodepage(encoding.m_codepage);
	newRevision.SetUnicoding(encoding.m_unicoding);
	newRevision.SetBom(encoding.m_bom);
	newRevision.SetCodepage(encoding.m_codepage);
	baseRevision.SetUnicoding(encoding.m_unicoding);
	baseRevision.SetBom(encoding.m_bom);
	baseRevision.SetCodepage(encoding.m_codepage);

	state = 0;
	bool linesToRead = true;
	do
	{
		bool lossy;
		String eol;
		linesToRead = conflictFile.ReadString(line, eol, &lossy);
		switch (state)
		{
			// in common section
		case 0:
			// search beginning of conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				// working copy section starts
				state = 1;
				bResult = true;
			}
			else
			{
				// we're in the common section, so write to both files
				newRevision.WriteString(line);
				newRevision.WriteString(eol);

				baseRevision.WriteString(line);
				baseRevision.WriteString(eol);

				workingCopy.WriteString(line);
				workingCopy.WriteString(eol);
			}
			break;

			// in working copy section
		case 1:
			// search beginning of conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				// nested conflict section starts
				state = 3;
				workingCopy.WriteString(line);
				workingCopy.WriteString(eol);
			}
			else
			{
				pos = line.find(BaseBegin);
				if (pos != std::string::npos)
				{
					line = line.substr(0, pos);
					if (!line.empty())
					{
						baseRevision.WriteString(line);
						baseRevision.WriteString(eol);
					}

					// base revision section
					state = 5;
					b3way = true;
				}
				else
				{
					pos = line.find(Separator);
					if ((pos != std::string::npos) && (pos == (line.length() - 7)))
					{
						line = line.substr(0, pos);
						if (!line.empty())
						{
							workingCopy.WriteString(line);
							workingCopy.WriteString(eol);
						}

						//  new revision section
						state = 2;
					}
					else
					{
						workingCopy.WriteString(line);
						workingCopy.WriteString(eol);
					}
				}
			}
			break;

			// in new revision section
		case 2:
			// search beginning of nested conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				// nested conflict section starts
				state = 4;
				newRevision.WriteString(line);
				newRevision.WriteString(eol);
			}
			else
			{
				pos = line.find(TheirsEnd);
				if (pos != std::string::npos)
				{
					revision = line.substr(pos + 8);
					line = line.substr(0, pos);
					if (!line.empty())
					{
						newRevision.WriteString(line);
						newRevision.WriteString(eol);
					}

					//  common section
					state = 0;
				}
				else
				{
					newRevision.WriteString(line);
					newRevision.WriteString(eol);
				}
			}
			break;


			// in nested section in working copy section
		case 3:
			// search beginning of nested conflict section
			bNestedConflicts = true;
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				iNestingLevel++;
			}
			else
			{
				pos = line.find(TheirsEnd);
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
			workingCopy.WriteString(line);
			workingCopy.WriteString(eol);
			break;

			// in nested section in new revision section
		case 4:
			// search beginning of nested conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				iNestingLevel++;
			}
			else
			{
				pos = line.find(TheirsEnd);
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
			newRevision.WriteString(line);
			newRevision.WriteString(eol);
			break;

			// in base revision section
		case 5:
			pos = line.find(Separator);
			if ((pos != std::string::npos) && (pos == (line.length() - 7)))
			{
				line = line.substr(0, pos);
				if (!line.empty())
				{
					baseRevision.WriteString(line);
					baseRevision.WriteString(eol);
				}

				//  new revision section
				state = 2;
			}
			else
			{
				baseRevision.WriteString(line);
				baseRevision.WriteString(eol);
			}
			break;

		}
	} while (linesToRead);

	// Close
	baseRevision.Close();
	newRevision.Close();
	workingCopy.Close();
	conflictFile.Close();
	return bResult;
}

}
