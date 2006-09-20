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
 * @file  FilterCommentsManager.cpp
 *
 * @brief FilterCommentsManager class implementation.
 */

#include "stdafx.h"
#include <string>
#include <map>

#include "coretools.h"
#include "FilterCommentsManager.h"

/**
@brief FilterCommentsManager constructor, which reads the INI file data
		and populates the mapped member variable m_FilterCommentsSetByFileType.
@param[in]  Optional full INI file name, to include path.
*/
FilterCommentsManager::FilterCommentsManager(const TCHAR* IniFileName /*= _T("")*/)
 : m_IniFileName(IniFileName)
{
	USES_CONVERSION;

	int SectionNo = 0;
	TCHAR SectionName[99];
	TCHAR buffer[1024];
	if (m_IniFileName.IsEmpty())
	{
		m_IniFileName = GetModulePath() + _T("\\IgnoreSectionMarkers.ini");
	}
	for(SectionNo = 0;;++SectionNo) 
	{//Get each set of markers
		FilterCommentsSet filtercommentsset;
		_sntprintf(SectionName, sizeof(SectionName)/sizeof(SectionName[0]), _T("set%i"), SectionNo);
		GetPrivateProfileString(SectionName, _T("StartMarker"), _T(""), buffer,sizeof(buffer), m_IniFileName);
		filtercommentsset.StartMarker = T2CA(buffer);
		GetPrivateProfileString(SectionName, _T("EndMarker"), _T(""), buffer,sizeof(buffer), m_IniFileName);
		filtercommentsset.EndMarker = T2CA(buffer);
		GetPrivateProfileString(SectionName, _T("InlineMarker"), _T(""), buffer,sizeof(buffer), m_IniFileName);
		filtercommentsset.InlineMarker = T2CA(buffer);
		if (filtercommentsset.StartMarker.empty() && 
			filtercommentsset.EndMarker.empty() &&
			filtercommentsset.InlineMarker.empty())
		{
			break;
		}
		int FileTypeNo = 0;
		TCHAR FileTypeFieldName[99];
		for(FileTypeNo = 0;;++FileTypeNo) 
		{//Get each file type associated with current set of markers
			_sntprintf(FileTypeFieldName, sizeof(FileTypeFieldName)/sizeof(FileTypeFieldName[0]), _T("FileType%i"), FileTypeNo);
			GetPrivateProfileString(SectionName, FileTypeFieldName, _T(""), buffer,sizeof(buffer), m_IniFileName);
			CString FileTypeExtensionName = buffer;
			if (FileTypeExtensionName.IsEmpty())
				break;
			m_FilterCommentsSetByFileType[FileTypeExtensionName] = filtercommentsset;
		}
	} 

	if (!SectionNo)
	{//If no markers were found, then initialize default markers
		CreateDefaultMarkers();
	}
}

/**
	@brief Get comment markers that are associated with this file type.
		If there are no comment markers associated with this file type,
		then return an empty set.
	@param[in]  The file name extension. Example:("cpp", "java", "c", "h")
				Must be lower case.
*/
FilterCommentsSet FilterCommentsManager::GetSetForFileType(const CString& FileTypeName) const
{
	std::map <CString, FilterCommentsSet> :: const_iterator pSet =
		m_FilterCommentsSetByFileType.find(FileTypeName);
	if (pSet == m_FilterCommentsSetByFileType.end())
		return FilterCommentsSet();
	return pSet->second;
}

void FilterCommentsManager::CreateDefaultMarkers()
{
	USES_CONVERSION;
	int SectionNo = 0;
	TCHAR SectionName[99];
	FilterCommentsSet filtercommentsset;
	filtercommentsset.StartMarker = "/*";
	filtercommentsset.EndMarker = "*/";
	filtercommentsset.InlineMarker = "//";
	TCHAR CommonFileTypes1[][9] = {_T("java"), _T("cs"), _T("cpp"), _T("c"), _T("h"), _T("cxx"), _T("cc"), _T("js"), _T("jsl"), _T("tli"), _T("tlh"), _T("rc")};
	_sntprintf(SectionName, sizeof(SectionName)/sizeof(SectionName[0]), _T("set%i"), SectionNo);
	++SectionNo;
	WritePrivateProfileString(SectionName, _T("StartMarker"), A2CT(filtercommentsset.StartMarker.c_str()), m_IniFileName);
	WritePrivateProfileString(SectionName, _T("EndMarker"), A2CT(filtercommentsset.EndMarker.c_str()), m_IniFileName);
	WritePrivateProfileString(SectionName, _T("InlineMarker"), A2CT(filtercommentsset.InlineMarker.c_str()), m_IniFileName);
	int FileTypeNo = 0;
	for(int i = 0;i < sizeof(CommonFileTypes1)/sizeof(CommonFileTypes1[0]);++i)
	{
		m_FilterCommentsSetByFileType[CommonFileTypes1[i]] = filtercommentsset;
		TCHAR FileTypeFieldName[99];
		_sntprintf(FileTypeFieldName, sizeof(FileTypeFieldName)/sizeof(FileTypeFieldName[0]), _T("FileType%i"), FileTypeNo);
		++FileTypeNo;
		WritePrivateProfileString(SectionName, FileTypeFieldName, CommonFileTypes1[i], m_IniFileName);
	}
}
