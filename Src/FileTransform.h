/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
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
 *  @file FileTransform.h
 *
 *  @brief Declaration of file transformations
 */ 
#pragma once

#include <vector>
#include "UnicodeString.h"
#include "MergeApp.h"

/**
 * @brief Modes for plugin (Modes for prediffing included)
 */
enum PLUGIN_MODE
{
	// Modes for "unpacking"
	PLUGIN_MANUAL,
	PLUGIN_AUTO,
	PLUGIN_BUILTIN_XML,

	// Modes for "prediffing"
	PREDIFF_MANUAL = PLUGIN_MANUAL,
	PREDIFF_AUTO = PLUGIN_AUTO,
};

namespace FileTransform
{
extern PLUGIN_MODE g_UnpackerMode;
extern PLUGIN_MODE g_PredifferMode;
}

class UniFile;

/**
 * @brief Plugin information for a given file
 *
 * @note Can be be passed/copied between threads
 */
class PluginForFile
{
public:
	void Initialize(PLUGIN_MODE Mode)
	{
		// initialize functions with a valid "do nothing" unpacker
		m_bWithFile = false;
		// and init Plugin/Prediffer mode and Plugin name accordingly
		m_PluginOrPredifferMode = Mode;
		if (Mode != PLUGIN_AUTO)
		{
			m_PluginName.erase();
		}
		else
		{
			m_PluginName = _("<Automatic>");
		}
	};
	explicit PluginForFile(PLUGIN_MODE Mode) 
	{
		Initialize(Mode);
	};
public:
	/// PLUGIN_AUTO if the plugin will be defined during the first use (via scan of all available plugins)
	PLUGIN_MODE	m_PluginOrPredifferMode;

	/// plugin name when it is defined
	String		m_PluginName;

	/// `true` if the plugins exchange data through a file, `false` is the data is passed as parameter (BSTR/ARRAY)
	bool		m_bWithFile;
};

/**
 * @brief Unpacking/packing information for a given file
 *
 * @note Can be be copied between threads
 * Each thread really needs its own instance so that subcode is really defined
 * during the unpacking (open file) of the thread
 */
class PackingInfo : public PluginForFile
{
public:
	explicit PackingInfo(PLUGIN_MODE Mode = FileTransform::g_UnpackerMode)
	: PluginForFile(Mode)
	, m_subcode(0)
	, m_pufile(nullptr)
	, m_bDisallowMixedEOL(false)
	{
	}
public:
	/// keep some info from unpacking for packing
	int			m_subcode;
	/// text type to override syntax highlighting
	String		m_textType;
	/// custom UniFile
	UniFile*	m_pufile;
	bool		m_bDisallowMixedEOL;
};

/**
 * @brief Prediffing information for a given file
 *
 * @note Can be be passed/copied between threads
 */
class PrediffingInfo : public PluginForFile
{
public:
	explicit PrediffingInfo(PLUGIN_MODE Mode = FileTransform::g_PredifferMode)
	: PluginForFile(Mode)
	{
	}
};

namespace FileTransform
{

// Events handler
// WinMerge uses one of these entry points to call a plugin

// bMayOverwrite : tells if we can overwrite the source file
// if we don't, don't forget do delete the temp file after use

/**
 * @brief Prepare one file for loading, scan all available plugins (events+filename filtering) 
 *
 * @param filepath : [in, out] Most plugins change this filename
 * @param handler : unpacking handler, to keep to pack again
 *
 * @return Tells if WinMerge handles this file
 *
 * @note Event FILE_UNPACK
 * Apply only the first correct handler
 */
bool Unpacking(String & filepath, const String& filteredText, PackingInfo * handler, int * handlerSubcode);
/**
 * @brief Prepare one file for loading, known handler
 *
 * @param filepath : [in, out] Most plugins change this filename
 */
bool Unpacking(String & filepath, const PackingInfo * handler, int * handlerSubcode);

bool Unpacking(PackingInfo * handler, String & filepath, const String& filteredText);

/**
 * @brief Prepare one file for saving, known handler
 *
 * @return Tells if we can save the file (really hope we can)
 *
 * @param filepath : [in, out] Most plugins change this filename
 *
 * @note Event FILE_PACK
 * Never do Unicode conversion, it was done in SaveFromFile
 */
bool Packing(String & filepath, PackingInfo handler);

/**
 * @brief Prepare one file for diffing, scan all available plugins (events+filename filtering) 
 *
 * @param filepath : [in, out] Most plugins change this filename
 * @param handler : unpacking handler, to keep to pack again
 *
 * @return Tells if WinMerge handles this file
 *
 * @note Event FILE_PREDIFF BUFFER_PREDIFF
 * Apply only the first correct handler
 */
bool Prediffing(String & filepath, const String& filteredText, PrediffingInfo * handler, bool bMayOverwrite);
/**
 * @brief Prepare one file for diffing, known handler
 *
 * @param filepath : [in, out] Most plugins change this filename
 */
bool Prediffing(String & filepath, PrediffingInfo handler, bool bMayOverwrite);

bool Prediffing(PrediffingInfo * handler, String & filepath, const String& filteredText, bool bMayOverwrite);

/**
 * @brief Transform all files to UTF8 aslong possible
 *
 * @param codepage : [in] codepage of source file
 * @param filepath : [in,out] path of file to be prepared. This filename is updated if bMayOverwrite is false
 * @param bMayOverwrite : [in] True only if the filepath points out a temp file
 *
 * @return Tells if we can go on with diffutils
 * convert all Ansi or unicode-files to UTF8 
 * if other file is unicode or uses a different codepage
 */
bool AnyCodepageToUTF8(int codepage, String & filepath, bool bMayOverwrite);


/**
 * @brief Get the list of all the free functions in all the scripts for this event :
 * 
 * @note the order is :
 * 1st script file, 1st function name
 * 1st script file, 2nd function name
 * 1st script file, ...
 * 1st script file, last function name
 * 2nd script file, 1st function name
 * 2nd script file, 2nd function name
 * 2nd script file, ...
 * 2nd script file, last function name
 * ... script file
 * last script file, 1st function name
 * last script file, 2nd function name
 * last script file, ...
 * last script file, last function name
 */
std::vector<String> GetFreeFunctionsInScripts(const wchar_t* TransformationEvent);

/** 
 * @brief : Execute one free function from one script
 *
 * @param iFncChosen : index of the function in the list returned by GetFreeFunctionsInScripts
 *
 * @return Tells if the text has been changed 
 *
 * @note Event EDITOR_SCRIPT, ?
 */
bool Interactive(String & text, const wchar_t *TransformationEvent, int iFncChosen);

}
