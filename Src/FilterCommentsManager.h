// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FilterCommentsManager.h
 *
 * @brief FilterCommentsManager class declaration.
 */
#pragma once

#include <string>
#include <map>
#include "UnicodeString.h"

//IngnoreComment logic developed by David Maisonave AKA (Axter)
/**
@struct FilterCommentsSet
@brief FilterCommentsSet holds search strings used to find comments in compared files.
		This data is used to find blocks that can be ignored when comparing to files.
@note
		The ignore-comment logic can only use ANSI strings, because the search buffer is
		char* type.
		Therefore, the data members should not be replaced with String type, and should
		remain std::string, or other non-unicode type string.
*/
struct FilterCommentsSet
{
	std::string StartMarker;
	std::string EndMarker;
	std::string InlineMarker;
};

/**
@class FilterCommentsManager
@brief FilterCommentsManager reads language comment start and end marker strings from
		an INI file, and stores it in the map member variable m_FilterCommentsSetByFileType.
		Each set of comment markers have a list of file types that can be used with
		the file markers.
@note
The ignore-comment logic can only use ANSI strings, because the search buffer is
char* type.
FilterCommentsManager uses _T logic, only so-as to allow UNICODE file names to be 
used for the INI file, or INI file base directory.
After retrieving data from INI file, the data is converted to ANSI.
If no INI file exist, or the INI file is empty, then a default INI file is 
created with default values that are assoicated with most commen languages.
*/
class FilterCommentsManager
{
public:
	explicit FilterCommentsManager(const String &IniFileName = _T(""));
	FilterCommentsSet GetSetForFileType(const String& FileTypeName) const;

private:
	FilterCommentsManager(const FilterCommentsManager&); //Don't allow copy
	FilterCommentsManager& operator=(const FilterCommentsManager&);//Don't allow assignment
	void CreateDefaultMarkers();
	void Load();

	//Use CString instead of std::string, so as to allow UNICODE file extensions
	std::map<String, FilterCommentsSet> m_FilterCommentsSetByFileType;
	String m_IniFileName;
};
