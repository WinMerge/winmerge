/** 
 * @file  PropArchive.h
 *
 * @brief Declaration file for PropArchive propertyheet
 *
 */
#pragma once

#include "OptionsPanel.h"

class COptionsMgr;

/** 
 * @brief Class for Archive Support options page.
 */
class PropArchive : public OptionsPanel
{
public:
	explicit PropArchive(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	enum { IDD = IDD_PROP_ARCHIVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void UpdateControls();

	afx_msg void OnEnableClicked();

	DECLARE_MESSAGE_MAP()
public:
	bool m_bEnableSupport;
	bool m_bProbeType;
};
