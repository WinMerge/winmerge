/** 
 * @file  OptionsPanel.h
 *
 * @brief Declaration file for OptionsPanel class.
 *
 */
#pragma once

#include "IOptionsPanel.h"

class COptionsMgr;

/**
 * @brief A base class for WinMerge options dialogs.
 */
class OptionsPanel : public CPropertyPage, public IOptionsPanel
{
public:
	OptionsPanel(COptionsMgr *optionsMgr, UINT nIDTemplate);

protected:
	COptionsMgr * GetOptionsMgr() const { return m_pOptionsMgr; }

private:
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};
