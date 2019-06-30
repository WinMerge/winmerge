#pragma once

#define POCO_NO_UNWINDOWS 1
#include <Poco/Mutex.h>
#include <memory>
#include <map>
// defines IPluginInfos
#include "DiffContext.h"
// defines PackingInfo and PrediffingInfo
#include "FileTransform.h"

/**
 * @brief Cached plugin info for one particular file info
 */
struct PluginFileInfo
{
	PackingInfo m_infoUnpacker;
	PrediffingInfo m_infoPrediffer;
};

typedef std::shared_ptr<PluginFileInfo> PluginFileInfoPtr;

/**
 * @brief Cache of known plugin infos
 */
class PluginManager : public IPluginInfos
{
public:
	typedef std::map<String, PluginFileInfoPtr> PluginFileInfoMap;

	~PluginManager();
	void SetPrediffSetting(const String& filteredFilenames, PLUGIN_MODE newsetting);
	void SetPrediffSettingAll(PLUGIN_MODE newsetting);
	void SetPrediffer(const String& filteredFilenames, const String& prediffer);
	// Implement IPluginInfos
	virtual void FetchPluginInfos(const String& filteredFilenames, 
                                      PackingInfo ** infoUnpacker, 
                                      PrediffingInfo ** infoPrediffer) override;
private:
	// Data
	PluginFileInfoMap m_pluginSettings;
	Poco::FastMutex m_mutex;
};
