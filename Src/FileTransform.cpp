/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file FileTransform.cpp
 *
 *  @brief Implementation of file transformations
 */ 

#include "pch.h"
#include "FileTransform.h"
#include <vector>
#include <Poco/Exception.h>
#include <Poco/Mutex.h>
#include "Plugins.h"
#include "multiformatText.h"
#include "Environment.h"
#include "TFile.h"
#include "paths.h"
#include "MergeApp.h"

using Poco::Exception;

static Poco::FastMutex g_mutex;

static std::pair<String, uint8_t> parseNameAndTargetFlags(const String& token)
{
	String name;
	uint8_t targetFlags = 0;
	const auto pos = token.find_first_of(':');
	if (pos != String::npos)
	{
		name = token.substr(0, pos);
		targetFlags = 0;
		for (size_t i = pos + 1; i < token.length(); ++i)
		{
			const tchar_t ch = token[i];
			if (ch >= '1' && ch <= '3')
				targetFlags |= 1 << (ch - '1');
		}
		if (targetFlags == 0)
			targetFlags = 0xff;
	}
	else
	{
		name = token;
		targetFlags = 0xff;
	}
	return { name, targetFlags };
}

static String makeTargetsPrefix(uint8_t targetFlags)
{
	if (targetFlags == 0xff)
		return _T("");
	std::vector<String> targets;
	for (int pos = 0; pos < 3; ++pos)
	{
		if (targetFlags & (1 << pos))
		{
			tchar_t ch = '1' + static_cast<tchar_t>(pos);
			targets.push_back(String(&ch, 1));
		}
	}
	return _T(":") + strutils::join(targets.begin(), targets.end(), _T(","));
}

static inline bool isTargetInFlags(int target, unsigned targetFlags)
{
	return ((1 << target) & targetFlags) != 0;
}

////////////////////////////////////////////////////////////////////////////////
// transformations : packing unpacking


std::vector<PluginForFile::PipelineItem> PluginForFile::ParsePluginPipeline(String& errorMessage) const
{
	return ParsePluginPipeline(m_PluginPipeline, errorMessage);
}

std::vector<PluginForFile::PipelineItem> PluginForFile::ParsePluginPipeline(const String& pluginPipeline, String& errorMessage)
{
	std::vector<PluginForFile::PipelineItem> result;
	bool inQuotes = false;
	tchar_t quoteChar = 0;
	std::vector<String> args;
	uint8_t targetFlags = 0xff;
	String token, name;
	errorMessage.clear();
	const tchar_t* p = pluginPipeline.c_str();
	while (tc::istspace(*p)) p++;
	while (*p)
	{
		tchar_t sep = 0;
		while (*p)
		{
			if (!inQuotes)
			{
				if (*p == '"' || *p == '\'')
				{
					inQuotes = true;
					quoteChar = *p;
				}
				else if (tc::istspace(*p))
				{
					sep = *p;
					break;
				}
				else if (*p == '|')
				{
					sep = *p;
					break;
				}
				else
					token += *p;
			}
			else
			{
				if (*p == quoteChar)
				{
					if (*(p + 1) == quoteChar)
					{
						token += *p;
						++p;
					}
					else
					{
						inQuotes = false;
					}
				}
				else
				{
					token += *p;
				}
			}
			++p;
		}
		if (name.empty())
		{
			std::tie(name, targetFlags) = parseNameAndTargetFlags(token);
		}
		else
		{
			args.push_back(token);
		}
		while (tc::istspace(*p)) p++;
		if (*p == '|')
			sep = *p;
		if (sep == '|')
			p++;
		token.clear();
		if (sep == '|' || !*p)
		{
			if (name.empty() || (sep == '|' && !*p))
			{
				errorMessage = strutils::format_string1(_("Missing plugin name in plugin pipeline: %1"), pluginPipeline);
				break;
			}
			result.push_back({ name, targetFlags, args, quoteChar });
			name.clear();
			args.clear();
			quoteChar = 0;
		}
	};
	if (inQuotes)
		errorMessage = strutils::format_string1(_("Missing quotation mark in plugin pipeline: %1"), pluginPipeline);
	return result;
}

