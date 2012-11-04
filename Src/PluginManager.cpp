/**
 *  @file PluginManager.cpp
 *
 *  @brief Implementation of PluginManager class
 */ 
//////////////////////////////////////////////////////////////////////

#include "PluginManager.h"

PluginManager::~PluginManager()
{
	// Manually free all items in cache
	// This could also be done with a DestructElements helper
	for (PluginFileInfoMap::iterator it = m_pluginSettings.begin(); it != m_pluginSettings.end(); ++it)
		delete it->second;
	m_pluginSettings.clear();
}

/**
 * @brief retrieve relevant plugin settings for specified comparison
 */
void PluginManager::FetchPluginInfos(const String& filteredFilenames, 
                                     PackingInfo ** infoUnpacker, 
                                     PrediffingInfo ** infoPrediffer)
{
	PluginFileInfo *fi;
	PluginFileInfoMap::iterator it = m_pluginSettings.find(filteredFilenames);
	if (it == m_pluginSettings.end())
	{
		// This might be a good place to set any user-specified default values
		fi = new PluginFileInfo;
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
void PluginManager::SetPrediffSetting(const String& filteredFilenames, int newsetting)
{
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);
	infoPrediffer->Initialize(newsetting);
}

void PluginManager::SetPrediffer(const String& filteredFilenames, const String & prediffer)
{
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);
	infoPrediffer->pluginName = prediffer;
}