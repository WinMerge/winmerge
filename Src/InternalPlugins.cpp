#include "pch.h"
#include "Plugins.h"
#define POCO_NO_UNWINDOWS 1
#include <Poco/FileStream.h>
#include <Poco/SAX/SAXParser.h>
#include <Poco/SAX/SAXException.h>
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
#include <Shlwapi.h>
#include "MergeApp.h"
#include "paths.h"
#include "Environment.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "codepage_detect.h"
#include "UniFile.h"
#include "WinMergePluginBase.h"
#include "TempFile.h"

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
			int64_t fileSize = file.GetFileSize();
			FileTextEncoding encoding = codepage_detect::Guess(
				paths::FindExtension(path), file.GetBase(), static_cast<size_t>(
					fileSize < static_cast<int64_t>(codepage_detect::BufSize) ?
					fileSize : static_cast<int64_t>(codepage_detect::BufSize)),
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
	Info(Info&& info) = default;
	String m_name;
	String m_event;
	String m_description;
	String m_fileFilters;
	bool m_isAutomatic = false;
	String m_unpackedFileExtension;
	String m_extendedProperties;
	String m_arguments;
	std::unique_ptr<Method> m_prediffFile;
	std::unique_ptr<Method> m_unpackFile;
	std::unique_ptr<Method> m_packFile;
	std::unique_ptr<Method> m_isFolder;
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
	inline static const std::string UnpackedFileExtensionElement = "unpacked-file-extension";
	inline static const std::string ExtendedPropertiesElement = "extended-properties";
	inline static const std::string ArgumentsElement = "arguments";
	inline static const std::string PrediffFileElement = "prediff-file";
	inline static const std::string UnpackFileElement = "unpack-file";
	inline static const std::string PackFileElement = "pack-file";
	inline static const std::string IsFolderElement = "is-folder";
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
						tchar_t ch = value.c_str()[0];
						plugin.m_isAutomatic = (ch == 't' || ch == 'T');
					}
					else if (localName == UnpackedFileExtensionElement)
						plugin.m_unpackedFileExtension = value;
					else if (localName == ExtendedPropertiesElement)
						plugin.m_extendedProperties = value;
					else if (localName == ArgumentsElement)
						plugin.m_arguments = std::move(value);
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
				else if (localName == IsFolderElement)
				{
					plugin.m_isFolder.reset(new Method());
					m_pMethod = plugin.m_isFolder.get();
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
	UnpackerGeneratedFromEditorScript(const PluginInfo& plugin, const std::wstring funcname, int id)
		: WinMergePluginBase(
			L"FILE_PACK_UNPACK",
			strutils::format_string1(_T("Unpacker to execute %1 script (automatically generated)"), funcname),
			L"\\.nomatch$", L"")
		, m_pDispatch(plugin.m_lpDispatch)
		, m_funcid(id)
		, m_hasArgumentsProperty(plugin.m_hasArgumentsProperty)
		, m_hasVariablesProperty(plugin.m_hasVariablesProperty)
	{
		auto desc = plugin.GetExtendedPropertyValue(funcname + _T(".Description"));
		if (desc.has_value())
			m_sDescription = *desc;
		m_pDispatch->AddRef();
		auto menuCaption = plugin.GetExtendedPropertyValue(funcname + _T(".MenuCaption"));
		String caption = menuCaption.has_value() ? strutils::to_str(*menuCaption) : funcname;
		m_sExtendedProperties = ucr::toUTF16(strutils::format(_T("ProcessType=Editor script;MenuCaption=%s"), caption))
			+ (plugin.GetExtendedPropertyValue(funcname + _T(".ArgumentsRequired")).has_value() ? L";ArgumentsRequired" : L"");
		StringView args = plugin.GetExtendedPropertyValue(funcname + _T(".Arguments")).value_or(_T(""));
		m_sArguments = strutils::to_str(args);
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
		if (m_hasVariablesProperty && !plugin::InvokePutPluginVariables(ucr::toTString(fileSrc), m_pDispatch))
			return E_FAIL;
		if (m_hasArgumentsProperty && !plugin::InvokePutPluginArguments(m_sArguments, m_pDispatch))
			return E_FAIL;
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

	HRESULT STDMETHODCALLTYPE PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, INT subcode, VARIANT_BOOL* pbSuccess) override
	{
		*pbChanged = VARIANT_FALSE;
		*pbSuccess = VARIANT_FALSE;
		return S_OK;
	}

private:
	IDispatch* m_pDispatch;
	int m_funcid;
	bool m_hasArgumentsProperty;
	bool m_hasVariablesProperty;
};

