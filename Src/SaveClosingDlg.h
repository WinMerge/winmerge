// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  SaveClosingDlg.h
 *
 * @brief Declaration file for SaveClosingDlg dialog
 */
#pragma once

#include "TrDialogs.h"
#include "CMoveConstraint.h"
#include "UnicodeString.h"

/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg dialog

/**
 * @brief Dialog asking if user wants to save modified left and/or right
 * files.
 *
 * The dialog has separate frames for both files and unneeded frame and
 * controls inside it are disabled. Asked file(s) are selected using
 * DoAskFor() function.
 */
class SaveClosingDlg : public CTrDialog
{
	DECLARE_DYNAMIC(SaveClosingDlg)

public:

	/** @brief Choices for modified files: save/discard changes. */
	enum SAVECLOSING_CHOICE
	{
		SAVECLOSING_SAVE = 0, /**< Save changes */
		SAVECLOSING_DISCARD,  /**< Discard changes */
	};

	explicit SaveClosingDlg(CWnd* pParent = nullptr);   // standard constructor
	void DoAskFor(bool bLeft = false, bool bMiddle = false, bool bRight = false);

// Dialog Data
	//{{AFX_DATA(SaveClosingDlg)
	enum { IDD = IDD_SAVECLOSING };
	String m_sLeftFile; /**< Path to left-file to save. */
	String m_sMiddleFile; /**< Path to middle-file to save. */
	String m_sRightFile; /**< Path to right-side file to save. */
	int m_leftSave; /**< User's choice for left-side save. */
	int m_middleSave; /**< User's choice for middle-side save. */
	int m_rightSave; /**< User's choice for righ-side save. */
	bool m_bDisableCancel; /**< Should we disable Cancel-button? */
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(SaveClosingDlg)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnDiscardAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation data
private:
	prdlg::CMoveConstraint m_constraint; /**< Resizes dialog controls when dialog resized */
	bool m_bAskForLeft; /**< Is left file modified? */
	bool m_bAskForMiddle; /**< Is middle file modified? */
	bool m_bAskForRight; /**< Is right file modified? */
};
