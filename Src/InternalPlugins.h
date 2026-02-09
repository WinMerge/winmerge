#pragma once

#include "UnicodeString.h"
#include <memory>
#include <map>

class PluginInfo;

namespace internal_plugin
{

enum LocationType
{
	InstallationPath,
	AppDataPath,
	DocumentsPath,
};

struct Script
{
	String m_body;
	String m_fileExtension;
};

struct Method
{
	Method() = default;
	Method(const Method& method)
		: m_command(method.m_command)
		, m_script(method.m_script ? new Script(*method.m_script) : nullptr)
	{
	}
	String m_command;
	std::unique_ptr<Script> m_script;
};

struct Info
{
	Info(const String& name) : m_name(name) {}
	Info(Info&& info) = default;
	Info(const Info& info)
		: m_name(info.m_name)
		, m_event(info.m_event)
		, m_description(info.m_description)
		, m_fileFilters(info.m_fileFilters)
		, m_unpackedFileExtension(info.m_unpackedFileExtension)
		, m_extendedProperties(info.m_extendedProperties)
		, m_arguments(info.m_arguments)
		, m_pipeline(info.m_pipeline)
		, m_isAutomatic(info.m_isAutomatic)
		, m_locationType(info.m_locationType)
		, m_prediffFile(info.m_prediffFile ? new Method(*info.m_prediffFile) : nullptr)
		, m_unpackFile(info.m_unpackFile ? new Method(*info.m_unpackFile) : nullptr)
		, m_packFile(info.m_packFile ? new Method(*info.m_packFile) : nullptr)
		, m_isFolder(info.m_isFolder ? new Method(*info.m_isFolder) : nullptr)
		, m_unpackFolder(info.m_unpackFolder ? new Method(*info.m_unpackFolder) : nullptr)
		, m_packFolder(info.m_packFolder ? new Method(*info.m_packFolder) : nullptr)
	{
	}
	String m_name;
	String m_event;
	String m_description;
	String m_fileFilters;
	String m_unpackedFileExtension;
	String m_extendedProperties;
	String m_arguments;
	String m_pipeline;
	bool m_isAutomatic = false;
	LocationType m_locationType = LocationType::InstallationPath;
	std::unique_ptr<Method> m_prediffFile;
	std::unique_ptr<Method> m_unpackFile;
	std::unique_ptr<Method> m_packFile;
	std::unique_ptr<Method> m_isFolder;
	std::unique_ptr<Method> m_unpackFolder;
	std::unique_ptr<Method> m_packFolder;
};

String GetPluginXMLPath(LocationType locationType);
bool LoadFromXML(LocationType locationType, std::list<Info>& internalPlugins, String& errmsg);
bool SaveToXML(LocationType locationType, const std::list<Info>& internalPlugins, String& errmsg);
Info* GetInternalPluginInfo(const PluginInfo* plugin);
bool FindPluginNameConflict(const Info& info);
Info CreateUnpackerPluginExample();
Info CreatePredifferPluginExample();
Info CreateAliasPluginExample(PluginInfo* plugin, const String& event, const String& pipeline);
bool AddPlugin(const Info& info, String& errmsg);
bool UpdatePlugin(const Info& info, String& errmsg);
bool RemovePlugin(const Info& info, String& errmsg);

}