String PluginForFile::MakePluginPipeline(const std::vector<PluginForFile::PipelineItem>& list)
{
	int i = 0;
	String pipeline;
	for (const auto& [name, targetFlags, args, quoteChar] : list)
	{
		if (quoteChar && name.find_first_of(_T(" '\"")) != String::npos)
		{
			String nameQuoted = name;
			strutils::replace(nameQuoted, String(1, quoteChar), String(2, quoteChar));
			pipeline += strutils::format(_T("%c%s%c"), quoteChar, nameQuoted, quoteChar);
		}
		else
		{
			pipeline += name;
		}
		pipeline += makeTargetsPrefix(targetFlags);
		if (!args.empty())
		{
			for (const auto& arg : args)
			{
				if (quoteChar)
				{
					String argQuoted = arg;
					strutils::replace(argQuoted, String(1, quoteChar), String(2, quoteChar));
					pipeline += strutils::format(_T(" %c%s%c"), quoteChar, argQuoted, quoteChar);
				}
				else
				{
					pipeline += _T(" ") + arg;
				}
			}
		}
		if (i < list.size() - 1)
			pipeline += _T("|");
		i++;
	}
	return pipeline;
}

String PluginForFile::MakeArguments(const std::vector<String>& args, const std::vector<StringView>& variables)
{
	String newstr;
	int i = 0;
	for (const auto& arg : args)
	{
		String newarg;
		for (const tchar_t* p = arg.c_str(); *p; ++p)
		{
			if (*p == '%' && *(p + 1) != 0)
			{
				++p;
				tchar_t c = *p;
				if (c == '%')
				{
					newarg += '%';
				}
				else if (c >= '1' && c <= '9')
				{
					if ((c - '1') < variables.size())
						newarg += strutils::to_str(variables[(c - '1')]);
				}
				else
				{
					newarg += *(p - 1);
					newarg += c;
				}
			}
			else
			{
				newarg += *p;
			}
		}
		if (newarg.find_first_of(_T(" \"")) != String::npos)
		{
			strutils::replace(newarg, _T("\""), _T("\"\""));
			newstr += _T("\"") + newarg + _T("\"");
		}
		else
		{
			newstr += newarg;
		}
		if (i < args.size() - 1)
			newstr += ' ';
		i++;
	}
	return newstr;
}

static std::vector<PluginForFile::PipelineItem>
ExpandAliases(const String& pluginPipeline, const String& filteredFilenames, const wchar_t* aliasEvent, String& errorMessage, int stack = 0)
{
	std::vector<PluginForFile::PipelineItem> pipelineResolved;
	auto parseResult = PluginForFile::ParsePluginPipeline(pluginPipeline, errorMessage);
	if (!errorMessage.empty())
		return pipelineResolved;
	for (auto& item : parseResult)
	{
		PluginInfo* plugin = nullptr;
		if (item.name == _T("<Automatic>") || item.name == _("<Automatic>"))
			plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(aliasEvent, filteredFilenames);
		else
			plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(aliasEvent, item.name);
		if (plugin)
		{
			if (stack > 20)
			{
				errorMessage = strutils::format_string1(_("Circular reference in plugin pipeline: %1"), pluginPipeline);
				return pipelineResolved;
			}
			String pipeline = plugin->m_pipeline;
			for (size_t i = 0; i < 9; ++i)
				strutils::replace(pipeline, _T("${") + strutils::to_str(i + 1) + _T("}"), (i < item.args.size()) ? item.args[i] : _T(""));
			String args = PluginForFile::MakeArguments(item.args, {});
			strutils::replace(pipeline, _T("${*}"), args);
			auto parseResult2 = ExpandAliases(pipeline, filteredFilenames, aliasEvent, errorMessage, stack + 1);
			if (!errorMessage.empty())
				return pipelineResolved;
			for (auto& item2 : parseResult2)
				pipelineResolved.push_back(item2);
		}
		else
			pipelineResolved.push_back(item);
	}
	return pipelineResolved;
}

