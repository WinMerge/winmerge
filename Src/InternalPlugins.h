#include "UnicodeString.h"
#include <memory>
#include <map>

class PluginInfo;

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
	String m_unpackedFileExtension;
	String m_extendedProperties;
	String m_arguments;
	String m_pipeline;
	bool m_isAutomatic = false;
	bool m_userDefined = false;
	std::unique_ptr<Method> m_prediffFile;
	std::unique_ptr<Method> m_unpackFile;
	std::unique_ptr<Method> m_packFile;
	std::unique_ptr<Method> m_isFolder;
	std::unique_ptr<Method> m_unpackFolder;
	std::unique_ptr<Method> m_packFolder;
};

String GetPluginXMLPath(bool userDefined);
bool LoadFromXML(const String& pluginsXMLPath, bool userDefined, std::list<Info>& internalPlugins, String& errmsg);
bool SaveToXML(const String& pluginsXMLPath, const std::list<Info>& internalPlugins, String& errmsg);
Info* GetInternalPluginInfo(const PluginInfo* plugin);
bool RemoveInternalPlugin(const PluginInfo* plugin, bool userDefined, String& errmsg);

}
