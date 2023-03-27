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

namespace FileTransform
{
extern bool AutoUnpacking;
extern bool AutoPrediffing;
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
		std::vector<String> args;
		tchar_t quoteChar;
	};

	void Initialize(bool automatic)
	{
		// and init Plugin/Prediffer mode and Plugin name accordingly
		m_PluginPipeline = automatic ? _T("<Automatic>") : _T("");
	}

	explicit PluginForFile(bool automatic) 
	{
		Initialize(automatic);
	}

	explicit PluginForFile(const String& pluginPipeline)
	: m_PluginPipeline(pluginPipeline)
	{
	}

	const String& GetPluginPipeline() const { return m_PluginPipeline; }
	void SetPluginPipeline(const String& pluginPipeline) { m_PluginPipeline = pluginPipeline; }
	void ClearPluginPipeline() { m_PluginPipeline.clear(); }

	std::vector<PipelineItem> ParsePluginPipeline(String& errorMessage) const;
	static std::vector<PipelineItem> ParsePluginPipeline(const String& pluginPipeline, String& errorMessage);
	static String MakePluginPipeline(const std::vector<PipelineItem>& list);
	static String MakeArguments(const std::vector<String>& args, const std::vector<StringView>& variables);

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
	explicit PackingInfo(bool automatic = FileTransform::AutoUnpacking)
	: PluginForFile(automatic), m_bWebBrowser(false)
	{
	}

	explicit PackingInfo(const String& pluginPipeline)
	: PluginForFile(pluginPipeline), m_bWebBrowser(false)
	{
	}

	bool GetPackUnpackPlugin(const String& filteredFilenames, bool bUrl, bool bReverse,
		std::vector<std::tuple<PluginInfo*, std::vector<String>, bool>>& plugins,
		String *pPluginPipelineResolved, String *pURLHandlerResolved, String& errorMessage) const;

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
	bool Unpacking(std::vector<int> * handlerSubcodes, String & filepath, const String& filteredText, const std::vector<StringView>& variables);

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
	bool pack(String & filepath, const String& dstFilepath, const std::vector<int>& handlerSubcodes, const std::vector<StringView>& variables) const;

	bool Packing(const String& srcFilepath, const String& dstFilepath, const std::vector<int>& handlerSubcodes, const std::vector<StringView>& variables) const;

	String GetUnpackedFileExtension(const String& filteredFilenames, int& preferredWindowType) const;

	void EnableWebBrowserMode() { m_bWebBrowser = true; }
private:
	String m_URLHandler;
	bool m_bWebBrowser;
};

/**
 * @brief Prediffing information for a given file
 *
 * @note Can be be passed/copied between threads
 */
class PrediffingInfo : public PluginForFile
{
public:
	explicit PrediffingInfo(bool automatic = FileTransform::AutoPrediffing)
	: PluginForFile(automatic)
	{
	}

	explicit PrediffingInfo(const String& pluginPipeline)
	: PluginForFile(pluginPipeline)
	{
	}

	bool GetPrediffPlugin(const String& filteredFilenames, bool bReverse,
		std::vector<std::tuple<PluginInfo*, std::vector<String>, bool>>& plugins,
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
	bool Prediffing(String & filepath, const String& filteredText, bool bMayOverwrite, const std::vector<StringView>& variables);
};

/**
 * @brief Editor script information
 *
 * @note Can be be passed/copied between threads
 */
class EditorScriptInfo : public PluginForFile
{
public:
	explicit EditorScriptInfo(const String& pluginPipeline)
	: PluginForFile(pluginPipeline)
	{
	}

	bool GetEditorScriptPlugin(std::vector<std::tuple<PluginInfo*, std::vector<String>, int>>& plugins,
		String& errorMessage) const;

	bool TransformText(String & text, const std::vector<StringView>& variables, bool& changed);
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

std::pair<
	std::vector<std::tuple<String, String, unsigned, PluginInfo *>>,
	std::map<String, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>>
>
CreatePluginMenuInfos(const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned baseId);

inline const std::vector<std::wstring> UnpackerEventNames = { L"BUFFER_PACK_UNPACK", L"FILE_PACK_UNPACK", L"FILE_FOLDER_PACK_UNPACK" };
inline const std::vector<std::wstring> PredifferEventNames = { L"BUFFER_PREDIFF", L"FILE_PREDIFF" };
inline const std::vector<std::wstring> EditorScriptEventNames = { L"EDITOR_SCRIPT" };

}
