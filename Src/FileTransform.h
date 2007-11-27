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
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef FileTransform_h
#define FileTransform_h

#include "resource.h"

class UniFile;

class CRegExp;
typedef CTypedPtrList<CPtrList, CRegExp*>RegList;



/**
 * @brief Modes for plugin (Modes for prediffing included)
 */
enum PLUGIN_MODE
{
	// Modes for unpacking
	PLUGIN_MANUAL,
	PLUGIN_AUTO,
	PLUGIN_BUILTIN_XML,
	// Modes for prediffing
	PREDIFF_MANUAL = PLUGIN_MANUAL,
	PREDIFF_AUTO = PLUGIN_AUTO,
};

C_ASSERT(PLUGIN_MANUAL == FALSE);
C_ASSERT(PLUGIN_AUTO == TRUE);

extern int g_bUnpackerMode;
extern int g_bPredifferMode;


/**
 * @brief List of transformation categories (events)
 *
 * @note If you add some event, you have to complete this array in FileTransform.cpp
 */
extern LPCWSTR TransformationCategories[];



/**
 * @brief Plugin information for a given file
 *
 * @note Can be be passed/copied between threads
 */
class PluginForFile
{
public:
	void Initialize(int bMode)
	{
		// TODO: Convert bMode to PLUGIN_MODE and fix compile errors
		// init functions as a valid "do nothing" unpacker
		bWithFile = FALSE;
		// and init bAutomatic flag and name according to global variable
		if (bMode != PLUGIN_AUTO)
		{
			pluginName.erase();
		}
		else
		{
			pluginName = LoadResString(IDS_USERCHOICE_AUTOMATIC);
		}
		bToBeScanned = bMode;
	};
	PluginForFile(PLUGIN_MODE bMode) 
	{
		Initialize(bMode);
	};
public:
	/// TRUE if the plugin will be defined during the first use (through scan of all available plugins)
	int bToBeScanned; // TODO: Convert to PLUGIN_MODE and fix compile errors
	/// plugin name when it is defined
	String pluginName;
	/// TRUE is the plugins exchange data through a file, FALSE is the data is passed as parameter (BSTR/ARRAY)
	BOOL    bWithFile;
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
	PackingInfo(PLUGIN_MODE bMode = (PLUGIN_MODE)g_bUnpackerMode)
	: PluginForFile(bMode)
	, subcode(0)
	, pufile(0)
	, disallowMixedEOL(false)
	{
	}
public:
	/// keep some info from unpacking for packing
	int subcode;
	/// text type to override syntax highlighting
	String textType;
	/// custom UniFile
	UniFile *pufile;
	bool disallowMixedEOL;
};

/**
 * @brief Prediffing information for a given file
 *
 * @note Can be be passed/copied between threads
 */
class PrediffingInfo : public PluginForFile
{
public:
	PrediffingInfo(PLUGIN_MODE bMode = (PLUGIN_MODE)g_bPredifferMode)
	: PluginForFile(bMode)
	{
	}
};


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
BOOL FileTransform_Unpacking(String & filepath, LPCTSTR filteredText, PackingInfo * handler, int * handlerSubcode);
/**
 * @brief Prepare one file for loading, known handler
 *
 * @param filepath : [in, out] Most plugins change this filename
 */
BOOL FileTransform_Unpacking(String & filepath, const PackingInfo * handler, int * handlerSubcode);
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
BOOL FileTransform_Packing(String & filepath, PackingInfo handler);

/**
 * @brief Normalize Unicode files to OLECHAR
 *
 * @param filepath : [in,out] path of file to be prepared. This filename is updated if bMayOverwrite is FALSE
 * @param bMayOverwrite : [in] True only if the filepath points out a temp file
 *
 * @return Tells if we succeed
 *
 * @note Ansi files are not changed
 */
BOOL FileTransform_NormalizeUnicode(String & filepath, BOOL bMayOverwrite);

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
BOOL FileTransform_Prediffing(String & filepath, LPCTSTR filteredText, PrediffingInfo * handler, BOOL bMayOverwrite);
/**
 * @brief Prepare one file for diffing, known handler
 *
 * @param filepath : [in, out] Most plugins change this filename
 */
BOOL FileTransform_Prediffing(String & filepath, PrediffingInfo handler, BOOL bMayOverwrite);


/**
 * @brief Transform OLECHAR files to UTF8
 *
 * @param filepath : [in,out] path of file to be prepared. This filename is updated if bMayOverwrite is FALSE
 * @param bMayOverwrite : [in] True only if the filepath points out a temp file
 *
 * @return Tells if we can go on with diffutils
 *
 * @todo Convert Ansi files to UTF8 if other file is unicode or uses a different codepage
 */
BOOL FileTransform_UCS2ToUTF8(String & filepath, BOOL bMayOverwrite);




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
void GetFreeFunctionsInScripts(CStringArray & sNamesArray, LPCWSTR TransformationEvent);

/** 
 * @brief : Execute one free function from one script
 *
 * @param iFncChosen : index of the function in the list returned by GetFreeFunctionsInScripts
 *
 * @return Tells if the text has been changed 
 *
 * @note Event EDITOR_SCRIPT, ?
 */
BOOL TextTransform_Interactive(CString & text, LPCWSTR TransformationEvent, int iFncChosen);

#endif // FileTransform_h