class InternalPlugin : public WinMergePluginBase
{
public:
	InternalPlugin(Info&& info)
		: WinMergePluginBase(info.m_event, info.m_description, info.m_fileFilters, info.m_unpackedFileExtension, info.m_extendedProperties, info.m_arguments, info.m_isAutomatic)
		, m_info(std::move(info))
	{
	}

	virtual ~InternalPlugin()
	{
	}

	HRESULT STDMETHODCALLTYPE PrediffFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL* pbChanged, VARIANT_BOOL* pbSuccess) override
	{
		if (!m_info.m_prediffFile)
		{
			*pbChanged = VARIANT_FALSE;
			*pbSuccess = VARIANT_FALSE;
			return S_OK;
		}
		TempFile scriptFile;
		String command = replaceMacros(m_info.m_prediffFile->m_command, fileSrc, fileDst);
		if (m_info.m_prediffFile->m_script)
		{
			createScript(*m_info.m_prediffFile->m_script, scriptFile);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptFile.GetPath());
		}
		DWORD dwExitCode;
		HRESULT hr = launchProgram(command, SW_HIDE, dwExitCode);

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
		TempFile scriptFile;
		String command = replaceMacros(m_info.m_unpackFile->m_command, fileSrc, fileDst);
		if (m_info.m_unpackFile->m_script)
		{
			createScript(*m_info.m_unpackFile->m_script, scriptFile);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptFile.GetPath());
		}
		DWORD dwExitCode;
		HRESULT hr = launchProgram(command, SW_HIDE, dwExitCode);

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
		TempFile scriptFile;
		String command = replaceMacros(m_info.m_packFile->m_command, fileSrc, fileDst);
		if (m_info.m_packFile->m_script)
		{
			createScript(*m_info.m_packFile->m_script, scriptFile);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptFile.GetPath());
		}
		DWORD dwExitCode;
		HRESULT hr = launchProgram(command, SW_HIDE, dwExitCode);

		*pbChanged = SUCCEEDED(hr);
		*pbSuccess = SUCCEEDED(hr);
		return hr;
	}

	HRESULT STDMETHODCALLTYPE IsFolder(BSTR file, VARIANT_BOOL* pbFolder) override
	{
		if (!m_info.m_isFolder)
		{
			*pbFolder = VARIANT_FALSE;
			return S_OK;
		}
		TempFile scriptFile;
		String command = replaceMacros(m_info.m_isFolder->m_command, file, file);
		if (m_info.m_isFolder->m_script)
		{
			createScript(*m_info.m_isFolder->m_script, scriptFile);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptFile.GetPath());
		}
		DWORD dwExitCode;
		HRESULT hr = launchProgram(command, SW_HIDE, dwExitCode);
		*pbFolder = SUCCEEDED(hr) && dwExitCode == 0;
		return hr;
	}

	HRESULT STDMETHODCALLTYPE UnpackFolder(BSTR fileSrc, BSTR folderDst, VARIANT_BOOL* pbChanged, INT* pSubcode, VARIANT_BOOL* pbSuccess) override
	{
		if (!m_info.m_unpackFolder)
		{
			*pSubcode = 0;
			*pbChanged = VARIANT_FALSE;
			*pbSuccess = VARIANT_FALSE;
			return S_OK;
		}
		TempFile scriptFile;
		String command = replaceMacros(m_info.m_unpackFolder->m_command, fileSrc, folderDst);
		if (m_info.m_unpackFolder->m_script)
		{
			createScript(*m_info.m_unpackFolder->m_script, scriptFile);
			strutils::replace(command, _T("${SCRIPT_FILE}"), scriptFile.GetPath());
		}
		DWORD dwExitCode;
		HRESULT hr = launchProgram(command, SW_HIDE, dwExitCode);

		*pSubcode = 0;
		*pbChanged = SUCCEEDED(hr);
		*pbSuccess = SUCCEEDED(hr);
		return hr;
	}