bool PackingInfo::GetPackUnpackPlugin(const String& filteredFilenames, bool bUrl, bool bReverse,
	std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, bool>>& plugins,
	String *pPluginPipelineResolved, String& errorMessage) const
{
	auto result = ExpandAliases(this->m_PluginPipeline, filteredFilenames, L"ALIAS_PACK_UNPACK", errorMessage);
	if (!errorMessage.empty())
		return false;
	if (bUrl)
	{
		uint8_t targetFlags = 0xff;
		std::vector<String> args;
		const auto filenames = strutils::split(filteredFilenames, '|');
		std::vector<std::pair<PluginInfo*, bool>> pluginInfos;
		for (int i = 0; i < filenames.size(); ++i)
		{
			bool bWithFile = true;
			const String filename{ filenames[i].data(), filenames[i].size() };
			PluginInfo* plugin = nullptr;
			plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"URL_PACK_UNPACK", filename);
			pluginInfos.push_back({ plugin, bWithFile });
		}
		if (!filenames.empty() &&
			std::all_of(pluginInfos.begin() + 1, pluginInfos.end(), 
				[&](const auto& elem) { return elem == pluginInfos.front(); }))
		{
			const auto& pluginInfo = pluginInfos.front();
			if (pluginInfo.first)
			{
				plugins.insert(bReverse ? plugins.begin() : plugins.end(),
					{ pluginInfo.first, targetFlags, args, pluginInfo.second });
			}
		}
		else
		{
			for (int i = 0; i < pluginInfos.size(); ++i)
			{
				const auto& pluginInfo = pluginInfos[i];
				if (isTargetInFlags(i, targetFlags) && pluginInfo.first)
				{
					uint8_t targetFlags2 = 1 << i;
					plugins.insert(bReverse ? plugins.begin() : plugins.end(),
						{ pluginInfo.first, targetFlags2, args, pluginInfo.second });
				}
			}
		}
	}
	std::vector<PluginForFile::PipelineItem> pipelineResolved;
	for (auto& [pluginName, targetFlags, args, quoteChar] : result)
	{
		PluginInfo* plugin = nullptr;
		if (pluginName == _T("<None>") || pluginName == _("<None>"))
			;
		else if (pluginName == _T("<Automatic>") || pluginName == _("<Automatic>"))
		{
			const auto filenames = strutils::split(filteredFilenames, '|');
			std::vector<std::pair<PluginInfo*, bool>> pluginInfos;
			for (int i = 0; i < filenames.size(); ++i)
			{
				bool bWithFile = true;
				const String filename{ filenames[i].data(), filenames[i].size() };
				plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_PACK_UNPACK", filename);
				if (plugin == nullptr)
					plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_FOLDER_PACK_UNPACK", filename);
				if (plugin == nullptr)
				{
					plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"BUFFER_PACK_UNPACK", filename);
					if (plugin)
						bWithFile = false;
				}
				pluginInfos.push_back({ plugin, bWithFile });
			}
			if (!filenames.empty() &&
				std::all_of(pluginInfos.begin() + 1, pluginInfos.end(), 
					[&](const auto& elem) { return elem == pluginInfos.front(); }))
			{
				const auto& pluginInfo = pluginInfos.front();
				if (pluginInfo.first)
				{
					pipelineResolved.push_back({ pluginInfo.first->m_name, targetFlags, args, quoteChar });
					plugins.insert(bReverse ? plugins.begin() : plugins.end(),
						{ pluginInfo.first, targetFlags, args, pluginInfo.second });
				}
			}
			else
			{
				for (int i = 0; i < pluginInfos.size(); ++i)
				{
					const auto& pluginInfo = pluginInfos[i];
					if (isTargetInFlags(i, targetFlags) && pluginInfo.first)
					{
						uint8_t targetFlags2 = 1 << i;
						pipelineResolved.push_back({ pluginInfo.first->m_name, targetFlags2, args, quoteChar });
						plugins.insert(bReverse ? plugins.begin() : plugins.end(),
							{ pluginInfo.first, targetFlags2, args, pluginInfo.second });
					}
				}
			}
		}
		else
		{
			bool bWithFile = true;
			plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_PACK_UNPACK", pluginName);
			if (plugin == nullptr)
			{
				plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_FOLDER_PACK_UNPACK", pluginName);
				if (plugin == nullptr)
				{
					plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"BUFFER_PACK_UNPACK", pluginName);
					if (plugin == nullptr)
					{
						plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(nullptr, pluginName);
						if (plugin == nullptr)
						{
							errorMessage = strutils::format_string1(_("Plugin not found or invalid: %1"), pluginName);
						}
						else
						{
							plugin = nullptr;
							errorMessage = strutils::format_string1(_("'%1' is not unpacker plugin"), pluginName);
						}
						return false;
					}
					bWithFile = false;
				}
			}
			if (plugin)
			{
				pipelineResolved.push_back({ plugin->m_name, targetFlags, args, quoteChar });
				plugins.insert(bReverse ? plugins.begin() : plugins.end(),
					{ plugin, targetFlags, args, bWithFile });
			}
		}
	}
	if (pPluginPipelineResolved)
		*pPluginPipelineResolved = MakePluginPipeline(pipelineResolved);
	return true;
}

