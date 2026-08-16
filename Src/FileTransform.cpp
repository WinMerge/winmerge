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
#include "Logger.h"
#include "MergeApp.h"
#include "DiffContext.h"
#include "UniFile.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "codepage_detect.h"
#include "FilterEngine/FilterExpression.h"
#include "FilterEngine/ILineDataProvider.h"
#include "FilterErrorMessages.h"

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

/**
 * @brief Set the Variables/Arguments properties on a plugin's script dispatch, if it has them
 *
 * @note Caller must already hold g_mutex before invoking this
 *
 * Shared by PackingInfo::Unpacking(), PackingInfo::pack(), PrediffingInfo::Prediffing()
 * and EditorScriptInfo::TransformText(), which all set these two properties identically
 * before invoking the plugin.
 */
static bool SetPluginVariablesAndArguments(PluginInfo* plugin, LPDISPATCH piScript,
	const std::vector<String>& args, const std::vector<StringView>& variables)
{
	if (plugin->m_hasVariablesProperty)
	{
		if (!plugin::InvokePutPluginVariables(strutils::to_str(variables[0]), piScript))
			return false;
	}
	if (plugin->m_hasArgumentsProperty)
	{
		if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : PluginForFile::MakeArguments(args, variables), piScript))
			return false;
	}
	return true;
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
	errorMessage.clear();

	// Split the pipeline into items by '|', ignoring '|' inside quotes.
	std::vector<String> items;
	String item;
	bool inQuotes = false;
	tchar_t quoteChar = 0;

	{
		const tchar_t* p = pluginPipeline.c_str();
		while (tc::istspace(*p))
			++p;

		if (!*p)
			return result;
	}

	for (const tchar_t* p = pluginPipeline.c_str(); *p; ++p)
	{
		if (inQuotes)
		{
			item += *p;
			if (*p == quoteChar)
			{
				if (*(p + 1) == quoteChar)
					item += *++p;
				else
					inQuotes = false;
			}
		}
		else
		{
			if (*p == '"' || *p == '\'')
			{
				inQuotes = true;
				quoteChar = *p;
				item += *p;
			}
			else if (*p == '|')
			{
				items.push_back(item);
				item.clear();
			}
			else
			{
				item += *p;
			}
		}
	}

	if (inQuotes)
	{
		errorMessage = strutils::format_string1(_("Missing quote in plugin pipeline: %1"), pluginPipeline);
		return result;
	}

	items.push_back(item);

	for (const auto& pipelineItem : items)
	{
		const tchar_t* p = pipelineItem.c_str();
		while (tc::istspace(*p))
			++p;

		if (!*p)
		{
			errorMessage = strutils::format_string1(_("Missing plugin name in pipeline: %1"), pluginPipeline);
			return result;
		}

		uint8_t targetFlags = 0xff;
		String token, name;

		// Line expression:
		//   le:pane:expression
		if (tc::tcsncmp(p, _T("le:"), 3) == 0)
		{
			const tchar_t* expression = p + 3;
			p += 3;

			const tchar_t* q = expression;
			while (*q == '1' || *q == '2' || *q == '3' || *q == ',')
				++q;

			if (q != expression && *q == ':')
			{
				const String target(expression, q);

				targetFlags = 0;
				for (const tchar_t ch : target)
				{
					if (ch >= '1' && ch <= '3')
						targetFlags |= 1 << (ch - '1');
				}

				if (targetFlags == 0)
					targetFlags = 0xff;

				expression = q + 1;
			}

			while (tc::istspace(*expression))
				++expression;

			if (!*expression)
			{
				errorMessage = strutils::format_string1(_("Missing line expression in pipeline: %1"), pluginPipeline);
				return result;
			}

			result.push_back(FilterExpressionPipelineItem{ targetFlags, expression });
			continue;
		}

		// Parse a normal plugin pipeline item.
		inQuotes = false;
		quoteChar = 0;
		std::vector<String> args;

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
					if (!token.empty())
					{
						if (name.empty())
							std::tie(name, targetFlags) = parseNameAndTargetFlags(token);
						else
							args.push_back(token);
						token.clear();
					}

					while (tc::istspace(*p))
						++p;
					continue;
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

		if (inQuotes)
		{
			errorMessage = strutils::format_string1(_("Missing quote in plugin pipeline: %1"), pluginPipeline);
			return result;
		}

		if (!token.empty())
		{
			if (name.empty())
			{
				std::tie(name, targetFlags) = parseNameAndTargetFlags(token);
			}
			else
			{
				args.push_back(token);
			}
		}

		if (name.empty())
		{
			errorMessage = strutils::format_string1(_("Missing plugin name in pipeline: %1"), pluginPipeline);
			return result;
		}

		result.push_back(PluginPipelineItem{ targetFlags, name, args, quoteChar });
	}

	return result;
}

