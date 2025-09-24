/** 
 * @file  PropCompareImage.cpp
 *
 * @brief Implementation of PropCompareImage propertysheet
 */

#include "stdafx.h"
#include "PropCompareImage.h"
#include "WildcardDropList.h"
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
 , m_bEnableImageCompare(false)
 , m_nOcrResultType(0)
{
	auto converter = [](String v, bool write) {
		if (!write) return v;
		WildcardRemoveDuplicatePatterns(v);
		return v;
	};
	BindOption(OPT_CMP_IMG_FILEPATTERNS, m_sFilePatterns, IDC_COMPAREIMAGE_PATTERNS, DDX_Text, converter);
	BindOption(OPT_CMP_ENABLE_IMGCMP_IN_DIRCMP, m_bEnableImageCompare, IDC_ENABLE_IMGCMP_IN_DIRCMP, DDX_Check);
	BindOption(OPT_CMP_IMG_OCR_RESULT_TYPE, m_nOcrResultType, IDC_COMPAREIMAGE_OCR_RESULT_TYPE, DDX_CBIndex);
}

void PropCompareImage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareImage)
	DDX_Control(pDX, IDC_COMPAREIMAGE_PATTERNS, m_comboPatterns);
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropCompareImage, OptionsPanel)
	//{{AFX_MSG_MAP(PropCompareImage)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_CBN_DROPDOWN(IDC_COMPAREIMAGE_PATTERNS, OnDropDownPatterns)
	ON_CBN_CLOSEUP(IDC_COMPAREIMAGE_PATTERNS, OnCloseUpPatterns)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompareImage::OnInitDialog()
{
	SetDlgItemComboBoxList(IDC_COMPAREIMAGE_OCR_RESULT_TYPE,
		{ _("Text only"), _("Line-by-line position and text"), _("Word-by-word position and text") });

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

/**
 * @brief Prepares multi-selection drop list 
 */
void PropCompareImage::OnDropDownPatterns()
{
	String patterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_IMG_FILEPATTERNS)
#ifdef _WIN64
		+ _T(";*.pdf;*.svg;*.wmf;*.emf");
#else
		+ _T(";*.wmf;*.emf");
#endif
	WildcardDropList::OnDropDown(m_comboPatterns, 6, patterns.c_str());
}

/**
 * @brief Finishes drop list multi-selection
 */
void PropCompareImage::OnCloseUpPatterns()
{
	WildcardDropList::OnCloseUp(m_comboPatterns);
}