// known handler
bool PackingInfo::pack(int target, String& filepath, const String& dstFilepath, const std::vector<int>& handlerSubcodes, const std::vector<StringView>& variables) const
{
	// no handler : return true
	bool bUrl = paths::IsURL(dstFilepath);
	if (m_PluginPipeline.empty() && !bUrl)
		return true;

	// control value
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, bool>> plugins;
	if (!GetPackUnpackPlugin(_T(""), bUrl, true, plugins, nullptr, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	if (m_bWebBrowser && m_PluginPipeline.empty())
		return true;

	auto itSubcode = handlerSubcodes.rbegin();
	for (auto& [plugin, targetFlags, args, bWithFile] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		bool bHandled = false;
		storageForPlugins bufferData;
		bufferData.SetDataFileAnsi(filepath);

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (plugin->m_hasVariablesProperty)
		{
			if (!plugin::InvokePutPluginVariables(strutils::to_str(variables[0]), piScript))
				return false;
		}
		if (plugin->m_hasArgumentsProperty)
		{
			if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : MakeArguments(args, variables), piScript))
				return false;
		}

		if (bWithFile)
		{
			// use a temporary dest name
			String srcFileName = bufferData.GetDataFileAnsi(); // <-Call order is important
			String dstFileName = plugin->m_event == L"URL_PACK_UNPACK" ?
				dstFilepath : bufferData.GetDestFileName(); // <-Call order is important
			bHandled = plugin::InvokePackFile(srcFileName,
				dstFileName,
				bufferData.GetNChanged(),
				piScript, *itSubcode);
			if (bHandled)
				bufferData.ValidateNewFile();
		}
		else
		{
			bHandled = plugin::InvokePackBuffer(*bufferData.GetDataBufferAnsi(),
				bufferData.GetNChanged(),
				piScript, *itSubcode);
			if (bHandled)
				bufferData.ValidateNewBuffer();
		}

		// if this packer does not work, that is an error
		if (!bHandled)
			return false;

		// if the buffer changed, write it before leaving
		if (bufferData.GetNChangedValid() > 0)
		{
			bool bSuccess = bufferData.SaveAsFile(filepath);
			if (!bSuccess)
				return false;
		}
		++itSubcode;
	}
	return true;
}

bool PackingInfo::Packing(int target, const String& srcFilepath, const String& dstFilepath, const std::vector<int>& handlerSubcodes, const std::vector<StringView>& variables) const
{
	String csTempFileName = srcFilepath;
	if (!pack(target, csTempFileName, dstFilepath, handlerSubcodes, variables))
		return false;
	try
	{
		if (!paths::IsURL(dstFilepath))
		{
			TFile file1(csTempFileName);
			file1.copyTo(dstFilepath);
			if (srcFilepath != csTempFileName)
				file1.remove();
		}
		return true;
	}
	catch (Poco::Exception& e)
	{
		DWORD dwErrCode = GetLastError();
		LogErrorStringUTF8(e.displayText());
		SetLastError(dwErrCode);
		return false;
	}
}

