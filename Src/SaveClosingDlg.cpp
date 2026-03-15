// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  SaveClosingDlg.cpp
 *
 * @brief Implementation file for SaveClosingDlg dialog
 */

#include "stdafx.h"
#include "SaveClosingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg dialog

IMPLEMENT_DYNAMIC(SaveClosingDlg, CTrDialog)

/**
 * @brief Constructor.
 * @param [in] pParent Dialog's parent window.
 */
SaveClosingDlg::SaveClosingDlg(CWnd* pParent /*= nullptr*/)
 : CTrDialog(SaveClosingDlg::IDD, pParent)
 , m_leftSave(SAVECLOSING_SAVE)
 , m_middleSave(SAVECLOSING_SAVE)
 , m_rightSave(SAVECLOSING_SAVE)
 , m_bAskForLeft(false)
 , m_bAskForMiddle(false)
 , m_bAskForRight(false)
 , m_bDisableCancel(false)
{
}

void SaveClosingDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SaveClosingDlg)
	DDX_Text(pDX, IDC_SAVECLOSING_LEFTFILE, m_sLeftFile);
	DDX_Text(pDX, IDC_SAVECLOSING_MIDDLEFILE, m_sMiddleFile);
	DDX_Text(pDX, IDC_SAVECLOSING_RIGHTFILE, m_sRightFile);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVELEFT, m_leftSave);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVEMIDDLE, m_middleSave);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVERIGHT, m_rightSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SaveClosingDlg, CTrDialog)
	//{{AFX_MSG_MAP(SaveClosingDlg)
	ON_BN_CLICKED(IDC_SAVECLOSING_DISCARDALL, OnDiscardAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg message handlers

/**
 * @brief Initialize dialog.
 * @return Always FALSE.
 */
BOOL SaveClosingDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	SetDlgItemFocus(IDC_SAVECLOSING_SAVELEFT);
	if (!m_bAskForLeft)
	{
		// Left items disabled move focus to middle side items
		SetDlgItemFocus(IDC_SAVECLOSING_SAVEMIDDLE);

		EnableDlgItem(IDC_SAVECLOSING_LEFTFRAME, false);
		EnableDlgItem(IDC_SAVECLOSING_LEFTFILE, false);
		EnableDlgItem(IDC_SAVECLOSING_SAVELEFT, false);
		EnableDlgItem(IDC_SAVECLOSING_DISCARDLEFT, false);
	}

	if (!m_bAskForMiddle)
	{
		// Middle items disabled move focus to right side items
		if (!m_bAskForLeft)
			SetDlgItemFocus(IDC_SAVECLOSING_SAVERIGHT);

		EnableDlgItem(IDC_SAVECLOSING_MIDDLEFRAME, false);
		EnableDlgItem(IDC_SAVECLOSING_MIDDLEFILE, false);
		EnableDlgItem(IDC_SAVECLOSING_SAVEMIDDLE, false);
		EnableDlgItem(IDC_SAVECLOSING_DISCARDMIDDLE, false);
	}
	
	if (!m_bAskForRight)
	{
		EnableDlgItem(IDC_SAVECLOSING_RIGHTFRAME, false);
		EnableDlgItem(IDC_SAVECLOSING_RIGHTFILE, false);
		EnableDlgItem(IDC_SAVECLOSING_SAVERIGHT, false);
		EnableDlgItem(IDC_SAVECLOSING_DISCARDRIGHT, false);
	}

	if (m_bDisableCancel)
	{
		EnableDlgItem(IDCANCEL, false);
	}

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("SaveClosingDlg"), false); // persist size via registry

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Select files to ask about saving.
 * @param [in] bLeft Do we ask about left-side file?
 * @param [in] bRight Do we ask about right-side file?
 */
void SaveClosingDlg::DoAskFor(bool bLeft /*= false*/, bool bMiddle /*= false*/, bool bRight /*= false*/)
{
	m_bAskForLeft = bLeft;
	m_bAskForMiddle = bMiddle;
	m_bAskForRight = bRight;
}

/** 
 * @brief Called when 'Discard All' button is selected.
 */
void SaveClosingDlg::OnDiscardAll()
{
	m_leftSave = SAVECLOSING_DISCARD;
	m_middleSave = SAVECLOSING_DISCARD;
	m_rightSave = SAVECLOSING_DISCARD;
	UpdateData(FALSE);
	OnOK();
}

/**
 * @brief Common helper to ask user about saving modified documents
 */
bool SaveClosingDlg::ShowAndSave(
	int nBuffers,
	const bool bModified[3],
	const String paths[3],
	const String descs[3],
	const String& saveAsPath,
	bool bAllowCancel,
	const std::function<bool(int)>& doSave,
	const std::function<void(int)>& setSavePoint)
{
	ASSERT(nBuffers == 2 || nBuffers == 3);
	if (nBuffers != 2 && nBuffers != 3)
		return false;

	bool result = true;
	SaveClosingDlg dlg;
	
	if (nBuffers == 3)
		dlg.DoAskFor(bModified[0], bModified[1], bModified[2]);
	else
		dlg.DoAskFor(bModified[0], false, bModified[1]);

	if (!bAllowCancel)
		dlg.m_bDisableCancel = true;

	if (!paths[0].empty())
		dlg.m_sLeftFile = saveAsPath.empty() ? paths[0] : saveAsPath;
	else
		dlg.m_sLeftFile = saveAsPath.empty() ? descs[0] : saveAsPath;

	if (nBuffers == 3)
	{
		if (!paths[1].empty())
			dlg.m_sMiddleFile = saveAsPath.empty() ? paths[1] : saveAsPath;
		else
			dlg.m_sMiddleFile = saveAsPath.empty() ? descs[1] : saveAsPath;
	}

	if (!paths[nBuffers - 1].empty())
		dlg.m_sRightFile = saveAsPath.empty() ? paths[nBuffers - 1] : saveAsPath;
	else
		dlg.m_sRightFile = saveAsPath.empty() ? descs[nBuffers - 1] : saveAsPath;

	if (dlg.DoModal() == IDOK)
	{
		if (bModified[0])
		{
			if (dlg.m_leftSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				bool canContinue = doSave(0);
				if (!canContinue)
					result = false;
			}
			else if (setSavePoint)
				setSavePoint(0);
		}

		if (nBuffers == 3 && bModified[1])
		{
			if (dlg.m_middleSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				bool canContinue = doSave(1);
				if (!canContinue)
					result = false;
			}
			else if (setSavePoint)
				setSavePoint(1);
		}

		if (bModified[nBuffers - 1])
		{
			if (dlg.m_rightSave == SaveClosingDlg::SAVECLOSING_SAVE)
			{
				bool canContinue = doSave(nBuffers - 1);
				if (!canContinue)
					result = false;
			}
			else if (setSavePoint)
				setSavePoint(nBuffers - 1);
		}
	}
	else
	{
		result = false;
	}

	return result;
}