protected:

	String replaceMacros(const String& cmd, const String & fileSrc, const String& fileDst)
	{
		String command = cmd;
		if (paths::IsURL(fileSrc))
		{
			PARSEDURL parsedURL{sizeof(PARSEDURL)};
			ParseURL(fileSrc.c_str(), &parsedURL);
			strutils::replace(command, _T("${SRC_URL_PROTOCOL}"), String{ parsedURL.pszProtocol, parsedURL.cchProtocol });
			strutils::replace(command, _T("${SRC_URL_SUFFIX}"), parsedURL.pszSuffix);
		}
		if (paths::IsURL(fileDst))
		{
			PARSEDURL parsedURL{sizeof(PARSEDURL)};
			ParseURL(fileDst.c_str(), &parsedURL);
			strutils::replace(command, _T("${DST_URL_PROTOCOL}"), String{ parsedURL.pszProtocol, parsedURL.cchProtocol });
			strutils::replace(command, _T("${DST_URL_SUFFIX}"), parsedURL.pszSuffix);
		}
		strutils::replace(command, _T("${SRC_FILE}"), fileSrc);
		strutils::replace(command, _T("${DST_FILE}"), fileDst);
		strutils::replace(command, _T("${DST_FOLDER}"), fileDst);
		std::vector<StringView> vars = strutils::split(m_sVariables, '\0');
		for (size_t i = 0; i < vars.size(); ++i)
			strutils::replace(command, strutils::format(_T("${%d}"), i), strutils::to_str(vars[i]));
		strutils::replace(command, _T("${*}"), m_sArguments);
		return command;
	}

	static HRESULT createScript(const Script& script, TempFile& tempFile)
	{
		String path = tempFile.Create(_T(""), script.m_fileExtension);
		return WriteFile(path, script.m_body, false);
	}

	static HRESULT launchProgram(const String& sCmd, WORD wShowWindow, DWORD &dwExitCode)
	{
		TempFile stderrFile;
		String sOutputFile = stderrFile.Create();
		if (_wgetenv(L"WINMERGE_HOME") == nullptr)
			_wputenv_s(L"WINMERGE_HOME", env::GetProgPath().c_str());
		String command = sCmd;
		strutils::replace(command, _T("${WINMERGE_HOME}"), env::GetProgPath());
		STARTUPINFO stInfo = { sizeof(STARTUPINFO) };
		stInfo.dwFlags = STARTF_USESHOWWINDOW;
		stInfo.wShowWindow = wShowWindow;
		SECURITY_ATTRIBUTES sa{ sizeof(sa) };
		sa.bInheritHandle = true;
		stInfo.hStdError = CreateFile(sOutputFile.c_str(), GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		stInfo.hStdOutput = stInfo.hStdError;
		stInfo.dwFlags |= STARTF_USESTDHANDLES;
		PROCESS_INFORMATION processInfo;
		bool retVal = !!CreateProcess(nullptr, (tchar_t*)command.c_str(),
			nullptr, nullptr, TRUE, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr,
			&stInfo, &processInfo);
		if (!retVal)
			return HRESULT_FROM_WIN32(GetLastError());
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		GetExitCodeProcess(processInfo.hProcess, &dwExitCode);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
		DWORD dwStdErrorSize = 0;
		if (stInfo.hStdError != nullptr && stInfo.hStdError != INVALID_HANDLE_VALUE)
		{
			DWORD dwStdErrorSizeHigh = 0;
			dwStdErrorSize = GetFileSize(stInfo.hStdError, &dwStdErrorSizeHigh);
			CloseHandle(stInfo.hStdError);
		}
		if (dwExitCode != 0 && dwStdErrorSize > 0)
		{
			String error;
			ReadFile(sOutputFile, error);
			ICreateErrorInfo* pCreateErrorInfo = nullptr;
			if (SUCCEEDED(CreateErrorInfo(&pCreateErrorInfo)))
			{
				pCreateErrorInfo->SetSource(const_cast<OLECHAR*>(command.c_str()));
				pCreateErrorInfo->SetDescription(const_cast<OLECHAR*>(ucr::toUTF16(error).c_str()));
				IErrorInfo* pErrorInfo = nullptr;
				pCreateErrorInfo->QueryInterface(&pErrorInfo);
				SetErrorInfo(0, pErrorInfo);
				pErrorInfo->Release();
				pCreateErrorInfo->Release();
				return DISP_E_EXCEPTION;
			}
		}
		return S_OK;
	}

private:
	Info m_info;
};

