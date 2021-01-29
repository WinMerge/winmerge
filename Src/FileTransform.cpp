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
#include "UniMarkdownFile.h"
#include "Environment.h"
#include "TFile.h"

using Poco::Exception;

namespace FileTransform
{

PLUGIN_MODE g_UnpackerMode = PLUGIN_MODE::PLUGIN_MANUAL;
PLUGIN_MODE g_PredifferMode = PLUGIN_MODE::PLUGIN_MANUAL;





////////////////////////////////////////////////////////////////////////////////
// transformations : packing unpacking

bool getPackUnpackPlugin(const String& pluginName, PluginInfo*& plugin, bool& bWithFile)
{
	bWithFile = true;
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
					AppErrorMessageBox(strutils::format_string1(_("Plugin not found or invalid: %1"), pluginName));
				}
				else
				{
					plugin = nullptr;
					AppErrorMessageBox(strutils::format(_T("'%s' is not PACK_UNPACK plugin"), pluginName));
				}
				return false;
			}
			bWithFile = false;
		}
	}
	return true;
}

// known handler
bool Packing(String & filepath, PackingInfo handler)
{
	// no handler : return true
	if (handler.m_PluginName.empty())
		return true;

	storageForPlugins bufferData;
	bufferData.SetDataFileAnsi(filepath);

	// control value
	bool bHandled = false;
	bool bWithFile = true;
	PluginInfo* plugin = nullptr;
	if (!getPackUnpackPlugin(handler.m_PluginName, plugin, bWithFile))
		return false;

	LPDISPATCH piScript = plugin->m_lpDispatch;
	if (bWithFile)
	{
		// use a temporary dest name
		String srcFileName = bufferData.GetDataFileAnsi(); // <-Call order is important
		String dstFileName = bufferData.GetDestFileName(); // <-Call order is important
		bHandled = plugin::InvokePackFile(srcFileName,
			dstFileName,
			bufferData.GetNChanged(),
			piScript, handler.m_subcode);
		if (bHandled)
			bufferData.ValidateNewFile();
	}
	else
	{
		bHandled = plugin::InvokePackBuffer(*bufferData.GetDataBufferAnsi(),
			bufferData.GetNChanged(),
			piScript, handler.m_subcode);
		if (bHandled)
			bufferData.ValidateNewBuffer();
	}

	// if this packer does not work, that is an error
	if (!bHandled)
		return false;

	// if the buffer changed, write it before leaving
	bool bSuccess = true;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}

// known handler
bool Unpacking(String & filepath, const PackingInfo * handler, int * handlerSubcode)
{
	// no handler : return true
	if (handler == nullptr || handler->m_PluginName.empty())
		return true;

	storageForPlugins bufferData;
	bufferData.SetDataFileAnsi(filepath);

	// temporary subcode 
	int subcode;

	// control value
	bool bHandled = false;
	bool bWithFile = true;
	PluginInfo* plugin = nullptr;
	if (!getPackUnpackPlugin(handler->m_PluginName, plugin, bWithFile))
		return false;

	LPDISPATCH piScript = plugin->m_lpDispatch;
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
	*handlerSubcode = subcode;

	// if the buffer changed, write it before leaving
	bool bSuccess = true;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}


// scan plugins for the first handler
bool Unpacking(String & filepath, const String& filteredText, PackingInfo * handler, int * handlerSubcode)
{
	// PLUGIN_MODE::PLUGIN_BUILTIN_XML : read source file through custom UniFile
	if (handler->m_PluginOrPredifferMode == PLUGIN_MODE::PLUGIN_BUILTIN_XML)
	{
		handler->m_pufile = new UniMarkdownFile;
		handler->m_textType = _T("xml");
		handler->m_bDisallowMixedEOL = true;
		handler->m_PluginName.erase(); // Make FileTransform_Packing() a NOP
		// Leave eToBeScanned alone so above lines will continue to execute on
		// subsequent calls to this function
		return true;
	}

	storageForPlugins bufferData;
	bufferData.SetDataFileAnsi(filepath);

	// control value
	bool bHandled = false;

	PluginInfo * plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_PACK_UNPACK", filteredText);
	if (plugin == nullptr)
		plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_FOLDER_PACK_UNPACK", filteredText);
	if (plugin != nullptr)
	{
		handler->m_PluginName = plugin->m_name;
		// use a temporary dest name
		bufferData.SetDestFileExtension(plugin->m_ext);
		String srcFileName = bufferData.GetDataFileAnsi(); // <-Call order is important
		String dstFileName = bufferData.GetDestFileName(); // <-Call order is important
		bHandled = plugin::InvokeUnpackFile(srcFileName,
			dstFileName,
			bufferData.GetNChanged(),
			plugin->m_lpDispatch, handler->m_subcode);
		if (bHandled)
			bufferData.ValidateNewFile();
	}

	// We can not assume that the file is text, so use a safearray and not a BSTR
	// TODO : delete this event ? 	Is anyone going to use this ?

	if (!bHandled)
	{
		plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"BUFFER_PACK_UNPACK", filteredText);
		if (plugin != nullptr)
		{
			handler->m_PluginName = plugin->m_name;
			bHandled = plugin::InvokeUnpackBuffer(*bufferData.GetDataBufferAnsi(),
				bufferData.GetNChanged(),
				plugin->m_lpDispatch, handler->m_subcode);
			if (bHandled)
				bufferData.ValidateNewBuffer();
		}
	}

	if (!bHandled)
	{
		// we didn't find any unpacker, just hope it is normal Ansi/Unicode
		handler->m_PluginName = _T("");
		handler->m_subcode = 0;
		bHandled = true;
	}

	// the handler is now defined
	handler->m_PluginOrPredifferMode = PLUGIN_MODE::PLUGIN_MANUAL;

	// assign the sucode
	*handlerSubcode = handler->m_subcode;

	// if the buffer changed, write it before leaving
	bool bSuccess = true;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}

