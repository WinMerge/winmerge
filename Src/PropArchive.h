/** 
 * @file  PropArchive.h
 *
 * @brief Declaration file for PropArchive propertyheet
 *
 */
#pragma once

#include "StatLink.h"
#include "OptionsPanel.h"

class COptionsMgr;

/** 
 * @brief Class for Archive Support options page.
 */
class PropArchive : public OptionsPanel
{
public:
	PropArchive(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	enum { IDD = IDD_PROP_ARCHIVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void UpdateControls();

	afx_msg void OnEnableClicked();

	DECLARE_MESSAGE_MAP()
public:
	CStaticLink m_wwwLink;
	bool m_bEnableSupport;
	int m_nInstallType;
	bool m_bProbeType;
};