class EditorScriptGeneratedFromUnpacker: public WinMergePluginBase
{
public:
	EditorScriptGeneratedFromUnpacker(const PluginInfo& plugin, const String& funcname, bool hasArgumentProperty)
		: WinMergePluginBase(
			L"EDITOR_SCRIPT",
			plugin.m_description,
			plugin.m_filtersTextDefault, L"", plugin.m_extendedProperties,
			plugin.m_argumentsDefault)
		, m_pDispatch(plugin.m_lpDispatch)
		, m_hasArgumentsProperty(hasArgumentProperty)
	{
		auto menuCaption = plugin.GetExtendedPropertyValue(_T("MenuCaption"));
		if (menuCaption.has_value())
		{
			String menuCaptionStr = strutils::to_str(*menuCaption);
			m_sExtendedProperties = strutils::format(_T("%s;%s.MenuCaption=%s"),
					plugin.m_extendedProperties, funcname, menuCaptionStr);
		}
		m_pDispatch->AddRef();
		AddFunction(ucr::toUTF16(funcname), CallUnpackFile);
	}

	virtual ~EditorScriptGeneratedFromUnpacker()
	{
		m_pDispatch->Release();
	}

	static HRESULT STDMETHODCALLTYPE CallUnpackFile(IDispatch *pDispatch, BSTR text, BSTR* pbstrResult)
	{
		TempFile src, dst;
		String fileSrc = src.Create();
		String fileDst = dst.Create();
		HRESULT hr = WriteFile(fileSrc, text);
		if (FAILED(hr))
			return hr;
		int changed = 0;
		int subcode = 0;
		auto* thisObj = static_cast<EditorScriptGeneratedFromUnpacker*>(pDispatch);
		auto* pInternalPlugin = dynamic_cast<InternalPlugin*>(thisObj->m_pDispatch);
		if (pInternalPlugin)
		{
			BSTR bstrArguments = SysAllocString(thisObj->m_sArguments.c_str());
			pInternalPlugin->put_PluginArguments(bstrArguments);
			BSTR bstrFileSrc = SysAllocString(ucr::toUTF16(fileSrc).c_str());
			BSTR bstrFileDst= SysAllocString(ucr::toUTF16(fileDst).c_str());
			VARIANT_BOOL bChanged;
			VARIANT_BOOL bSuccess;
			hr = pInternalPlugin->UnpackFile(bstrFileSrc, bstrFileDst, &bChanged, &subcode, &bSuccess);
			SysFreeString(bstrFileSrc);
			SysFreeString(bstrFileDst);
			SysFreeString(bstrArguments);
			if (FAILED(hr))
				return hr;
		}
		else
		{
			if (thisObj->m_hasArgumentsProperty)
			{
				if (!plugin::InvokePutPluginArguments(thisObj->m_sArguments, thisObj->m_pDispatch))
					return E_FAIL;
			}
			if (!plugin::InvokeUnpackFile(fileSrc, fileDst, changed, thisObj->m_pDispatch, subcode))
				return E_FAIL;
		}
		String unpackedText;
		hr = ReadFile(fileDst, unpackedText);
		if (FAILED(hr))
			return hr;
		*pbstrResult = SysAllocStringLen(ucr::toUTF16(unpackedText).c_str(), 
			static_cast<unsigned>(unpackedText.length()));
		return S_OK;
	}

private:
	bool m_hasArgumentsProperty;
	IDispatch* m_pDispatch;
};

