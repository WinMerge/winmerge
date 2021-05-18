#include "pch.h"
#include "Plugins.h"
#define POCO_NO_UNWINDOWS 1
#include <Poco/FileStream.h>
#include <Poco/SAX/SAXParser.h>
#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/Attributes.h>
#include <Poco/Exception.h>
#include <vector>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <windows.h>
#include "MergeApp.h"
#include "paths.h"
#include "Environment.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "codepage_detect.h"
#include "UniFile.h"
#include "WinMergePluginBase.h"

using Poco::XML::SAXParser;
using Poco::XML::ContentHandler;
using Poco::XML::Locator;
using Poco::XML::XMLChar;
using Poco::XML::XMLString;
using Poco::XML::Attributes;
using namespace std::literals::string_literals;

namespace
{
	HRESULT ReadFile(const String& path, String& text)
	{
		UniMemFile file;
		if (!file.OpenReadOnly(path))
			return HRESULT_FROM_WIN32(GetLastError());
		file.ReadBom();
		if (!file.HasBom())
		{
			int iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
			FileTextEncoding encoding = codepage_detect::Guess(
				paths::FindExtension(path), file.GetBase(),
				file.GetFileSize() < codepage_detect::BufSize ? file.GetFileSize() : codepage_detect::BufSize,
				iGuessEncodingType);
			file.SetCodepage(encoding.m_codepage);
		}
		file.ReadStringAll(text);
		file.Close();
		return S_OK;
	}

	HRESULT WriteFile(const String& path, const String& text, bool bom = true)
	{
		UniStdioFile fileOut;
		if (!fileOut.Open(path, _T("wb")))
			return HRESULT_FROM_WIN32(GetLastError());
		fileOut.SetUnicoding(ucr::UNICODESET::UTF8);
		if (bom)
		{
			fileOut.SetBom(true);
			fileOut.WriteBom();
		}
		fileOut.WriteString(text);
		fileOut.Close();
		return S_OK;
	}
}

namespace internal_plugin
{

struct Script
{
	String m_body;
	String m_fileExtension;
};

struct Method
{
	String m_command;
	std::unique_ptr<Script> m_script;
};

struct Info
{
	Info(const String& name) : m_name(name) {}
	String m_name;
	String m_event;
	String m_description;
	String m_fileFilters;
	bool m_isAutomatic = false;
	String m_unpackedFileExtension;
	std::unique_ptr<Method> m_prediffFile;
	std::unique_ptr<Method> m_unpackFile;
	std::unique_ptr<Method> m_packFile;
	std::unique_ptr<Method> m_unpackFolder;
	std::unique_ptr<Method> m_packFolder;
	std::map<String, Method> m_editorScripts;
};

class XMLHandler : public Poco::XML::ContentHandler
{
public:
	inline static const std::string Empty = "";
	inline static const std::string PluginsElement = "plugins";
	inline static const std::string PluginElement = "plugin";
	inline static const std::string EventElement = "event";
	inline static const std::string DescriptionElement = "description";
	inline static const std::string FileFiltersElement = "file-filters";
	inline static const std::string IsAutomaticElement = "is-automatic";
	inline static const std::string UnpackedFileExtensionElement = "unpacked-file-extention";
	inline static const std::string PrediffFileElement = "prediff-file";
	inline static const std::string UnpackFileElement = "unpack-file";
	inline static const std::string PackFileElement = "pack-file";
	inline static const std::string UnpackFolderElement = "unpack-folder";
	inline static const std::string PackFolderElement = "pack-folder";
	inline static const std::string CommandElement = "command";
	inline static const std::string ScriptElement = "script";
	inline static const std::string NameAttribute = "name";
	inline static const std::string ValueAttribute = "value";
	inline static const std::string FileExtensionAttribute = "fileExtension";

	explicit XMLHandler(std::list<Info>* pPlugins) : m_pPlugins(pPlugins) {}