bool PackingInfo::Unpacking(int target, std::vector<int>* handlerSubcodes, String& filepath, const String& filteredText, const std::vector<StringView>& variables)
{
	if (handlerSubcodes)
		handlerSubcodes->clear();

	// no handler : return true
	bool bUrl = paths::IsURL(filepath);
	if (m_PluginPipeline.empty() && !bUrl)
		return true;

	// control value
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, bool>> plugins;
	if (!GetPackUnpackPlugin(filteredText, bUrl, false, plugins, &m_PluginPipeline, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	if (m_bWebBrowser && m_PluginPipeline.empty())
		return true;

	for (auto& [plugin, targetFlags, args, bWithFile] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		bool bHandled = false;
		storageForPlugins bufferData;
		bufferData.SetDataFileAnsi(filepath);

		// temporary subcode 
		int subcode = 0;

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (plugin->m_hasVariablesProperty)
		{
			if (!plugin::InvokePutPluginVariables(strutils::to_str(variables[0]), piScript))
				return false;
		}
		if (plugin->m_hasArgumentsProperty)
		{
			if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : MakeArguments(args, variables), piScript))
				return false;
		}

		if (bWithFile)
		{
			// use a temporary dest name
			bufferData.SetDestFileExtension(!plugin->m_ext.empty() ? plugin->m_ext : paths::FindExtension(filepath));
			String srcFileName = bufferData.GetDataFileAnsi(); // <-Call order is important
			String dstFileName = bufferData.GetDestFileName(); // <-Call order is important
			bHandled = plugin::InvokeUnpackFile(srcFileName,
				dstFileName,
				bufferData.GetNChanged(),
				piScript, subcode);
			if (bHandled)
				bufferData.ValidateNewFile();
		}
		else
		{
			bHandled = plugin::InvokeUnpackBuffer(*bufferData.GetDataBufferAnsi(),
				bufferData.GetNChanged(),
				piScript, subcode);
			if (bHandled)
				bufferData.ValidateNewBuffer();
		}

		// if this unpacker does not work, that is an error
		if (!bHandled)
			return false;

		// valid the subcode
		if (handlerSubcodes)
			handlerSubcodes->push_back(subcode);

		// if the buffer changed, write it before leaving
		if (bufferData.GetNChangedValid() > 0)
		{
			bool bSuccess = bufferData.SaveAsFile(filepath);
			if (!bSuccess)
				return false;
		}
	}
	return true;
}

String PackingInfo::GetUnpackedFileExtension(int target, const String& filteredFilenames, int& preferredWindowType) const
{
	preferredWindowType = -1;
	String ext;
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, bool>> plugins;
	if (GetPackUnpackPlugin(filteredFilenames, false, false, plugins, nullptr, errorMessage))
	{
		for (auto& [plugin, targetFlags, args, bWithFile] : plugins)
		{
			if (target != -1 && !isTargetInFlags(target, targetFlags))
				continue;

			ext += plugin->m_ext;
			auto preferredWindowTypeStr = plugin->GetExtendedPropertyValue(_T("PreferredWindowType"));
			if (preferredWindowTypeStr.has_value())
			{
				if (preferredWindowTypeStr == L"Text")
					preferredWindowType = 0;
				else if (preferredWindowTypeStr == L"Table")
					preferredWindowType = 1;
				else if (preferredWindowTypeStr == L"Binary")
					preferredWindowType = 2;
				else if (preferredWindowTypeStr == L"Image")
					preferredWindowType = 3;
				else if (preferredWindowTypeStr == L"Webpage")
					preferredWindowType = 4;
			}
		}
	}
	return ext;
}

////////////////////////////////////////////////////////////////////////////////
// transformation prediffing

