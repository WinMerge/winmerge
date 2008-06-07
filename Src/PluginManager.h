#ifndef PluginManager_h_included
#define PluginManager_h_included


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

/**
 * @brief Cache of known plugin infos
 */
class PluginManager : public IPluginInfos
{
public:
	~PluginManager();
	void SetPrediffSetting(LPCTSTR filteredFilenames, int newsetting);
	void SetPrediffer(LPCTSTR filteredFilenames, const CString & prediffer);

private:
	// Implement IPluginInfos
	virtual void FetchPluginInfos(LPCTSTR filteredFilenames, 
                                      PackingInfo ** infoUnpacker, 
                                      PrediffingInfo ** infoPrediffer);
private:
	// Data
	CTypedPtrMap<CMapStringToPtr, CString, PluginFileInfo*> m_pluginSettings;
};

#endif // PluginManager_h_included