	void setDocumentLocator(const Locator* loc) {}
	void startDocument() {}
	void endDocument() {}
	void startElement(const XMLString& uri, const XMLString& localName, const XMLString& qname, const Attributes& attributes)
	{
		if (!m_stack.empty())
		{
			if (m_stack.top() == PluginsElement)
			{
				if (localName == PluginElement)
				{
					String name;
					int index = attributes.getIndex(Empty, NameAttribute);
					if (index >= 0)
						name = ucr::toTString(attributes.getValue(index));
					m_pPlugins->emplace_back(name);
					m_pMethod = nullptr;
				}
			}
			else if (m_stack.top() == PluginElement)
			{
				Info& plugin = m_pPlugins->back();
				String value;
				int index = attributes.getIndex(Empty, ValueAttribute);
				if (index >= 0)
				{
					value = ucr::toTString(attributes.getValue(index));
					if (localName == EventElement)
						plugin.m_event = value;
					else if (localName == DescriptionElement)
						plugin.m_description = value;
					else if (localName == FileFiltersElement)
						plugin.m_fileFilters = value;
					else if (localName == IsAutomaticElement)
					{
						TCHAR ch = value.c_str()[0];
						plugin.m_isAutomatic = (ch == 't' || ch == 'T');
					}
					else if (localName == UnpackedFileExtensionElement)
						plugin.m_unpackedFileExtension = value;
				}
				else if (localName == PrediffFileElement)
				{
					plugin.m_prediffFile.reset(new Method());
					m_pMethod = plugin.m_prediffFile.get();
				}
				else if (localName == UnpackFileElement)
				{
					plugin.m_unpackFile.reset(new Method());
					m_pMethod = plugin.m_unpackFile.get();
				}
				else if (localName == PackFileElement)
				{
					plugin.m_packFile.reset(new Method());
					m_pMethod = plugin.m_packFile.get();
				}
				else if (localName == UnpackFolderElement)
				{
					plugin.m_unpackFolder.reset(new Method());
					m_pMethod = plugin.m_unpackFolder.get();
				}
				else if (localName == PackFolderElement)
				{
					plugin.m_packFolder.reset(new Method());
					m_pMethod = plugin.m_packFolder.get();
				}
			}
			else if (m_pMethod)
			{
				if (localName == ScriptElement)
				{
					m_pMethod->m_script.reset(new Script);
					int index = attributes.getIndex(Empty, FileExtensionAttribute);
					if (index >= 0)
						m_pMethod->m_script->m_fileExtension = ucr::toTString(attributes.getValue(index));
				}
			}
		}
		m_stack.push(localName);
	}
	void endElement(const XMLString& uri, const XMLString& localName, const XMLString& qname)
	{
		m_stack.pop();
	}
	void characters(const XMLChar ch[], int start, int length)
	{
		if (m_stack.empty())
			return;
		if (m_stack.top() == CommandElement && m_pMethod)
		{
			m_pMethod->m_command += xmlch2tstr(ch, length);
		}
		else if (m_stack.top() == ScriptElement && m_pMethod && m_pMethod->m_script)
		{
			m_pMethod->m_script->m_body += xmlch2tstr(ch, length);
		}
	}
	void ignorableWhitespace(const XMLChar ch[], int start, int length) {}
	void processingInstruction(const XMLString& target, const XMLString& data) {}
	void startPrefixMapping(const XMLString& prefix, const XMLString& uri) {}
	void endPrefixMapping(const XMLString& prefix) {}
	void skippedEntity(const XMLString& name) {}

private:
	static String xmlch2tstr(const XMLChar* ch, int length)
	{
		return ucr::toTString(std::string(ch, length));
	}