bool Unpacking(PackingInfo *handler, String& filepath, const String& filteredText)
{
	if (handler->m_PluginOrPredifferMode != PLUGIN_MODE::PLUGIN_MANUAL)
		return Unpacking(filepath, filteredText, handler, &handler->m_subcode);
	else
		return Unpacking(filepath, handler, &handler->m_subcode);
}

////////////////////////////////////////////////////////////////////////////////
// transformation prediffing

bool getPrediffPlugin(const String& pluginName, PluginInfo*& plugin, bool& bWithFile)
{
	bWithFile = true;
	plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_PREDIFF", pluginName);
	if (plugin == nullptr)
	{
		plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"BUFFER_PREDIFF", pluginName);
		if (plugin == nullptr)
		{
			plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(nullptr, pluginName);
			if (plugin == nullptr)
			{
				AppErrorMessageBox(strutils::format_string1(_("Plugin not found or invalid: %1"), pluginName));
			}
			else
			{
				plugin = nullptr;
				AppErrorMessageBox(strutils::format(_T("'%s' is not PREDIFF plugin"), pluginName));
			}
			return false;
		}
		bWithFile = false;
	}
	return true;
}

// known handler
bool Prediffing(String & filepath, PrediffingInfo handler, bool bMayOverwrite)
{
	// no handler : return true
	if (handler.m_PluginName.empty())
		return true;

	storageForPlugins bufferData;
	// detect Ansi or Unicode file
	bufferData.SetDataFileUnknown(filepath, bMayOverwrite);
	// TODO : set the codepage
	// bufferData.SetCodepage();

	// control value
	bool bHandled = false;
	bool bWithFile = true;
	PluginInfo* plugin = nullptr;
	if (!getPrediffPlugin(handler.m_PluginName, plugin, bWithFile))
		return false;

	LPDISPATCH piScript = plugin->m_lpDispatch;
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
	bool bSuccess = true;
	if (bufferData.GetNChangedValid() > 0)
	{
		// bufferData changes filepath here to temp filepath
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}


// scan plugins for the first handler
bool Prediffing(String & filepath, const String& filteredText, PrediffingInfo * handler, bool bMayOverwrite)
{
	storageForPlugins bufferData;
	// detect Ansi or Unicode file
	bufferData.SetDataFileUnknown(filepath, bMayOverwrite);
	// TODO : set the codepage
	// bufferData.SetCodepage();

	// control value
	bool bHandled = false;

	PluginInfo * plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_PREDIFF", filteredText);
	if (plugin != nullptr)
	{
		handler->m_PluginName = plugin->m_name;
		// use a temporary dest name
		String srcFileName = bufferData.GetDataFileAnsi(); // <-Call order is important
		String dstFileName = bufferData.GetDestFileName(); // <-Call order is important
		bHandled = plugin::InvokePrediffFile(srcFileName,
			dstFileName,
			bufferData.GetNChanged(),
			plugin->m_lpDispatch);
		if (bHandled)
			bufferData.ValidateNewFile();
	}

	if (!bHandled)
	{
		plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"BUFFER_PREDIFF", filteredText);
		if (plugin != nullptr)
		{
			handler->m_PluginName = plugin->m_name;
			// probably it is for VB/VBscript so use a BSTR as argument
			bHandled = plugin::InvokePrediffBuffer(*bufferData.GetDataBufferUnicode(),
				bufferData.GetNChanged(),
				plugin->m_lpDispatch);
			if (bHandled)
				bufferData.ValidateNewBuffer();
		}
	}

	if (!bHandled)
	{
		// we didn't find any prediffer, that is OK anyway
		handler->m_PluginName = _T("");
		bHandled = true;
	}

	// the handler is now defined
	handler->m_PluginOrPredifferMode = PLUGIN_MODE::PLUGIN_MANUAL;

	// if the buffer changed, write it before leaving
	bool bSuccess = true;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}

bool Prediffing(PrediffingInfo * handler, String & filepath, const String& filteredText, bool bMayOverwrite)
{
	if (handler->m_PluginOrPredifferMode != PLUGIN_MODE::PLUGIN_MANUAL)
		return Prediffing(filepath, filteredText, handler, bMayOverwrite);
	else
		return Prediffing(filepath, *handler, bMayOverwrite);
}


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

bool Interactive(String & text, const wchar_t *TransformationEvent, int iFncChosen)
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

	// iFncChosen is the index of the function in the script file
	// we must convert it to the function ID
	int fncID = plugin::GetMethodIDInScript(piScriptArray->at(iScript)->m_lpDispatch, iFncChosen);

	// execute the transform operation
	int nChanged = 0;
	plugin::InvokeTransformText(text, nChanged, piScriptArray->at(iScript)->m_lpDispatch, fncID);

	return (nChanged != 0);
}

}

////////////////////////////////////////////////////////////////////////////////
