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
#include "Plugins.h"
#include "multiformatText.h"
#include "Environment.h"
#include "TFile.h"
#include "MergeApp.h"

using Poco::Exception;

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
	TCHAR quoteChar = 0;
	std::vector<String> tokens;
	String token, name, param;
	for (const TCHAR* p = pluginPipeline.c_str(); *p;)
	{
		while (_istspace(*p)) p++;
		while (*p)
		{
			if (!inQuotes)
			{
				if (*p == '"' || *p == '\'')
				{
					inQuotes = true;
					quoteChar = *p;
				}
				else if (_istspace(*p))
				{
					break;
				}
				else if (*p == '|')
					break;
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
			name = token;
			if (name.empty())
				errorMessage = _T("Missing plugin name in plugin pipeline: ") + pluginPipeline;
		}
		else
		{
			param += token + ((*p == ' ') ? _T(" ") : _T(""));
		}
		if (!_istspace(*p))
		{
			result.push_back({name, strutils::trim_ws_end(param), quoteChar});
			name.clear();
			param.clear();
			quoteChar = 0;
		}
		if (*p)
			++p;
		token.clear();
	}
	if (inQuotes)
		errorMessage = _T("Missing quotation mark in plugin pipeline: ") + pluginPipeline;
	return result;
}

String PluginForFile::MakePipeline(const std::vector<PluginForFile::PipelineItem> list)
{
	String pipeline;
	for (const auto& [name, args, quoteChar] : list)
	{
		if (pipeline.empty())
			pipeline = name;
		else
			pipeline += _T("|") + name;
		if (!args.empty())
		{
			String argsQuoted = args;
			strutils::replace(argsQuoted, _T("'"), _T("''"));
			pipeline += _T(" '") + argsQuoted + _T("'");
		}
	}
	return pipeline;
}

bool PluginForFile::IsValidPluginPipeline(const String& pluginPipeline)
{
	String errorMessage;
	auto result = ParsePluginPipeline(pluginPipeline, errorMessage);
	return errorMessage.empty();
}

bool PluginForFile::IsValidPluginPipeline() const
{
	return IsValidPluginPipeline(m_PluginPipeline);
}

bool PackingInfo::GetPackUnpackPlugin(const String& filteredFilenames, bool bReverse,
	std::vector<std::tuple<PluginInfo*, String, bool>>& plugins,
	String *pPluginPipelineResolved, String& errorMessage) const
{
	auto result = ParsePluginPipeline(errorMessage);
	if (!errorMessage.empty())
		return false;
	std::vector<PluginForFile::PipelineItem> pipelineResolved;
	for (const auto& [pluginName, args, quoteChar] : result)
	{
		PluginInfo* plugin = nullptr;
		bool bWithFile = true;
		if (pluginName == _T("<None>") || pluginName == _("<None>"))
			;
		else if (pluginName == _T("<Automatic>") || pluginName == _("<Automatic>"))
		{
			plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_PACK_UNPACK", filteredFilenames);
			if (plugin == nullptr)
				plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_FOLDER_PACK_UNPACK", filteredFilenames);
			if (plugin == nullptr)
			{
				plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"BUFFER_PACK_UNPACK", filteredFilenames);
				bWithFile = false;
			}
		}
		else
		{
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
							errorMessage = strutils::format(_T("'%s' is not PACK_UNPACK plugin"), pluginName);
						}
						return false;
					}
					bWithFile = false;
				}
			}
		}
		if (plugin)
		{
			pipelineResolved.push_back({ plugin->m_name, args, quoteChar });
			if (bReverse)
				plugins.insert(plugins.begin(), { plugin, args, bWithFile });
			else
				plugins.push_back({ plugin, args, bWithFile });
		}
	}
	if (pPluginPipelineResolved)
		*pPluginPipelineResolved = MakePipeline(pipelineResolved);
	return true;
}