	std::list<Info>* m_pPlugins = nullptr;
	std::stack<std::string> m_stack;
	Method* m_pMethod = nullptr;
};

class UnpackerGeneratedFromEditorScript : public WinMergePluginBase
{
public:
	UnpackerGeneratedFromEditorScript(IDispatch* pDispatch, const std::wstring funcname, int id)
		: WinMergePluginBase(
			L"FILE_PACK_UNPACK",
			strutils::format_string1(_T("Unpacker to execute %1 script (automatically generated)"), funcname),
			_T("\\.nomatch$"))
		, m_pDispatch(pDispatch)
		, m_funcid(id)
	{
		m_pDispatch->AddRef();
	}

	virtual ~UnpackerGeneratedFromEditorScript()
	{
		m_pDispatch->Release();
	}

	HRESULT STDMETHODCALLTYPE UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT* pSubcode, VARIANT_BOOL* pbSuccess) override
	{
		String text;
		HRESULT hr = ReadFile(fileSrc, text);
		if (FAILED(hr))
			return hr;
		int changed = 0;
		if (!plugin::InvokeTransformText(text, changed, m_pDispatch, m_funcid))
			return E_FAIL;
		hr = WriteFile(fileDst, text);
		if (FAILED(hr))
			return hr;
		*pSubcode = 0;
		*pbChanged = VARIANT_TRUE;
		*pbSuccess = VARIANT_TRUE;
		return S_OK;
	}

private:
	IDispatch* m_pDispatch;
	int m_funcid;
};

class InternalPlugin : public WinMergePluginBase
{
public:
	InternalPlugin(Info& info)
		: WinMergePluginBase(info.m_event, info.m_description, info.m_fileFilters, info.m_unpackedFileExtension, info.m_isAutomatic)
		, m_info(info)
	{
	}

	virtual ~InternalPlugin()
	{
	}

	static String createScript(const Script& script)
	{
		String path = env::GetTemporaryFileName(env::GetTemporaryPath(), _T ("_SC")) + script.m_fileExtension;
		WriteFile(path, script.m_body, false);
		return path;
	}

	static HRESULT launchProgram(const String& sCmd, WORD wShowWindow)
	{
		if (_wgetenv(L"WINMERGE_HOME") == nullptr)
			_wputenv_s(L"WINMERGE_HOME", env::GetProgPath().c_str());
		String command = sCmd;
		strutils::replace(command, _T("${WINMERGE_HOME}"), env::GetProgPath());
		STARTUPINFO stInfo = { sizeof(STARTUPINFO) };
		stInfo.dwFlags = STARTF_USESHOWWINDOW;
		stInfo.wShowWindow = wShowWindow;
		PROCESS_INFORMATION processInfo;
		bool retVal = !!CreateProcess(nullptr, (LPTSTR)command.c_str(),
			nullptr, nullptr, FALSE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr,
			&stInfo, &processInfo);
		if (!retVal)
			return HRESULT_FROM_WIN32(GetLastError());
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE PrediffFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, VARIANT_BOOL* pbSuccess) override
	{
		if (!m_info.m_prediffFile)
		{
			*pbChanged = VARIANT_FALSE;
			*pbSuccess = VARIANT_FALSE;
			return S_OK;
		}
		String command = m_info.m_prediffFile->m_command;
		String scriptPath;
		strutils::replace(command, _T("${SRC_FILE}"), fileSrc);
		strutils::replace(command, _T("${DST_FILE}"), fileDst);
		if (m_info.m_prediffFile->m_script)
		{
			scriptPath = createScript(*m_info.m_prediffFile->m_script);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptPath);
		}
		HRESULT hr = launchProgram(command, SW_SHOW);
		if (!scriptPath.empty())
			DeleteFile(scriptPath.c_str());

