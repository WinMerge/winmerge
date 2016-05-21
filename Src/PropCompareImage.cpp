/** 
 * @file  PropCompareImage.cpp
 *
 * @brief Implementation of PropCompareImage propertysheet
 */

#include "stdafx.h"
#include "PropCompareImage.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompareImage::PropCompareImage(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompareImage::IDD)
{
}

void PropCompareImage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareImage)
	DDX_Text(pDX, IDC_COMPAREIMAGE_PATTERNS, m_sFilePatterns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompareImage, CPropertyPage)
	//{{AFX_MSG_MAP(PropCompareImage)
	ON_BN_CLICKED(IDC_COMPAREIMAGE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompareImage::ReadOptions()
{
	m_sFilePatterns = GetOptionsMgr()->GetString(OPT_CMP_IMG_FILEPATTERNS);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompareImage::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_FILEPATTERNS, m_sFilePatterns);
}

/** 
 * @brief Sets options to defaults
 */
void PropCompareImage::OnDefaults()
{
	m_sFilePatterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_IMG_FILEPATTERNS);
	UpdateData(FALSE);
}