bool PrediffingInfo::GetPrediffPlugin(const String& filteredFilenames, bool bReverse,
	std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, bool>>& plugins,
	String *pPluginPipelineResolved, String& errorMessage) const
{
	auto result = ExpandAliases(this->m_PluginPipeline, filteredFilenames, L"ALIAS_PREDIFF", errorMessage);
	if (!errorMessage.empty())
		return false;
	std::vector<PluginForFile::PipelineItem> pipelineResolved;
	for (auto& [pluginName, targetFlags, args, quoteChar] : result)
	{
		PluginInfo* plugin = nullptr;
		if (pluginName == _T("<None>") || pluginName == _("<None>"))
			;
		else if (pluginName == _T("<Automatic>") || pluginName == _("<Automatic>"))
		{
			const auto filenames = strutils::split(filteredFilenames, '|');
			std::vector<std::pair<PluginInfo*, bool>> pluginInfos;
			for (int i = 0; i < filenames.size(); ++i)
			{
				bool bWithFile = true;
				const String filename{ filenames[i].data(), filenames[i].size() };
				plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_PREDIFF", filename);
				if (plugin == nullptr)
				{
					plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"BUFFER_PREDIFF", filename);
					if (plugin)
						bWithFile = false;
				}
				pluginInfos.push_back({ plugin, bWithFile });
			}
			if (!filenames.empty() &&
				std::all_of(pluginInfos.begin() + 1, pluginInfos.end(), 
					[&](const auto& elem) { return elem == pluginInfos.front(); }))
			{
				const auto& pluginInfo = pluginInfos.front();
				if (pluginInfo.first)
				{
					pipelineResolved.push_back({ pluginInfo.first->m_name, targetFlags, args, quoteChar });
					plugins.insert(bReverse ? plugins.begin() : plugins.end(),
						{ pluginInfo.first, targetFlags, args, pluginInfo.second });
				}
			}
			else
			{
				for (int i = 0; i < pluginInfos.size(); ++i)
				{
					const auto& pluginInfo = pluginInfos[i];
					if (isTargetInFlags(i, targetFlags) && pluginInfo.first)
					{
						uint8_t targetFlags2 = 1 << i;
						pipelineResolved.push_back({ pluginInfo.first->m_name, targetFlags2, args, quoteChar });
						plugins.insert(bReverse ? plugins.begin() : plugins.end(),
							{ pluginInfo.first, targetFlags2, args, pluginInfo.second });
					}
				}
			}
		}
		else
		{
			bool bWithFile = true;
			plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_PREDIFF", pluginName);
			if (plugin == nullptr)
			{
				plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"BUFFER_PREDIFF", pluginName);
				if (plugin == nullptr)
				{
					plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(nullptr, pluginName);
					if (plugin == nullptr)
					{
						errorMessage = strutils::format_string1(_("Plugin not found or invalid: %1"), pluginName);
					}
					else
					{
						plugin = nullptr;
						errorMessage = strutils::format_string1(_("'%1' is not prediffer plugin"), pluginName);
					}
					return false;
				}
				bWithFile = false;
			}
			if (plugin)
			{
				pipelineResolved.push_back({ plugin->m_name, targetFlags, args, quoteChar });
				plugins.insert(bReverse ? plugins.begin() : plugins.end(),
					{ plugin, targetFlags, args, bWithFile });
			}
		}
	}
	if (pPluginPipelineResolved)
		*pPluginPipelineResolved = MakePluginPipeline(pipelineResolved);
	return true;
}