		*pbChanged = SUCCEEDED(hr);
		*pbSuccess = SUCCEEDED(hr);
		return hr;
	}

	HRESULT STDMETHODCALLTYPE UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT* pSubcode, VARIANT_BOOL* pbSuccess) override
	{
		if (!m_info.m_unpackFile)
		{
			*pSubcode = 0;
			*pbChanged = VARIANT_FALSE;
			*pbSuccess = VARIANT_FALSE;
			return S_OK;
		}
		String command = m_info.m_unpackFile->m_command;
		String scriptPath;
		strutils::replace(command, _T("${SRC_FILE}"), fileSrc);
		strutils::replace(command, _T("${DST_FILE}"), fileDst);
		if (m_info.m_unpackFile->m_script)
		{
			scriptPath = createScript(*m_info.m_unpackFile->m_script);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptPath);
		}
		HRESULT hr = launchProgram(command, SW_SHOW);
		if (!scriptPath.empty())
			DeleteFile(scriptPath.c_str());

		*pSubcode = 0;
		*pbChanged = SUCCEEDED(hr);
		*pbSuccess = SUCCEEDED(hr);
		return hr;
	}

	HRESULT STDMETHODCALLTYPE PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT subcode, VARIANT_BOOL* pbSuccess) override
	{
		if (!m_info.m_packFile)
		{
			*pbChanged = VARIANT_FALSE;
			*pbSuccess = VARIANT_FALSE;
			return S_OK;
		}
		String command = m_info.m_packFile->m_command;
		String scriptPath;
		strutils::replace(command, _T("${SRC_FILE}"), fileSrc);
		strutils::replace(command, _T("${DST_FILE}"), fileDst);
		if (m_info.m_packFile->m_script)
		{
			scriptPath = createScript(*m_info.m_packFile->m_script);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptPath);
		}
		HRESULT hr = launchProgram(command, SW_SHOW);
		if (!scriptPath.empty())
			DeleteFile(scriptPath.c_str());

		*pbChanged = SUCCEEDED(hr);
		*pbSuccess = SUCCEEDED(hr);
		return hr;
	}

private:
	Info& m_info;
};

struct Loader
{
	Loader()
	{
		CAllThreadsScripts::RegisterInternalPluginsLoader(&Load);
	}

	static void Load(std::map<String, PluginArrayPtr>& plugins)
	{
		if (plugins.find(L"EDITOR_SCRIPT") != plugins.end())
		{
			for (auto plugin : *plugins[L"EDITOR_SCRIPT"])
			{
				std::vector<String> namesArray;
				std::vector<int> idArray;
				int validFuncs = plugin::GetMethodsFromScript(plugin->m_lpDispatch, namesArray, idArray);
				for (int i = 0; i < validFuncs; ++i)
				{
					if (plugins.find(L"FILE_PACK_UNPACK") == plugins.end())
						plugins[L"FILE_PACK_UNPACK"].reset(new PluginArray);
					PluginInfoPtr pluginNew(new PluginInfo());
					IDispatch* pDispatch = new UnpackerGeneratedFromEditorScript(plugin->m_lpDispatch, namesArray[i], idArray[i]);
					pDispatch->AddRef();
					pluginNew->MakeInfo(plugin->m_filepath + _T(":") + namesArray[i], pDispatch);
					plugins[L"FILE_PACK_UNPACK"]->push_back(pluginNew);
				}
			}
		}

		m_plugins.clear();
		XMLHandler handler(&m_plugins);
		SAXParser parser;
		parser.setContentHandler(&handler);
		try
		{
			parser.parse(ucr::toUTF8(paths::ConcatPath(env::GetProgPath(), _T("MergePlugins\\InternalPlugins.xml"))));
		}
		catch (Poco::FileNotFoundException&)
		{
		}

		for (auto& info : m_plugins)
		{
			if (plugins.find(info.m_event) == plugins.end())
				plugins[info.m_event].reset(new PluginArray);
			PluginInfoPtr pluginNew(new PluginInfo());
			IDispatch* pDispatch = new InternalPlugin(info);
			pDispatch->AddRef();
			pluginNew->MakeInfo(info.m_name, pDispatch);
			plugins[info.m_event]->push_back(pluginNew);
		}
	}
	inline static std::list<Info> m_plugins;
} g_loader;

}