// known handler
bool PackingInfo::Packing(String & filepath, const std::vector<int>& handlerSubcodes) const
{
	// no handler : return true
	if (m_PluginPipeline.empty())
		return true;

	// control value
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, String, bool>> plugins;
	if (!GetPackUnpackPlugin(_T(""), true, plugins, nullptr, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	auto itSubcode = handlerSubcodes.rbegin();
	for (auto& [plugin, args, bWithFile] : plugins)
	{
		bool bHandled = false;
		storageForPlugins bufferData;
		bufferData.SetDataFileAnsi(filepath);

		LPDISPATCH piScript = plugin->m_lpDispatch;

		if (plugin->m_hasArgumentProperty)
		{
			if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : args, piScript))
				return false;
		}

		if (bWithFile)
		{
			// use a temporary dest name
			String srcFileName = bufferData.GetDataFileAnsi(); // <-Call order is important
			String dstFileName = bufferData.GetDestFileName(); // <-Call order is important
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

bool PackingInfo::Packing(const String& srcFilepath, const String& dstFilepath, const std::vector<int>& handlerSubcodes) const
{
	String csTempFileName = srcFilepath;
	if (!Packing(csTempFileName, handlerSubcodes))
		return false;
	try
	{
		TFile file1(csTempFileName);
		file1.copyTo(dstFilepath);
		if (srcFilepath!= csTempFileName)
			file1.remove();
		return true;
	}
	catch (Poco::Exception& e)
	{
		LogErrorStringUTF8(e.displayText());
		return false;
	}
}

bool PackingInfo::Unpacking(std::vector<int> * handlerSubcodes, String & filepath, const String& filteredText)
{
	// no handler : return true
	if (m_PluginPipeline.empty())
		return true;

	// control value
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, String, bool>> plugins;
	if (!GetPackUnpackPlugin(filteredText, false, plugins, &m_PluginPipeline, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	if (handlerSubcodes)
		handlerSubcodes->clear();

	for (auto& [plugin, args, bWithFile] : plugins)
	{
		bool bHandled = false;
		storageForPlugins bufferData;
		bufferData.SetDataFileAnsi(filepath);

		// temporary subcode 
		int subcode = 0;

		LPDISPATCH piScript = plugin->m_lpDispatch;

		if (plugin->m_hasArgumentProperty)
		{
			if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : args, piScript))
				return false;
		}

		if (bWithFile)
		{
			// use a temporary dest name
			bufferData.SetDestFileExtension(plugin->m_ext);
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

String PackingInfo::GetUnpackedFileExtension(const String& filteredFilenames) const
{
	String ext;
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, String, bool>> plugins;
	if (GetPackUnpackPlugin(filteredFilenames, false, plugins, nullptr, errorMessage))
	{
		for (auto& [plugin, args, bWithFile] : plugins)
			ext += plugin->m_ext;
	}
	return ext;
}

////////////////////////////////////////////////////////////////////////////////
// transformation prediffing

bool PrediffingInfo::GetPrediffPlugin(const String& filteredFilenames, bool bReverse,
	std::vector<std::tuple<PluginInfo*, String, bool>>& plugins,
	String *pPluginPipelineResolved, String& errorMessage) const
{
	auto result = ParsePluginPipeline(errorMessage);
	if (!errorMessage.empty())
		return false;
	std::vector<PluginForFile::PipelineItem> pipelineResolved;
	for (const auto& [pluginName, args, quoteChar] : result)
	{
		PluginInfo* plugin = nullptr;
		bool bWithFile = true;
		if (pluginName == _T("<None>") || pluginName == _("<None>"))
			;
		else if (pluginName == _T("<Automatic>") || pluginName == _("<Automatic>"))
		{
			plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_PREDIFF", filteredFilenames);
			if (plugin == nullptr)
			{
				plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"BUFFER_PREDIFF", filteredFilenames);
				if (plugin)
					bWithFile = false;
			}
		}
		else
		{
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
						errorMessage = strutils::format(_T("'%s' is not PREDIFF plugin"), pluginName);
					}
					return false;
				}
				bWithFile = false;
			}
		}
		if (plugin)
		{
			pipelineResolved.push_back({ plugin->m_name, args, quoteChar });
			if (bReverse)
				plugins.insert(plugins.begin(), { plugin, args, bWithFile });
			else
				plugins.push_back({ plugin, args, bWithFile });
		}
	}
	if (pPluginPipelineResolved)
		*pPluginPipelineResolved = MakePipeline(pipelineResolved);
	return true;
}

bool PrediffingInfo::Prediffing(String & filepath, const String& filteredText, bool bMayOverwrite)
{
	// no handler : return true
	if (m_PluginPipeline.empty())
		return true;

	// control value
	bool bHandled = false;
	String errorMessage;
	std::vector<std::tuple<PluginInfo*, String, bool>> plugins;
	if (!GetPrediffPlugin(filteredText, false, plugins, &m_PluginPipeline, errorMessage))
	{
		AppErrorMessageBox(errorMessage);
		return false;
	}

	for (const auto& [plugin, args, bWithFile] : plugins)
	{
		storageForPlugins bufferData;
		// detect Ansi or Unicode file
		bufferData.SetDataFileUnknown(filepath, bMayOverwrite);
		// TODO : set the codepage
		// bufferData.SetCodepage();

		LPDISPATCH piScript = plugin->m_lpDispatch;

		if (plugin->m_hasArgumentProperty)
		{
			if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : args, piScript))
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

namespace FileTransform
{

PLUGIN_MODE g_UnpackerMode = PLUGIN_MODE::PLUGIN_MANUAL;
PLUGIN_MODE g_PredifferMode = PLUGIN_MODE::PLUGIN_MANUAL;

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


////////////////////////////////////////////////////////////////////////////////
// transformation : TextTransform_Interactive (editor scripts)

std::vector<String> GetFreeFunctionsInScripts(const wchar_t *TransformationEvent)
{
	std::vector<String> sNamesArray;

	// get an array with the available scripts
	PluginArray * piScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(TransformationEvent);

	// fill in these structures
	int nFnc = 0;	
	size_t iScript;
	for (iScript = 0 ; iScript < piScriptArray->size() ; iScript++)
	{
		const PluginInfoPtr & plugin = piScriptArray->at(iScript);
		if (plugin->m_disabled)
			continue;
		LPDISPATCH piScript = plugin->m_lpDispatch;
		std::vector<String> scriptNamesArray;
		std::vector<int> scriptIdsArray;
		int nScriptFnc = plugin::GetMethodsFromScript(piScript, scriptNamesArray, scriptIdsArray);
		sNamesArray.resize(nFnc+nScriptFnc);

		int iFnc;
		for (iFnc = 0 ; iFnc < nScriptFnc ; iFnc++)
			sNamesArray[nFnc+iFnc] = scriptNamesArray[iFnc];

		nFnc += nScriptFnc;
	}
	return sNamesArray;
}

bool Interactive(String & text, const String& args, const wchar_t *TransformationEvent, int iFncChosen)
{
	if (iFncChosen < 0)
		return false;

	// get an array with the available scripts
	PluginArray * piScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(TransformationEvent);

	size_t iScript;
	for (iScript = 0 ; iScript < piScriptArray->size() ; iScript++)
	{
		if (iFncChosen < piScriptArray->at(iScript)->m_nFreeFunctions)
			// we have found the script file
			break;
		iFncChosen -= piScriptArray->at(iScript)->m_nFreeFunctions;
	}

	if (iScript >= piScriptArray->size())
		return false;

	PluginInfo* plugin = piScriptArray->at(iScript).get();

	// iFncChosen is the index of the function in the script file
	// we must convert it to the function ID
	int fncID = plugin::GetMethodIDInScript(plugin->m_lpDispatch, iFncChosen);

	if (plugin->m_hasArgumentProperty)
	{
		if (!plugin::InvokePutPluginArguments(args.empty() ? plugin->m_arguments : args, plugin->m_lpDispatch))
			return false;
	}

	// execute the transform operation
	int nChanged = 0;
	plugin::InvokeTransformText(text, nChanged, plugin->m_lpDispatch, fncID);

	return (nChanged != 0);
}

std::pair<
	std::vector<std::tuple<String, String, unsigned, PluginInfo *>>,
	std::map<String, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>>
>
CreatePluginMenuInfos(const String& filteredFilenames, const std::vector<std::wstring>& events, unsigned baseId)
{
	std::vector<std::tuple<String, String, unsigned, PluginInfo *>> suggestedPlugins;
	std::map<String, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>> allPlugins;
	unsigned id = baseId;
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
					const auto menuCaption = plugin->GetExtendedPropertyValue(_T("MenuCaption"));
					const auto processType = plugin->GetExtendedPropertyValue(_T("ProcessType"));
					const String caption = menuCaption.has_value() ?
						String{ menuCaption->data(), menuCaption->size() } : plugin->m_name;
					const String process = processType.has_value() ?
						String{ processType->data(), processType->size() } : _T("Others");

					if (plugin->TestAgainstRegList(filteredFilenames))
						suggestedPlugins.emplace_back(caption, plugin->m_name, id, plugin.get());

					if (allPlugins.find(process) == allPlugins.end())
						allPlugins.insert_or_assign(process, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>());
					allPlugins[process].emplace_back(caption, plugin->m_name, id, plugin.get());

					id++;
				}
				else
				{
					LPDISPATCH piScript = plugin->m_lpDispatch;
					std::vector<String> scriptNamesArray;
					std::vector<int> scriptIdsArray;
					int nScriptFnc = plugin::GetMethodsFromScript(piScript, scriptNamesArray, scriptIdsArray);
					bool matched = plugin->TestAgainstRegList(filteredFilenames);
					for (int i = 0; i < nScriptFnc; ++i, ++id)
					{
						const auto menuCaption = plugin->GetExtendedPropertyValue(scriptNamesArray[i] + _T(".MenuCaption"));
						auto processType = plugin->GetExtendedPropertyValue(scriptNamesArray[i] + _T(".ProcessType"));
						if (!processType.has_value())
							processType = plugin->GetExtendedPropertyValue(_T("ProcessType"));
						const String caption = menuCaption.has_value() ?
							String{ menuCaption->data(), menuCaption->size() } : scriptNamesArray[i];
						const String process = processType.has_value() ?
							String{ processType->data(), processType->size() } : _T("Others");
						if (matched)
							suggestedPlugins.emplace_back(caption, plugin->m_name, id, plugin.get());
						if (allPlugins.find(process) == allPlugins.end())
							allPlugins.insert_or_assign(process, std::vector<std::tuple<String, String, unsigned, PluginInfo *>>());
						allPlugins[process].emplace_back(caption, plugin->m_name, id, plugin.get());
					}
				}
			}
		}
	}
	return { suggestedPlugins, allPlugins };
}

}

////////////////////////////////////////////////////////////////////////////////