String PluginForFile::MakePluginPipeline(const std::vector<PluginForFile::PipelineItem>& list)
{
	int i = 0;
	String pipeline;
	for (const auto& pipelineItem : list)
	{
		if (std::holds_alternative<FilterExpressionPipelineItem>(pipelineItem))
		{
			const auto& lineExpressionItem = std::get<FilterExpressionPipelineItem>(pipelineItem);
			pipeline += _T("le");
			if (lineExpressionItem.targetFlags != 0xff)
				pipeline += makeTargetsPrefix(lineExpressionItem.targetFlags);
			pipeline += _T(":") + lineExpressionItem.expression;
		}
		else
		{
			const auto& pluginItem = std::get<PluginPipelineItem>(pipelineItem);
			if (pluginItem.quoteChar && pluginItem.name.find_first_of(_T(" '\"")) != String::npos)
			{
				String nameQuoted = pluginItem.name;
				strutils::replace(nameQuoted, String(1, pluginItem.quoteChar), String(2, pluginItem.quoteChar));
				pipeline += strutils::format(_T("%c%s%c"), pluginItem.quoteChar, nameQuoted, pluginItem.quoteChar);
			}
			else
			{
				pipeline += pluginItem.name;
			}
			pipeline += makeTargetsPrefix(pluginItem.targetFlags);
			for (const auto& arg : pluginItem.args)
			{
				if (pluginItem.quoteChar)
				{
					String argQuoted = arg;
					strutils::replace(argQuoted, String(1, pluginItem.quoteChar), String(2, pluginItem.quoteChar));
					pipeline += strutils::format(_T(" %c%s%c"), pluginItem.quoteChar, argQuoted, pluginItem.quoteChar);
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
		if (newarg.empty())
		{
			newstr += _T("\"\"");
		}
		else if (newarg.find_first_of(_T(" \t\"")) != String::npos)
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

template <typename PluginTuple, typename ExpandFunc>
static bool ExpandPluginAliases(
	const String& aliasEvent, std::vector<PluginTuple>& plugins, String& errorMessage, int stack, ExpandFunc&& expandFunc)
{
	std::vector<PluginTuple> plugins2;

	for (auto& [plugin, expressions, targetFlags, args, bWithFile] : plugins)
	{
		if (plugin && plugin->m_event == aliasEvent)
		{
			if (stack > 20)
			{
				errorMessage = strutils::format_string1(_("Circular reference in plugin pipeline: %1"), plugin->m_pipeline);
				return false;
			}

			String pipeline = plugin->m_pipeline;
			for (size_t i = 0; i < 9; ++i)
				strutils::replace(pipeline, _T("${") + strutils::to_str(i + 1) + _T("}"), (i < args.size()) ? args[i] : _T(""));

			String argsStr = PluginForFile::MakeArguments(args, {});
			strutils::replace(pipeline, _T("${*}"), argsStr);

			std::vector<PluginTuple> newPlugins;
			if (!expandFunc(pipeline, newPlugins, errorMessage, stack + 1))
				return false;

			plugins2.insert(plugins2.end(), newPlugins.begin(), newPlugins.end());
		}
		else
		{
			plugins2.push_back({ plugin, expressions, targetFlags, args, bWithFile });
		}
	}

	std::swap(plugins, plugins2);
	return true;
}

struct PackUnpackPluginTraits
{
	// fallback lookup order when resolving a named or <Automatic> plugin;
	// the "true" entries are BUFFER_* events (bWithFile = false for those)
	static const std::vector<std::pair<const wchar_t*, bool>>& LookupChain()
	{
		static const std::vector<std::pair<const wchar_t*, bool>> chain = {
			{ L"FILE_PACK_UNPACK", false },
			{ L"FILE_FOLDER_PACK_UNPACK", false },
			{ L"ALIAS_PACK_UNPACK", false },
			{ L"BUFFER_PACK_UNPACK", true },
		};
		return chain;
	}
	static const String& AliasEvent()
	{
		static const String event = _T("ALIAS_PACK_UNPACK");
		return event;
	}
	static String NotThisKindMessage(const String& pluginName)
	{
		return strutils::format_string1(_("'%1' is not an unpacker plugin"), pluginName);
	}
};

struct PrediffPluginTraits
{
	static const std::vector<std::pair<const wchar_t*, bool>>& LookupChain()
	{
		static const std::vector<std::pair<const wchar_t*, bool>> chain = {
			{ L"FILE_PREDIFF", false },
			{ L"ALIAS_PREDIFF", false },
			{ L"BUFFER_PREDIFF", true },
		};
		return chain;
	}
	static const String& AliasEvent()
	{
		static const String event = _T("ALIAS_PREDIFF");
		return event;
	}
	static String NotThisKindMessage(const String& pluginName)
	{
		return strutils::format_string1(_("'%1' is not a prediffer plugin"), pluginName);
	}
};

/**
 * @brief Resolve an already-parsed plugin pipeline (<None>/<Automatic>/named entries and
 *        line-expression items) into the concrete plugin list. Shared by PackingInfo and
 *        PrediffingInfo, which only differ in which plugin events they search (see Traits).
 *
 * @note Unlike the two hand-written versions this replaces, the "consecutive line-expression
 *        merge" below is always bReverse-aware (matches what PrediffingInfo::GetPrediffPlugin
 *        used to do); the old PackingInfo::GetPackUnpackPlugin unconditionally used
 *        plugins.back(), which was wrong when bReverse is true (as in PackingInfo::pack()).
 */
template <typename Traits, typename RecurseFunc>
static bool ResolvePluginPipeline(const std::vector<PluginForFile::PipelineItem>& result,
	const String& filteredFilenames, bool bReverse,
	std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, bool>>& plugins,
	String* pPluginPipelineResolved, String& errorMessage, int stack, RecurseFunc&& recurse)
{
	std::vector<PluginForFile::PipelineItem> pipelineResolved;
	for (size_t i = 0; i < result.size(); ++i)
	{
		auto& pipelineItem = result[i];
		if (std::holds_alternative<PluginForFile::FilterExpressionPipelineItem>(pipelineItem))
		{
			auto& [ targetFlags, expression ] = std::get<PluginForFile::FilterExpressionPipelineItem>(pipelineItem);
			if (i == 0 || std::holds_alternative<PluginForFile::FilterExpressionPipelineItem>(result[i - 1]) || 
				std::get<PluginForFile::FilterExpressionPipelineItem>(result[i - 1]).targetFlags != targetFlags)
			{
				pipelineResolved.push_back( PluginForFile::FilterExpressionPipelineItem{ targetFlags, expression });
				plugins.insert(bReverse ? plugins.begin() : plugins.end(),
					{ nullptr, { expression }, targetFlags, {}, true });
			}
			else
			{
				auto& lastPlugin = bReverse ? plugins.front() : plugins.back();
				std::get<1>(lastPlugin).push_back(expression);
			}
			continue;
		}
		auto& [ targetFlags, pluginName, args, quoteChar] = PluginForFile::GetPluginPipelineItem(pipelineItem);
		PluginInfo* plugin = nullptr;
		if (pluginName == _T("<None>") || pluginName == _("<None>"))
			;
		else if (pluginName == _T("<Automatic>") || pluginName == _("<Automatic>"))
		{
			const auto filenames = strutils::split(filteredFilenames, '|');
			std::vector<std::pair<PluginInfo*, bool>> pluginInfos;
			for (int j = 0; j < filenames.size(); ++j)
			{
				bool bWithFile = true;
				const String filename{ filenames[j].data(), filenames[j].size() };
				for (const auto& [eventName, isBufferEvent] : Traits::LookupChain())
				{
					plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(eventName, filename);
					if (plugin)
					{
						bWithFile = !isBufferEvent;
						break;
					}
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
					pipelineResolved.push_back(PluginForFile::PluginPipelineItem{ targetFlags, pluginInfo.first->m_name, args, quoteChar });
					plugins.insert(bReverse ? plugins.begin() : plugins.end(),
						{ pluginInfo.first, {}, targetFlags, args, pluginInfo.second });
				}
			}
			else
			{
				for (int j = 0; j < pluginInfos.size(); ++j)
				{
					const auto& pluginInfo = pluginInfos[j];
					if (isTargetInFlags(j, targetFlags) && pluginInfo.first)
					{
						uint8_t targetFlags2 = 1 << j;
						pipelineResolved.push_back(PluginForFile::PluginPipelineItem{ targetFlags2, pluginInfo.first->m_name, args, quoteChar });
						plugins.insert(bReverse ? plugins.begin() : plugins.end(),
							{ pluginInfo.first, {}, targetFlags2, args, pluginInfo.second });
					}
				}
			}
		}
		else
		{
			bool bWithFile = true;
			for (const auto& [eventName, isBufferEvent] : Traits::LookupChain())
			{
				plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(eventName, pluginName);
				if (plugin)
				{
					bWithFile = !isBufferEvent;
					break;
				}
			}
			if (!plugin)
			{
				if (CAllThreadsScripts::GetActiveSet()->GetPluginByName(nullptr, pluginName) == nullptr)
					errorMessage = strutils::format_string1(_("Plugin not found or invalid: %1"), pluginName);
				else
					errorMessage = Traits::NotThisKindMessage(pluginName);
				return false;
			}
			pipelineResolved.push_back(PluginForFile::PluginPipelineItem{ targetFlags, plugin->m_name, args, quoteChar });
			plugins.insert(bReverse ? plugins.begin() : plugins.end(),
				{ plugin, {}, targetFlags, args, bWithFile });
		}
	}
	if (pPluginPipelineResolved)
		*pPluginPipelineResolved = PluginForFile::MakePluginPipeline(pipelineResolved);

	return ExpandPluginAliases(Traits::AliasEvent(), plugins, errorMessage, stack, std::forward<RecurseFunc>(recurse));
}

bool PackingInfo::GetPackUnpackPlugin(const String& filteredFilenames, bool bUrl, bool bReverse,
	std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, bool>>& plugins,
	String *pPluginPipelineResolved, String& errorMessage, int stack) const
{
	auto result = PluginForFile::ParsePluginPipeline(m_PluginPipeline, errorMessage);
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
					{ pluginInfo.first, {}, targetFlags, args, pluginInfo.second });
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
						{ pluginInfo.first, {}, targetFlags2, args, pluginInfo.second });
				}
			}
		}
	}

	return ResolvePluginPipeline<PackUnpackPluginTraits>(result, filteredFilenames, bReverse,
		plugins, pPluginPipelineResolved, errorMessage, stack,
		[&](const String& pipeline, auto& newPlugins, String& error, int nextStack)
		{
			return PackingInfo(pipeline).GetPackUnpackPlugin(filteredFilenames, false, bReverse, newPlugins, nullptr, error, nextStack);
		});
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
	std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, bool>> plugins;
	if (!GetPackUnpackPlugin(_T(""), bUrl, true, plugins, nullptr, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	if (m_bWebBrowser && m_PluginPipeline.empty())
		return true;

	auto itSubcode = handlerSubcodes.rbegin();
	for (auto& [plugin, expressions, targetFlags, args, bWithFile] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		if (!plugin)
			continue;

		bool bHandled = false;
		storageForPlugins bufferData;
		bufferData.SetDataFileAnsi(filepath);

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (!SetPluginVariablesAndArguments(plugin, piScript, args, variables))
			return false;

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
		RootLogger::Error(e.displayText());
		SetLastError(dwErrCode);
		return false;
	}
}

class LineDataProvider: public ILineDataProvider
{
public:
	LineDataProvider(std::vector<std::string> lines, const FileTextEncoding& encoding)
	 : m_lines(std::move(lines)), m_encoding(encoding) {}
	int GetLineCount() const
	{
		return static_cast<int>(m_lines.size());
	}
	std::string GetLine(int pane, int lineIndex) const
	{
		if (lineIndex < 0 || lineIndex >= m_lines.size())
			return {};
		auto& line = m_lines[lineIndex];
		return line.substr(0, line.size() - GetEolLength(line));
	}
	int GetColumnCount(int pane, int lineIndex) const
	{
		return 0;
	}
	std::string GetColumn(int pane, int lineIndex, int columnIndex) const
	{
		return GetLine(pane, lineIndex);
	}
	int GetRealLineNumber(int pane, int lineIndex) const
	{
		return lineIndex;
	}
	unsigned GetLineFlags(int pane, int lineIndex) const
	{
		return 0;
	}
	unsigned GetLineEol(int pane, int lineIndex) const
	{
		if (lineIndex < 0 || lineIndex >= m_lines.size())
			return ILineDataProvider::EOL_NONE;
		const std::string& line = m_lines[lineIndex];
		return GetEolType(line);
	}

	int GetEolLength(const std::string& line) const
	{
		EOLFLAGS eolType = GetEolType(line);
		if (eolType == EOL_CRLF)
			return 2;
		else if (eolType == EOL_LF || eolType == EOL_CR)
			return 1;
		else
			return 0;
	}
	ILineDataProvider::EOLFLAGS GetEolType(const std::string& line) const
	{
		if (line.empty())
			return ILineDataProvider::EOL_NONE;
		if (line.back() == '\n')
		{
			if (line.size() >= 2 && line[line.size() - 2] == '\r')
				return ILineDataProvider::EOL_CRLF;
			return ILineDataProvider::EOL_LF;
		}
		if (line.back() == '\r')
			return ILineDataProvider::EOL_CR;
		return ILineDataProvider::EOL_NONE;
	}
	const std::string& GetFullLine(int pane, int lineIndex) const
	{
		return m_lines[lineIndex];
	}
	void SetLine(int pane, int lineIndex, const std::string& line)
	{
		if (lineIndex < 0 || lineIndex >= m_lines.size())
			return;
		const std::string& oldLine = m_lines[lineIndex];
		const std::string eol = oldLine.substr(oldLine.size() - GetEolLength(oldLine));
		m_lines[lineIndex] = line + eol;
	}
	const FileTextEncoding& GetEncoding() const
	{
		return m_encoding;
	}

private:
	std::vector<std::string> m_lines;
	FileTextEncoding m_encoding;
};

static std::unique_ptr<LineDataProvider> CreateFileLineDataProvider(const String& filepath)
{
	UniMemFile file;
	if (!file.OpenReadOnly(filepath))
		return nullptr;
	file.ReadBom();
	FileTextEncoding encoding;
	if (!file.HasBom())
	{
		int iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
		int64_t fileSize = file.GetFileSize();
		encoding = codepage_detect::Guess(
			paths::FindExtension(filepath), file.GetBase(), static_cast<size_t>(
				fileSize < static_cast<int64_t>(codepage_detect::BufSize) ?
				fileSize : static_cast<int64_t>(codepage_detect::BufSize)),
			iGuessEncodingType);
		file.SetCodepage(encoding.m_codepage);
	}
	else
	{
		encoding.SetUnicoding(file.GetUnicoding());
		encoding.m_bom = true;
	}
	std::vector<std::string> lines;
	bool linesToRead = true;
	do
	{
		bool lossy;
		String line, eol;
		linesToRead = file.ReadString(line, eol, &lossy);
		lines.push_back(ucr::toUTF8(line + eol));
	} while (linesToRead);
	return std::make_unique<LineDataProvider>(std::move(lines), encoding);
}

static std::unique_ptr<LineDataProvider> CreateTextLineDataProvider(const String& Text)
{
	FileTextEncoding encoding;
	encoding.SetUnicoding(ucr::UTF8);
	encoding.m_bom = true;
	std::vector<std::string> lines;
	const tchar_t* pLineBegin = Text.c_str();
	for (const tchar_t* p = pLineBegin; *p; )
	{
		if (*p == '\r' && *(p + 1) == '\n')
		{
			p += 2;
			lines.push_back(ucr::toUTF8(String(pLineBegin, p)));
			pLineBegin = p;
			continue;
		}
		else if (*p == '\r' || *p == '\n')
		{
			++p;
			lines.push_back(ucr::toUTF8(String(pLineBegin, p)));
			pLineBegin = p;
			continue;
		}
		else
			++p;
	}
	if (*pLineBegin != 0)
		lines.push_back(ucr::toUTF8(String(pLineBegin)));
	return std::make_unique<LineDataProvider>(std::move(lines), encoding);
}

static std::pair<std::unique_ptr<CDiffContext>, std::unique_ptr<DIFFITEM>> CreateDiffItem(const String& filepath, const FileTextEncoding& encoding)
{
	PathContext paths;
	auto pdi = std::make_unique<DIFFITEM>();
	pdi->diffcode.diffcode = DIFFCODE::TEXT | DIFFCODE::FILE;
	pdi->diffcode.setSideFlag(0);
	paths.SetPath(0, paths::GetParentPath(filepath));
	pdi->diffFileInfo[0].SetFile(paths::FindFileName(filepath));
	pdi->diffFileInfo[0].Update(filepath);
	pdi->diffFileInfo[0].encoding = encoding;
	auto result = std::make_pair<std::unique_ptr<CDiffContext>, std::unique_ptr<DIFFITEM>>(std::make_unique<CDiffContext>(paths, 0), std::move(pdi));
	return result;
}

/**
 * @brief Parsed FilterExpression + FilterEvalContext pairs for one filter pipeline, plus the
 *        CDiffContext/DIFFITEM they point into. The diff context must outlive the
 *        FilterExpression objects (they hold a raw pointer to it), so callers keep this alive
 *        for as long as they keep using the FilterExpression/FilterEvalContext vectors.
 */
struct FilterExpressionSet
{
	std::vector<FilterExpression> filterExpressions;
	std::vector<FilterEvalContext> evalContexts;
	std::unique_ptr<CDiffContext> diffContext;
	std::unique_ptr<DIFFITEM> diffItem;
};

static std::optional<FilterExpressionSet> BuildFilterExpressionSet(const String& filepath, const FileTextEncoding& encoding,
	const std::vector<String>& expressions, const LineDataProvider& lineDataProvider, String& errorMessage)
{
	FilterExpressionSet set;
	FilterExpression::SetLogger([](int level, const std::string& msg) {
		if (level == 0)
			RootLogger::Error(msg);
		else if (level == 1)
			RootLogger::Warn(msg);
		else
			RootLogger::Info(msg);
	});
	auto [diffContext, diffItem] = CreateDiffItem(filepath, encoding);
	set.diffContext = std::move(diffContext);
	set.diffItem = std::move(diffItem);
	for (const auto& expression : expressions)
	{
		set.filterExpressions.emplace_back(ucr::toUTF8(expression));
		auto& lastFilterExpression = set.filterExpressions.back();
		lastFilterExpression.SetDiffContext(set.diffContext.get());
		if (!lastFilterExpression.Parse())
		{
			errorMessage = FormatFilterErrorSummary(lastFilterExpression);
			return std::nullopt;
		}
		set.evalContexts.emplace_back();
		auto& lastEvalContext = set.evalContexts.back();
		lastEvalContext.provider = &lineDataProvider;
		lastEvalContext.expr = &lastFilterExpression;
		lastEvalContext.di = set.diffItem.get();
	}
	return set;
}

static bool TransformLines(LineDataProvider& lineDataProvider,
	std::vector<FilterExpression>& filterExpressions, std::vector<FilterEvalContext>& evalContexts, String& errorMessage)
{
	int lineCount = lineDataProvider.GetLineCount();
	FilterSharedContext sharedContext[3];
	for (int i = 0; i < lineCount; ++i)
	{
		for (size_t j = 0; j < filterExpressions.size(); ++j)
		{
			auto& filterExpression = filterExpressions[j];
			auto& evalContext = evalContexts[j];
			evalContext.lineIndex = i;
			evalContext.sharedContext = &sharedContext[j];
			std::string line = filterExpression.TransformLine(evalContext);
			if (filterExpression.errorCode != FILTER_ERROR_NO_ERROR)
			{
				errorMessage = ucr::toTString(filterExpression.errorMessage);
				return false;
			}
			lineDataProvider.SetLine(0, i, line);
		}
	}
	return true;
}

static String ApplyFilterExpressionsToFile(const String& filepath, bool bMayOverwrite, const std::vector<String>& expressions, String& errorMessage)
{
	auto lineDataProvider = CreateFileLineDataProvider(filepath);
	if (!lineDataProvider)
	{
		errorMessage = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), filepath, GetSysError());
		return _T("");
	}
	auto exprSet = BuildFilterExpressionSet(filepath, lineDataProvider->GetEncoding(), expressions, *lineDataProvider, errorMessage);
	if (!exprSet)
		return _T("");
	if (!TransformLines(*lineDataProvider, exprSet->filterExpressions, exprSet->evalContexts, errorMessage))
		return _T("");

	String tempPath = bMayOverwrite ? filepath : env::GetTemporaryFileName(env::GetTemporaryPath(), _T("WM"));
	UniStdioFile file;
	file.OpenCreateUtf8(tempPath);
	int lineCount = lineDataProvider->GetLineCount();
	for (int i = 0; i < lineCount; ++i)
		file.WriteString(ucr::toTString(lineDataProvider->GetFullLine(0, i)));
	return tempPath;
}

