/** 
 * @file  PropTextColors.h
 *
 * @brief Declaration file for CPropTextColors propertyheet
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(PropTextColors_h_included)
#define PropTextColors_h_included

#include "ColorButton.h"
#include "IOptionsPanel.h"

class COptionsMgr;
class SyntaxColors;


/////////////////////////////////////////////////////////////////////////////
// CPropTextColors dialog

/** @brief Property page for colors options; used in options property sheet */
class CPropTextColors : public CPropertyPage, public IOptionsPanel
{

// Construction
public:

	CPropTextColors(COptionsMgr *optionsMgr, SyntaxColors *pColors);
	virtual ~CPropTextColors();

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
private:

	SyntaxColors *m_pTempColors;
	COLORREF m_cCustColors[COLORINDEX_COUNT];
	COptionsMgr * m_pOptionsMgr;

	//{{AFX_DATA(CPropTextColors)
	enum { IDD = IDD_PROPPAGE_COLORS_TEXT };
	CColorButton    m_btnDefaultStandardColors;
	CColorButton    m_btnWhitespaceBackground;
	CColorButton    m_btnRegularBackground;
	CColorButton    m_btnRegularText;
	CColorButton    m_btnSelectionBackground;
	CColorButton    m_btnSelectionText;
	BOOL            m_bCustomColors;
	//}}AFX_DATA

	COLORREF	m_clrWhitespaceBackground;
	COLORREF	m_clrRegularBackground;
	COLORREF	m_clrRegularText;
	COLORREF	m_clrSelectionBackground;
	COLORREF	m_clrSelectionText;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropTextColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	typedef enum { SET_DEFAULTS, READ_OPTIONS, LOAD_COLORS } OPERATION;

	virtual BOOL OnInitDialog();
	void LoadCustomColors();
	void SaveCustomColors();
	void BrowseColorAndSave(CColorButton & colorButton, int colorIndex);
	void SerializeColorsToFromScreen(OPERATION op);
	void SerializeColorToFromScreen(OPERATION op, CColorButton & btn, int colorIndex);
	void EnableColorButtons(BOOL bEnable);

	// Generated message map functions
	//{{AFX_MSG(CPropTextColors)
	afx_msg void OnWhitespaceBackgroundColor();
	afx_msg void OnRegularBackgroundColor();
	afx_msg void OnRegularTextColor();
	afx_msg void OnSelectionBackgroundColor();
	afx_msg void OnSelectionTextColor();
	afx_msg void OnDefaultsStandardColors();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // PropTextColors_h_included
