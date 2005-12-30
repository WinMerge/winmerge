#ifndef PluginManager_h_included
#define PluginManager_h_included


#if !defined(AFX_DIFFCONTEXT_H__D3CC86BE_F11E_11D2_826C_00A024706EDC__INCLUDED_)
// defines IPluginInfos
#include "DiffContext.h"
#endif

#ifndef FileTransform_h
// defines PackingInfo and PrediffingInfo
#include "FileTransform.h"
#endif

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
	void SetPrediffSetting(const CString & filteredFilenames, int newsetting);
	void SetPrediffer(const CString & filteredFilenames, const CString & prediffer);

private:
	// Implement IPluginInfos
	virtual void FetchPluginInfos(const CString& filteredFilenames, 
                                      PackingInfo ** infoUnpacker, 
                                      PrediffingInfo ** infoPrediffer);
private:
	// Data
	CTypedPtrMap<CMapStringToPtr, CString, PluginFileInfo*> m_pluginSettings;
};

#endif // PluginManager_h_included