bool PrediffingInfo::Prediffing(int target, String & filepath, const String& filteredText, bool bMayOverwrite, const std::vector<StringView>& variables)
{
	// no handler : return true
	if (m_PluginPipeline.empty())
		return true;

	// control value
	bool bHandled = false;
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, bool>> plugins;
	if (!GetPrediffPlugin(filteredText, false, plugins, &m_PluginPipeline, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	for (const auto& [plugin, targetFlags, args, bWithFile] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		storageForPlugins bufferData;
		// detect Ansi or Unicode file
		bufferData.SetDataFileUnknown(filepath, bMayOverwrite);
		// TODO : set the codepage
		// bufferData.SetCodepage();

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (plugin->m_hasVariablesProperty)
		{
			if (!plugin::InvokePutPluginVariables(strutils::to_str(variables[0]), piScript))
				return false;
		}
		if (plugin->m_hasArgumentsProperty)
		{
			if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : MakeArguments(args, variables), piScript))
				return false;
		}

		if (bWithFile)
		{
			// use a temporary dest name
			String srcFileName = bufferData.GetDataFileAnsi(); // <-Call order is important
			String dstFileName = bufferData.GetDestFileName(); // <-Call order is important
			bHandled = plugin::InvokePrediffFile(srcFileName,
				dstFileName,
				bufferData.GetNChanged(),
				piScript);
			if (bHandled)
				bufferData.ValidateNewFile();
		}
		else
		{
			// probably it is for VB/VBscript so use a BSTR as argument
			bHandled = plugin::InvokePrediffBuffer(*bufferData.GetDataBufferUnicode(),
				bufferData.GetNChanged(),
				piScript);
			if (bHandled)
				bufferData.ValidateNewBuffer();
		}

		// if this unpacker does not work, that is an error
		if (!bHandled)
			return false;

		// if the buffer changed, write it before leaving
		if (bufferData.GetNChangedValid() > 0)
		{
			// bufferData changes filepath here to temp filepath
			bool bSuccess = bufferData.SaveAsFile(filepath);
			if (!bSuccess)
				return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// transformation text

bool EditorScriptInfo::GetEditorScriptPlugin(std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, int>>& plugins,
	String& errorMessage) const
{
	auto result = ExpandAliases(this->m_PluginPipeline, _T(""), L"ALIAS_EDITOR_SCRIPT", errorMessage);
	if (!errorMessage.empty())
		return false;
	for (auto& [pluginName, targetFlags, args, quoteChar] : result)
	{
		bool found = false;
		PluginArray *pluginInfoArray = CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"EDITOR_SCRIPT");
		for (const auto& plugin : *pluginInfoArray)
		{
			std::vector<String> namesArray;
			std::vector<int> idArray;
			int nFunc = plugin::GetMethodsFromScript(plugin->m_lpDispatch, namesArray, idArray);
			for (int i = 0; i < nFunc; ++i)
			{
				if (namesArray[i] == pluginName)
				{
					plugins.push_back({ plugin.get(), targetFlags, args, idArray[i]});
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
		if (!found)
		{
			errorMessage = strutils::format_string1(_("Plugin not found or invalid: %1"), pluginName);
			return false;
		}
	}
	return true;
}

bool EditorScriptInfo::TransformText(int target, String& text, const std::vector<StringView>& variables, bool& changed)
{
	changed = false;
	// no handler : return true
	if (m_PluginPipeline.empty())
		return true;

	// control value
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, uint8_t, std::vector<String>, int>> plugins;
	if (!GetEditorScriptPlugin(plugins, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	for (const auto& [plugin, targetFlags, args, fncID] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (plugin->m_hasVariablesProperty)
		{
			if (!plugin::InvokePutPluginVariables(strutils::to_str(variables[0]), piScript))
				return false;
		}
		if (plugin->m_hasArgumentsProperty)
		{
			if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : MakeArguments(args, variables), piScript))
				return false;
		}

		// execute the transform operation
		int nChanged = 0;
		if (!plugin::InvokeTransformText(text, nChanged, plugin->m_lpDispatch, fncID))
			return false;
		if (!changed)
			changed = (nChanged != 0);
	}
	return true;
}

namespace FileTransform
{

bool AutoUnpacking = false;
bool AutoPrediffing = false;

////////////////////////////////////////////////////////////////////////////////

bool AnyCodepageToUTF8(int codepage, String & filepath, bool bMayOverwrite)
{
	String tempDir = env::GetTemporaryPath();
	if (tempDir.empty())
		return false;
	String tempFilepath = env::GetTemporaryFileName(tempDir, _T("_W3"));
	if (tempFilepath.empty())
		return false;
	// TODO : is it better with the BOM or without (just change the last argument)
	int nFileChanged = 0;
	bool bSuccess = ::AnyCodepageToUTF8(codepage, filepath, tempFilepath, nFileChanged, false); 
	if (bSuccess && nFileChanged!=0)
	{
		// we do not overwrite so we delete the old file
		if (bMayOverwrite)
		{
			try
			{
				TFile(filepath).remove();
			}
			catch (Exception& e)
			{
				LogErrorStringUTF8(e.displayText());
			}
		}
		// and change the filepath if everything works
		filepath = tempFilepath;
	}
	else
	{
		try
		{
			TFile(tempFilepath).remove();
		}
		catch (Exception& e)
		{
			LogErrorStringUTF8(e.displayText());
		}
	}

	return bSuccess;
}

std::pair<
	std::vector<std::tuple<String, String, unsigned, PluginInfo *>>,
	std::map<String, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>>
>
CreatePluginMenuInfos(const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned baseId)
{
	std::vector<std::tuple<String, String, unsigned, PluginInfo *>> suggestedPlugins;
	std::map<String, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>> allPlugins;
	std::map<String, int> captions;
	unsigned id = baseId + 2;
	bool addedNoneAutomatic = false;
	static PluginInfo noPlugin;
	static PluginInfo autoPlugin;
	auto tr2 = [](const String& text)
	{
		const bool containsNonAsciiChars = std::any_of(text.begin(), text.end(), [](auto c) { return (c >= 0x80); });
		return containsNonAsciiChars ? text : tr(ucr::toUTF8(strutils::to_str(text)));
	};
	if (autoPlugin.m_name.empty())
		autoPlugin.m_name = _T("<Automatic>");
	for (const auto& event: events)
	{
		auto pScriptArray =
			CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(event.c_str());
		for (auto& plugin : *pScriptArray)
		{
			if (!plugin->m_disabled)
			{
				if (event != L"EDITOR_SCRIPT")
				{
					if (!addedNoneAutomatic)
					{
						String process = _T("");
						allPlugins.insert_or_assign(process, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>());
						allPlugins[process].emplace_back(_("<None>"), _T(""), baseId, &noPlugin);
						allPlugins[process].emplace_back(_("<Automatic>"), _T("<Automatic>"), baseId + 1, &autoPlugin);
						addedNoneAutomatic = true;
					}
					const auto menuCaption = plugin->GetExtendedPropertyValue(_T("MenuCaption"));
					const auto processType = plugin->GetExtendedPropertyValue(_T("ProcessType"));
					const String caption = tr2(menuCaption.has_value() ? strutils::to_str(*menuCaption) : plugin->m_name);
					const String process = tr2(processType.has_value() ? strutils::to_str(*processType) : _T("&Others"));

					if (plugin->TestAgainstRegList(filteredFilenames))
						suggestedPlugins.emplace_back(caption, plugin->m_name, id, plugin.get());

					if (allPlugins.find(process) == allPlugins.end())
						allPlugins.insert_or_assign(process, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>());
					allPlugins[process].emplace_back(caption, plugin->m_name, id, plugin.get());
					captions[caption]++;
					id++;
				}
				else
				{
					if (!addedNoneAutomatic)
					{
						String process = _T("");
						allPlugins.insert_or_assign(process, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>());
						allPlugins[process].emplace_back(_("<None>"), _T(""), baseId, &noPlugin);
						addedNoneAutomatic = true;
					}
					LPDISPATCH piScript = plugin->m_lpDispatch;
					std::vector<String> scriptNamesArray;
					std::vector<int> scriptIdsArray;
					int nScriptFnc = plugin::GetMethodsFromScript(piScript, scriptNamesArray, scriptIdsArray);
					bool matched = plugin->TestAgainstRegList(filteredFilenames);
					for (int i = 0; i < nScriptFnc; ++i, ++id)
					{
						if (scriptNamesArray[i] == L"PluginOnEvent" || scriptNamesArray[i] == L"ShowSettingsDialog")
							continue;
						const auto menuCaption = plugin->GetExtendedPropertyValue(scriptNamesArray[i] + _T(".MenuCaption"));
						auto processType = plugin->GetExtendedPropertyValue(scriptNamesArray[i] + _T(".ProcessType"));
						if (!processType.has_value())
							processType = plugin->GetExtendedPropertyValue(_T("ProcessType"));
						const String caption = tr2(menuCaption.has_value() ? strutils::to_str(*menuCaption) : scriptNamesArray[i]);
						const String process = tr2(processType.has_value() ? strutils::to_str(*processType) : _T("&Others"));
						if (matched)
							suggestedPlugins.emplace_back(caption, scriptNamesArray[i], id, plugin.get());
						if (allPlugins.find(process) == allPlugins.end())
							allPlugins.insert_or_assign(process, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>());
						allPlugins[process].emplace_back(caption, scriptNamesArray[i], id, plugin.get());
					}
				}
			}
		}
	}
	auto ResolveConflictMenuCaptions = [&captions](auto& plugins)
	{
		for (auto& plugin : plugins)
		{
			const String& caption = std::get<0>(plugin);
			if (captions[caption] > 1)
				std::get<0>(plugin) = caption + _T("(") + std::get<1>(plugin) + _T(")");
		}
	};
	ResolveConflictMenuCaptions(suggestedPlugins);
	for (auto& [processType, plugins] : allPlugins)
		ResolveConflictMenuCaptions(plugins);
	return { suggestedPlugins, allPlugins };
}

}

////////////////////////////////////////////////////////////////////////////////
