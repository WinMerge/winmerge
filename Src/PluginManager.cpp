/**
 *  @file PluginManager.cpp
 *
 *  @brief Implementation of PluginManager class
 */ 
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "PluginManager.h"

using Poco::FastMutex;

PluginManager::~PluginManager() = default;

/**
 * @brief retrieve relevant plugin settings for specified comparison
 */
void PluginManager::FetchPluginInfos(const String& filteredFilenames, 
                                     PackingInfo ** infoUnpacker, 
                                     PrediffingInfo ** infoPrediffer)
{
	FastMutex::ScopedLock lock(m_mutex);
	PluginFileInfoPtr fi;
	PluginFileInfoMap::iterator it = m_pluginSettings.find(filteredFilenames);
	if (it == m_pluginSettings.end())
	{
		// This might be a good place to set any user-specified default values
		fi.reset(new PluginFileInfo);
		m_pluginSettings[filteredFilenames] = fi;
	}
	else
	{
		fi = it->second;
	}
	*infoUnpacker = &fi->m_infoUnpacker;
	*infoPrediffer = &fi->m_infoPrediffer;
}

void PluginManager::SetUnpackerSettingAll(bool automatic)
{
	FastMutex::ScopedLock lock(m_mutex);
	for (PluginFileInfoMap::iterator it = m_pluginSettings.begin(); it != m_pluginSettings.end(); ++it)
		it->second->m_infoUnpacker.Initialize(automatic);
}

void PluginManager::SetPrediffSettingAll(bool automatic)
{
	FastMutex::ScopedLock lock(m_mutex);
	for (PluginFileInfoMap::iterator it = m_pluginSettings.begin(); it != m_pluginSettings.end(); ++it)
		it->second->m_infoPrediffer.Initialize(automatic);
}

void PluginManager::SetPrediffer(const String& filteredFilenames, const String& predifferPipeline)
{
	PackingInfo * infoUnpacker = nullptr;
	PrediffingInfo * infoPrediffer = nullptr;
	FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);
	infoPrediffer->SetPluginPipeline(predifferPipeline);
}