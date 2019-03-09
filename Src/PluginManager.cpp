/**
 *  @file PluginManager.cpp
 *
 *  @brief Implementation of PluginManager class
 */ 
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "PluginManager.h"
#include <Poco/ScopedLock.h>

using Poco::FastMutex;

PluginManager::~PluginManager()
{
}

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

/**
 * @brief Store specified prediff choice for specified comparison
 */
void PluginManager::SetPrediffSetting(const String& filteredFilenames, PLUGIN_MODE newsetting)
{
	PackingInfo * infoUnpacker = nullptr;
	PrediffingInfo * infoPrediffer = nullptr;
	FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);
	infoPrediffer->Initialize(newsetting);
}

void PluginManager::SetPrediffSettingAll(PLUGIN_MODE newsetting)
{
	FastMutex::ScopedLock lock(m_mutex);
	for (PluginFileInfoMap::iterator it = m_pluginSettings.begin(); it != m_pluginSettings.end(); ++it)
	{
		PluginFileInfoPtr fi;
		fi.reset(new PluginFileInfo);
		fi->m_infoPrediffer.Initialize(newsetting);
		(*it).second = fi;
	}
}

void PluginManager::SetPrediffer(const String& filteredFilenames, const String & prediffer)
{
	PackingInfo * infoUnpacker = nullptr;
	PrediffingInfo * infoPrediffer = nullptr;
	FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);
	infoPrediffer->m_PluginName = prediffer;
}