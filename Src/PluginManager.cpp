/**
 *  @file PluginManager.cpp
 *
 *  @brief Implementation of PluginManager class
 */ 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PluginManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



PluginManager::~PluginManager()
{
	// Manually free all items in cache
	// This could also be done with a DestructElements helper
	CString filteredFilenames;
	PluginFileInfo * fi=0;
	for (POSITION pos = m_pluginSettings.GetStartPosition(); pos; )
	{
		m_pluginSettings.GetNextAssoc(pos, filteredFilenames, fi);
		delete fi;
	}
	m_pluginSettings.RemoveAll();
}

/**
 * @brief retrieve relevant plugin settings for specified comparison
 */
void PluginManager::FetchPluginInfos(const CString& filteredFilenames, 
                                     PackingInfo ** infoUnpacker, 
                                     PrediffingInfo ** infoPrediffer)
{
	PluginFileInfo * fi=0;
	if (!m_pluginSettings.Lookup(filteredFilenames, fi))
	{
		fi = new PluginFileInfo;
		// This might be a good place to set any user-specified default values
		m_pluginSettings.SetAt(filteredFilenames, fi);
	}
	*infoUnpacker = &fi->m_infoUnpacker;
	*infoPrediffer = &fi->m_infoPrediffer;
}

/**
 * @brief Store specified prediff choice for specified comparison
 */
void PluginManager::SetPrediffSetting(const CString & filteredFilenames, int newsetting)
{
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);
	infoPrediffer->Initialize(newsetting);
}