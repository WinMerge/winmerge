/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file FileTransform.h
 *
 *  @brief Declaration of file transformations
 */ 
#pragma once

#include <vector>
#include "UnicodeString.h"

class PluginInfo;

/**
 * @brief Modes for plugin (Modes for prediffing included)
 */
enum class PLUGIN_MODE
{
	// Modes for "unpacking"
	PLUGIN_MANUAL,
	PLUGIN_AUTO,

	// Modes for "prediffing"
	PREDIFF_MANUAL = PLUGIN_MANUAL,
	PREDIFF_AUTO = PLUGIN_AUTO,
};

namespace FileTransform
{
extern PLUGIN_MODE g_UnpackerMode;
extern PLUGIN_MODE g_PredifferMode;
}

/**
 * @brief Plugin information for a given file
 *
 * @note Can be be passed/copied between threads
 */
class PluginForFile
{
public:
	struct PipelineItem
	{
		String name;
		String args;
		TCHAR quoteChar;
	};

	void Initialize(PLUGIN_MODE Mode)
	{
		// and init Plugin/Prediffer mode and Plugin name accordingly
		m_PluginPipeline = (Mode == PLUGIN_MODE::PLUGIN_AUTO) ? _T("<Automatic>") : _T("");
	};
	explicit PluginForFile(PLUGIN_MODE Mode) 
	{
		Initialize(Mode);
	};
	const String& GetPluginPipeline() const { return m_PluginPipeline; }
	void SetPluginPipeline(const String& pluginPipeline) { m_PluginPipeline = pluginPipeline; }
	void ClearPluginPipeline() { m_PluginPipeline.clear(); }

	std::vector<PipelineItem> ParsePluginPipeline(String& errorMessage) const;
	static std::vector<PipelineItem> ParsePluginPipeline(const String& pluginPipeline, String& errorMessage);
	static String MakePipeline(const std::vector<PipelineItem> list);

	bool IsValidPluginPipeline() const;
	static bool IsValidPluginPipeline(const String& pluginPipeline);

protected:
	/// plugin name when it is defined
	String m_PluginPipeline;
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
	{
	}

	bool GetPackUnpackPlugin(const String& filteredFilenames, bool bReverse,
		std::vector<std::tuple<PluginInfo*, String, bool>>& plugins,
		String *pPluginPipelineResolved, String& errorMessage) const;

	// Events handler
	// WinMerge uses one of these entry points to call a plugin

	// bMayOverwrite : tells if we can overwrite the source file
	// if we don't, don't forget do delete the temp file after use

	/**
	 * @brief Prepare one file for loading, scan all available plugins (events+filename filtering) 
	 *
	 * @param filepath : [in, out] Most plugins change this filename
	 *
	 * @return Tells if WinMerge handles this file
	 *
	 * @note Event FILE_UNPACK
	 * Apply only the first correct handler
	 */
	bool Unpacking(std::vector<int> * handlerSubcodes, String & filepath, const String& filteredText);

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
	bool Packing(String & filepath, const std::vector<int>& handlerSubcodes) const;

	bool Packing(const String& srcFilepath, const String& dstFilepath, const std::vector<int>& handlerSubcodes) const;

	String GetUnpackedFileExtension(const String& filteredFilenames) const;
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

	bool GetPrediffPlugin(const String& filteredFilenames, bool bReverse,
		std::vector<std::tuple<PluginInfo*, String, bool>>& plugins,
		String* pPluginPipelineResolved, String& errorMessage) const;

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
	bool Prediffing(String & filepath, const String& filteredText, bool bMayOverwrite);
};

namespace FileTransform
{
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
bool Interactive(String & text, const String& params, const wchar_t *TransformationEvent, int iFncChosen);

std::pair<
	std::vector<std::tuple<String, String, unsigned, PluginInfo *>>,
	std::map<String, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>>
>
CreatePluginMenuInfos(const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned baseId);

}