static String ApplyFilterExpressionsToString(const String& text, const std::vector<String>& expressions, const String& filepath, String& errorMessage)
{
	auto lineDataProvider = CreateTextLineDataProvider(text);
	if (!lineDataProvider)
	{
		errorMessage = _T("Failed to read text: ") + GetSysError();
		return _T("");
	}
	auto exprSet = BuildFilterExpressionSet(filepath, lineDataProvider->GetEncoding(), expressions, *lineDataProvider, errorMessage);
	if (!exprSet)
		return _T("");
	if (!TransformLines(*lineDataProvider, exprSet->filterExpressions, exprSet->evalContexts, errorMessage))
		return _T("");

	String result;
	int lineCount = lineDataProvider->GetLineCount();
	for (int i = 0; i < lineCount; ++i)
		result += ucr::toTString(lineDataProvider->GetFullLine(0, i));
	return result;
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
	std::vector < std::tuple < PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, bool >> plugins;
	if (!GetPackUnpackPlugin(filteredText, bUrl, false, plugins, &m_PluginPipeline, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	if (m_bWebBrowser && m_PluginPipeline.empty())
		return true;

	for (auto& [plugin, expressions, targetFlags, args, bWithFile] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		if (!expressions.empty())
		{
			filepath = ApplyFilterExpressionsToFile(filepath, false, expressions, errorMessage);
			if (!errorMessage.empty())
			{
				AppErrorMessageBox(errorMessage);
				return false;
			}
			continue;
		}

		bool bHandled = false;
		storageForPlugins bufferData;
		bufferData.SetDataFileAnsi(filepath);

		// temporary subcode 
		int subcode = 0;

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (!SetPluginVariablesAndArguments(plugin, piScript, args, variables))
			return false;

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
	std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, bool>> plugins;
	if (GetPackUnpackPlugin(filteredFilenames, false, false, plugins, nullptr, errorMessage))
	{
		for (auto& [plugin, expressions, targetFlags, args, bWithFile] : plugins)
		{
			if ((target != -1 && !isTargetInFlags(target, targetFlags)) || !plugin)
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
	std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, bool>>& plugins,
	String *pPluginPipelineResolved, String& errorMessage, int stack) const
{
	auto result = PluginForFile::ParsePluginPipeline(m_PluginPipeline, errorMessage);
	if (!errorMessage.empty())
		return false;

	return ResolvePluginPipeline<PrediffPluginTraits>(result, filteredFilenames, bReverse,
		plugins, pPluginPipelineResolved, errorMessage, stack,
		[&](const String& pipeline, auto& newPlugins, String& error, int nextStack)
		{
			return PrediffingInfo(pipeline).GetPrediffPlugin(filteredFilenames, bReverse, newPlugins, nullptr, error, nextStack);
		});
}

bool PrediffingInfo::Prediffing(int target, String & filepath, const String& filteredText, bool bMayOverwrite, const std::vector<StringView>& variables)
{
	// no handler : return true
	if (m_PluginPipeline.empty())
		return true;

	// control value
	bool bHandled = false;
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, bool>> plugins;
	if (!GetPrediffPlugin(filteredText, false, plugins, &m_PluginPipeline, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	for (const auto& [plugin, expressions, targetFlags, args, bWithFile] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		if (!expressions.empty())
		{
			filepath = ApplyFilterExpressionsToFile(filepath, bMayOverwrite, expressions, errorMessage);
			if (!errorMessage.empty())
			{
				AppErrorMessageBox(errorMessage);
				return false;
			}
			continue;
		}

		storageForPlugins bufferData;
		// detect Ansi or Unicode file
		bufferData.SetDataFileUnknown(filepath, bMayOverwrite);
		// TODO : set the codepage
		// bufferData.SetCodepage();

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (!SetPluginVariablesAndArguments(plugin, piScript, args, variables))
			return false;

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

bool EditorScriptInfo::GetEditorScriptPlugin(std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, int>>& plugins,
	String& errorMessage, int stack) const
{
	auto result = PluginForFile::ParsePluginPipeline(m_PluginPipeline, errorMessage);
	if (!errorMessage.empty())
		return false;
	for (size_t i = 0; i < result.size(); ++i)
	{
		auto& pipelineItem = result[i];
		if (std::holds_alternative<PluginForFile::FilterExpressionPipelineItem>(pipelineItem))
		{
			auto& [ targetFlags, expression ] = std::get<PluginForFile::FilterExpressionPipelineItem>(pipelineItem);
			if (i == 0 || std::holds_alternative<PluginForFile::FilterExpressionPipelineItem>(result[i - 1]) || 
				std::get<PluginForFile::FilterExpressionPipelineItem>(result[i - 1]).targetFlags != targetFlags)
				plugins.push_back({ nullptr, { expression }, targetFlags, {}, true });
			else
				std::get<1>(plugins.back()).push_back(expression);
			i++;
			continue;
		}
		auto& [ targetFlags, pluginName, args, quoteChar] = PluginForFile::GetPluginPipelineItem(pipelineItem);
		bool found = false;
		PluginArray *pluginInfoArray = CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"EDITOR_SCRIPT");
		for (const auto& plugin : *pluginInfoArray)
		{
			std::vector<String> namesArray;
			std::vector<int> idArray;
			int nFunc = plugin::GetMethodsFromScript(plugin->m_lpDispatch, namesArray, idArray);
			for (int j = 0; j < nFunc; ++j)
			{
				if (namesArray[j] == pluginName)
				{
					plugins.push_back({ plugin.get(), {}, targetFlags, args, idArray[j] });
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
		if (!found)
		{
			PluginInfo* plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"ALIAS_EDITOR_SCRIPT", pluginName);
			if (plugin)
			{
				plugins.push_back({ plugin, {}, targetFlags, args, 0 });
				found = true;
			}
		}
		if (!found)
		{
			errorMessage = strutils::format_string1(_("Plugin not found or invalid: %1"), pluginName);
			return false;
		}
	}

	return ExpandPluginAliases(_T("ALIAS_EDITOR_SCRIPT"), plugins, errorMessage, stack,
		[&](const String& pipeline, auto& newPlugins, String& error, int nextStack)
		{
			return EditorScriptInfo(pipeline).GetEditorScriptPlugin(newPlugins, error, nextStack);
		});
}

bool EditorScriptInfo::TransformText(int target, String& text, const std::vector<StringView>& variables, bool& changed)
{
	changed = false;
	// no handler : return true
	if (m_PluginPipeline.empty())
		return true;

	// control value
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, std::vector<String>, uint8_t, std::vector<String>, int>> plugins;
	if (!GetEditorScriptPlugin(plugins, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	for (const auto& [plugin, expressions, targetFlags, args, fncID] : plugins)
	{
		if (!isTargetInFlags(target, targetFlags))
			continue;

		if (!expressions.empty())
		{
			String filepath = variables.empty() ? _T("") : String(variables[0].data(), variables[0].size());
			text = ApplyFilterExpressionsToString(text, expressions, filepath, errorMessage);
			if (!errorMessage.empty())
			{
				AppErrorMessageBox(errorMessage);
				return false;
			}
			changed = true;
			continue;
		}

		LPDISPATCH piScript = plugin->m_lpDispatch;
		Poco::FastMutex::ScopedLock lock(g_mutex);

		if (!SetPluginVariablesAndArguments(plugin, piScript, args, variables))
			return false;

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
				RootLogger::Error(e.displayText());
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
			RootLogger::Error(e.displayText());
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
		return containsNonAsciiChars ? text : I18n::tr(ucr::toUTF8(strutils::to_str(text)));
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