struct Loader
{
	Loader()
	{
		CAllThreadsScripts::RegisterInternalPluginsLoader(&Load);
	}

	static bool Load(std::map<String, PluginArrayPtr>& plugins, String& errmsg)
	{
		if (plugins.find(L"EDITOR_SCRIPT") != plugins.end())
		{
			for (auto plugin : *plugins[L"EDITOR_SCRIPT"])
			{
				if (!plugin->m_disabled && plugin->GetExtendedPropertyValue(_T("GenerateUnpacker")).has_value())
				{
					std::vector<String> namesArray;
					std::vector<int> idArray;
					int validFuncs = plugin::GetMethodsFromScript(plugin->m_lpDispatch, namesArray, idArray);
					for (int i = 0; i < validFuncs; ++i)
					{
						if (namesArray[i] == L"PluginOnEvent")
							continue;
						if (plugins.find(L"FILE_PACK_UNPACK") == plugins.end())
							plugins[L"FILE_PACK_UNPACK"].reset(new PluginArray);
						PluginInfoPtr pluginNew(new PluginInfo());
						IDispatch* pDispatch = new UnpackerGeneratedFromEditorScript(*plugin, namesArray[i], idArray[i]);
						pDispatch->AddRef();
						pluginNew->MakeInfo(namesArray[i], pDispatch);
						plugins[L"FILE_PACK_UNPACK"]->push_back(pluginNew);
					}
				}
			}
		}

		std::list<Info> internalPlugins;
		XMLHandler handler(&internalPlugins);
		SAXParser parser;
		parser.setContentHandler(&handler);
		try
		{
			for (const auto& path : {
				paths::ConcatPath(env::GetProgPath(), _T("MergePlugins\\Plugins.xml")),
				env::ExpandEnvironmentVariables(_T("%APPDATA%\\WinMerge\\MergePlugins\\Plugins.xml"))
				})
			{
				try
				{
					parser.parse(ucr::toUTF8(path));
				}
				catch (Poco::FileNotFoundException&)
				{
				}
			}
		}
		catch (Poco::XML::SAXParseException& e)
		{
			errmsg = ucr::toTString(e.message());
			return false;
		}

		for (auto& info : internalPlugins)
		{
			String event = info.m_event;
			String name = info.m_name;
			if (plugins.find(event) == plugins.end())
				plugins[event].reset(new PluginArray);
			PluginInfoPtr pluginNew(new PluginInfo());
			IDispatch* pDispatch = new InternalPlugin(std::move(info));
			pDispatch->AddRef();
			pluginNew->MakeInfo(name, pDispatch);
			plugins[event]->push_back(pluginNew);
		}

		if (plugins.find(L"FILE_PACK_UNPACK") != plugins.end())
		{
			for (auto plugin : *plugins[L"FILE_PACK_UNPACK"])
			{
				if (!plugin->m_disabled && plugin->GetExtendedPropertyValue(_T("GenerateEditorScript")).has_value())
				{
					if (plugins.find(L"EDITOR_SCRIPT") == plugins.end())
						plugins[L"EDITOR_SCRIPT"].reset(new PluginArray);
					PluginInfoPtr pluginNew(new PluginInfo());
					IDispatch* pDispatch = new EditorScriptGeneratedFromUnpacker(*plugin, plugin->m_name, plugin->m_hasArgumentsProperty);
					pDispatch->AddRef();
					pluginNew->MakeInfo(plugin->m_name, pDispatch);
					plugins[L"EDITOR_SCRIPT"]->push_back(pluginNew);
				}
			}
		}

		return true;
	}
} g_loader;

}
