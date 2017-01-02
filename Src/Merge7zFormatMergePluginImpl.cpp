/** 
 * @file  Merge7zFormatMergePluginImpl.cpp
 *
 * @brief Implementation file for Merge7zFormatMergePluginImpl class
 */
#include "Merge7zFormatMergePluginImpl.h"
#include "paths.h"
#include "Plugins.h"
#include "Merge7zFormatRegister.h"
#include <list>
#include <Poco/Mutex.h>

static Merge7zFormatRegister g_autoregister(&Merge7zFormatMergePluginImpl::GuessFormat);
static __declspec(thread) Merge7zFormatMergePluginImpl *g_pluginformat;
static std::list<std::unique_ptr<Merge7zFormatMergePluginImpl> > g_pluginformat_list;
static Poco::FastMutex g_mutex;

static Merge7zFormatMergePluginImpl *GetInstance()
{
	if (!g_pluginformat)
	{
		g_pluginformat = new Merge7zFormatMergePluginImpl();
		Poco::FastMutex::ScopedLock lock(g_mutex);
		g_pluginformat_list.emplace_back(g_pluginformat);
	}
	return g_pluginformat;
}

Merge7z::Format *Merge7zFormatMergePluginImpl::GuessFormat(const String& path)
{
	Merge7zFormatMergePluginImpl *format = GetInstance();
	PluginInfo *plugin = NULL;
	if (format->m_infoUnpacker.bToBeScanned)
		plugin = CAllThreadsScripts::GetActiveSet()->GetAutomaticPluginByFilter(L"FILE_FOLDER_PACK_UNPACK", path);
	else if (!format->m_infoUnpacker.pluginName.empty())
		plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_FOLDER_PACK_UNPACK", format->m_infoUnpacker.pluginName);
	if (!plugin)
		return NULL;
	if (!plugin::InvokeIsFolder(path, plugin->m_lpDispatch))
		return NULL;
	format->m_plugin = plugin;
	return format;
}

HRESULT Merge7zFormatMergePluginImpl::DeCompressArchive(HWND, LPCTSTR path, LPCTSTR folder)
{
	if (!m_plugin)
		return E_FAIL;
	paths::CreateIfNeeded(path);
	int nChanged = 0;
	return plugin::InvokeUnpackFolder(path, folder, nChanged, m_plugin->m_lpDispatch, m_infoUnpacker.subcode) ? S_OK : E_FAIL;
}

HRESULT Merge7zFormatMergePluginImpl::CompressArchive(HWND, LPCTSTR path, Merge7z::DirItemEnumerator *)
{
	return E_FAIL;
}

Merge7z::Format::Inspector *Merge7zFormatMergePluginImpl::Open(HWND, LPCTSTR) { return NULL; }
Merge7z::Format::Updater *Merge7zFormatMergePluginImpl::Update(HWND, LPCTSTR) { return NULL; }
HRESULT Merge7zFormatMergePluginImpl::GetHandlerProperty(HWND, PROPID, PROPVARIANT *, VARTYPE) { return E_FAIL; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerName(HWND) { return NULL; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerClassID(HWND) { return NULL; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerExtension(HWND) { return NULL; }
BSTR Merge7zFormatMergePluginImpl::GetHandlerAddExtension(HWND) { return NULL; }
VARIANT_BOOL Merge7zFormatMergePluginImpl::GetHandlerUpdate(HWND) { return VARIANT_FALSE; }
VARIANT_BOOL Merge7zFormatMergePluginImpl::GetHandlerKeepName(HWND) { return VARIANT_FALSE; }

BSTR Merge7zFormatMergePluginImpl::GetDefaultName(HWND, LPCTSTR path)
{
	return SysAllocString(L"");
}

void Merge7zFormatMergePluginImpl::SetPackingInfo(const PackingInfo* infoUnpacker)
{
	GetInstance()->m_infoUnpacker = infoUnpacker ? *infoUnpacker : PackingInfo();
}

PackingInfo *Merge7zFormatMergePluginImpl::GetPackingInfo()
{
	return &GetInstance()->m_infoUnpacker;
}
