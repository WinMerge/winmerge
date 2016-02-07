/** 
 * @file  OptionsPanel.cpp
 *
 * @brief Implementation of OptionsPanel class.
 */

#include "stdafx.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

/**
 * @brief Constructor.
 */
OptionsPanel::OptionsPanel(COptionsMgr *optionsMgr, UINT nIDTemplate)
 : CTrPropertyPage(nIDTemplate)
 , m_pOptionsMgr(optionsMgr)
{
}
